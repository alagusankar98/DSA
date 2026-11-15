int rob(vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    if(n == 1) return nums[0];
    std::vector<int> dp(n, 0);
    dp[0] = nums[0];
    dp[1] = std::max(nums[1], nums[0]);
    for(int i = 2; i < n; i++){
        dp[i] = std::max(nums[i] + dp[i-2], dp[i-1]);
    }
    return dp[n-1];
}