void rotate(std::vector<vector<int>>& matrix) {
    if(matrix.empty()) return;
    const int rows = static_cast<int>(matrix.size());
    const int columns = static_cast<int>(matrix[0].size());

    // Swap rows from top to middle (Leave middle out in case of odd rows)
    for(int i = 0; i < (rows / 2); i++) {
        int other_row = rows - i - 1;
        std::swap(matrix[i], matrix[other_row]);
    }

    // Transpose Matrix
    for(int i = 0; i < rows; i++) {
        for(int j = i + 1; j < columns; j++) {
            std::swap(matrix[i][j], matrix[j][i]);
        }
    }
}