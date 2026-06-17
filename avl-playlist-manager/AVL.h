#ifndef AVL_H
#define AVL_H

#include "IBST.h"

#include <sstream>

template <typename K, typename V>
class AVL : public IBST<K, V> {
public:
    struct Node {
        K key; 
        V value;

        int bfactor; 
        int height;
        int subtreeSize;

        Node* pLeft;
        Node* pRight;

        Node(const K& k, const V& v)
            : key(k), value(v), bfactor(0), height(1), subtreeSize(1),
              pLeft(nullptr), pRight(nullptr) {}

        virtual ~Node() = default;

        int balance() const { return bfactor; }

        // Default formatter must compile for any K,V.

        static std::string defaultEntry2Str(const K&, const V&) {
            return "<key, value>";
        }

        std::string toString(std::string (*entry2str)(const K&, const V&) = nullptr) const {
            auto entryStr = [&](const K& kk, const V& vv) -> std::string {
                return entry2str ? entry2str(kk, vv) : defaultEntry2Str(kk, vv);
            };

            std::ostringstream out;
            // AVL luôn in kèm balance cho debug
            if (!pLeft && !pRight) {
                out << "[" << entryStr(key, value) << ":" << balance() << "]";
            }
            else if (!pLeft && pRight) {
                out << " (" << entryStr(key, value) << ":" << balance()
                    << "[.]" << pRight->toString(entry2str) << ")";
            }
            else if (pLeft && !pRight) {
                out << " (" << entryStr(key, value) << ":" << balance()
                    << pLeft->toString(entry2str) << "[.]" << ")";
            }
            else {
                out << " (" << entryStr(key, value) << ":" << balance()
                    << pLeft->toString(entry2str)
                    << pRight->toString(entry2str)
                    << ") ";
            }
            return out.str();
        }
    };

protected:
    Node* pRoot;

protected:
    // =======================
    // Hooks for ThreadedAVL
    // =======================
    // Base AVL does nothing. ThreadedAVL overrides these to maintain prev/next threading.

    virtual void onInserted(Node* /*newNode*/, Node* /*pred*/, Node* /*succ*/) {}
    virtual void onErasing(Node* /*delNode*/, Node* /*pred*/, Node* /*succ*/) {}
    virtual void onReplaceBySuccessor(Node* /*target*/, Node* /*successor*/, Node* /*successorNext*/) {}

    // Factory for subclasses to create extended node types
    virtual Node* createNode(const K& key, const V& value) { return new Node(key, value); }
    
    // ===== basic helpers =====
    int heightOf(Node* node) const {
        // TODO
        return node ? node->height : 0;
    }

    int sizeOf(Node* node) const {
        return node ? node->subtreeSize : 0;
    }

    void clearRec(Node*& node) {
        // TODO
        if (node == nullptr) return;
        clearRec(node->pLeft);
        clearRec(node->pRight);
        delete node;
        node = nullptr;
    }

    void inorderRec(Node* node, std::list<K>& out) const {
        // TODO
        if (node == nullptr) return;
        inorderRec(node->pLeft, out);
        out.push_back(node->key);
        inorderRec(node->pRight, out);
    }

    void revInorderRec(Node* node, std::list<K>& out) const {
        // TODO
        if (node == nullptr) return;
        revInorderRec(node->pRight, out);
        out.push_back(node->key);
        revInorderRec(node->pLeft, out);
    }

    Node* findMin(Node* node) const {
        // TODO
        if (node == nullptr) return nullptr;
        while (node->pLeft != nullptr) node = node->pLeft;
        return node;
    }

    Node* findMax(Node* node) const {
        // TODO
        if (node == nullptr) return nullptr;
        while (node->pRight != nullptr) node = node->pRight;
        return node;
    }

    Node* predecessorOf(const K& key) const {
        Node* cur = pRoot;
        Node* pred = nullptr;

        while (cur != nullptr) {
            if (key <= cur->key) {
                cur = cur->pLeft;
            } else {
                pred = cur;
                cur = cur->pRight;
            }
        }
        return pred;
    }

    Node* successorOf(const K& key) const {
        Node* cur = pRoot;
        Node* succ = nullptr;

        while (cur != nullptr) {
            if (key < cur->key) {
                succ = cur;
                cur = cur->pLeft;
            } else {
                cur = cur->pRight;
            }
        }
        return succ;
    }

    // ===== AVL helpers =====
    void updateBalance(Node* node) {
        if (node == nullptr) return;

        int hl = heightOf(node->pLeft);
        int hr = heightOf(node->pRight);

        //update height
        node->height = 1 + (hl > hr ? hl : hr);

        //update bfactor
        node->bfactor = hl - hr;

        //update subtree size
        node->subtreeSize = 1 + sizeOf(node->pLeft) + sizeOf(node->pRight);
    }

    Node* rotateLeft(Node* x) {
        // TODO
        Node* y = x->pRight; //new root
        Node* child = y->pLeft; //become new right child of x

        y->pLeft = x;
        x->pRight = child;

        updateBalance(x);
        updateBalance(y);

        return y;
    }

    Node* rotateRight(Node* y) {
        // TODO
        Node* x = y->pLeft;
        Node* child = x->pRight;

        x->pRight = y;
        y->pLeft = child;

        updateBalance(y);
        updateBalance(x);

        return x;
    }

