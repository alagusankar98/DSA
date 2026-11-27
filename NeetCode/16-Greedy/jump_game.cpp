bool canJump(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    int farthestIndex = 0;

    for(int i = 0; i < n; i++) {
        if(farthestIndex < i) return false;
        farthestIndex = std::max(i + nums[i], farthestIndex);
        if(farthestIndex >= (n - 1)) return true;
    }
    return true;
}