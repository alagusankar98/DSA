int maxProfit(const std::vector<int>& prices) {
    int minLeft = std::numeric_limits<int>::max();
    int maxDifference = 0;
    for(const int price : prices){
        minLeft = std::min(price, minLeft);
        maxDifference = std::max(maxDifference, (price - minLeft));
    }
    return maxDifference;
}