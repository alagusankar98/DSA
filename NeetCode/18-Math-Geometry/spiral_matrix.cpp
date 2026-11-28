std::vector<int> spiralOrder(std::vector<std::vector<int>>& matrix) {
    static constexpr std::array<std::pair<int, int>, 4> directionArray = {{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    int directionArrayIdx = 0;
    int rowIdx = 0;
    int colIdx = 0;
    int rows = static_cast<int>(matrix.size());
    int cols = static_cast<int>(matrix[0].size());
    const int n = rows * cols;
    std::vector<int> spiralVector;
    spiralVector.reserve(n);
    spiralVector.push_back(matrix[0][0]);
    matrix[0][0] = std::numeric_limits<int>::max();

    while(spiralVector.size() < n) {
        auto [rowOffset, colOffset] = directionArray[directionArrayIdx % 4];
        rowIdx += rowOffset;
        colIdx += colOffset;

        if(rowIdx < 0 || rowIdx >= rows || colIdx < 0 || colIdx >= cols || matrix[rowIdx][colIdx] == std::numeric_limits<int>::max()) {
            rowIdx -= rowOffset;
            colIdx -= colOffset;
            directionArrayIdx++;
        } else {
            spiralVector.push_back(matrix[rowIdx][colIdx]);
            matrix[rowIdx][colIdx] = std::numeric_limits<int>::max();
        }
    }

    return spiralVector;
}