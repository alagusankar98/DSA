int jump(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());

    std::vector<int> dp(n, n);
    dp[n - 1] = 0;

    for(int i = n - 1; i >= 0; i--) {
        int upperBound = std::min(n - 1, nums[i] + i); // Ceiling to last element 'n-1'
        for(int j = i; j < upperBound; j++){
            dp[i] = std::min(1 + dp[j + 1], dp[i]);
        }
    }
    return dp[0];
}