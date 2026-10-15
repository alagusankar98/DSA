void sumHelper(const std::vector<int>& nums, std::vector<int>& tempVector, int tempSum, size_t i, int target, std::vector<std::vector<int>>& resultVector){
    if(tempSum == target){
        resultVector.push_back(tempVector);
        return;
    }

    if(i >= nums.size()) return; // (tempSum > target) check not needed since it is enforced before recursive function is ever called

    if((tempSum + nums[i]) > target) return;

    tempVector.push_back(nums[i]);
    sumHelper(nums, tempVector, tempSum + nums[i], i, target, resultVector);

    tempVector.pop_back();
    sumHelper(nums, tempVector, tempSum, i + 1, target, resultVector);
}
std::vector<std::vector<int>> combinationSum(const std::vector<int>& nums, int target) {
    std::vector<std::vector<int>> resultVector;
    std::vector<int> tempVector;

    std::vector<int> numsCopy(nums.begin(), nums.end());
    std::sort(numsCopy.begin(), numsCopy.end());

    sumHelper(numsCopy, tempVector, 0, 0, target, resultVector);

    return resultVector;
}