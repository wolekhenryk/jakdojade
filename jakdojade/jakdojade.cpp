#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include "str.h"
#include "Vector.h"
#include "heap.h"
#include "queue.h"
#include "hash_map.h"

using std::cin, std::cout, std::endl;

constexpr int inf = INT_MAX;

constexpr int dx[] = { -1, 0, 1, 0 };
constexpr int dy[] = { 0, 1, 0, -1 };

struct flight
{
	str from, to;
	int duration;
};

struct query
{
	str from, to;
	int type;
};

struct shift
{
	int i, j;
};

struct pair
{
	int to, weight;

    bool operator < (const pair& other) const
    {
        return weight < other.weight;
    }

    bool operator > (const pair& other) const
    {
        return weight > other.weight;
    }

    bool operator==(const pair& other) const
    {
        return (to == other.to) && (weight == other.weight);
    }
};

bool inside_map(const int height, const int width, const int i, const int j)
{
    return i >= 0 && j >= 0 && i < height && j < width;
}

str find_city_name(const Vector<str>& board, const int i, const int j)
{
    int start_index = j;
    int end_index = j;

    while (end_index < board[i].length() && isalnum(board[i][end_index])) {
        end_index++;
    }

    while (start_index >= 0 && isupper(board[i][start_index])) {
        start_index--;
    }

    str city_name = board[i].substr(start_index + 1, end_index - start_index - 1);
    return city_name;
}

shift where_city(const Vector<str>& board, const int i, const int j)
{
    shift shifts[] = {
        {0, -1},
        {-1, 0},
        {0, 1},
        {1, 0},
        {-1, -1},
        {-1, 1},
        {1, 1},
        {1, -1}
    };

    for (const auto& [delta_i, delta_j] : shifts)
    {
        if (!inside_map(board.size(), board[0].length(), i + delta_i, j + delta_j)) continue;
        if (isupper(board[i + delta_i][j + delta_j])) return { i + delta_i, j + delta_j };
    }

    return {};
}

int get_vertex_id(const Vector<str>& board, const int i, const int j)
{
    return i * board[0].length() + j;
}

void dijkstra(const Vector<Vector<pair>>& graph, const int start_city_id, const int end_city_id, const hash_map<int, str>& code_to_city, const int query_type)
{
    const auto n = graph.size();
    Vector distances(n, INT_MAX);
    distances[start_city_id] = 0;
    Vector prev(n, -1);

    heap<pair> pq;
    pq.push({ start_city_id, 0 });

    while (!pq.empty())
    {
        const auto mp = pq.top();
        pq.pop();

        if (mp.weight > distances[mp.to]) continue;

        for (const auto neighbor : graph[mp.to])
        {
            const int new_distance = neighbor.weight + mp.weight;
            if (new_distance < distances[neighbor.to])
            {
                distances[neighbor.to] = new_distance;
                prev[neighbor.to] = mp.to;
                pq.push({ neighbor.to, new_distance });
            }
        }

    }

    if (query_type == 0) cout << distances[end_city_id] << endl;
    else
    {
        cout << distances[end_city_id] << " ";
        Vector<int> path;
        int current = end_city_id;
        while (current != start_city_id)
        {
            path.push_back(current);
            current = prev[current];
        }

        path.reverse();
        path.pop_back();

        for (const auto i : path) cout << code_to_city.at(i) << " ";

        cout << endl;
    }
}

void bfs_build_graph(const Vector<Vector<pair>>& graph, Vector<Vector<pair>>& adj_list, const int start_city, const Vector<int>& cities)
{
    const int n = graph.size();
    Vector distances(n, -1);
    distances[start_city] = 0;

    queue<int> q;
    q.push(start_city);

    while (!q.empty())
    {
        const int curr = q.front();
        q.pop();

        for (const auto& [neighbor, w] : graph[curr])
        {
            if (distances[neighbor] == -1)
            {
                distances[neighbor] = distances[curr] + 1;
                q.push(neighbor);
            }
        }
    }

    Vector<pair> temp;
    for (const auto city : cities)
    {
        if (distances[city] != -1)
        {
            temp.push_back({ city, distances[city] });
        }
    }
    adj_list.push_back(temp);
}

void dfs_find_neighbors(const Vector<Vector<int>>& grid, const int i, const int j, const int start, Vector<Vector<bool>>& visited, Vector<int>& neighbors)
{
    if (i < 0 || j < 0 || i >= grid.size() || j >= grid[0].size()) return;
    if (grid[i][j] == -2) return;
    if (visited[i][j]) return;
    if (grid[i][j] >= 0 && grid[i][j] != start)
    {
        const auto id = grid[i][j];
        for (const auto& element : neighbors) if (grid[i][j] == element) return;
        neighbors.push_back(grid[i][j]);

        return;
    }

    visited[i][j] = true;

    dfs_find_neighbors(grid, i + 1, j, start, visited, neighbors);
    dfs_find_neighbors(grid, i - 1, j, start, visited, neighbors);
    dfs_find_neighbors(grid, i, j + 1, start, visited, neighbors);
    dfs_find_neighbors(grid, i, j - 1, start, visited, neighbors);
}

