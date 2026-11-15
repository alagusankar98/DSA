int robHelper(const std::vector<int>& nums, int start, int end){
    int prev1 = 0, prev2 = 0;
    for(int i = start; i < end; i++){
        int temp = std::max(nums[i] + prev2, prev1);
        prev2 = prev1;
        prev1 = temp;
    }
    return prev1;
}
int rob(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    if(n == 1) return nums[0];
    return std::max(robHelper(nums, 1, n), robHelper(nums, 0, n - 1));
}