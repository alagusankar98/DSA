int minCostClimbingStairs(const std::vector<int>& cost) {
    const int n = static_cast<int>(cost.size());
    int prev2 = 0, prev1 = 0;
    for(int i = 2; i <= n; i++){
        int temp = std::min(cost[i-1] + prev1, cost[i-2] + prev2);
        prev2 = prev1;
        prev1 = temp;
    }
    return prev1;
}