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
    std::vector<uint8_t> seen(n, 0);

    for(int i = 0; i < n; i++) {
        spiralVector.push_back(matrix[rowIdx][colIdx]);
        seen[rowIdx * cols + colIdx] = 1;

        int nextRowIdx = rowIdx + directionArray[directionArrayIdx].first;
        int nextColIdx = colIdx + directionArray[directionArrayIdx].second;

        if(nextRowIdx < 0 || nextRowIdx >= rows || nextColIdx < 0 || nextColIdx >= cols || seen[nextRowIdx * cols + nextColIdx]) {
            directionArrayIdx = (directionArrayIdx + 1) % 4;
        }

        rowIdx = rowIdx + directionArray[directionArrayIdx].first;
        colIdx = colIdx + directionArray[directionArrayIdx].second;
    }

    return spiralVector;
}