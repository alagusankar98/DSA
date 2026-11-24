int changeHelper(const std::vector<int>& coins, size_t startIdx, int remaining, std::vector<std::vector<int>>& dp){
    if(remaining < 0 || startIdx == coins.size()) return 0;
    if(remaining == 0) return 1;

    if(dp[startIdx][remaining] != -1) return dp[startIdx][remaining];

    return dp[startIdx][remaining] = changeHelper(coins, startIdx, remaining - coins[startIdx], dp) + changeHelper(coins, startIdx + 1, remaining, dp);
}
int change(int amount, const std::vector<int>& coins) {
    std::vector<std::vector<int>> dp(coins.size(), std::vector<int>(amount + 1, -1));
    return changeHelper(coins, 0, amount, dp);
}