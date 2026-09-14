bool isValidSudoku(const std::vector<std::vector<char>>& board) {
    std::array<uint16_t, 9> columnFlags{};
    std::array<uint16_t, 9> rowFlags{};
    std::array<uint16_t, 9> boxFlags{};

    for(size_t i = 0; i < board.size(); i++){
        for(size_t j = 0; j < board[i].size(); j++){
            uint8_t digit = board[i][j] - '0';
            if (!(digit > 0 && digit <= 9)) continue;

            // Check Row flag
            if(((rowFlags[i] >> digit) & 1) > 0){
                return false;
            }

            // Check Column flag
            if(((columnFlags[j] >> digit) & 1) > 0){
                return false;
            }

            // Check Box flag
            size_t currentBoxIdx = 3 * (j / 3) + (i/3);
            if(((boxFlags[currentBoxIdx] >> digit) & 1) > 0){
                return false;
            }

            rowFlags[i] = rowFlags[i] | (1 << digit);
            columnFlags[j] = columnFlags[j] | (1 << digit);
            boxFlags[currentBoxIdx] = boxFlags[currentBoxIdx] | (1 << digit);
        }
    }
    return true;
}