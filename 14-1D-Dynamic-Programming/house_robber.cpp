int rob(vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    if(n < 3) return *std::max_element(nums.begin(), nums.end());
    std::vector<int> dp(n, 0);
    dp[0] = nums[0];
    dp[1] = nums[1];
    dp[2] = nums[0] + nums[2];
    for(int i = 3; i < n; i++){
        dp[i] = nums[i] + std::max(dp[i-2], dp[i-3]);
    }
    return std::max(dp[n-1], dp[n-2]);
}