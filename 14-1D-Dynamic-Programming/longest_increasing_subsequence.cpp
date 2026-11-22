int maxIncreaseSubsequence(const std::vector<int>& nums, int startIdx, int prevIdx) {
    if(startIdx == nums.size()) return 0;

    int count = maxIncreaseSubsequence(nums, startIdx + 1, prevIdx);
    if(prevIdx == -1 || nums[startIdx] > nums[prevIdx]) {
        count = std::max(1 + maxIncreaseSubsequence(nums, startIdx + 1, startIdx), count);
    }

    return count;
}
int lengthOfLIS(const std::vector<int>& nums) {
    return maxIncreaseSubsequence(nums, 0, -1);
}