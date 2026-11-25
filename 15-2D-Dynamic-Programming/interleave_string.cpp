bool isInterleave(std::string_view s1, std::string_view s2, std::string_view s3) {
    const int n1 = static_cast<int>(s1.size());
    const int n2 = static_cast<int>(s2.size());
    const int n3 = static_cast<int>(s3.size());

    if((n1 + n2) != n3) return false;

    std::vector<std::vector<uint8_t>> dp(s1.size() + 1, std::vector<uint8_t>(s2.size() + 1, 0));
    dp[n1][n2] = 1;

    for(int i = n1; i >= 0; i--) {
        for(int j = n2; j >= 0; j--) {
            if(i < n1 && s3[i + j] == s1[i]) {
                // Still have characters left in s1
                dp[i][j] = dp[i + 1][j];
            }
            if(j < n2 && s3[i + j] == s2[j]) {
                // Still have characters left in s2
                dp[i][j] = dp[i][j] || dp[i][j + 1];
            }
        }
    }
    return dp[0][0];
}