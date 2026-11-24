int longestCommonSubsequenceHelper(std::string_view text1, std::string_view text2, std::vector<std::vector<int>>& dp, size_t i, size_t j) {
    if(i >= text1.size() || j >= text2.size()) return 0;

    if(dp[i][j] != -1) return dp[i][j];

    int count = 0;
    if(text1[i] != text2[j]) {
        count = std::max(longestCommonSubsequenceHelper(text1, text2, dp, i, j + 1), longestCommonSubsequenceHelper(text1, text2, dp, i + 1, j));
    } else {
        count = 1 + longestCommonSubsequenceHelper(text1, text2, dp, i + 1, j + 1);
    }

    return dp[i][j] = count;
}
int longestCommonSubsequence(string text1, string text2) {
    std::vector<std::vector<int>> dp(text1.size(), std::vector<int>(text2.size() ,-1));
    return longestCommonSubsequenceHelper(text1, text2, dp, 0, 0);
}