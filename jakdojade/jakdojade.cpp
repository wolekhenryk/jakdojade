#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include <chrono>
#include <cstring>

#include "str.h"
#include "Vector.h"
#include "heap.h"
#include "arr.h"
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

struct pair_comparator
{
    bool operator() (const pair& a, const pair& b) const {
        return a.weight < b.weight;
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

    heap<pair, pair_comparator> pq;
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


void bfs(arr<int> grid, const int x, const int y, const int start, Vector<Vector<bool>>& visited, Vector<Vector<pair>>& adj_list, const int n, const int m)
{
    queue<tuple> q;
    q.push({ x, y, 0 });
    visited[x][y] = true;

    while (!q.empty())
    {
        const auto [i, j, distance] = q.front();
        q.pop();

        if (grid[i][j] >= 0 && grid[i][j] != start)
        {
            const auto neighbor_id = grid[i][j];

            adj_list[start].push_back({ neighbor_id, distance });
            continue;
        }

        if (i > 0 && !visited[i - 1][j] && grid[i - 1][j] != -2)
        {
            q.push({ i - 1, j, distance + 1 });
            visited[i - 1][j] = true;
        }
        if (i < n - 1 && !visited[i + 1][j] && grid[i + 1][j] != -2)
        {
            q.push({ i + 1, j, distance + 1 });
            visited[i + 1][j] = true;
        }
        if (j > 0 && !visited[i][j - 1] && grid[i][j - 1] != -2)
        {
            q.push({ i, j - 1, distance + 1 });
            visited[i][j - 1] = true;
        }
        if (j < m - 1 && !visited[i][j + 1] && grid[i][j + 1] != -2)
        {
            q.push({ i, j + 1, distance + 1 });
            visited[i][j + 1] = true;
        }
    }
}

int main()
{
    auto before_program = std::chrono::high_resolution_clock::now();
    std::ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    int width, height, count_of_flights, time, query_count, query_type;

    auto start_time = std::chrono::high_resolution_clock::now();

    Vector<str> board;
    Vector<query> queries;

    cin >> width >> height;

    Vector new_graph(width * height, Vector<pair>());

    auto line = new char[width + 1] {};

    for (int i = 0; i < height; i++)
    {
        int j = 0;
        while (j < width)
        {
            const char c = getchar();
            if (c != '\n') line[j++] = c;
        }
        str board_line(line);
        board_line.trim();

        board.push_back(str(board_line));

        memset(line, '\0', width + 1);
    }

    delete[] line;

    Vector<str> cities;
    Vector<int> city_ids;

    int city_count = 0;
    hash_map<int, str> code_to_city;
    hash_map<str, int> city_to_code;

    for (int i = 0; i < board.size(); i++)
    {
        const auto& row = board[i];
        for (int j = 0; j < row.length(); j++)
        {
            if (row[j] == '*')
            {
                const auto& [city_i, city_j] = where_city(board, i, j);
                const auto& result = find_city_name(board, city_i, city_j);

                city_to_code.insert(result, i * width + j);
                code_to_city.insert(i * width + j, result);

                city_ids.push_back(i * width + j);

                cities.push_back(result);
                city_count++;
            }
        }
    }

    bool no_roads = true;
    arr<int> grid2d(height, width);

    for (int i = 0; i < height; i++)
    {
        const auto& row = board[i];

        for (int j = 0; j < row.length(); j++)
        {
            if (row[j] == '.' || isalpha(row[j]) || isalnum(row[j])) grid2d[i][j] = -2;
            if (row[j] == '*') grid2d[i][j] = i * width + j;
            if (row[j] == '#') {
                grid2d[i][j] = -1;
                no_roads = false;
            }
        }
    }

    if (!no_roads)
    {
        for (int i = 0; i < width * height; i++)
        {
            if (code_to_city.contains(i))
            {
                Vector visited(height, Vector(width, false));
                Vector<int> neighbors;
                bfs(grid2d, i / width, i % width, i, visited, new_graph, height, width);
            }
            else
            {
                new_graph.push_back(Vector<pair>());
            }
        }
    }

    cin >> count_of_flights;
    cin.ignore();

    str from_city_flight;
    str to_city_flight;

    from_city_flight.reserve(16);
    to_city_flight.reserve(16);

    char buffer[64]{};

    int j = 0;
    int counter_f = 0;

    while (counter_f < count_of_flights && fgets(buffer, 64, stdin))
    {
        char* from_buffer = buffer;
        char* to_buffer = strchr(from_buffer, ' ');
        if (to_buffer) *to_buffer++ = '\0';

        char* duration_buffer = strchr(to_buffer, ' ');
        if (duration_buffer) *duration_buffer++ = '\0';

        const int duration = atoi(duration_buffer);

        from_city_flight.put_chars(from_buffer);
        to_city_flight.put_chars(to_buffer);

        const int from_city_flight_id = city_to_code.at(from_city_flight);
        const int to_city_flight_id = city_to_code.at(to_city_flight);

        new_graph[from_city_flight_id].push_back({ to_city_flight_id, duration });
        ++counter_f;
    }

    str source, destination;

    auto aux_line = new char[256] {};
    cin >> query_count;
    cin.ignore();
    for (int i = 0; i < query_count; i++) {
        int j = 0;
        while (j < 256)
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

        memset(aux_line, '\0', 256);
    }

    delete[] aux_line;

    for (const auto& [from, to, type] : queries)
    {
        const auto from_city_id = city_to_code.at(from);
        const auto to_city_id = city_to_code.at(to);

        dijkstra(new_graph, from_city_id, to_city_id, code_to_city, type);
    }

    return 0;
}