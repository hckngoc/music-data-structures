#include "Playlist.h"

// =======================
// Song implementation
// =======================

Song::Song(int id,
           string title,
           string artist,
           string album,
           int duration,
           int score,
           string url) 
    :   id(id), 
        title(title),
        artist(artist),
        album(album),
        duration(duration),
        score(score),
        url(url)
{
    // TODO: Student implementation
}

int Song::getID() const
{
    // TODO: Student implementation
    return id;
}

int Song::getDuration() const
{
    // TODO: Student implementation
    return duration;
}

int Song::getScore() const
{
    // TODO: Student implementation
    return score;
}

string Song::toString() const
{
    // TODO: Student implementation
    return title + "-" + artist;
}

//overload cho Song
bool Song::operator==(const Song& other) const {
    return id == other.id;
}

// =======================
// Playlist implementation
// =======================

Playlist::Playlist(string name) : name(name), size(0), currentSong(nullptr)
{
    // TODO: Student implementation
}

int Playlist::getSize() const
{
    // TODO: Student implementation
    return size;
}

bool Playlist::empty() const
{
    // TODO: Student implementation
    if (size == 0) return true;
    return false;
}

void Playlist::clear()
{
    // TODO: Student implementation
    lstSong.clear();
    size = 0;
    currentSong = nullptr;
}

void Playlist::addSong(Song* s)
{
    // TODO: Student implementation
    lstSong.add(s);
    size++;

    if (size == 1) {
        currentSong = lstSong.head;
    }
}

void Playlist::removeSong(int index)
{
    // TODO: Student implementation
    if (index < 0 || index >= size)
            throw std::out_of_range("Index is invalid!");

    typename BotkifyLinkedList<Song*>::Node* delSong = lstSong.head;
    for (int i = 0; i < index; i++){
        delSong = delSong->next;
    }

    typename BotkifyLinkedList<Song*>::Node* nextSong = delSong->next;
    bool check = (delSong == currentSong);

    lstSong.removeAt(index);
    size--;

    if (size == 0) {
        currentSong = nullptr;
        return;
    }

    //if we remove the current song
    if (check){
        if (nextSong != nullptr){
            currentSong = nextSong;
        } else { //detelete tail
            currentSong = lstSong.head; 
        }
    }
}

Song* Playlist::getSong(int index) const
{
    // TODO: Student implementation
    if (index < 0 || index >= size)
            throw std::out_of_range("Index is invalid!");
    Song* curSong = lstSong.get(index);
    return curSong;
}

// =======================
// Playing control
// =======================

Song* Playlist::playNext()
{
    // TODO: Student implementation
    if (size == 0) 
        throw std::out_of_range("Index is invalid!");
    
    if (size == 1)
        return currentSong->data;

    if (currentSong == lstSong.tail) currentSong = lstSong.head; //last song -> first song
    else currentSong = currentSong->next;

    return currentSong->data;
}

Song* Playlist::playPrevious()
{
    // TODO: Student implementation
    if (size == 0) 
        throw std::out_of_range("Index is invalid!");
    
    if (size == 1)
        return currentSong->data;

    if (currentSong == lstSong.head) currentSong = lstSong.tail; //first song -> last song
    else currentSong = currentSong->extra;

    return currentSong->data;
}

// =======================
// Score-related
// =======================

int Playlist::getTotalScore()
{
    // TODO: Student implementation





    return 0;
}

double computeAvg(Playlist& pl, int numSong) {

    int n = pl.getSize();
    if (n < numSong || numSong <= 0)
        return 0;

    long long sumMax = 0;

    typename BotkifyLinkedList<Song*>::Node* cur = pl.lstSong.head;
    typename BotkifyLinkedList<Song*>::Node* windowStart = pl.lstSong.head;

    DQNode* dqHead = nullptr;
    DQNode* dqTail = nullptr;

    int index = 0;

    while (cur != nullptr) {

        // 1️⃣ Remove elements out of window
        if (index >= numSong) {
            if (dqHead != nullptr && dqHead->data == windowStart) {
                DQNode* temp = dqHead;
                dqHead = dqHead->next;
                if (dqHead) dqHead->prev = nullptr;
                else dqTail = nullptr;
                delete temp;
            }
            windowStart = windowStart->next;
        }

        // 2️⃣ Maintain decreasing order
        while (dqTail != nullptr &&
               dqTail->data->data->getScore() <= cur->data->getScore()) {

            DQNode* temp = dqTail;
            dqTail = dqTail->prev;
            if (dqTail) dqTail->next = nullptr;
            else dqHead = nullptr;
            delete temp;
        }

        // 3️⃣ Push current node
        DQNode* newNode = new DQNode(cur);
        if (dqTail == nullptr) {
            dqHead = dqTail = newNode;
        } else {
            dqTail->next = newNode;
            newNode->prev = dqTail;
            dqTail = newNode;
        }

        // 4️⃣ If window valid, add max
        if (index >= numSong - 1) {
            sumMax += dqHead->data->data->getScore();
        }

        cur = cur->next;
        index++;
    }

    // clean remaining deque nodes
    while (dqHead != nullptr) {
        DQNode* temp = dqHead;
        dqHead = dqHead->next;
        delete temp;
    }

    int totalWindow = n - numSong + 1;
    return (double)sumMax / totalWindow;
}

bool Playlist::compareTo(Playlist p, int numSong)
{
    // TODO: Student implementation
    double avg1 = computeAvg(*this, numSong);
    double avg2 = computeAvg(p, numSong);

    return avg1 >= avg2;
}

// =======================
// Advanced playing modes
// =======================

void Playlist::playRandom(int index)
{
    // TODO: Student implementation
    int n = size;

    Song** songs = new Song*[n];
    int* dur = new int[n];

    typename BotkifyLinkedList<Song*>::Node* cur = lstSong.head;
    int i = 0;
    while (cur != nullptr){
        songs[i] = cur->data;
        dur[i] = cur->data->getDuration();
        cur = cur->next;
        i++;
    }

    int* left = new int[n];
    int* right = new int[n];
    int* nxt = new int[n];
    for (int k = 0; k < n; k++) left[k] = right[k] = nxt[k] = -1;

    int* st = new int[n];
    // left
    int top = -1;
    for (int k = 0; k < n; k++) {
        while (top >= 0 && dur[st[top]] <= dur[k]) top--;
        left[k] = (top >= 0 ? st[top] : -1);
        st[++top] = k;
    }

    // right
    top = -1;
    for (int k = n - 1; k >= 0; k--) {
        while (top >= 0 && dur[st[top]] <= dur[k]) top--;
        right[k] = (top >= 0 ? st[top] : -1);
        st[++top] = k;
    }

    for (int k = 0; k < n; k++) {
        int L = left[k], R = right[k];
        if (L == -1 && R == -1) nxt[k] = -1;
        else if (L == -1) nxt[k] = R;
        else if (R == -1) nxt[k] = L;
        else {
            int dl = k - L;
            int dr = R - k;
            if (dl < dr) nxt[k] = L;
            else if (dr < dl) nxt[k] = R;
        }
    }

    bool firstOut = true;
    int cur = index;
    while (cur != -1) {
        if (!firstOut) std::cout << ",";
        firstOut = false;
        std::cout << songs[cur]->toString();
        cur = nxt[cur];
    }

    delete[] st;
    delete[] left;
    delete[] right;
    delete[] nxt;
    delete[] songs;
    delete[] dur;
}

int Playlist::playApproximate(int step)
{
    // TODO: Student implementation
    
    return 0;
}
