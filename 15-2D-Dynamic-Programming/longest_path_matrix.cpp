int longestIncreasingPathHelper(const std::vector<std::vector<int>>& matrix, int r, int c, std::vector<std::vector<int>>& dp) {
    if(dp[r][c] != -1) return dp[r][c];
    static constexpr std::array<std::pair<int, int>, 4> directionArray = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

    int count = 1;
    for(const auto [rowOffset, colOffset] : directionArray) {
        if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); (rowIdx >= 0 && rowIdx < static_cast<int>(matrix.size()) && colIdx >= 0 && colIdx < static_cast<int>(matrix[0].size()) && matrix[rowIdx][colIdx] > matrix[r][c])) {
            count = std::max(count, 1 + longestIncreasingPathHelper(matrix, rowIdx, colIdx, dp));
        }
    }
    return dp[r][c] = count;
}

int longestIncreasingPath(const std::vector<std::vector<int>>& matrix) {
    const int rows = static_cast<int>(matrix.size());
    const int columns = static_cast<int>(matrix[0].size());
    std::vector<std::vector<int>> dp(rows, std::vector<int>(columns, -1));
    int count = 0;
    for(int i = 0; i < matrix.size(); i++) {
        for(int j = 0; j < matrix[0].size(); j++) {
            count = std::max(count, longestIncreasingPathHelper(matrix, i, j, dp));
        }
    }
    return count;
}