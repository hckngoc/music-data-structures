#include "MusicGraph.h"

// =============================================================================
// PRIVATE HELPER METHODS
// =============================================================================

int MusicGraph::getSongIndex(const string& id) const {
    for (size_t i = 0; i < songsList.size(); ++i) {
        if (songsList[i].id == id) {
            return (int)i;
        }
    }
    return -1;
}

bool MusicGraph::isVisited(const string& id, const vector<string>& visitedList) const {
    for (size_t i = 0; i < visitedList.size(); ++i) {
        if (visitedList[i] == id) {
            return true;
        }
    }
    return false;
}

// =============================================================================
// PUBLIC METHODS
// =============================================================================

void MusicGraph::addSong(const string& id, const string& title, const string& artist, const string& genre) {
    if (getSongIndex(id) != -1) {
        return;
    }

    Song song;
    song.id = id;
    song.title = title;
    song.artist = artist;
    song.genre = genre;

    SongEntry entry;
    entry.id = id;
    entry.data = song;
    songsList.push_back(entry);

    this->addVertex(id);
}

void MusicGraph::printSongInfo(const string& id) const {
    int idx = getSongIndex(id);
    if (idx == -1) {
        cout << "[" << id << "]";
        return;
    }
    cout << "[" << songsList[idx].data.id << "] "
         << songsList[idx].data.title << " - "
         << songsList[idx].data.artist;
}

// =============================================================================
// REQUIREMENT 1: Recommend related songs (BFS)
// =============================================================================
void MusicGraph::recommendRelatedSongs(const string& startId) const {
    if (!this->hasVertex(startId)) return;

    cout << "\n[1] SUGGEST RELATED SONGS FOR: ";
    printSongInfo(startId);
    cout << "\n-------------------------------------------------\n";

    vector<string> visited;
    vector<string> customQueue;
    int head = 0;

    visited.push_back(startId);
    customQueue.push_back(startId);

    while (head < (int)customQueue.size()) {
        string current = customQueue[head];
        head++;

        vector<Edge> neighbors = this->getNeighbors(current);
        for (size_t i = 0; i < neighbors.size(); ++i) {
            string nextId = neighbors[i].target;
            if (!isVisited(nextId, visited)) {
                visited.push_back(nextId);
                customQueue.push_back(nextId);
                cout << "-> ";
                printSongInfo(nextId);
                cout << "\n";
            }
        }
    }
}

// =============================================================================
// REQUIREMENT 2: Create playlist by clusters (Connected Components using BFS)
// =============================================================================
void MusicGraph::generatePlaylistsByClusters() const {
    cout << "\n[2] CREATE PLAYLIST BY CLUSTERS (CONNECTED COMPONENTS)\n";
    cout << "-------------------------------------------------\n";

    vector<string> globalVisited;
    int playlistCount = 0;

    for (size_t i = 0; i < this->adjList.size(); ++i) {
        string startId = this->adjList[i].vertex;
        if (isVisited(startId, globalVisited)) {
            continue;
        }

        playlistCount++;
        cout << "=== Playlist " << playlistCount << " ===\n";

        vector<string> customQueue;
        int head = 0;

        globalVisited.push_back(startId);
        customQueue.push_back(startId);

        while (head < (int)customQueue.size()) {
            string current = customQueue[head];
            head++;

            cout << "  * "; //space 2c
            printSongInfo(current);
            cout << "\n";

            // Traverse outgoing edges first.
            vector<Edge> outgoing = this->getNeighbors(current);
            for (size_t j = 0; j < outgoing.size(); ++j) {
                string nextId = outgoing[j].target;
                if (!isVisited(nextId, globalVisited)) {
                    globalVisited.push_back(nextId);
                    customQueue.push_back(nextId);
                }
            }

            // Then traverse incoming edges to treat the graph as undirected for clusters.
            for (size_t src = 0; src < this->adjList.size(); ++src) {
                string sourceId = this->adjList[src].vertex;
                vector<Edge> incomingCheck = this->adjList[src].neighbors;
                for (size_t e = 0; e < incomingCheck.size(); ++e) {
                    if (incomingCheck[e].target == current && !isVisited(sourceId, globalVisited)) {
                        globalVisited.push_back(sourceId);
                        customQueue.push_back(sourceId);
                    }
                }
            }
        }
    }
}

