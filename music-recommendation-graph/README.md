# Music Recommendation Graph

A C++ music recommendation system implemented with graph data structures and graph traversal algorithms.

This project models songs as vertices and song relationships as weighted edges. It focuses on using graph algorithms to recommend related songs, group songs into playlist clusters, find smooth transitions between songs, identify hub songs, and detect cycles in the music network.

## Features

* Add songs and weighted relationships between songs
* Recommend related songs using BFS
* Generate playlist clusters using connected components
* Find the smoothest transition between two songs using Dijkstra's algorithm
* Identify the network hub song using in-degree analysis
* Detect music loops using DFS cycle detection

## How to Build

From the `graph-based-recommendation` directory:

```bash
g++ -std=c++17 src/main.cpp src/MusicGraph.cpp -I include -o app
```
