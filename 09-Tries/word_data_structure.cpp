class TrieNode{
    public:
        std::array<TrieNode*, 26> next;
        bool isDone;
        TrieNode() : next({}), isDone(false) {}
};
class WordDictionary {
private:
    TrieNode* root;

    bool findNode(std::string_view word, size_t currentIdx, TrieNode* searchNode){
        if(!searchNode) return false;
        if(currentIdx >= word.size()) return searchNode->isDone;

        if (word[currentIdx] == '.'){
            for(const auto& node : searchNode->next){
                auto resultFlag = findNode(word, currentIdx + 1, node);
                if(resultFlag) return true; // Return only when true to give other branches a chance to match, Fail only outside when none of the branches match
            }
            return false;
        }
        int charIdx = word[currentIdx] - 'a';
        return findNode(word, currentIdx + 1, searchNode->next[charIdx]);
    }
public:
    WordDictionary() {
        root = new TrieNode();
    }
    
    void addWord(std::string_view word) {
        auto current = root;
        for(const char c : word){
            int charIdx = c - 'a';
            if(!current->next[charIdx]) current->next[charIdx] = new TrieNode();
            current = current->next[charIdx];
        }
        current->isDone = true;
    }
    
    bool search(std::string_view word) {
        return findNode(word, 0, root);
    }

    // Delete default copy constructor
    WordDictionary(const WordDictionary&) = delete;
    WordDictionary& operator=(const WordDictionary&) = delete;

    ~WordDictionary(){
        std::stack<TrieNode*> trieStack;
        trieStack.push(root);

        while(!trieStack.empty()){
            auto nodeToDelete = trieStack.top(); trieStack.pop();

            for(const auto& node : nodeToDelete->next){
                if(node) trieStack.push(node);
            }

            delete nodeToDelete;
        }
    }
};