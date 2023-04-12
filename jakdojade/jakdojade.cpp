
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include "str.h"
#include "Vector.h"
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

Vector<edge> create_graph()
{
	int width, height, count_of_flights, time, query_count, query_type;

	Vector<str> board;
	Vector<flight> flights;
	Vector<query> queries;

	cin >> width >> height;
	cin.ignore();

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

	auto result = map_to_graph(new_map);

	//Add flights to map

	for (const auto& [origin, destination, duration] : flights)
	{
		const auto& origin_city = city_to_code.at(origin);
		const auto& destination_city = city_to_code.at(destination);

		result.push_back({ origin_city, destination_city, duration });
	}

	return result;
}

int main()
{
	const auto ready_map = create_graph();

	for (const auto& [from, to, weight] : ready_map)
	{
		cout << from << " => " << to << " [" << weight << "]" << endl;
	} 

	return 0;
}