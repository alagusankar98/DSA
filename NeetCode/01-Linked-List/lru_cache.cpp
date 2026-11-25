#include <unordered_map>

class Node {
    public:
        int val;
        int key;
        Node* next;
        Node* prev;
        Node() : val(-1), key(-1), next(nullptr), prev(nullptr) {}
        Node(int val_, int key_) : val(val_), key(key_), next(nullptr), prev(nullptr) {}
};
class LRUCache {
private:
    Node* head_;
    Node* tail_;
    int capacity_;
    int size_;
    std::unordered_map<int, Node*> nodeMap_;
public:
    LRUCache(int capacity) : capacity_(capacity), size_(0) {
        head_ = new Node();
        tail_ = new Node();
        head_->next = tail_;
        tail_->prev = head_;
    }

    void removeNode(Node* current){
        Node* prevNode = current->prev;
        Node* nextNode = current->next;
        prevNode->next = nextNode;
        nextNode->prev = prevNode;
        size_--;
    }

    // Delete copy and move semantics
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    void insertToHead(Node* current){
        Node* nextNode = head_->next;
        current->next = nextNode;
        current->prev = head_;
        nextNode->prev = current;
        head_->next = current;
        size_++;
    }

    void deleteFromTail(){
        auto nodeToDelete = tail_->prev;
        nodeMap_.erase(nodeToDelete->key);
        removeNode(nodeToDelete);
        delete nodeToDelete;
    }
    
    int get(int key) {
        // Check for key
        if(auto itr = nodeMap_.find(key); itr != nodeMap_.end()){
            // If Key exists, Remove node and then insert to head
            Node* valNode = itr->second;
            removeNode(valNode);
            insertToHead(valNode);
            return valNode->val;
        }
        
        // And return the value, else return -1
        return -1;
    }
    
    void put(int key, int value) {
        // Check for key
        if(auto itr = nodeMap_.find(key); itr != nodeMap_.end()){
            // If key exists, Remove node, update value and then insert to head
            Node* valNode = itr->second;
            removeNode(valNode);
            valNode->val = value;
            insertToHead(valNode);
        } else {
            // If does not exist, Create node check capacity
            Node* valNode = new Node(value, key);
            if(size_ == capacity_){
                // If capacity is not in reach, delete tail
                deleteFromTail();
            }
            // Insert to head and add to map
            insertToHead(valNode);
            nodeMap_[key] = valNode;
        }
    }

    ~LRUCache(){
        auto current = head_;
        while(current){
            auto nodeToDelete = current;
            current = current->next;
            delete nodeToDelete;
        }
        head_ = nullptr;
        tail_ = nullptr;
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */