int minDistance(std::string_view word1, std::string_view word2) {
    const int n1 = static_cast<int>(word1.size());
    const int n2 = static_cast<int>(word2.size());
    std::vector<std::vector<int>> dp(n1 + 1, std::vector<int>(n2 + 1, -1));
    
    for(int i = 0; i <= n1; i++) dp[i][n2] = (n1 - i);
    for(int j = 0; j <= n2; j++) dp[n1][j] = (n2 - j);

    for(int i = n1 - 1; i >= 0; i--) {
        for(int j = n2 - 1; j >= 0; j--) {
            if(word1[i] == word2[j]){
                dp[i][j] = dp[i + 1][j + 1];
            } else {
                int count = std::min(dp[i + 1][j + 1], dp[i + 1][j]);
                count = std::min(count, dp[i][j + 1]);
                dp[i][j] = 1 + count;
            }
        }
    }
    return dp[0][0];
}