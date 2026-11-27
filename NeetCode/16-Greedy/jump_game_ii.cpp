int jumpHelper(const std::vector<int>& nums, int n, int i) {
    if((i + 1) >= n) return 0;

    int count = std::numeric_limits<int>::max();
    for(int j = i; j < (nums[i] + i); j++){
        count = std::min(jumpHelper(nums, n, j + 1), count);
    }

    return (count == std::numeric_limits<int>::max()) ? std::numeric_limits<int>::max() : (1 + count);
}
int jump(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());

    return jumpHelper(nums, n, 0);
}