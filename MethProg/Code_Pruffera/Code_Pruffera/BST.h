#pragma once
#include <optional>
#include <stack>
#include <random>

template<typename KeyType, typename ValueType>
class BST {
 public:
    BST() : root(nullptr) { }

    BST(const BST<KeyType, ValueType>& other) {
        if (other.root == nullptr) return;

        std::stack<std::pair<Node*, const Node*>> nodes;

        root = new Node(other.root->key, other.root->value);
        nodes.push({root, other.root});

        while (nodes.size()) {
            auto [current_copy, current_other] = nodes.top();
            nodes.pop();

            if (current_other->right != nullptr) {
                current_copy->right = new Node(current_other->right->key, current_other->right->value);
                nodes.push({current_copy->right, current_other->right});
            }

            if (current_other->left != nullptr) {
                current_copy->left = new Node(current_other->left->key, current_other->left->value);
                nodes.push({current_copy->left, current_other->left});
            }
        }
    }

    BST(BST<KeyType, ValueType>&& other) : root(other.root){
        other.root = nullptr;
    }
    // -----------------------------------------------
    ~BST() {
        std::stack<Node*> S;
        Node* current = root;

        while (current != nullptr || S.size()) {
            if (current != nullptr) {
                S.push(current);
                current = current->left;
            } else {
                current = S.top()->right;
                delete S.top();
                S.pop();
            }
        }
    }
    // -----------------------------------------------
    BST& operator=(const BST<KeyType, ValueType>& other) { 
        if (this != &other) {
            BST<KeyType, ValueType> temp(other);
            std::swap(root, temp.root);
        }
        return *this;
    }

    BST& operator=(BST<KeyType, ValueType>&& other) {
        if (this != &other) {
            this->~BST();
            this->root = other.root;
            other.root = nullptr;
        }
        return *this;
    }
    // -----------------------------------------------
    void Insert(KeyType key, ValueType value) {
        if (root == nullptr) {
            root = new Node(key, value);
            return;
        }
        Node* current = root;

        while (true) {
            if (current->key == key) {
                current->value = value;
                return;
            } else if (current->key < key && current->right != nullptr) {
                current = current->right;
            } else if (current->key < key && current->right == nullptr) {
                current->right = new Node(key, value);
                return;
            } else if (current->key > key && current->left != nullptr) {
                current = current->left;
            } else {
                current->left = new Node(key, value);
                return;
            }
        }
    }
    // -----------------------------------------------
    std::optional<ValueType> SearchByKey(KeyType key) {
        Node* current = root;

        while (current != nullptr) {
            if (current->key == key)
                return current->value;
            else if (current->key < key)
                current = current->right;
            else 
                current = current->left;
        }
        return std::nullopt;
    }

    std::optional<ValueType> SearchNext(KeyType key) {
        Node* current = root;
        Node* next = nullptr;

        while (current != nullptr) {
            if (current->key == key) {
                break;
            } else if (current->key < key) {
                current = current->right;
            } else if (current->key > key) {
                next = current;
                current = current->left;
            }

        }
        if (current->right == nullptr)
            return next->value;
        else {
            current = current->right;
            while (current->left != nullptr)
                current = current->left;
            return current->value;
        }
    }

    std::optional<ValueType> SearchPrev(KeyType key) {
        Node* current = root;
        Node* next = nullptr;

        while (current != nullptr) {
            if (current->key == key) {
                break;
            }
            else if (current->key < key) {
                next = current;
                current = current->right;
            }
            else if (current->key > key) {
                current = current->left;
            }

        }
        if (current->left == nullptr)
            return next->value;
        else {
            current = current->left;
            while (current->right != nullptr)
                current = current->right;
            return current->value;
        }
    }
    // -----------------------------------------------
    void DeleteNode(KeyType key) {
        if (root == nullptr) return;
        Node* currnet = root;
        Node* papa = nullptr;

        while (currnet != nullptr) {
            if (currnet->key == key) {
                break;
            } else if (currnet->key < key) {
                papa = currnet;
                currnet = currnet->right;
            } else {
                papa = currnet;
                currnet = currnet->left;
            }
        }

        if (currnet == nullptr) return;

        if (currnet->left == nullptr && currnet->right == nullptr) {
            if (papa == nullptr)
                root = nullptr;
            else if (papa->left == currnet)
                papa->left = nullptr;
            else 
                papa->right = nullptr;
            delete currnet;
        } else if (currnet->left == nullptr) {
            if (papa == nullptr)
                root = currnet->right;
            else if (papa->left == currnet)
                papa->left = currnet->right;
            else 
                papa->right = currnet->right;
            delete currnet;
        } else if (currnet->right == nullptr) {
            if (papa == nullptr)
                root = currnet->left;
            else if (papa->left == currnet)
                papa->left = currnet->left;
            else
                papa->right = currnet->left;
            delete currnet;
        } else {
            Node* min_node = currnet->right;
            papa = currnet;

            while (min_node->left != nullptr) {
                papa = min_node;
                min_node->left;
            }

            currnet->key = std::move(min_node->key);
            currnet->value = std::move(min_node->value);

            if (papa->left == min_node)
                papa->left = min_node->right;
            else 
                papa->right = min_node->right;
            delete min_node;
        }
            
    }

 private:
    struct Node{
        Node* left;
        Node* right;
        KeyType key;
        ValueType value;

        Node(KeyType k, ValueType v) : key(k), value(v), left(nullptr), right(nullptr) {}
    }*root = nullptr;
};

template <typename KeyType, typename ValueType>
class RBST {
 public:
    RBST() : root(nullptr), rnd(std::random_device()) {}
    void Insert(KeyType k, ValueType v) {
        return;
    }
 private:
    struct Node {
        Node* left;
        Node* right;

        KeyType key;
        KeyType value;

        size_t size;

        Node(KeyType k, ValueType v) : key(k), value(v), left(nullptr), right(nullptr), size(1) {}
    }*root = nullptr;
    std::mt19937 rnd;
};