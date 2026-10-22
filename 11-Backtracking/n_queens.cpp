bool isQueenPositionValid(const std::vector<std::string>& board, int rowIdx, int colIdx){
    // Given an already existing valid board with queens
    // check if new position is valid

    // Check column
    for(int i = 0; i < rowIdx; i++){
        if(board[i][colIdx] != '.') return false;
    }

    // Check diagonal (to left)
    for(int i = rowIdx - 1, j = colIdx - 1; (i >= 0 && j >= 0); i--, j--){
        if(board[i][j] != '.') return false;
    }

    // Check diagonal (to right)
    for(int i = rowIdx - 1, j = colIdx + 1; (i >= 0 && j < std::ssize(board[0])); i--, j++){
        if(board[i][j] != '.') return false;
    }

    return true;
}

void boardGenerator(std::vector<std::vector<std::string>>& resultVector, std::vector<std::string>& currentBoard, int n, int rowIdx){
    if(rowIdx >= n){
        resultVector.push_back(currentBoard);
        return;
    }

    for(int i = 0; i < n; i++){
        if(isQueenPositionValid(currentBoard, rowIdx, i)){
            currentBoard.emplace_back(n, '.');
            currentBoard.back()[i] = 'Q';
            boardGenerator(resultVector, currentBoard, n, rowIdx + 1);
            currentBoard.pop_back();
        }
    }
}

std::vector<std::vector<std::string>> solveNQueens(int n) {
    std::vector<std::vector<std::string>> resultVector;
    std::vector<std::string> currentBoard;
    currentBoard.reserve(n);
    boardGenerator(resultVector, currentBoard, n, 0);
    return resultVector;
}