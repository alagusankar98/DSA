void sumHelper(const std::vector<int>& nums, std::vector<int>& tempVector, int tempSum, size_t i, int target, std::vector<std::vector<int>>& resultVector){
    if(tempSum == target){
        resultVector.push_back(tempVector);
        return;
    }

    if(i >= nums.size() || tempSum > target) return;

    tempVector.push_back(nums[i]); tempSum += nums[i];

    sumHelper(nums, tempVector, tempSum, i, target, resultVector);

    tempVector.pop_back(); tempSum -= nums[i];

    sumHelper(nums, tempVector, tempSum, i + 1, target, resultVector);
}
std::vector<std::vector<int>> combinationSum(const std::vector<int>& nums, int target) {
    std::vector<std::vector<int>> resultVector;
    std::vector<int> tempVector;

    sumHelper(nums, tempVector, 0, 0, target, resultVector);

    return resultVector;
}