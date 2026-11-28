std::vector<int> spiralOrder(const std::vector<std::vector<int>>& matrix) {
    const int rows = static_cast<int>(matrix.size());
    const int cols = static_cast<int>(matrix[0].size());
    const int n = rows * cols;
    std::vector<int> spiralVector;
    spiralVector.reserve(n);
    int top = 0;
    int bottom = rows - 1;
    int left = 0;
    int right = cols - 1;

    while(static_cast<int>(spiralVector.size()) < n) {
        for(int j = left; j <= right && top <= bottom; j++) {
            spiralVector.push_back(matrix[top][j]);
        }
        top++;

        for(int i = top; i <= bottom && right >= left; i++) {
            spiralVector.push_back(matrix[i][right]);
        }
        right--;

        for(int j = right; j >= left && top <= bottom; j--) {
            spiralVector.push_back(matrix[bottom][j]);
        }
        bottom--;

        for(int i = bottom; i >= top && left <= right; i--) {
            spiralVector.push_back(matrix[i][left]);
        }
        left++;
    }

    return spiralVector;
}