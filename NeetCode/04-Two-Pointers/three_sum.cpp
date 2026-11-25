std::vector<std::vector<int>> threeSum(std::vector<int>& nums) {
    std::vector<std::vector<int>> resultVector;
    std::sort(nums.begin(), nums.end());
    const int n = std::ssize(nums);
    for(int i = 0; i < n; i++){
        if(nums[i] > 0) break; // Smallest number became positive
        if((i > 0) && (nums[i] == nums[i-1])){
            continue;
        }
        int left = i + 1;
        int right = n - 1;
        
        while(left < right){
            auto sum = nums[left] + nums[right] + nums[i];
            if(sum == 0){
                resultVector.push_back({nums[i], nums[left], nums[right]});
                while((left < right) && (nums[left] == nums[left+1])) left++;
                while((left < right) && (nums[right] == nums[right-1])) right--;
                right--;
                left++;
            } else if (sum > 0){
                right--;
            } else {
                left++;
            }
        }
    }
    return resultVector;
}