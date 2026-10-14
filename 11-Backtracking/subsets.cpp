void createSubset(const std::vector<int>& nums, size_t i, std::vector<int>& tempVector, std::vector<std::vector<int>>& resultVector){
    if(i >= nums.size()){
        resultVector.push_back(tempVector);
        return;
    }

    tempVector.push_back(nums[i]);

    createSubset(nums, i + 1, tempVector, resultVector);
    
    tempVector.pop_back();

    createSubset(nums, i + 1, tempVector, resultVector);
}

std::vector<std::vector<int>> subsets(const vector<int>& nums) {
    std::vector<std::vector<int>> resultVector;
    std::vector<int> tempVector;
    createSubset(nums, 0, tempVector, resultVector);
    return resultVector;
}