bool canPartition(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    int fullSum = 0;
    for(const int num : nums) {
        fullSum += num;
    }

    if(fullSum & 1) return false; // Even sum cannot be split

    int remainingSum = (fullSum / 2);
    std::vector<std::vector<uint8_t>> dp(n + 1, std::vector<uint8_t>(remainingSum + 1, 0));

    // Set all dp[i][0] to 1 to indicate 0 remaining sum as answer
    for(int i = n; i >= 0; i--) dp[i][0] = 1;

    // If a subset with (fullSum / 2) exists, we can conclude that another similar subset exists
    for(int i = n - 1; i >= 0; i--) {
        for(int left = 1; left <= remainingSum; left++) {
            dp[i][left] = dp[i+1][left];
            if(left >= nums[i]) {
                dp[i][left] = dp[i][left] || dp[i+1][left - nums[i]];
            }
            if(dp[i][remainingSum]) return true; // Early return when any index's remaining sum becomes 1
        }
    }

    return false; // return dp[0][remainingSum]; in case early return branching needs to be pruned
}