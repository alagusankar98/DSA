void setZeroes(vector<vector<int>>& matrix) {
    if(matrix.empty()) return;
    const int rows = static_cast<int>(matrix.size());
    const int cols = static_cast<int>(matrix[0].size());
    
    bool firstRowZero = false;
    bool firstColZero = false;

    for(int i = 0; i < rows; i++) {
        firstColZero = firstColZero || (matrix[i][0] == 0);
    }

    for(int j = 0; j < cols; j++) {
        firstRowZero = firstRowZero || (matrix[0][j] == 0);
    }

    for(int i = 1; i < rows; i++) {
        for(int j = 1; j < cols; j++) {
            if(matrix[i][j] == 0) {
                matrix[0][j] = 0;
                matrix[i][0] = 0;
            }
        }
    }

    // matrix[0][j] is top row that holds all columns that go to 0
    // matrix[i][0] is left column that holds all rows that go to 0
    for(int i = 1; i < rows; i++) {
        for(int j = 1; j < cols; j++) {
            if(matrix[0][j] == 0 || matrix[i][0] == 0) {
                matrix[i][j] = 0;
            }
        }
    }

    if(firstColZero) {
        for(int i = 0; i < rows; i++) {
            matrix[i][0] = 0;
        }
    }

    if(firstRowZero) {
            for(int j = 0; j < cols; j++) {
            matrix[0][j] = 0;
        }
    }
}