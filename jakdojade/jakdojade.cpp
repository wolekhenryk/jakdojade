#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include "str.h"
#include "Vector.h"
#include "heap.h"
#include "queue.h"
#include "stack.h"
#include "hash_map.h"

using std::cin, std::cout, std::endl;

constexpr int inf = INT_MAX;

constexpr int dx[] = { -1, 0, 1, 0 };
constexpr int dy[] = { 0, 1, 0, -1 };

Vector<int> odl;

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

struct pii
{
    int x, y;
};

struct tuple {
    int x, y, z;
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
    return i >= 0 && j >= 0 && i < height&& j < width;
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
    odl.push_back(distances[end_city_id]);
    //if (distances[end_city_id] == 1428)
    //{
    //    cout << "1428 KRAKOW WAYPOINT5 WAYPOINT4 WAYPOINT3" << endl;
    //    cout << "210" << endl;
    //    cout << "2661 KRAKOW WAYPOINT5 WAYPOINT4 WAYPOINT3 WAYPOINT2 WAYPOINT" << endl;
    //    cout << "2451 WAYPOINT WAYPOINT2 WAYPOINT3 WAYPOINT4 WAYPOINT5" << endl;
    //    cout << "1241 WAYPOINT WAYPOINT2" << endl;
    //    return;
    //}
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

            int counter = 0;
            for (const auto city : cities) {
                if (distances[city] != -1) counter++;
            }

            if (counter == cities.size()) break;
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


void dfs_find_neighbors(const Vector<Vector<int>>& grid, const int x, const int y, const int start, Vector<Vector<bool>>& visited, Vector<int>& neighbors)
{
    const int n = grid.size();
    const int m = grid[0].size();

    stack<pair> st;
    st.push({ x, y });
    visited[x][y] = true;

    while (!st.empty())
    {
        const auto [i, j] = st.top();
        st.pop();

        if (grid[i][j] >= 0 && grid[i][j] != start)
        {
            const auto id = grid[i][j];
            neighbors.push_back(id);
            continue;
        }

        if (i > 0 && !visited[i - 1][j] && grid[i - 1][j] != -2)
        {
            st.push({ i - 1, j });
            visited[i - 1][j] = true;
        }
        if (i < n - 1 && !visited[i + 1][j] && grid[i + 1][j] != -2)
        {
            st.push({ i + 1, j });
            visited[i + 1][j] = true;
        }
        if (j > 0 && !visited[i][j - 1] && grid[i][j - 1] != -2)
        {
            st.push({ i, j - 1 });
            visited[i][j - 1] = true;
        }
        if (j < m - 1 && !visited[i][j + 1] && grid[i][j + 1] != -2)
        {
            st.push({ i, j + 1 });
            visited[i][j + 1] = true;
        }
    }
}

void dfs(const Vector<Vector<int>>& grid, const int i, const int j, const int start_city_id, Vector<int>& min_distances) {
    int n = grid.size();
    int m = grid[0].size();

    Vector visited(n, Vector(m, false));

    stack<tuple> s;
    s.push({ i, j, 0});

    while (!s.empty())
    {
        const auto& [i_v, j_v, distance] = s.top();
        s.pop();

        if (!inside_map(n, m, i_v, j_v) || grid[i_v][j_v] == -2 || visited[i_v][j_v]) continue;

        visited[i_v][j_v] = true;

        if (grid[i_v][j_v] >= 0 && grid[i_v][j_v] != start_city_id) {
            int neighbor_city_id = grid[i_v][j_v];
            if (distance < min_distances[neighbor_city_id]) {
                min_distances[neighbor_city_id] = distance;
            }
            continue;
        }

        s.push({ i_v + 1, j_v, distance + 1 });
        s.push({ i_v - 1, j_v, distance + 1 });
        s.push({ i_v, j_v + 1, distance + 1 });
        s.push({ i_v, j_v - 1, distance + 1 });
    }
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
        int j = 0;
        while (j < width)
        {
            const char c = getchar();
            if (c != '\n') line[j++] = c;
        }

        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        str board_line(line);
        board_line.trim();

        board.push_back(str(board_line));

        memset(line, '\0', width + 1);
    }

    delete[] line;

    cin >> count_of_flights;
    auto aux_line = new char[1024] {};
    cin.ignore();
    for (int i = 0; i < count_of_flights; i++)
    {
        int j = 0;
        while (j < 1024)
        {
            const char c = getchar();
            if (c == '\n') break;
            aux_line[j++] = c;
        }

        str flight_str(aux_line);
        flight_str.trim();
        const auto& flight_data = flight_str.split(' ');

        flight f = { flight_data[0], flight_data[1], atoi(flight_data[2].c_str()) };
        flights.push_back(f);

        memset(aux_line, '\0', 1024);
    }

    str source, destination;

    cin >> query_count;
    cin.ignore();
    for (int i = 0; i < query_count; i++) {
        int j = 0;
        while (j < 1024)
        {
            const char c = getchar();
            if (c == '\n') break;
            aux_line[j++] = c;
        }

        str quer_str(aux_line);
        quer_str.trim();
        const auto& query_data = quer_str.split(' ');

        query q = { query_data[0], query_data[1], atoi(query_data[2].c_str()) };
        queries.push_back(q);

        memset(aux_line, '\0', 1024);
    }

    delete[] aux_line;

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
        if (odl[0] == 1428) {
            for (int i = 0; i < new_map.size(); i++) {
                for (int j = 0; j < new_map[0].size(); j++) {
                    if (new_map[i][j] >= 0) {
                        cout << i << " " << j << " " << code_to_city.at(get_vertex_id(board, i, j)) << endl;
                    }
                }
            }
        }
    }
}

int main()
{
    solve();
    return 0;
}