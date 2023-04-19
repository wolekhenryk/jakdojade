
# Jakdojade

Jak dojade is a C++ console application built in Visual Studio. It reads the width and height of the map, followed by the map itself, which is composed of alphanumeric characters, hashtags, dots and asterisks symbolizing cities.

After reading the map, the user is prompted to provide the number of one-directional flight connections and the connections themselves, line by line, of format:

    FROM TO DURATION
Where duration denotes the time of flight. The last user input is the number of queries, followed by the queries themselves, line by line, which have the following format: 

    FROM TO TYPE
The TYPE parameter can be either 0 or 1. The query 0 will display only the shortest distance between two cities, while query 1 will display the shortest distance and the cities visited along the way. The distance is always correct.

This project was created to help getting familiar with constructing and representing graphs, such as adjacency lists, graph search, and traversal algorithms such as breadth-first search, as well as more complex graph algorithms such as Dijkstra's algorithm.

Additionally, this project includes custom-made data structures essential for optimization, including hashmaps, heaps, queues, strings, and vectors.

## Installation

To install and run Jak dojade, you need to have Visual Studio installed on your computer.

    1. Clone this repository to your local machine.

    2. Open the project in Visual Studio.

    3. Build the project.

    4. Run the executable.

## Usage
Once you run the executable, the program will prompt you to input the width and height of the map, followed by the map itself.

Next, you will be prompted to input the number of one-directional flight connections and the connections themselves.

Finally, you will be prompted to input the number of queries and the queries themselves, with the format:

     FROM TO TYPE

## Output

Based on the query type, the program output will either be

    DISTANCE
or, if the query was 1, then it will be

    DISTANCE CITY_1 CITY_2 ... CITY_N
where DISTANCE is the shortest possible path between 2 cities specified in query, and consecutive cities represent intermediate nodes which will be traversed.

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)

[![C++17](https://img.shields.io/badge/C++-17-red.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
## Authors

- [@wolekhenryk](https://www.github.com/wolekhenryk)


## Features

- User-friendly command-line interface
- Always finds the shortest path if exists
- Clever graph-constructing method
- Efficient memory and resource management

## Optimizations

Binary heap was used to implement highly-efficient priority queue.

