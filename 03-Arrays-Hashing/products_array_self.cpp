std::vector<int> productExceptSelf(const std::vector<int>& nums) {
    const int n = std::ssize(nums);
    std::vector<int> productArray(n);
    
    productArray.front() = 1;
    for(int i = 1; i < n; i++){
        productArray[i] = productArray[i-1] * nums[i-1];
    }

    int product = 1;
    for(int i = n - 1; i > 0; --i){
        product *= nums[i];
        productArray[i-1] *= product;
    }

    return productArray;
}