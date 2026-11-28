void setZeroes(vector<vector<int>>& matrix) {
    if(matrix.empty()) return;
    const int rows = static_cast<int>(matrix.size());
    const int cols = static_cast<int>(matrix[0].size());
    
    std::vector<bool> zeroIndexRows(rows, false);
    std::vector<bool> zeroIndexCols(cols, false);

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(matrix[i][j] == 0){
                zeroIndexRows[i] = true;
                zeroIndexCols[j] = true;
            }
        }
    }

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(zeroIndexRows[i] || zeroIndexCols[j]){
                matrix[i][j] = 0;
            }
        }
    }
}