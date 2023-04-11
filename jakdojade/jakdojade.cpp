#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cctype>

//ILLEGAL
#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <limits>
#include <map>

#include "str.h"
#include "vector.h"

using namespace std;

constexpr int inf = numeric_limits<int>::max();

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

struct node
{
	int id;
	int weight;

	bool operator>(const node& other) const { return weight > other.weight; }
};

struct edge
{
	pair<int, int> route;
	int weight;
};

bool compareEdges(const edge& a, const edge& b) {
	if (a.route.first == b.route.first)
		return a.route.second < b.route.second;
	return a.route.first < b.route.first;
}

vector<edge> cleanEdges(vector<edge>& edges) {
	sort(edges.begin(), edges.end(), compareEdges);

	vector<edge> cleanedEdges;
	cleanedEdges.push_back(edges[0]);

	for (int i = 1; i < edges.size(); i++) {
		if (edges[i].route != edges[i - 1].route)
			cleanedEdges.push_back(edges[i]);
	}

	return cleanedEdges;
}

bool is_valid(const vector<vector<int>>& map, const int i, const int j) {
	const int n = map.size(); // number of rows
	const int m = map[0].size(); // number of columns

	return (i >= 0 && i < n && j >= 0 && j < m);
}

void dfs(vector<vector<int>>& map, const int i, const int j, const int start_city_id, int& distance, vector<edge>& edges)
{
	if (!is_valid(map, i, j) || map[i][j] == -2 || map[i][j] == -3) return;

	if (map[i][j] >= 0 && map[i][j] != start_city_id)
	{
		edges.push_back({make_pair(start_city_id, map[i][j]), distance});
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

vector<vector<int>> to_graph(vector<vector<int>>& map)
{
	const int rows = map.size();
	const int cols = map[0].size();

	vector<vector<int>> result;

	// Define the directions for BFS to check
	vector<int> dx = { 0, 0, -1, 1 };
	vector<int> dy = { -1, 1, 0, 0 };

	for (int i = 0; i < rows; i++)
	{
		vector<int> temp_distances;
		for (int j = 0; j < cols; j++)
		{
			if (map[i][j] >= 0)
			{
				// If map[i][j] is -1, then it's a path
				// If map[i][j] is -2, then it's a wall
				// If map[i][j] is >= 0, then it's a city.

				int distance = 0;
				//DFS




				temp_distances.push_back(distance);
			}
		}
		result.push_back(temp_distances);
	}

	return result;
}

bool inside_map(const int i, const int j, const Vector<str>& board)
{
	return (i >= 0 && j >= 0) && (i < board.size() && j < board[0].length());
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

void keep_shortest_paths(vector<edge>& edges)
{
	map<pair<int, int>, int> shortest_paths;

	for (const auto& e : edges)
	{
		auto& p = e.route;
		if (shortest_paths.find(p) == shortest_paths.end() || e.weight < shortest_paths[p])
		{
			shortest_paths[p] = e.weight;
		}
	}

	edges.clear();
	for (const auto& p : shortest_paths)
	{
		edges.push_back({ p.first, p.second });
	}
}

void read()
{
	int width, height, count_of_flights, time, query_count, query_type;

	Vector<str> board;
	Vector<flight> flights;
	Vector<query> queries;

	cin >> width >> height;
	cin.ignore(); // discard the newline character from the input buffer

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
		cin.ignore();
		cin >> from >> to >> time;
		flight f = { from, to, time };
		flights.push_back(f);
	}

	str source, destination;

	cin >> query_count;
	for (int i = 0; i < query_count; i++)
	{
		cin.ignore();
		cin >> source >> destination >> query_type;
		query q = { source, destination, query_type};
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

	vector<vector<int>> new_map;
	unordered_map<int, str> city_codes;
	int city_count = 0;

	for (const auto& row : board)
	{
		vector<int> temp;
		for (int i = 0; i < row.length(); i++)
		{
			if (row[i] == '*')
			{
				city_codes.insert(make_pair(city_count, cities[city_count]));
				temp.push_back(city_count);
				city_count++;
			}
			if (row[i] == '#') temp.push_back(-1);
			if (row[i] == '.' || isalpha(row[i])) temp.push_back(-2);

		}
		new_map.push_back(temp);
	}

	//for (const auto& row : new_map)
	//{
	//	for (const auto num : row)
	//	{
	//		cout << num << " ";
	//	}
	//	cout << endl;
	//}

	vector<edge> edges;
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
	keep_shortest_paths(edges);

}

int main()
{
	read();
	return 0;
}
