int maxProfit(const std::vector<int>& prices) {
    const int n = static_cast<int>(prices.size());
    std::vector<int> buyDp(n + 2, 0);
    std::vector<int> sellDp(n + 2, 0);

    for(int i = n - 1; i >= 0; i--) {
        buyDp[i] = std::max(buyDp[i+1], sellDp[i + 1] - prices[i]);
        sellDp[i] = std::max(sellDp[i+1], buyDp[i + 2] + prices[i]);
    }
    return buyDp[0];
}