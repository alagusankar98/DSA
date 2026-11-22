int lengthOfLIS(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(n + 1, 0));

    for(int startIdx = n - 1; startIdx >= 0; startIdx--) {
        for(int prevIdx = -1; prevIdx < startIdx; prevIdx++) {
            int count = dp[startIdx + 1][prevIdx + 1];
            if(prevIdx == -1 || nums[startIdx] > nums[prevIdx]) {
                count = std::max(1 + dp[startIdx + 1][startIdx + 1], count);
            }
            dp[startIdx][prevIdx + 1] = count;
        }
    }

    return dp[0][0];
}