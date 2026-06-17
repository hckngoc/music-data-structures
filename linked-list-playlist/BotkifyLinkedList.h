#ifndef BOTKIFY_LINKED_LIST_H
#define BOTKIFY_LINKED_LIST_H

#include "main.h"

template <class T>
class BotkifyLinkedList
{
    friend class Playlist; //for playlist to access 

private:
    class Node //double linked list
    {
    public:
        T data;
        Node* next;
        Node* extra;

        Node() : next(nullptr), extra(nullptr){}
        Node(const T& data, Node* next = nullptr, Node* extra = nullptr) : data(data), next(next), extra(extra) {}
    };

    Node* head;
    Node* tail;
    int count;

public:
    BotkifyLinkedList(){
        head = nullptr;
        tail = nullptr;
        count = 0;
    };
    ~BotkifyLinkedList(){
        clear();
    };

    void add(T e){
        Node* newsong = new Node(e);
        if (head == nullptr){ //empty list
            head = newsong;
            //head->next = nullptr; constructor already did this
            tail = head;
            //tail->next = nullptr;
        } else {
            tail->next = newsong;
            newsong->extra = tail;
            tail = newsong;
        }
        count++;
    };
    void add(int index, T e){
        if (index < 0 || index > count)
            throw std::out_of_range("Index is invalid!");
        
        Node* newsong = new Node(e);
        if (index == 0){ //add at 1st position
            if (head == nullptr){ //empty list
                head = newsong;
                tail = head;
            } else {
                newsong->next = head;
                head->extra = newsong;
                head = newsong;
            }
            count++;
            return;
        }

        if (index == count) { //add at last position
            add(e);
            return;
        } 

        Node* current = head;
        for (int i = 0; i < index - 1; i++){
            current = current->next;
        }
        /* index = 2
        i = 0; current = no.1;
        i = 1; stop
        after loop, current is at (index - 1) position
        */    
        newsong->next = current->next;
        newsong->extra = current;
        current->next->extra = newsong;
        current->next = newsong;
        count++;
    };
    T removeAt(int index){
    if (index < 0 || index >= count)
        throw std::out_of_range("Index is invalid!");

    Node* del = head;

    // đi tới node cần xóa
    for (int i = 0; i < index; i++){
        del = del->next;
    }

    T value = del->data;

    // nối prev
    if (del->extra != nullptr)
        del->extra->next = del->next;
    else
        head = del->next;   // del là head

    // nối next
    if (del->next != nullptr)
        del->next->extra = del->extra;
    else
        tail = del->extra;  // del là tail

    delete del;
    count--;

    return value;
    }
    bool removeItem(T item){
    Node* current = head;

    while (current != nullptr){
        if (current->data == item){

            // nối prev
            if (current->extra != nullptr)
                current->extra->next = current->next;
            else
                head = current->next;

            // nối next
            if (current->next != nullptr)
                current->next->extra = current->extra;
            else
                tail = current->extra;

            delete current;
            count--;
            return true;
        }
        current = current->next;
    }

    return false;
    }

    bool empty() const{
        if (count == 0) return true;
        else return false;
    };
    int size() const{
        return count;
    };
    void clear(){
        while (head != nullptr){
            Node* temp = head;
            head = head->next;
            delete temp;   
        }
        tail = nullptr;
        count = 0;
    };

    T& get(int index) const{
        if (index < 0 || index >= count)
            throw std::out_of_range("Index is invalid!"); //error warning and out

        Node* find = head;
        for (int i = 0; i < index; i++){
            find = find->next;
        } 
        return find->data;
    };

    string toString() const{
        ostringstream s;

        Node* current = head;
        while (current != nullptr){
            s << current->data;

            if (current->next != nullptr) s << ',';

            current = current->next;
        }
        return s.str();
    };
};

#endif // BOTKIFY_LINKED_LIST_H
