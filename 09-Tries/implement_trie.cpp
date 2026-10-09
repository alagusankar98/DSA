class TrieNode {
    public:
        std::array<TrieNode*, 26> next;
        bool isDone;
        TrieNode() :next({}), isDone(false) {}
};

class PrefixTree {
private:
    TrieNode* head_;

    TrieNode* returnNode(std::string_view word){
        auto current = head_;
        for(const char c : word){
            int charIdx = c - 'a';
            if(!current->next[charIdx]) return nullptr; // Character does not exist
            current = current->next[charIdx];
        }
        return current;
    }
public:
    PrefixTree() {
        head_ = new TrieNode();
    }
    
    void insert(std::string_view word) {
        auto current = head_;
        for(const char c : word){
            int charIdx = c - 'a';
            if (!current->next[charIdx]) current->next[charIdx] = new TrieNode();
            current = current->next[charIdx];
        }
        current->isDone = true;
    }
    
    bool search(std::string_view word) {
        auto current = returnNode(word);
        return current && current->isDone;
    }
    
    bool startsWith(std::string_view prefix) {
        return returnNode(prefix) != nullptr;
    }

    ~PrefixTree() {
        std::stack<TrieNode*> trieStack;
        if(head_) trieStack.push(head_);
        while(!trieStack.empty()){
            auto nodeToDelete = trieStack.top(); trieStack.pop();

            for(auto& node : nodeToDelete->next){
                if(node) trieStack.push(node);
            }

            delete nodeToDelete;
        }
    }
};