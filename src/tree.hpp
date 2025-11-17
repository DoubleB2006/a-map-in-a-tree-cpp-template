#ifndef TREEMAP_HPP
#define TREEMAP_HPP

#include <string>

// =======================
// KeyValuePair
// =======================

class KeyValuePair {
public:
    std::string key;
    std::string value;

    KeyValuePair() = default;

    KeyValuePair(const std::string& k, const std::string& v = "")
        : key(k), value(v) {}

    // Compare ONLY by key
    bool operator<(const KeyValuePair& other) const {
        return key < other.key;
    }
    bool operator>(const KeyValuePair& other) const {
        return key > other.key;
    }
    bool operator<=(const KeyValuePair& other) const {
        return key <= other.key;
    }
    bool operator>=(const KeyValuePair& other) const {
        return key >= other.key;
    }
    bool operator==(const KeyValuePair& other) const {
        return key == other.key;
    }
};

// =======================
// SplayTree<T>
// =======================

template <typename T>
class SplayTree {
private:
    struct Node {
        T data;
        Node* left;
        Node* right;
        Node* parent;

        Node(const T& d)
            : data(d), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    Node* root = nullptr;

    // Allow TreeMap to see Node when it uses findNode(...)
    template <typename U>
    friend class SplayTree; // (needed by template rules, harmless)
    friend class TreeMap;

    // ---- rotations ----

    void rotateLeft(Node* x) {
        Node* y = x->right;
        if (!y) return;

        x->right = y->left;
        if (y->left)
            y->left->parent = x;

        y->parent = x->parent;

        if (!x->parent)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node* x) {
        Node* y = x->left;
        if (!y) return;

        x->left = y->right;
        if (y->right)
            y->right->parent = x;

        y->parent = x->parent;

        if (!x->parent)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->right = x;
        x->parent = y;
    }

    void splay(Node* x) {
        if (!x) return;

        while (x->parent) {
            Node* p = x->parent;
            Node* g = p->parent;

            if (!g) {
                // Zig
                if (x == p->left)
                    rotateRight(p);
                else
                    rotateLeft(p);
            } else if ((x == p->left && p == g->left) ||
                       (x == p->right && p == g->right)) {
                // Zig-zig
                if (x == p->left) {
                    rotateRight(g);
                    rotateRight(p);
                } else {
                    rotateLeft(g);
                    rotateLeft(p);
                }
            } else {
                // Zig-zag
                if (x == p->left) {
                    rotateRight(p);
                    rotateLeft(g);
                } else {
                    rotateLeft(p);
                    rotateRight(g);
                }
            }
        }
    }

    // ---- helpers ----

    Node* subtreeMin(Node* x) const {
        if (!x) return nullptr;
        while (x->left)
            x = x->left;
        return x;
    }

    void replaceNode(Node* u, Node* v) {
        if (!u->parent)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;

        if (v)
            v->parent = u->parent;
    }

    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    SplayTree() = default;

    ~SplayTree() {
        clear(root);
    }

    // Insert: BST insert + splay inserted node
    void insert(const T& value) {
        if (!root) {
            root = new Node(value);
            return;
        }

        Node* cur = root;
        Node* parent = nullptr;

        while (cur) {
            parent = cur;
            if (value < cur->data)
                cur = cur->left;
            else if (value > cur->data)
                cur = cur->right;
            else {
                // Equal key: replace data, splay existing node
                cur->data = value;
                splay(cur);
                return;
            }
        }

        Node* newNode = new Node(value);
        newNode->parent = parent;

        if (value < parent->data)
            parent->left = newNode;
        else
            parent->right = newNode;

        splay(newNode);
    }

    // Find node with given value; splay last accessed
    Node* findNode(const T& value) {
        Node* cur = root;
        Node* last = nullptr;

        while (cur) {
            last = cur;
            if (value < cur->data)
                cur = cur->left;
            else if (value > cur->data)
                cur = cur->right;
            else {
                // Found
                splay(cur);
                return cur;
            }
        }

        if (last)
            splay(last);
        return nullptr;
    }

    bool contains(const T& value) {
        return findNode(value) != nullptr;
    }

    // Remove value if present
    void erase(const T& value) {
        Node* node = findNode(value);
        if (!node) return;

        splay(node); // should already be root

        if (!node->left) {
            replaceNode(node, node->right);
        } else if (!node->right) {
            replaceNode(node, node->left);
        } else {
            Node* minRight = subtreeMin(node->right);
            if (minRight->parent != node) {
                replaceNode(minRight, minRight->right);
                minRight->right = node->right;
                minRight->right->parent = minRight;
            }
            replaceNode(node, minRight);
            minRight->left = node->left;
            minRight->left->parent = minRight;
        }

        delete node;
    }

    const T* rootData() const {
        return root ? &root->data : nullptr;
    }
};

// =======================
// TreeMap
// =======================

class TreeMap {
private:
    SplayTree<KeyValuePair> tree;

public:
    TreeMap() = default;

    // Insert or update
    void insert(const std::string& key, const std::string& value) {
        KeyValuePair kv(key, value);
        tree.insert(kv);
    }

    // Get value for key, or "" if not found
    std::string get(const std::string& key) {
        KeyValuePair searchKey(key);
        auto* node = tree.findNode(searchKey);

        if (node && node->data.key == key) {
            return node->data.value;
        }
        return ""; // default if not found
    }

    // Delete key if present
    void deleteKey(const std::string& key) {
        KeyValuePair searchKey(key);
        tree.erase(searchKey);
    }
};

#endif // TREEMAP_HPP
