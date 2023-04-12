
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include "str.h"
#include "Vector.h"
#include "heap.h"
#include "hash_map.h"

using std::cin, std::cout, std::endl;

constexpr int inf = std::numeric_limits<int>::max();

struct flight
{
	str from;
	str to;
	int duration;
};

struct query
{
	str source;
	str target;

	int type;
};

struct shift
{
	int i, j;
};

struct edge
{
	int from;
	int to;
	int weight;
};

struct road
{
	int distance;
	Vector<int> path;
};

bool inside_map(const int i, const int j, const Vector<str>& board)
{
	return (i >= 0 && j >= 0) && (i < board.size() && j < board[0].length());
}

str find_city_name(const Vector<str>& board, const int i, const int j)
{
	// Initialize the starting and ending indices to the current position
	int start_index = j;
	int end_index = j;

	// Search to the right for the end of the city name
	while (end_index < board[i].length() && isupper(board[i][end_index])) {
		end_index++;
	}

	// Search to the left for the start of the city name
	while (start_index >= 0 && isupper(board[i][start_index])) {
		start_index--;
	}

	// Extract the city name from the board and return it
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
		if (!inside_map(i + delta_i, j + delta_j, board)) continue;
		if (isupper(board[i + delta_i][j + delta_j])) return { i + delta_i, j + delta_j };
	}

	return {};
}

bool is_valid(const Vector<Vector<int>>& map, const int i, const int j) {
	const int n = map.size(); // number of rows
	const int m = map[0].size(); // number of columns

	return (i >= 0 && i < n && j >= 0 && j < m);
}

void dfs(Vector<Vector<int>>& map, const int i, const int j, const int start_city_id, int& distance, Vector<edge>& edges)
{
	if (!is_valid(map, i, j) || map[i][j] == -2 || map[i][j] == -3) return;

	if (map[i][j] >= 0 && map[i][j] != start_city_id)
	{
		edges.push_back({ start_city_id, map[i][j], distance });
		return;
	}

	map[i][j] = -3;
	distance++;

	dfs(map, i + 1, j, start_city_id, distance, edges);
	dfs(map, i - 1, j, start_city_id, distance, edges);
	dfs(map, i, j + 1, start_city_id, distance, edges);
	dfs(map, i, j - 1, start_city_id, distance, edges);

	distance--;
}

Vector<edge> map_to_graph(const Vector<Vector<int>>& new_map)
{
	Vector<edge> edges;
	for (int i = 0; i < new_map.size(); i++)
	{
		for (int j = 0; j < new_map[i].size(); j++)
		{
			if (new_map[i][j] >= 0)
			{
				auto map_copy = new_map;
				int d = 0;
				dfs(map_copy, i, j, new_map[i][j], d, edges);
			}
		}
	}

	return edges;
}

void dijkstra(const Vector<edge>& graph, const int start_city, const int end_city, const hash_map<int, str>& code_to_city, const int query_type)
{
	if (graph.empty())
	{
		cout << 0 << endl;
		return;
	}

	Vector distances(graph.size(), inf);
	Vector<Vector<int>> paths(graph.size());
	Vector<road> found_roads;

	distances[start_city] = 0;

	heap<int> pq;
	pq.push(start_city);

	while (!pq.empty())
	{
		const auto curr = pq.top();
		pq.pop();

		if (curr == end_city)
		{
			if (query_type == 0) found_roads.push_back({ distances[end_city], Vector<int>()});
			if (query_type == 1)
			{
				found_roads.push_back({ distances[end_city], paths[end_city] });
			}
		}

		for (const auto& [from, to, weight] : graph)
		{
			if (from == curr)
			{
				const auto next = to;
				const auto cost = weight;

				if (distances[curr] + cost < distances[next])
				{
					distances[next] = distances[curr] + cost;
					pq.push(next);

					// Store the path to next as a list of vertices
					if (curr != end_city && curr != start_city)
					{
						paths[next] = paths[curr];
						paths[next].push_back(curr);
					}
				}
			}
		}
	}

	if (query_type == 0)
	{
		auto& min_distance = found_roads[0].distance;
		for (const auto& [distance, path] : found_roads)
		{
			if (min_distance > distance) min_distance = distance;
		}

		cout << min_distance << endl;
	}

	if (query_type == 1)
	{
		auto& min_road = found_roads[0];
		for (const auto& road : found_roads)
		{
			if (min_road.distance > road.distance) min_road = road;
		}

		cout << min_road.distance << " ";
		for (const auto& city_code : min_road.path) cout << code_to_city.at(city_code) << " ";
		cout << endl;
	}
}

void solve()
{
	int width, height, count_of_flights, time, query_count, query_type;

	Vector<str> board;
	Vector<flight> flights;
	Vector<query> queries;

	cin >> width >> height;
	//cin.ignore();

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

	Vector<str> cities;

	for (int i = 0; i < board.size(); i++)
	{
		const auto& row = board[i];
		for (int j = 0; j < row.length(); j++)
		{
			if (row[j] == '*')
			{
				//Found city, figure out where letters are
				const auto& [city_i, city_j] = where_city(board, i, j);

				const auto& result = find_city_name(board, city_i, city_j);

				cities.push_back(result);
			}
		}
	}

	Vector<Vector<int>> new_map;
	int city_count = 0;

	hash_map<int, str> code_to_city;
	hash_map<str, int> city_to_code;

	for (const auto& row : board)
	{
		Vector<int> temp;
		for (int i = 0; i < row.length(); i++)
		{
			if (row[i] == '*')
			{
				temp.push_back(city_count);

				code_to_city.insert(city_count, cities[city_count]);
				city_to_code.insert(cities[city_count], city_count);

				city_count++;
			}
			if (row[i] == '#') temp.push_back(-1);
			if (row[i] == '.' || isalpha(row[i])) temp.push_back(-2);

		}
		new_map.push_back(temp);
	}

	auto ready_graph = map_to_graph(new_map);

	//Add flights to map
	for (const auto& [origin, destination, duration] : flights)
	{
		const auto& origin_city = city_to_code.at(origin);
		const auto& destination_city = city_to_code.at(destination);

		ready_graph.push_back({ origin_city, destination_city, duration });
	}

	for (const auto& [starting_city, ending_city, query_type] : queries)
	{
		const auto& starting_city_code = city_to_code.at(starting_city);
		const auto& ending_city_code = city_to_code.at(ending_city);

		dijkstra(ready_graph, starting_city_code, ending_city_code, code_to_city, query_type);
	}
}

int main()
{
	solve();

	return 0;
}