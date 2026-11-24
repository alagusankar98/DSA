int maxProfit(const std::vector<int>& prices) {
    const int n = static_cast<int>(prices.size());
    std::vector<std::vector<int>> dp(n + 2, std::vector<int>(2, 0));

    for(int i = n - 1; i >= 0; i--) {
        for(int j = 0; j < 2; j++){
            int profit = dp[i + 1][j];
            if(j) {
                profit = std::max(profit, dp[i + 2][0] + prices[i]);
            } else {
                profit = std::max(profit, dp[i + 1][1] - prices[i]);
            }
            dp[i][j] = profit;
        }
    }
    return dp[0][0];
}