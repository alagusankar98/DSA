int jump(const std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());

    int numberOfJumps = 0;
    int currentFarthestIdx = 0;
    int consumeIdx = 0;

    while (currentFarthestIdx < (n - 1)) {
        int nextFarthestIdx = -1;
        // Consume every node till currentFarthestIdx
        for(; consumeIdx <= currentFarthestIdx; consumeIdx++) {
            nextFarthestIdx = std::max(nextFarthestIdx, consumeIdx + nums[consumeIdx]);
        }

        if(currentFarthestIdx == nextFarthestIdx) return -1; // Next group does not reach beyond current group

        currentFarthestIdx = nextFarthestIdx;
        numberOfJumps++;
    }

    return numberOfJumps;
}