int jumpHelper(const std::vector<int>& nums, int n, int i, std::vector<int>& dp) {
    if((i + 1) >= n) return 0;

    if(dp[i] != -1) return dp[i];

    int count = n;
    for(int j = i; j < (nums[i] + i); j++){
        count = std::min(1 + jumpHelper(nums, n, j + 1, dp), count);
    }

    return dp[i] = count;
}
int jump(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());

    std::vector<int> dp(n, -1);
    return jumpHelper(nums, n, 0, dp);
}