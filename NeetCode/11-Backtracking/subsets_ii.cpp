void subsetHelper(const std::vector<int>& nums, std::vector<int>& tempVector, std::vector<std::vector<int>>& resultVector, size_t i){
    if(i >= nums.size()){
        resultVector.push_back(tempVector);
        return;
    }

    tempVector.push_back(nums[i]);

    subsetHelper(nums, tempVector, resultVector, i + 1);

    tempVector.pop_back();

    while((i + 1) < nums.size() && nums[i] == nums[i+1]) i++;

    subsetHelper(nums, tempVector, resultVector, i + 1);
}
std::vector<std::vector<int>> subsetsWithDup(const std::vector<int>& nums) {
    std::vector<int> numsCopy(nums);
    std::sort(numsCopy.begin(), numsCopy.end());
    std::vector<int> tempVector;
    tempVector.reserve(nums.size());

    std::vector<std::vector<int>> resultVector;

    subsetHelper(numsCopy, tempVector, resultVector, 0);

    return resultVector;
}