// =============================================================================
// REQUIREMENT 3: Smooth song transition (Dijkstra's Algorithm)
// =============================================================================
void MusicGraph::findSmoothTransition(const string& startId, const string& endId) const {
    cout << "\n[3] SMOOTHEST TRANSITION (DIJKSTRA)\n";
    cout << "From: "; printSongInfo(startId); cout << "\n";
    cout << "To: "; printSongInfo(endId); cout << "\n";
    cout << "-------------------------------------------------\n";

    int n = this->adjList.size();
    int startIdx = this->getVertexIndex(startId);
    int endIdx = this->getVertexIndex(endId);

    if (startIdx == -1 || endIdx == -1) {
        cout << "Error: Song not found!\n";
        return;
    }

    const double INF = 999999999.0;
    vector<double> dist(n, INF);
    vector<int> prev(n, -1);
    vector<bool> visited(n, false);

    dist[startIdx] = 0.0;

    for (int step = 0; step < n; ++step) {
        int u = -1;
        double best = INF;

        for (int i = 0; i < n; ++i) {
            if (!visited[i] && dist[i] < best) {
                best = dist[i];
                u = i;
            }
        }

        if (u == -1) {
            break;
        }

        visited[u] = true;

        if (u == endIdx) {
            break;
        }

        for (size_t j = 0; j < this->adjList[u].neighbors.size(); ++j) {
            Edge edge = this->adjList[u].neighbors[j];
            int v = this->getVertexIndex(edge.target);

            if (v != -1 && !visited[v] && dist[u] + edge.weight < dist[v]) {
                dist[v] = dist[u] + edge.weight;
                prev[v] = u;
            }
        }
    }

    if (dist[endIdx] >= INF) {
        cout << "No transition path between these two songs.\n";
        return;
    }

    vector<int> path;
    int current = endIdx;
    while (current != -1) {
        path.push_back(current);
        current = prev[current];
    }

    cout << "-> Total Deviation (Cost): " << dist[endIdx] << "\n";
    cout << "-> Playback Order:\n";

    int order = 1;
    for (int i = (int)path.size() - 1; i >= 0; --i) {
        cout << "   " << order << ". ";  // space 3c
        printSongInfo(this->adjList[path[i]].vertex);
        cout << "\n";
        order++;
    }
}

// =============================================================================
// REQUIREMENT 4: Find the network hub song (In-degree Calculation)
// =============================================================================
void MusicGraph::findMostPopularSong() const {
    cout << "\n[4] FIND NETWORK HUB SONG (IN-DEGREE)\n";
    cout << "-------------------------------------------------\n";

    int n = this->adjList.size();
    if (n == 0) {
        cout << "No songs in the graph.\n";
        return;
    }

    vector<int> inDegree(n, 0);

    for (int i = 0; i < n; ++i) {
        for (size_t j = 0; j < this->adjList[i].neighbors.size(); ++j) {
            int targetIdx = this->getVertexIndex(this->adjList[i].neighbors[j].target);
            if (targetIdx != -1) {
                inDegree[targetIdx]++;
            }
        }
    }

    int maxIdx = 0;
    for (int i = 1; i < n; ++i) {
        if (inDegree[i] > inDegree[maxIdx]) {
            maxIdx = i;
        }
    }

    cout << "-> Network Hub Song:\n   "; //space 3l
    printSongInfo(this->adjList[maxIdx].vertex);
    cout << "\n   (In-degree: " << inDegree[maxIdx] << ")\n"; //space 3l
}

// =============================================================================
// REQUIREMENT 5: Detect music loop (DFS Cycle Detection)
// =============================================================================

// DFS recursive helper function
bool MusicGraph::dfsCycleHelper(int idx, vector<bool>& visited, vector<bool>& recursionStack,
                                 vector<int>& parent, bool& found) const {
    visited[idx] = true;
    recursionStack[idx] = true;

    for (size_t i = 0; i < this->adjList[idx].neighbors.size(); ++i) {
        int nextIdx = this->getVertexIndex(this->adjList[idx].neighbors[i].target);
        if (nextIdx == -1) {
            continue;
        }

        if (!visited[nextIdx]) {
            parent[nextIdx] = idx;
            if (dfsCycleHelper(nextIdx, visited, recursionStack, parent, found)) {
                return true;
            }
        }
        else if (recursionStack[nextIdx]) {
            found = true;

            cout << "-> Music loop detected!\n";
            cout << "-> Loop:\n   ";

            vector<int> cycle;
            int current = idx;
            cycle.push_back(current);

            while (current != nextIdx && parent[current] != -1) {
                current = parent[current];
                cycle.push_back(current);
            }

            for (int j = (int)cycle.size() - 1; j >= 0; --j) {
                printSongInfo(this->adjList[cycle[j]].vertex);
                cout << "\n   ";
            }
            printSongInfo(this->adjList[nextIdx].vertex);
            cout << "\n   ";

            return true;
        }
    }

    recursionStack[idx] = false;
    return false;
}

void MusicGraph::detectMusicLoop() const {
    cout << "\n[5] DETECT MUSIC LOOP (DFS CYCLE DETECTION)\n";
    cout << "-------------------------------------------------\n";

    int n = this->adjList.size();
    vector<bool> visited(n, false);
    vector<bool> recursionStack(n, false);
    vector<int> parent(n, -1);
    bool found = false;

    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            if (dfsCycleHelper(i, visited, recursionStack, parent, found)) {
                break;
            }
        }
    }

    if (!found) {
        cout << "No music loop detected.\n";
    }
}
