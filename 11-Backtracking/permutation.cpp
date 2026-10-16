int factorial(int n){
    if(n == 0 || n == 1) return 1;
    return n * factorial(n - 1);
}
void permutationHelper(const std::vector<int>& nums, std::vector<std::vector<int>>& resultVector, std::vector<int>& tempVector, std::unordered_set<size_t>& seen){
    if(tempVector.size() >= nums.size()){
        resultVector.push_back(tempVector);
        return;
    }

    for(size_t i = 0; i < nums.size(); i++){
        if(seen.contains(i)) continue;
        seen.insert(i); tempVector.push_back(nums[i]);
        permutationHelper(nums, resultVector, tempVector, seen);
        seen.erase(i); tempVector.pop_back();
    }
}
std::vector<std::vector<int>> permute(const std::vector<int>& nums) {
    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(factorial(std::ssize(nums)));
    std::vector<int> tempVector;
    tempVector.reserve(nums.size());
    std::unordered_set<size_t> seen;
    
    permutationHelper(nums, resultVector, tempVector, seen);

    return resultVector;
}