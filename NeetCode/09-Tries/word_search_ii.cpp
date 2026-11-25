class TrieNode {
    public:
        std::array<TrieNode*, 26> next;
        bool isDone;
        TrieNode() : next({}), isDone(false) {}
};

class PrefixTrie {
    public:
        TrieNode* root;
        PrefixTrie(){
            root = new TrieNode();
        }
        void insert(std::string_view word){
            auto current = root;
            for(const char c : word){
                int charIdx = c - 'a';
                if(!current->next[charIdx]) current->next[charIdx] = new TrieNode();
                current = current->next[charIdx];
            }
            current->isDone = true;
        }
        
        // Delete copy and assignment operator
        PrefixTrie(const PrefixTrie&) = delete;
        PrefixTrie& operator=(const PrefixTrie&) = delete;

        ~PrefixTrie(){
            std::stack<TrieNode*> trieStack;
            trieStack.push(root);

            while(!trieStack.empty()){
                auto nodeToDelete = trieStack.top(); trieStack.pop();

                for(auto& node : nodeToDelete->next){
                    if(node) trieStack.push(node);
                }

                delete nodeToDelete;
            }
        }
};

class Solution {
private:
    using coordinates = std::pair<int, int>;
    void checkBoardForWords(const std::vector<std::vector<char>>& board, TrieNode* root, std::vector<std::vector<bool>>& seen, const coordinates current, std::vector<std::string>& resultVector, std::string& stringSoFar){
        if(!root) return; // Check what to do

        auto& [i, j] = current;

        // Check boundaries
        if((i < 0) || (j < 0) || (i >= board.size()) || (j >= board[0].size()) || seen[i][j]) return;

        // Check current board character against node
        int charIdx = board[i][j] - 'a';
        auto nextNode = root->next[charIdx];
        if(!nextNode) return;

        stringSoFar.push_back(board[i][j]); // Push once it is a valid character
        seen[i][j] = true;

        if(nextNode->isDone) {
            resultVector.push_back(stringSoFar);
            nextNode->isDone = false;
        }

        // Recursive calls
        checkBoardForWords(board, nextNode, seen, {i-1, j}, resultVector, stringSoFar);
        checkBoardForWords(board, nextNode, seen, {i+1, j}, resultVector, stringSoFar);
        checkBoardForWords(board, nextNode, seen, {i, j-1}, resultVector, stringSoFar);
        checkBoardForWords(board, nextNode, seen, {i, j+1}, resultVector, stringSoFar);
        
        // Check if this is the end of tree before recursive calls
        bool isEndTree = true;
        for(auto& node : nextNode->next){
            if(node){
                isEndTree = false; 
                break; // Atleast one node exists further in this path
            }
        }

        if(isEndTree){
            delete nextNode;
            root->next[charIdx] = nullptr;
        }
        

        stringSoFar.pop_back();
        seen[i][j] = false;
        return;
    }
public:
    std::vector<std::string> findWords(const std::vector<std::vector<char>>& board, std::vector<std::string>& words) {
        PrefixTrie wordTrie;

        // Convert words into a trie
        for(const auto& word : words){
            wordTrie.insert(word);
        }

        std::vector<std::string> resultVector;
        resultVector.reserve(words.size()); // Worse case
        std::string tempString;
        std::vector<std::vector<bool>> seen(board.size(), std::vector<bool>(board[0].size(), false));
        for(int i = 0; i < std::ssize(board); i++){
            for(int j = 0; j < std::ssize(board[i]); j++){
                checkBoardForWords(board, wordTrie.root, seen, {i, j}, resultVector, tempString);
            }
        }
        return resultVector;
    }
};