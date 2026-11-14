int minCostClimbingStairs(const std::vector<int>& cost) {
    const int n = static_cast<int>(cost.size());
    if(n < 3) return *std::min_element(cost.begin(), cost.end());
    std::vector<int> dp(n + 1, 0);
    dp[0] = cost[0]; dp[1] = cost[1];
    for(int i = 2; i < n; i++){
        dp[i] = cost[i] + std::min(dp[i-1], dp[i-2]);
    }
    return std::min(dp[n-1], dp[n-2]);
}