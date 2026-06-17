#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "main.h"
#include "BotkifyLinkedList.h"

// =======================
// Class Song
// =======================
class Song
{
#ifdef TESTING
    friend class TestHelper;
#endif

private:
    int id;
    string title;
    string artist;
    string album;
    int duration;
    int score;
    string url;

public:
    Song(int id,
         string title,
         string artist,
         string album,
         int duration,
         int score,
         string url);

    int getID() const;
    int getDuration() const;
    int getScore() const;

    string toString() const;

    bool operator==(const Song& other) const; //overload cho Song

};

// =======================
// Class Playlist
// =======================
class Playlist
{
#ifdef TESTING
    friend class TestHelper;
#endif

private:
    string name;
    BotkifyLinkedList<Song*> lstSong;
    int size;
    //typename -> a class/struct not a function/method/variable
    typename BotkifyLinkedList<Song*>::Node* currentSong;

public:
    Playlist(string name);

    int getSize() const;
    bool empty() const;
    void clear();

    void addSong(Song* s);
    void removeSong(int index);
    Song* getSong(int index) const;

    Song* playNext();
    Song* playPrevious();

    int getTotalScore();
    bool compareTo(Playlist p, int numSong);

    void playRandom(int index);
    int playApproximate(int step);
};

#endif // PLAYLIST_H
