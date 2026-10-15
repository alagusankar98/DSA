void combinationHelper(const std::vector<int>& candidates, std::vector<std::vector<int>>& resultVector, std::vector<int>& tempVector, size_t i, int tempSum, int target){
    if(tempSum == target){
        resultVector.push_back(tempVector);
        return;
    }

    if(i >= candidates.size() || tempSum > target) return;

    tempVector.push_back(candidates[i]);

    combinationHelper(candidates, resultVector, tempVector, i+1, tempSum + candidates[i], target);

    tempVector.pop_back();

    while((i + 1) < candidates.size() && candidates[i] == candidates[i+1]) i++;

    combinationHelper(candidates, resultVector, tempVector, i+1, tempSum, target);
}
std::vector<std::vector<int>> combinationSum2(const std::vector<int>& candidates, int target) {
    std::vector<std::vector<int>> resultVector;
    std::vector<int> tempVector;
    std::vector<int> candidatesCopy(candidates);
    std::sort(candidatesCopy.begin(), candidatesCopy.end());
    combinationHelper(candidatesCopy, resultVector, tempVector, 0, 0, target);
    return resultVector;
}