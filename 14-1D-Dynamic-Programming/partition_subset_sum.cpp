bool subsetSumExists(const std::vector<int>& nums, size_t i, int remainingSum) {
    if(remainingSum == 0) return true;
    if(remainingSum < 0 || i == nums.size()) return false;

    return subsetSumExists(nums, i + 1, remainingSum - nums[i]) || subsetSumExists(nums, i + 1, remainingSum);
}
bool canPartition(const std::vector<int>& nums) {
    int fullSum = 0;
    for(const int num : nums) {
        fullSum += num;
    }

    if(fullSum & 1) return false; // Even sum cannot be split

    // If a subset with (fullSum / 2) exists, we can conclude that another similar subset exists
    return subsetSumExists(nums, 0, (fullSum / 2));
}