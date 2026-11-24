int longestCommonSubsequence(string text1, string text2) {
    const int n1 = static_cast<int>(text1.size());
    const int n2 = static_cast<int>(text2.size());
    std::vector<std::vector<int>> dp(n1 + 1, std::vector<int>(n2 + 1 ,0));
    
    for(int i = n1 - 1; i >= 0; i--){
        for(int j = n2 - 1; j >= 0; j--){
            if(text1[i] != text2[j]) {
                dp[i][j] = std::max(dp[i][j + 1], dp[i + 1][j]);
            } else {
                dp[i][j] = 1 + dp[i + 1][j + 1];
            }
        }
    }

    return dp[0][0];
}