int maxProduct(const std::vector<int>& nums) {
    int maxResult = std::numeric_limits<int>::min();
    const int n = static_cast<int>(nums.size());
    int product = 1;
    for(const int num : nums) {
        product *= num;
        maxResult = std::max(maxResult, product);
        if(product == 0) product = 1;
    }

    product = 1;
    for(int i = n - 1; i >= 0; i--) {
        product *= nums[i];
        maxResult = std::max(maxResult, product);
        if(product == 0) product = 1;
    }
    return maxResult;
}