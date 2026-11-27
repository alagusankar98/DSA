bool canJump(const std::vector<int>& nums) {
    int stepsLeft = 1;
    for(const int num : nums) {
        if(--stepsLeft < 0) return false;
        stepsLeft = std::max(num, stepsLeft);
    }
    return true;
}