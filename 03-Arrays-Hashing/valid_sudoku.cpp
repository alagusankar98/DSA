bool isValidSudoku(const std::vector<std::vector<char>>& board) {
    std::array<uint16_t, 9> columnFlags{};
    std::array<uint16_t, 9> rowFlags{};
    std::array<uint16_t, 9> boxFlags{};

    for(size_t i = 0; i < board.size(); i++){
        for(size_t j = 0; j < board[i].size(); j++){
            if (board[i][j] == '.') continue;
                
            const uint8_t digit = board[i][j] - '1';
            const size_t currentBoxIdx = 3 * (j / 3) + (i/3);
            const uint16_t mask = (1 << digit);

            // Check Row flag
            if((rowFlags[i] | columnFlags[j] | boxFlags[currentBoxIdx]) & mask){
                return false;
            }

            rowFlags[i] = rowFlags[i] | mask;
            columnFlags[j] = columnFlags[j] | mask;
            boxFlags[currentBoxIdx] = boxFlags[currentBoxIdx] | mask;
        }
    }
    return true;
}