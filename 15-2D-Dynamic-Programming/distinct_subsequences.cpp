int numDistinctHelper(std::string_view s, std::string_view t, size_t i, size_t j, std::vector<std::vector<int>>& dp) {
    if(j == t.size()) return 1;
    if(i == s.size()) return 0;

    if(dp[i][j] != -1) return dp[i][j];

    int count = numDistinctHelper(s, t, i + 1, j, dp);

    if(s[i] == t[j]) count += numDistinctHelper(s, t, i + 1, j + 1, dp);

    return dp[i][j] = count;
}
int numDistinct(std::string_view s, std::string_view t) {
    const int n1 = static_cast<int>(s.size());
    const int n2 = static_cast<int>(t.size());

    if(n2 > n1) return 0;

    std::vector<std::vector<int>> dp(n1, std::vector<int>(n2, -1));
    return numDistinctHelper(s, t, 0, 0, dp);
}