int maxSubArray(const std::vector<int>& nums) {
    int sum = 0;
    int maxSum = std::numeric_limits<int>::min();

    for(const int num : nums) {
        if(sum < 0) {
            sum = 0;
        }
        sum += num;
        maxSum = std::max(sum, maxSum);
    }

    return maxSum;
}