void solve()
{
    int width, height, count_of_flights, time, query_count, query_type;

    Vector<str> board;
    Vector<flight> flights;
    Vector<query> queries;

    cin >> width >> height;

    auto line = new char[width + 1] {};

    for (int i = 0; i < height; i++)
    {
        cin >> line;
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        str board_line(line);
        board_line.trim();

        board.push_back(str(board_line));

        memset(line, '\0', width + 1);
    }

    delete[] line;

    str from, to;

    cin >> count_of_flights;
    for (int i = 0; i < count_of_flights; i++)
    {
        cin >> from >> to >> time;
        flight f = { from, to, time };
        flights.push_back(f);
    }

    str source, destination;

    cin >> query_count;
    for (int i = 0; i < query_count; i++) {
        cin >> source >> destination >> query_type;
        query q = { source, destination, query_type };
        queries.push_back(q);
    }

    //if (width == 100 && height == 10)
    //{
    //    cout << width << " " << height << endl;
    //    for (const auto& row : board) cout << row << endl;
    //    cout << count_of_flights << endl;
    //    for (const auto& [from, to, time] : flights) cout << from << " " << to << " " << time << endl;
    //    cout << query_count << endl;
    //    for (const auto& [src, dst, type] : queries) cout << src << " " << dst << " " << type << endl;
    //    return;
    //}

    Vector<str> cities;

    for (int i = 0; i < board.size(); i++)
    {
        const auto& row = board[i];
        for (int j = 0; j < row.length(); j++)
        {
            if (row[j] == '*')
            {
                const auto& [city_i, city_j] = where_city(board, i, j);
                const auto& result = find_city_name(board, city_i, city_j);

                cities.push_back(result);
            }
        }
    }

    Vector<Vector<int>> new_map;
    for (int i = 0; i < height; i++)
    {
        const auto& row = board[i];

        Vector<int> temp_vec;
        for (int j = 0; j < row.length(); j++)
        {
            if (row[j] == '.' || isalpha(row[j]) || isalnum(row[j])) temp_vec.push_back(-2);
            if (row[j] == '*') temp_vec.push_back(get_vertex_id(board, i, j));
            if (row[j] == '#') temp_vec.push_back(-1);
        }
        new_map.push_back(temp_vec);
        temp_vec.clear();
    }

    int city_count = 0;
    hash_map<int, str> code_to_city;
    hash_map<str, int> city_to_code;

    Vector<Vector<pair>> graph(width * height);
    Vector<int> city_ids;

    for (int i = 0; i < height; i++)
    {
	    for (int j = 0; j < width; j++)
	    {
            if (board[i][j] == '*' || board[i][j] == '#')
            {
                const int current_id = get_vertex_id(board, i, j);

                for (int k = 0; k < 4; k++)
                {
                    const int ni = i + dx[k];
                    const int nj = j + dy[k];
                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        if (board[ni][nj] == '*' || board[ni][nj] == '#')
                        {
                            const int nid = get_vertex_id(board, ni, nj);
                            const int weight = (board[ni][nj] == '*') ? 0 : 1;
                            graph[current_id].push_back({ nid, 1 });
                        }
                    }
                }

                if (board[i][j] == '*')
                {
                    code_to_city.insert(current_id, cities[city_count]);
                    city_to_code.insert(cities[city_count], current_id);

                    city_count++;
                    city_ids.push_back(current_id);
                }
            }
	    }
    }

    //for (int i = 0; i < graph.size(); i++)
    //{
    //    cout << "From " << i << endl;
    //    for (const auto& [v, w] : graph[i])
    //    {
    //        cout << "=> " << v << "[" << w << "], ";
    //    }
    //    cout << endl;
    //}

    //for (const auto& row : new_map)
    //{
    //    for (const auto num : row) cout << num << " ";
    //    cout << endl;
    //}

    //for (const auto& element : city_ids)
    //{
    //    cout << "City " << code_to_city.at(element) << " is at " << element << endl;
    //}

    Vector<Vector<pair>> new_graph;

    for (int i = 0; i < width * height; i++)
    {
        if (city_ids.contains(i))
        {
            Vector visited(height, Vector(width, false));
            Vector<int> neighbors;

            dfs_find_neighbors(new_map, i / width, i % width, i, visited, neighbors);

            bfs_build_graph(graph, new_graph, i, neighbors);
        }
        else
        {
            new_graph.push_back(Vector<pair>());
        }
    }


    for (const auto& [src, dst, time] : flights)
    {
        const auto src_city_id = city_to_code.at(src);
        const auto dst_city_id = city_to_code.at(dst);

        new_graph[src_city_id].push_back({ dst_city_id, time });
    }

    for (const auto& [from, to, type] : queries)
    {
        const auto from_city_id = city_to_code.at(from);
        const auto to_city_id = city_to_code.at(to);

        dijkstra(new_graph, from_city_id, to_city_id, code_to_city, type);
    }
}

int main()
{
    solve();
    return 0;
}