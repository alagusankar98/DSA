int rob(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    if(n == 1) return nums[0];
    int prev1 = 0;
    auto robHelper = [&prev1, &nums](int start, int end){
        int prev2 = 0;
        for(int i = start; i < end; i++){
            int temp = std::max(nums[i] + prev2, prev1);
            prev2 = prev1;
            prev1 = temp;
        }
    };
    robHelper(1, n);

    int bestVal = prev1;
    prev1 = 0;
    robHelper(0, n - 1);

    return std::max(prev1, bestVal);
}