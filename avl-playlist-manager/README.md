# AVL Playlist Manager

A C++ playlist management program implemented with AVL Tree and Threaded AVL Tree data structures.

This project focuses on practicing balanced binary search trees, tree traversal, and object-oriented programming through a music playlist management system.

## Features

* Manage playlist data with tree-based structures
* Insert, remove, and search playlist items
* Maintain balanced tree structure using AVL rotations
* Support basic AVL mode
* Support Threaded AVL mode as an optional bonus implementation

## How to Build

From the `avl-playlist-manager` directory:

Basic AVL mode:

```bash id="g71ag4"
g++ -std=c++17 src/main.cpp src/Playlist.cpp -I include -o app
```

Threaded AVL mode:

```bash id="5t65s9"
g++ -std=c++17 -DUSE_THREADED_AVL src/main.cpp src/Playlist.cpp -I include -o app
```
