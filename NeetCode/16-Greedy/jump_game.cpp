bool canJump(const std::vector<int>& nums) {
    int stepsLeft = 1;
    const int n = static_cast<int>(nums.size());
    for(int i = 0; i < n; i++) {
        if(--stepsLeft < 0) return false;
        stepsLeft = std::max(nums[i], stepsLeft);
        
        // Early return in case of enough fuel to directly jump to end index
        if((stepsLeft + i) >= (n - 1)) return true;
    }
    return true;
}