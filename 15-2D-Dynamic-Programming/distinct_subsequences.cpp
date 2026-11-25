int numDistinct(std::string_view s, std::string_view t) {
    const int n1 = static_cast<int>(s.size());
    const int n2 = static_cast<int>(t.size());

    if(n2 > n1) return 0;

    std::vector<std::vector<unsigned int>> dp(n1 + 1, std::vector<unsigned int>(n2 + 1, 0));

    for(int i = 0; i <= n1; i++) dp[i][n2] = 1; // if(j == t.size()) return 1;

    for(int i = n1 - 1; i >= 0; i--) {
        for(int j = n2 - 1; j >= 0; j--) {
            dp[i][j] = dp[i + 1][j];
            if(s[i] == t[j]) dp[i][j] += dp[i + 1][j + 1];
        }
    }

    return static_cast<int>(dp[0][0]);
}