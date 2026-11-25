
class Solution {
private:
    using coordinates = std::pair<int, int>;
    bool searchWord(const std::vector<std::vector<char>>& board, std::string_view word, size_t searchIdx, std::vector<std::vector<bool>>& seen, coordinates currentCoordinate){
        if(searchIdx >= word.size()) return true;

        auto& [i, j] = currentCoordinate;

        // Check current co-ordinate under boundaries, not seen already and matches
        if((i < 0) || (j < 0) || (i >= board.size()) || (j >= board[i].size()) || seen[i][j] ||
        board[i][j] != word[searchIdx]) return false;

        // Check for potential matches in all four directions
        seen[i][j] = true;

        bool resultFlag = searchWord(board, word, searchIdx + 1, seen, {i - 1, j}) ||
                          searchWord(board, word, searchIdx + 1, seen, {i + 1, j}) ||
                          searchWord(board, word, searchIdx + 1, seen, {i, j - 1}) ||
                          searchWord(board, word, searchIdx + 1, seen, {i, j + 1});
        seen[i][j] = false;
        return resultFlag;

    }
public:
    bool exist(const std::vector<std::vector<char>>& board, std::string_view word) {
        std::vector<std::vector<bool>> seen(board.size(), std::vector<bool>(board[0].size(), false));
        for(int i = 0; i < std::ssize(board); i++){
            for(int j = 0; j < std::ssize(board[i]); j++){
                    // Early return in case of match
                    if(searchWord(board, word, 0, seen, {i, j})) return true;
            }
        }
        return false;
    }
};
