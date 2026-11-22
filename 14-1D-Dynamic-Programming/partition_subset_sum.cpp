bool canPartition(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    int fullSum = 0;
    for(const int num : nums) {
        fullSum += num;
    }

    if(fullSum & 1) return false; // Even sum cannot be split

    int remainingSum = (fullSum / 2);
    const int W = remainingSum + 1;
    std::vector<uint8_t> current(W, 0);
    // Set all dp[i][0] to 1 to indicate 0 remaining sum as answer
    current[0] = 1;

    // If a subset with (fullSum / 2) exists, we can conclude that another similar subset exists
    for(int i = n - 1; i >= 0; i--) {
        for(int left = remainingSum; left >= nums[i]; left--) {
            current[left] = current[left] || current[(left - nums[i])];
        }
        if(current[remainingSum]) return true; // Early return when any index's remaining sum becomes 1
    }

    return false; // return dp[0][remainingSum]; in case early return branching needs to be pruned
}