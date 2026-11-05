void solve(std::vector<std::vector<char>>& board) {
    if(board.empty() || board[0].empty()) return;
    static constexpr std::array<std::pair<int, int>, 4> directionArray = {{{1,0},{-1,0},{0,1},{0,-1}}};

    int rows = static_cast<int>(board.size());
    int columns = static_cast<int>(board[0].size());
    std::vector<std::pair<int, int>> bfsVector;
    bfsVector.reserve(rows * columns);
    std::vector<uint8_t> cannotMark(rows*columns, 0);

    // Left and Right borders
    for(int i = 0; i < rows; i++){
        // [][0] and [][column -1]
        if(board[i][0] == 'O'){
            bfsVector.emplace_back(i, 0);
            cannotMark[i*columns] = 1;
        }
        if(board[i][columns-1] == 'O'){
            bfsVector.emplace_back(i, columns-1);
            cannotMark[i*columns + (columns - 1)] = 1;
        }
    }

    // Top and Bottom borders without start and end element
    for(int j = 1; j < columns - 1; j++){
        // [0][j] and [rows - 1][j]
        if(board[0][j] == 'O'){
            bfsVector.emplace_back(0, j);
            cannotMark[j] = 1;
        }
        if(board[rows-1][j] == 'O'){
            bfsVector.emplace_back(rows-1, j);
            cannotMark[(rows - 1)*columns + j] = 1;
        }
    }

    
    std::size_t read = 0;
    while(read < bfsVector.size()){
        auto [r, c] = bfsVector[read];
        read++;

        for(const auto& [rowOffset, colOffset] : directionArray){
            if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); 
            (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && (board[rowIdx][colIdx] == 'O') && (!cannotMark[rowIdx * columns + colIdx]))){
                cannotMark[rowIdx * columns + colIdx] = 1;
                bfsVector.emplace_back(rowIdx, colIdx);
            }
        }
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if(board[i][j] == 'O' && !cannotMark[i * columns + j]){
                board[i][j] = 'X';
            }
        }
    }
}