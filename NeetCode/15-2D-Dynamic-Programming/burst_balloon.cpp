int maxCoinHelper(const std::vector<int>& nums, int start, int end, int n) {
    if(end < start) return 0;

    int prevVal = (start - 1) >= 0 ? nums[start - 1] : 1;
    int nextVal = (end + 1) < n ? nums[end + 1] : 1;

    int count = 0;
    for(int i = start; i <= end; i++) {
        // Assume 'i' as the balloon currently being blast
        count = std::max(count, (prevVal * nextVal * nums[i]) + maxCoinHelper(nums, start, i - 1, n) + maxCoinHelper(nums, i + 1, end, n));
    }
    return count;
}

int maxCoins(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    return maxCoinHelper(nums, 0, n - 1, n);
}