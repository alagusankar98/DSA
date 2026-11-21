int maxProduct(const std::vector<int>& nums) {
    int maxResult = std::numeric_limits<int>::min();
    int currentMinProduct = 1;
    int currentMaxProduct = 1;
    for(const int num : nums) {
        if(num <  0) std::swap(currentMaxProduct, currentMinProduct);
        currentMaxProduct = std::max(num, currentMaxProduct * num);
        currentMinProduct = std::min(num, currentMinProduct * num);
        maxResult = std::max(maxResult, currentMaxProduct);
    }
    return maxResult;
}