int maxProfitHelper(const std::vector<int>& prices, size_t i, bool holdingStock, std::vector<std::vector<int>>& dp) {
    if(i >= prices.size()) return 0;

    if(dp[i][holdingStock] != -1) return dp[i][holdingStock];

    int profit = maxProfitHelper(prices, i + 1, holdingStock, dp);
    if(holdingStock) {
        profit = std::max(profit, maxProfitHelper(prices, i + 2, false, dp) + prices[i]);
    } else {
        profit = std::max(profit, maxProfitHelper(prices, i + 1, true, dp) - prices[i]);
    }

    return dp[i][holdingStock] = profit;
}
int maxProfit(const std::vector<int>& prices) {
    std::vector<std::vector<int>> dp(prices.size(), std::vector<int>(2, -1));
    return maxProfitHelper(prices, 0, false, dp);
}