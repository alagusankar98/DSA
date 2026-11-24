int change(int amount, const std::vector<int>& coins) {
    const int n = static_cast<int>(coins.size());
    std::vector<std::vector<unsigned int>> dp(n + 1, std::vector<unsigned int>(amount + 1, 0));

    for(int i = 0; i <= n; i++) dp[i][0] = 1;
    
    for(int i = n - 1; i >= 0; i--) {
        for(int fillAmount = 0; fillAmount <= amount; fillAmount++) {
            dp[i][fillAmount] = dp[i + 1][fillAmount];
            if(fillAmount >= coins[i]) dp[i][fillAmount] += dp[i][fillAmount - coins[i]];
        }
    }
    
    return static_cast<int>(dp[0][amount]);
}