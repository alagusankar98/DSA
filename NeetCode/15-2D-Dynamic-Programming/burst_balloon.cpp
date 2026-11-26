int maxCoinHelper(const std::vector<int>& nums, int start, int end, int n, std::vector<std::vector<int>>& dp) {
    if(end < start) return 0;

    if(dp[start][end] != -1) return dp[start][end];

    int prevVal = (start - 1) >= 0 ? nums[start - 1] : 1;
    int nextVal = (end + 1) < n ? nums[end + 1] : 1;

    int count = 0;
    for(int i = start; i <= end; i++) {
        // Assume 'i' as the balloon currently being blast
        count = std::max(count, (prevVal * nextVal * nums[i]) + maxCoinHelper(nums, start, i - 1, n, dp) + maxCoinHelper(nums, i + 1, end, n, dp));
    }
    return dp[start][end] = count;
}

int maxCoins(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, -1));
    return maxCoinHelper(nums, 0, n - 1, n, dp);
}