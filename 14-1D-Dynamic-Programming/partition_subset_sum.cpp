bool subsetSumExists(const std::vector<int>& nums, size_t i, int remainingSum, std::vector<std::vector<uint8_t>>& dp) {
    if(remainingSum == 0) return true;
    if(remainingSum < 0 || i == nums.size()) return false;

    if(dp[i][remainingSum] != std::numeric_limits<uint8_t>::max()) return dp[i][remainingSum];

    return dp[i][remainingSum] = subsetSumExists(nums, i + 1, remainingSum - nums[i], dp) || subsetSumExists(nums, i + 1, remainingSum, dp);
}
bool canPartition(const std::vector<int>& nums) {
    int fullSum = 0;
    for(const int num : nums) {
        fullSum += num;
    }

    if(fullSum & 1) return false; // Even sum cannot be split

    int remainingSum = (fullSum / 2);
    std::vector<std::vector<uint8_t>> dp(nums.size(), std::vector<uint8_t>(remainingSum + 1, std::numeric_limits<uint8_t>::max()));

    // If a subset with (fullSum / 2) exists, we can conclude that another similar subset exists
    return subsetSumExists(nums, 0, remainingSum, dp);
}