int maxIncreaseSubsequence(const std::vector<int>& nums, int n, std::vector<int>& dp, int startIdx, int prevIdx) {
    if(startIdx == n) return 0;

    // Flat array conversion (i * col + j). Offset of +1 to prevIdx to transform states from [-1...(n-2)] to [0...(n-1)]
    if(dp[startIdx * n + (prevIdx + 1)] != -1) return dp[startIdx * n + (prevIdx + 1)];

    int count = maxIncreaseSubsequence(nums, n, dp, startIdx + 1, prevIdx);
    if(prevIdx == -1 || nums[startIdx] > nums[prevIdx]) {
        count = std::max(1 + maxIncreaseSubsequence(nums, n, dp, startIdx + 1, startIdx), count);
    }

    return dp[startIdx * n + (prevIdx + 1)] = count;
}
int lengthOfLIS(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    std::vector<int> dp(n * n, -1); // 'startIdx' and 'prevIdx' both have 'n' possible states. startIdx -> [0...(n-1)], prevIdx -> [-1...(n-2)]
    return maxIncreaseSubsequence(nums, n, dp, 0, -1);
}