class TrieNode{
    public:
        std::array<TrieNode*, 26> next;
        bool isDone;
        TrieNode() : next({}), isDone(false) {}
};
class WordDictionary {
private:
    TrieNode* root;

    TrieNode* findNode(std::string_view word, size_t currentIdx, TrieNode* searchNode){
        if(!searchNode || currentIdx >= word.size()) return searchNode;

        if (word[currentIdx] == '.'){
            for(const auto& node : searchNode->next){
                if(node){
                    auto current = findNode(word, currentIdx + 1, node);
                    if(current && current->isDone) return current;
                }
            }
            return nullptr;
        }
        int charIdx = word[currentIdx] - 'a';
        if(!searchNode->next[charIdx]) return nullptr;
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
        auto current = findNode(word, 0, root);
        return current && current->isDone;
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