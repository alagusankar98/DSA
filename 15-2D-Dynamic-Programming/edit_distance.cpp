int minDistanceHelper(std::string_view word1, std::string_view word2, const int n1, const int n2, int i, int j, std::vector<std::vector<int>>& dp) {
    if(i == n1 || j == n2) return ((n1 - i) + (n2 - j));
    if(word1[i] == word2[j]) return minDistanceHelper(word1, word2, n1, n2, i + 1, j + 1, dp);

    if(dp[i][j] != -1) return dp[i][j];

    int count = std::min(minDistanceHelper(word1, word2, n1, n2, i + 1, j + 1, dp), minDistanceHelper(word1, word2, n1, n2, i + 1, j, dp));
    count = std::min(count, minDistanceHelper(word1, word2, n1, n2, i, j + 1, dp));
    return dp[i][j] = 1 + count;
}
int minDistance(std::string_view word1, std::string_view word2) {
    const int n1 = static_cast<int>(word1.size());
    const int n2 = static_cast<int>(word2.size());
    std::vector<std::vector<int>> dp(n1, std::vector<int>(n2, -1));
    return minDistanceHelper(word1, word2, n1, n2, 0, 0, dp);
}