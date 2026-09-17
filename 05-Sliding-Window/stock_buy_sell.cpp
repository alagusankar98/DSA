int maxProfit(vector<int>& prices) {
    int minLeft = INT_MAX;
    int maxDifference = 0;
    for(const int price : prices){
        minLeft = std::min(price, minLeft);
        maxDifference = std::max(maxDifference, (price - minLeft));
    }
    return maxDifference;
}