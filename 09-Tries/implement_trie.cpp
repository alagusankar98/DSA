class TrieNode {
    public:
        char val;
        std::array<TrieNode*, 26> next;
        bool isDone;
        TrieNode() : val('\0'), next({}), isDone(false) {}
        TrieNode(char val_) : val(val_), next({}), isDone(false) {}
};

class PrefixTree {
private:
    TrieNode* head_;
public:
    PrefixTree() {
        auto dummyNode = new TrieNode();
        head_ = dummyNode;
    }
    
    void insert(string word) {
        auto current = head_;
        for(const char c : word){
            int charIdx = c - 'a';
            if (!current->next[charIdx]) current->next[charIdx] = new TrieNode(c);
            current = current->next[charIdx];
        }
        current->isDone = true;
    }
    
    bool search(string word) {
        auto current = head_;
        for(const char c : word){
            int charIdx = c - 'a';
            if(!current->next[charIdx]) return false; // Character does not exist
            current = current->next[charIdx];
        }
        return current->isDone;
    }
    
    bool startsWith(string prefix) {
        auto current = head_;
        for(const char c : prefix){
            int charIdx = c - 'a';
            if(!current->next[charIdx]) return false; // Character does not exist
            current = current->next[charIdx];
        }
        return true;
    }
};