int targetHelper(const std::vector<int>& nums, size_t i,  int remaining, const int offset, std::vector<std::vector<int>>& dp) {
    if(i == nums.size()) return (remaining == 0);
    size_t remainingIdx = remaining + offset;

    if(dp[i][remainingIdx] != -1) return dp[i][remainingIdx];

    return dp[i][remainingIdx] = targetHelper(nums, i + 1, remaining - nums[i], offset, dp) + targetHelper(nums, i + 1, remaining + nums[i], offset, dp);
}
int findTargetSumWays(const std::vector<int>& nums, int target) {
    unsigned int absSum = 0;
    for(const int num : nums) {
        absSum += std::abs(num);
    }

    if(absSum < std::abs(target)) return 0;

    std::vector<std::vector<int>> dp(nums.size(), std::vector((absSum * 2 + 1), -1));
    int offset = absSum - target;
    return targetHelper(nums, 0, target, offset, dp);
}