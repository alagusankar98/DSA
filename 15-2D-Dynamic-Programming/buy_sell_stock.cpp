int maxProfitHelper(const std::vector<int>& prices, size_t i, bool holdingStock) {
    if(i >= prices.size()) return 0;

    int profit;
    if(holdingStock) {
        profit = std::max(maxProfitHelper(prices, i + 1, true), maxProfitHelper(prices, i + 2, false) + prices[i]);
    } else {
        profit = std::max(maxProfitHelper(prices, i + 1, false), maxProfitHelper(prices, i + 1, true) - prices[i]);
    }

    return profit;
}
int maxProfit(const std::vector<int>& prices) {
    return maxProfitHelper(prices, 0, false);
}