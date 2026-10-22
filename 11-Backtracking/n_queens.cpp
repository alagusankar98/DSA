bool isQueenPositionValid(const std::vector<std::string>& board, int rowIdx, int colIdx){
    // Given an already existing valid board with queens
    // check if new position is valid

    // Check column
    for(int i = 0; i < rowIdx; i++){
        if(board[i][colIdx] != '.') return false;
    }

    // Check diagonal (upwards to left)
    for(int i = rowIdx - 1, j = colIdx - 1; (i >= 0 && j >= 0); i--, j--){
        if(board[i][j] != '.') return false;
    }

    // Check diagonal (upwards to right)
    for(int i = rowIdx - 1, j = colIdx + 1; (i >= 0 && j < board[0].size()); i--, j++){
        if(board[i][j] != '.') return false;
    }

    return true;
}

void boardGenerator(std::vector<std::vector<std::string>>& resultVector, std::vector<std::string>& currentBoard, int n, int rowIdx, int colIdx){
    if(rowIdx >= n){
        resultVector.push_back(currentBoard);
        return;
    }

    if(colIdx >= n) return; // No more valid position

    if(isQueenPositionValid(currentBoard, rowIdx, colIdx)){
        currentBoard.emplace_back(n, '.');
        currentBoard.back()[colIdx] = 'Q';
        boardGenerator(resultVector, currentBoard, n, rowIdx + 1, 0);
        currentBoard.pop_back();
    }
    boardGenerator(resultVector, currentBoard, n, rowIdx, colIdx + 1);
}

std::vector<std::vector<std::string>> solveNQueens(int n) {
    std::vector<std::vector<std::string>> resultVector;
    std::vector<std::string> currentBoard;
    currentBoard.reserve(n);
    boardGenerator(resultVector, currentBoard, n, 0, 0);
    return resultVector;
}