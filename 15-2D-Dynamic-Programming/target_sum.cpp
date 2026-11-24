int targetHelper(const std::vector<int>& nums, size_t i, int remaining) {
    if(i == nums.size()) return (remaining == 0);

    return targetHelper(nums, i + 1, remaining - nums[i]) + targetHelper(nums, i + 1, remaining + nums[i]);
}
int findTargetSumWays(const std::vector<int>& nums, int target) {
    return targetHelper(nums, 0, target);
}