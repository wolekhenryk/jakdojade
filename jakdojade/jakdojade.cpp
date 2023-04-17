#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <chrono>

#include "str.h"
#include "Vector.h"
#include "heap.h"
#include "queue.h"
#include "hash_map.h"

using std::cin, std::cout, std::endl;

constexpr int inf = INT_MAX;

constexpr int dx[] = { -1, 0, 1, 0 };
constexpr int dy[] = { 0, 1, 0, -1 };

int city_count_num = 0;
int count_of_edges = 0;

int count_of_flights_pushed = 0;

std::chrono::milliseconds graph_construction_time_ms;

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
    int x, y, distance;
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

void bfs(const Vector<int>& grid, const int x, const int y, const int start, Vector<bool>& visited, Vector<Vector<pair>>& adj_list, const int grid_width, const int grid_height)
{
    const int n = grid_height;
    const int m = grid_width;
    queue<tuple> q;
    q.push({ x, y, 0 });
    visited[x * grid_width + y] = true;

    while (!q.empty())
    {
        const auto [i, j, distance] = q.front();
        q.pop();

        if (grid[i * grid_width + j] >= 0 && grid[i * grid_width + j] != start)
        {
            const auto neighbor_id = grid[i * grid_width + j];
            adj_list[start].push_back({ neighbor_id, distance });
            continue;
        }

        if (i > 0 && !visited[(i - 1) * grid_width + j] && grid[(i - 1) * grid_width + j] != -2)
        {
            q.push({ i - 1, j, distance + 1 });
            visited[(i - 1) * grid_width + j] = true;
        }
        if (i < n - 1 && !visited[(i + 1) * grid_width + j] && grid[(i + 1) * grid_width + j] != -2)
        {
            q.push({ i + 1, j, distance + 1 });
            visited[(i + 1) * grid_width + j] = true;
        }
        if (j > 0 && !visited[i * grid_width + j - 1] && grid[i * grid_width + j - 1] != -2)
        {
            q.push({ i, j - 1, distance + 1 });
            visited[i * grid_width + j - 1] = true;
        }
        if (j < m - 1 && !visited[i * grid_width + j + 1] && grid[i * grid_width + j + 1] != -2)
        {
            q.push({ i, j + 1, distance + 1 });
            visited[i * grid_width + j + 1] = true;
        }
    }
}

void solve()
{
    int width, height, count_of_flights, time, query_count, query_type, city_count = 0;

    Vector<str> board;
    Vector<flight> flights;
    Vector<query> queries;
    Vector<str> cities;
    Vector<int> city_ids;

    hash_map<int, str> code_to_city;
    hash_map<str, int> city_to_code;

    cin >> width >> height;

    bool no_roads = true;

    auto start_time = std::chrono::high_resolution_clock::now();

    Vector<int> og_map(width * height);

    auto end_time = std::chrono::high_resolution_clock::now();

    auto res = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    Vector new_graph(width * height, Vector<pair>());

    str city_name_getchar;
    for (int i = 0; i < width * height; i++)
    {
        const char c = getchar();

        if (c == '\n')
        {
            i--;
            continue;
        }

        if (c == '.')
        {
            og_map[i] = -2;
            if (city_name_getchar.length() != 0)
            {
                cities.push_back(city_name_getchar);
                city_name_getchar = "";
            }
        } else if (isalnum(c))
        {
            og_map[i] = -2;
            city_name_getchar += c;
        } else if (c == '#')
        {
            no_roads = false;
            og_map[i] = -1;
            if (city_name_getchar.length() != 0)
            {
                cities.push_back(city_name_getchar);
                city_name_getchar = "";
            }
        } else if (c == '*')
        {
            city_ids.push_back(i);
            og_map[i] = i;
            if (city_name_getchar.length() != 0)
            {
                cities.push_back(city_name_getchar);
                city_name_getchar = "";
            }
        }
    }

    city_count = cities.size();
    for (int i = 0; i < city_count; i++)
    {
        code_to_city.insert(city_ids[i], cities[i]);
        city_to_code.insert(cities[i], city_ids[i]);
    }

    if (!no_roads)
    {
        for (int i = 0; i < width * height; i++)
        {
            if (code_to_city.contains(i))
            {
                Vector visited(height * width, false);
                Vector<int> neighbors;
                bfs(og_map, i / width, i % width, i, visited, new_graph, width, height);
                city_count_num++;
            }
            else
            {
                new_graph.push_back(Vector<pair>());
            }
        }
    }

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

        new_graph[city_to_code.at(flight_data[0])].push_back({ city_to_code.at(flight_data[1]), atoi(flight_data[2].c_str()) });

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