int targetHelper(const std::vector<int>& nums, size_t i, int remaining, unsigned int absSum, std::vector<std::vector<int>>& dp) {
    if(i == nums.size()) return (remaining == 0);
    size_t remainingIdx = (remaining >= 0) ? remaining : std::abs(remaining) + absSum;

    if(dp[i][remainingIdx] != -1) return dp[i][remainingIdx];

    return dp[i][remainingIdx] = targetHelper(nums, i + 1, remaining - nums[i], absSum, dp) + targetHelper(nums, i + 1, remaining + nums[i], absSum, dp);
}
int findTargetSumWays(const std::vector<int>& nums, int target) {
    unsigned int absSum = 0;
    for(const int num : nums) {
        absSum += std::abs(num);
    }
    absSum = absSum + std::abs(target);
    std::vector<std::vector<int>> dp(nums.size(), std::vector((absSum * 2 + 1), -1));
    return targetHelper(nums, 0, target, absSum, dp);
}