    Node* rebalance(Node* node) {
        // TODO
        if (node == nullptr) return node;

        updateBalance(node);

        if (node->bfactor > 1) {
            if (node->pLeft != nullptr) updateBalance(node->pLeft);

            if (node->pLeft != nullptr && node->pLeft->bfactor < 0) {
                node->pLeft = rotateLeft(node->pLeft);
            }
            return rotateRight(node);
        }

        if (node->bfactor < -1) {
            if (node->pRight != nullptr) updateBalance(node->pRight);

            if (node->pRight != nullptr && node->pRight->bfactor > 0) {
                node->pRight = rotateRight(node->pRight);
            }
            return rotateLeft(node);
        }

        return node;
    }

    // ===== recursive core =====
    Node* insertRec(
        Node* node,
        const K& key,
        const V& value,
        bool& inserted,
        Node*& newNode,
        Node*& pred,
        Node*& succ
    ) {
        // TODO
        if (node == nullptr) {
            inserted = true;
            newNode = createNode(key, value);
            return newNode;
        }

        if (key == node->key) {
            inserted = false;
            return node;
        }

        if (key < node->key) {
            succ = node;
            node->pLeft = insertRec(node->pLeft, key, value, inserted, newNode, pred, succ);
        } else {
            pred = node;
            node->pRight = insertRec(node->pRight, key, value, inserted, newNode, pred, succ);
        }

        if (inserted == true) return rebalance(node);

        updateBalance(node);
        return node;
    }

    Node* eraseRec(
        Node* node,
        const K& key,
        bool& erased,
        bool fireEraseHook = true
    ) {
        if (node == nullptr) return nullptr;

        if (key < node->key) {
            node->pLeft = eraseRec(node->pLeft, key, erased, fireEraseHook);
        }
        else if (key > node->key) {
            node->pRight = eraseRec(node->pRight, key, erased, fireEraseHook);
        }
        else {
            erased = true;

            if (node->pLeft == nullptr || node->pRight == nullptr) {
                if (fireEraseHook) {
                    Node* pred = predecessorOf(node->key);
                    Node* succ = successorOf(node->key);
                    onErasing(node, pred, succ);
                }

                Node* child = (node->pLeft != nullptr) ? node->pLeft : node->pRight;
                delete node;
                return child;
            }

            Node* succ = findMin(node->pRight);
            Node* succNext = successorOf(succ->key);

            onReplaceBySuccessor(node, succ, succNext);

            node->key = succ->key;
            node->value = succ->value;

            bool dummy = false;
            node->pRight = eraseRec(node->pRight, succ->key, dummy, false);
        }

        return rebalance(node);
    }

    Node* findNodeByIndexRec(Node* node, int index) const {
        if (node == nullptr) return nullptr;
        if (index < 0 || index >= sizeOf(node)) return nullptr;

        int leftSize = sizeOf(node->pLeft);

        if (index < leftSize) return findNodeByIndexRec(node->pLeft, index);
        if (index > leftSize) return findNodeByIndexRec(node->pRight, index - leftSize - 1);
        return node;
    }
  
public:
    AVL() : pRoot(nullptr) {}
    virtual ~AVL() { clear(); }

    // =======================
    // IBST implementation (TODO)
    // =======================

    bool insert(const K& key, const V& value) override {
        // TODO
        bool inserted = false;
        Node* newNode = nullptr;
        Node* pred = nullptr;
        Node* succ = nullptr;

        pRoot = insertRec(pRoot, key, value, inserted, newNode, pred, succ);

        if (inserted) onInserted(newNode, pred, succ);

        return inserted;
    }

    bool erase(const K& key) override {
        // TODO
        bool erased = false;
        pRoot = eraseRec(pRoot, key, erased);
        return erased;
    }

    V* find(const K& key) override {
        // TODO
        Node* cur = pRoot;

        while(cur != nullptr){
            if (key == cur->key) return &(cur->value); //return pointer to type V value (address)
            else if (key < cur->key) cur = cur->pLeft;
            else cur = cur->pRight;
        }

        return nullptr;
    }

    bool contains(const K& key) const override {
        // TODO
        Node* cur = pRoot;

        while(cur != nullptr){
            if (key == cur->key) return true; //return pointer to type V value (address)
            else if (key < cur->key) cur = cur->pLeft;
            else cur = cur->pRight;
        }

        return false;
    }

    int size() const override {
        // TODO
        return sizeOf(pRoot);
    }

    bool empty() const override {
        // TODO
        // tree empty or not?
        return pRoot == nullptr;
    }

    void clear() override {
        // TODO
        clearRec(pRoot);
        pRoot = nullptr;
    }

    int height() const override {
        // TODO
        return heightOf(pRoot);
    }

    std::list<K> ascendingList() const override {
        // TODO
        std::list<K> out;
        inorderRec(pRoot, out);
        return out;
    }

    std::list<K> descendingList() const override {
        // TODO
        std::list<K> out;
        revInorderRec(pRoot, out);
        return out;
    }

    V* findByIndex(int index) const {
        Node* node = findNodeByIndexRec(pRoot, index);
        return node ? &(node->value) : nullptr;
    }

    std::string toString(std::string (*entry2str)(const K&, const V&) = nullptr) const override {
        if (!pRoot) return "(NULL)";
        return pRoot->toString(entry2str);
    }
};

#endif /* AVL_H */
