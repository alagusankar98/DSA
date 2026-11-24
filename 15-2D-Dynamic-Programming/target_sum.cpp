int targetHelper(const std::vector<int>& nums, size_t i, int remaining, std::vector<std::vector<int>>& dp) {
    if(remaining < 0) return 0;
    if(i == nums.size()) return (remaining == 0);

    if(dp[i][remaining] != -1) return dp[i][remaining];

    return dp[i][remaining] = targetHelper(nums, i + 1, remaining - nums[i], dp) + targetHelper(nums, i + 1, remaining, dp);
}
int findTargetSumWays(const std::vector<int>& nums, int target) {
    int absSum = 0;
    for(const int num : nums) {
        absSum += std::abs(num);
    }

    if(absSum < std::abs(target)) return 0;

    int positiveSum = (absSum + target);
    if(positiveSum & 1) return 0; // If poisitive sum is odd, target cannot be reached

    std::vector<std::vector<int>> dp(nums.size(), std::vector((positiveSum / 2) + 1, -1));
    return targetHelper(nums, 0, (positiveSum / 2), dp);
}