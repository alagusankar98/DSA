int findTargetSumWays(const std::vector<int>& nums, int target) {
    int absSum = 0;
    for(const int num : nums) {
        absSum += std::abs(num);
    }

    if(absSum < std::abs(target)) return 0;

    int positiveSum = (absSum + target);
    if(positiveSum & 1) return 0; // If poisitive sum is odd, target cannot be reached

    const int n = static_cast<int>(nums.size());

    std::vector<std::vector<int>> dp(n + 1, std::vector((positiveSum / 2) + 1, 0));
    dp[n][0] = 1;

    for(int i = n - 1; i >= 0; i--) {
        for(int remSum = 0; remSum <= (positiveSum / 2); remSum++) {
            dp[i][remSum] = dp[i + 1][remSum];
            if(remSum >= nums[i]) dp[i][remSum] += dp[i + 1][remSum - nums[i]];
        }
    }

    return dp[0][positiveSum / 2];
}