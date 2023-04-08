#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include "str.h"
#include "vector.h"

using namespace std;

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
	int x;
	int y;
};

bool inside_map (const int x, const int y, const Vector<str>& board)
{
	return (x >= 0 && x < board[0].length()) && (y >= 0 && y < board.size());
}

shift where_city(const Vector<str>& board, const int x, const int y)
{
	shift shifts[] = {
		{0, -1},
		{-1, -1},
		{-1, 0},
		{-1, 1},
		{0, 1},
		{1, 1},
		{1, 0},
		{1, -1}
	};

	for (const auto& [delta_x, delta_y] : shifts)
	{
		if (!inside_map(x + delta_x, y + delta_y, board)) continue;
		if (isupper(board[y + delta_y][x + delta_x])) return { x + delta_x, y + delta_y };
	}
	return {};
}

void read()
{
	int width, height, flight_connections, query_count, duration, type;
	str source, destination;
	cin >> width >> height;
	cin.ignore();

	auto line = new char[width + 1]{};

	Vector<str> board;
	Vector<flight> flights;
	Vector<query> queries;

	int i = 0;
	for (i = 0; i < height; i++)
	{
		fgets(line, width + 1, stdin);

		str str_line(line);
		str_line.trim();

		board.push_back(str_line);

		memset(line, '\0', width + 1);
	}

	delete[] line;

	cin >> flight_connections;
	cin.ignore();

	for (int i = 0; i < flight_connections; i++)
	{
		cin >> source >> destination >> duration;
		flight f;

		f.from = source;
		f.to = destination;
		f.duration = duration;

		flights.push_back(f);
	}

	cin >> query_count;
	cin.ignore();

	for (int i = 0; i < query_count; i++)
	{
		cin >> source >> destination >> type;
		query q;

		q.source = source;
		q.target = destination;
		q.type = type;

		queries.push_back(q);
	}

	//for (const auto& row : board)
	//{
	//	cout << row << endl;
	//}

	for (int i = 0; i < board.size(); i++)
	{
		const auto& row = board[i];
		for (int j = 0; j < row.length(); j++)
		{
			if (row[j] == '*')
			{
				const auto& [new_x, new_y] = where_city(board, j, i);
				//const auto& new_row = board[new_y];

				//cout << city_begin << endl;
			}
		}
	}
}

int main()
{
	read();
	return 0;
}
