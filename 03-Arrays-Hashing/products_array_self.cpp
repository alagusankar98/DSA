std::vector<int> productExceptSelf(const std::vector<int>& nums) {
    if(nums.empty()) return {};
    std::vector<int> productArray(nums.size());
    productArray.front() = 1;
    const int n = std::ssize(nums);

    for(int i = 1; i < n; i++){
        productArray[i] = productArray[i-1] * nums[i-1];
    }

    int product = 1;
    for(int i = n - 1; i > 0; --i){
        product *= nums[i];
        productArray[i-1] *= product;
    }

    return std::move(productArray);
}