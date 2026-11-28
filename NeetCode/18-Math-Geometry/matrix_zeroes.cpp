void setZeroes(vector<vector<int>>& matrix) {
    if(matrix.empty()) return;
    const int rows = static_cast<int>(matrix.size());
    const int cols = static_cast<int>(matrix[0].size());
    
    std::vector<std::pair<int, int>> zeroIndices;
    zeroIndices.reserve(rows * cols);

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(matrix[i][j] == 0) zeroIndices.emplace_back(i, j);
        }
    }

    size_t read = 0;
    while(read < zeroIndices.size()) {
        auto [r, c] = zeroIndices[read]; read++;

        for(int j = 0; j < cols; j++) matrix[r][j] = 0;
        for(int i = 0; i < rows; i++) matrix[i][c] = 0;
    }
}