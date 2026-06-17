#include "Playlist.h"

// =======================
// Song implementation (TODO)
// =======================

Song::Song(int id,
           string title,
           string artist,
           string album,
           int duration,
           int score,
           string url)
    : id(id),
      title(title),
      artist(artist),
      album(album),
      duration(duration),
      score(score),
      url(url),
      play_count(0)
{
}

std::string Song::toString() const {
    std::ostringstream oss;
    oss << "Song[id=" << id 
        << ", title=\"" << title << "\""
        << ", artist=\"" << artist << "\""
        << ", album=\"" << album << "\""
        << ", duration=" << duration
        << ", score=" << score
        << ", url=\"" << url << "\""
        << ", play_count=" << play_count
        << "]";
    return oss.str();
}

// TODO: Student can implement additional methods for Song here

// =======================
// Playlist implementation (TODO)
// =======================

Playlist::Playlist(string name)
    : name(name), size(0), currentIndex(-1), totalScoreCache(0)
#ifdef USE_THREADED_AVL
    , currentIt(songs.endIt()), hasCurrent(false)
#endif
{
}

SongKey Playlist::makeKey(Song* s) const {
    // TODO
    return SongKey(s->getTitle(), s->getId());
}

void Playlist::resetPlayback() {
    // TODO
    currentIndex = -1;
#ifdef USE_THREADED_AVL
    hasCurrent = false;
    currentIt = songs.endIt();
#endif
}

int Playlist::getSize() const {
    // TODO
    return size;
}

bool Playlist::empty() const {
    // TODO
    return size == 0;
}

void Playlist::clear() {
#ifdef USE_THREADED_AVL
    ThreadedAVL<SongKey, Song*>::Iterator it = songs.beginIt();
    while (!it.isNull()) {
        Song* s = it.value();
        ++it;
        delete s;
    }
    songs.clear();
#else
    while (!songs.empty()) {
        Song** pp = songs.findByIndex(0);
        if (pp == nullptr || *pp == nullptr) break;

        Song* s = *pp;
        SongKey key = makeKey(s);
        songs.erase(key);
        delete s;
    }
#endif

    size = 0;
    totalScoreCache = 0;
    resetPlayback();
}

void Playlist::addSong(Song* s) {
    if (s == nullptr) return;

    Song* currentSong = nullptr;
    if (currentIndex >= 0 && currentIndex < size) {
        currentSong = getSong(currentIndex);
    }

    SongKey key = makeKey(s);
    if (!songs.insert(key, s)) {
        delete s;
        return;
    }

    size++;
    totalScoreCache += s->getScore();

    if (currentSong != nullptr && key < makeKey(currentSong)) {
        currentIndex++;
    }

#ifdef USE_THREADED_AVL
    if (currentSong != nullptr) {
        currentIt = songs.findIt(makeKey(currentSong));
        hasCurrent = !currentIt.isNull();
    }
#endif
}

void Playlist::removeSong(int index) {
    if (index < 0 || index >= size) return;

    Song* s = getSong(index);
    if (s == nullptr) return;

    SongKey key = makeKey(s);
    bool deletingCurrent = (currentIndex == index);
    int oldCurrent = currentIndex;

    totalScoreCache -= s->getScore();

    if (!songs.erase(key)) {
        totalScoreCache += s->getScore();
        return;
    }

    delete s;
    size--;

    if (size == 0) {
        resetPlayback();
        return;
    }

    if (oldCurrent == -1) return;

    if (deletingCurrent) {
        int nextIndex = (index >= size) ? 0 : index;
        playRandom(nextIndex);
    }
    else if (index < oldCurrent) {
        currentIndex--;

#ifdef USE_THREADED_AVL
        playRandom(currentIndex);
#endif
    }
}

Song* Playlist::getSong(int index) const {
    if (index < 0 || index >= size) return nullptr;

    Song** pp = songs.findByIndex(index);
    if (pp == nullptr) return nullptr;
    return *pp;
}

// =======================
// Playing control (TODO)
// =======================

Song* Playlist::playNext() {
    if (empty()) return nullptr;

#ifdef USE_THREADED_AVL
    if (!hasCurrent) {
        currentIt = songs.beginIt();
        if (currentIt.isNull()) return nullptr;
        hasCurrent = true;
        currentIndex = 0;
        return currentIt.value();
    }

    ++currentIt;
    ++currentIndex;

    if (currentIt.isNull() || currentIndex >= size) {
        currentIt = songs.beginIt();
        currentIndex = 0;
    }

    return currentIt.value();
#else
    if (currentIndex == -1) {
        currentIndex = 0;
    } else {
        currentIndex = (currentIndex + 1) % size;
    }

    return getSong(currentIndex);
#endif
}

Song* Playlist::playPrevious() {
    if (empty()) return nullptr;

#ifdef USE_THREADED_AVL
    if (!hasCurrent) {
        currentIt = songs.rbeginIt();
        if (currentIt.isNull()) return nullptr;
        hasCurrent = true;
        currentIndex = size - 1;
        return currentIt.value();
    }

    --currentIt;
    --currentIndex;

    if (currentIt.isNull() || currentIndex < 0) {
        currentIt = songs.rbeginIt();
        currentIndex = size - 1;
    }

    return currentIt.value();
#else
    if (currentIndex == -1) {
        currentIndex = size - 1;
    } else {
        currentIndex = (currentIndex - 1 + size) % size;
    }

    return getSong(currentIndex);
#endif
}

// =======================
// Score-related (TODO)
// =======================

int Playlist::getTotalScore() {
    return totalScoreCache;
}

bool Playlist::compareTo(const Playlist& p, int numSong) {
    if (numSong <= 0) return true;

    int limitThis = (numSong < this->size) ? numSong : this->size;
    int limitOther = (numSong < p.size) ? numSong : p.size;

    int scoreThis = 0;
    int scoreOther = 0;

#ifdef USE_THREADED_AVL
    ThreadedAVL<SongKey, Song*>::Iterator it1 = songs.beginIt();
    for (int i = 0; i < limitThis && !it1.isNull(); ++i, ++it1) {
        Song* s = it1.value();
        if (s != nullptr) scoreThis += s->getScore();
    }

    ThreadedAVL<SongKey, Song*>::Iterator it2 = p.songs.beginIt();
    for (int i = 0; i < limitOther && !it2.isNull(); ++i, ++it2) {
        Song* s = it2.value();
        if (s != nullptr) scoreOther += s->getScore();
    }
#else
    for (int i = 0; i < limitThis; i++) {
        Song* s = this->getSong(i);
        if (s != nullptr) scoreThis += s->getScore();
    }

    for (int i = 0; i < limitOther; i++) {
        Song* s = p.getSong(i);
        if (s != nullptr) scoreOther += s->getScore();
    }
#endif

    return scoreThis >= scoreOther;
}

// =======================
// Advanced playing modes (TODO)
// =======================

void Playlist::playRandom(int index) {
    if (index < 0 || index >= size) return;

    currentIndex = index;

#ifdef USE_THREADED_AVL
    Song* s = getSong(index);
    if (s == nullptr) {
        resetPlayback();
        return;
    }

    currentIt = songs.findIt(makeKey(s));
    hasCurrent = !currentIt.isNull();
#endif
}

int Playlist::playApproximate(int step) {
    if (empty()) return -1;

    int start = (currentIndex == -1) ? 0 : currentIndex;
    int target = (start + step) % size;
    if (target < 0) target += size;

    playRandom(target);
    return currentIndex;
}