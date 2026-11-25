int maxProfit(const std::vector<int>& prices) {
    const int n = static_cast<int>(prices.size());
    int buyTomorrow = 0; // buyDp[i+1]
    int sellTomorrow = 0; // sellDp[i + 1]
    int buyDayAfterTomorrow = 0; // buyDp[i + 2]

    for(int i = n - 1; i >= 0; i--) {
        int buyToday = std::max(buyTomorrow, sellTomorrow - prices[i]); // buyDp[i] = std::max(buyDp[i+1], sellDp[i + 1] - prices[i]);
        int sellToday = std::max(sellTomorrow, buyDayAfterTomorrow + prices[i]); // sellDp[i] = std::max(sellDp[i+1], buyDp[i + 2] + prices[i]);
        buyDayAfterTomorrow = buyTomorrow; // buyDp[i + 2] = buyDp[i + 1]
        buyTomorrow = buyToday; // buyDp[i + 1] = buyDp[i]
        sellTomorrow = sellToday; // sellDp[i + 1] = sellDp[i]
    }
    return buyTomorrow; // buyDp[0] -> buyDp[last_executed_i] -> buyToday -> Stored in buyTomorrow after loop
}