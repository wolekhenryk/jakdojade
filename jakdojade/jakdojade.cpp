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

struct pair
{
	int to;
	int weight;

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

bool inside_map(const int i, const int j, const Vector<str>& board)
{
	return (i >= 0 && j >= 0) && (i < board.size() && j < board[0].length());
}

bool in_bounds(const Vector<Vector<int>>& grid, int x, int y) {
	return x >= 0 && x < grid.size() && y >= 0 && y < grid[0].size();
}

str find_city_name(const Vector<str>& board, const int i, const int j)
{
	int start_index = j;
	int end_index = j;

	while (end_index < board[i].length() && isupper(board[i][end_index])) {
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

void dfs(const Vector<Vector<int>>& map, const int i, const int j, const int start_city_id, int& distance, Vector<Vector<pair>>& adj_list, Vector<Vector<bool>>& visited)
{
	if (!is_valid(map, i, j) || map[i][j] == -2 || visited[i][j]) return;

	if (map[i][j] >= 0 && map[i][j] != start_city_id)
	{
		const pair p = { map[i][j], distance };

		//Check if edge already exists, and update weight if needed
		bool found = false;
		for (auto& [to, weight] : adj_list[start_city_id])
		{
			if (to == map[i][j] && weight > distance)
			{
				weight = distance;
				found = true;
			}
		}

		//Add only if there was no such edge
		if (!found) adj_list[start_city_id].push_back(p);
		
		return;
	}

	visited[i][j] = true;

	distance++;
	dfs(map, i, j + 1, start_city_id, distance, adj_list, visited);
	distance--;

	distance++;
	dfs(map, i, j - 1, start_city_id, distance, adj_list, visited);
	distance--;

	distance++;
	dfs(map, i + 1, j, start_city_id, distance, adj_list, visited);
	distance--;

	distance++;
	dfs(map, i - 1, j, start_city_id, distance, adj_list, visited);
	distance--;
}

Vector<Vector<pair>> map_to_graph(const Vector<Vector<int>>& new_map, const int city_count)
{
	Vector<Vector<pair>> adj_list;
	for (int i = 0; i < city_count; i++)
	{
		Vector<pair> temp;
		adj_list.push_back(temp);
	}

	for (int i = 0; i < new_map.size(); i++)
	{
		for (int j = 0; j < new_map[i].size(); j++)
		{
			if (new_map[i][j] >= 0)
			{
				Vector temp(new_map[i].size(), false);
				Vector visited(new_map.size(), temp);
				
				int d = 0;
				dfs(new_map, i, j, new_map[i][j], d, adj_list, visited);
			}
		}
	}

	return adj_list;
}

void dijkstra(const Vector<Vector<pair>>& graph, const int start_city, const int end_city, const hash_map<int, str>& code_to_city, const int query_type)
{
	if (graph.empty())
	{
		cout << 0 << endl;
		return;
	}

	const auto n = graph.size();

	Vector distances(n, inf);
	distances[start_city] = 0;

	Vector prev(n, -1);

	heap<pair> pq;
	pq.push({ start_city, 0 });

	while (!pq.empty())
	{
		const auto [vertex, dist] = pq.top();
		pq.pop();

		if (dist > distances[vertex]) continue;

		for (const auto [to, weight] : graph[vertex])
		{
			const int new_distance = dist + weight;
			if (new_distance < distances[to])
			{
				prev[to] = vertex;
				distances[to] = new_distance;
				pq.push({ to, new_distance });
			}
		}
	}

	if (query_type == 0) cout << distances[end_city] << endl;
	else if (query_type == 1)
	{
		cout << distances[end_city] << " ";
		Vector<int> path;
		int current = end_city;
		while (current != start_city)
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

	auto ready_graph = map_to_graph(new_map, city_count);

	//Add flights to map
	for (const auto& [origin, destination, duration] : flights)
	{
		const auto& origin_city = city_to_code.at(origin);
		const auto& destination_city = city_to_code.at(destination);

		ready_graph[origin_city].push_back({ destination_city, duration });
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