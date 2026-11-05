void solve(std::vector<std::vector<char>>& board) {
    if(board.empty() || board[0].empty()) return;
    static constexpr std::array<std::pair<int, int>, 4> directionArray = {{{1,0},{-1,0},{0,1},{0,-1}}};

    int rows = static_cast<int>(board.size());
    int columns = static_cast<int>(board[0].size());
    std::vector<std::pair<int, int>> bfsVector;
    bfsVector.reserve(rows * columns);

    auto markAndPush = [&](int i, int j){
        if(board[i][j] == 'O'){
            bfsVector.emplace_back(i, j);
            board[i][j] = '#';
        }
    };

    // Left and Right borders
    for(int i = 0; i < rows; i++){
        // [][0] and [][column -1]
        markAndPush(i, 0);
        markAndPush(i, columns - 1);
    }

    // Top and Bottom borders without start and end element
    for(int j = 1; j < columns - 1; j++){
        // [0][j] and [rows - 1][j]
        markAndPush(0, j);
        markAndPush(rows - 1, j);
    }

    
    std::size_t read = 0;
    while(read < bfsVector.size()){
        auto [r, c] = bfsVector[read];
        read++;

        for(const auto& [rowOffset, colOffset] : directionArray){
            if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); 
            (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns)){
                markAndPush(rowIdx, colIdx);
            }
        }
    }

    for(int i = 0; i < rows; i++){
        char* row = board[i].data();
        for(int j = 0; j < columns; j++){
            if(row[j] == 'O'){
                row[j] = 'X';
            } else if(row[j] == '#'){
                row[j] = 'O';
            }
        }
    }
}