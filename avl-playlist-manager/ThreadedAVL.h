#ifndef THREADED_AVL_H
#define THREADED_AVL_H

#include "AVL.h"

// =======================
// ThreadedAVL (SKELETON - BONUS)
// =======================
// TODO (bonus):
// - Add inorder threading (prev/next) to support O(1) next/previous navigation.
// - ThreadedAVL inherits AVL and uses hooks to maintain threads.
// - Rotation does NOT change inorder order, so threading is updated mainly in insert/erase hooks.

template <typename K, typename V>
class ThreadedAVL : public AVL<K, V> {
private:
    using Base = AVL<K, V>;
    using AVLNode = typename Base::Node;

    struct TNode : public AVLNode {
        TNode* prev;
        TNode* next;

        TNode(const K& k, const V& v)
            : AVLNode(k, v), prev(nullptr), next(nullptr) {}
    };

    TNode* head_; // smallest
    TNode* tail_; // largest

public:
    class Iterator {
        friend class ThreadedAVL<K, V>;
        TNode* p_;
        explicit Iterator(TNode* p) : p_(p) {}
    public:
        Iterator() : p_(nullptr) {}

        bool operator==(const Iterator& other) const { return p_ == other.p_; }
        bool operator!=(const Iterator& other) const { return p_ != other.p_; }

        // TODO: O(1) move using next/prev
        Iterator& operator++() { if (p_) p_ = p_->next; return *this; }
        Iterator& operator--() { if (p_) p_ = p_->prev; return *this; }

        const K& key() const { return p_->key; }
        V& value() { return p_->value; }
        const V& value() const { return p_->value; }
        bool isNull() const { return p_ == nullptr; }
    };

public:
    ThreadedAVL() : Base(), head_(nullptr), tail_(nullptr) {}
    ~ThreadedAVL() override { this->clear(); }

    // TODO: These should traverse via threading (O(n) but very fast in practice)
    
    TNode* getHead() const {
        return head_;
    }

     TNode* getTail() const {
        return tail_;
    }

    std::list<K> ascendingList() const override {
        // TODO
        std::list<K> out;
        TNode* cur = head_;
        while (cur != nullptr) {
            out.push_back(cur->key);
            cur = cur->next;
        }
        return out;

    }

    std::list<K> descendingList() const override {
        // TODO
        std::list<K> out;
        TNode* cur = tail_;
        while (cur != nullptr) {
            out.push_back(cur->key);
            cur = cur->prev;
        }

        return out;
    }

    void clear() override {
        // TODO: call Base::clear() + reset head_/tail_
        Base::clear();
        head_ = tail_ = nullptr;
    }

    // Iteration helpers
    Iterator beginIt() const { return Iterator(head_); }
    Iterator endIt() const { return Iterator(nullptr); }
    Iterator rbeginIt() const { return Iterator(tail_); }

    Iterator findIt(const K& key) {
        // TODO: return iterator to node by key
        AVLNode* cur = this->pRoot;

        while (cur != nullptr) {
            if (key == cur->key) {
                return Iterator(static_cast<TNode*>(cur));
            }
            else if (key < cur->key) {
                cur = cur->pLeft;
            }
            else {
                cur = cur->pRight;
            }
        }

        return Iterator(nullptr);

    }

protected:
    // TODO: allocate a TNode instead of Base::Node
    AVLNode* createNode(const K& key, const V& value) override {
        // TODO
        return new TNode(key, value);
    }

    // TODO: maintain threading on insertion
    void onInserted(AVLNode* newNode, AVLNode* pred, AVLNode* succ) override {
        TNode* n = static_cast<TNode*>(newNode);
        TNode* p = static_cast<TNode*>(pred);
        TNode* s = static_cast<TNode*>(succ);

        n->prev = p;
        n->next = s;

        if (p != nullptr) p->next = n;
        else head_ = n;

        if (s != nullptr) s->prev = n;
        else tail_ = n;
    }

    void onErasing(AVLNode* delNode, AVLNode* pred, AVLNode* succ) override {
        TNode* d = static_cast<TNode*>(delNode);
        TNode* p = static_cast<TNode*>(pred);
        TNode* s = static_cast<TNode*>(succ);

        (void)d;

        if (p != nullptr) p->next = s;
        else head_ = s;

        if (s != nullptr) s->prev = p;
        else tail_ = p;
    }

    void onReplaceBySuccessor(AVLNode* target, AVLNode* successor, AVLNode* successorNext) override {
        TNode* t = static_cast<TNode*>(target);
        TNode* s = static_cast<TNode*>(successor);
        TNode* sn = static_cast<TNode*>(successorNext);

        (void)s;

        t->next = sn;

        if (sn != nullptr) sn->prev = t;
        else tail_ = t;
    }
};

#endif /* THREADED_AVL_H */
