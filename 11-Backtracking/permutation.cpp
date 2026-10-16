int factorial(int n){
    if(n == 0 || n == 1) return 1;
    return n * factorial(n - 1);
}
void permutationHelper(const std::vector<int>& nums, std::vector<std::vector<int>>& resultVector, std::vector<int>& tempVector, std::unordered_set<int> seen){
    if(tempVector.size() >= nums.size()){
        resultVector.push_back(tempVector);
        return;
    }

    for(const auto num : nums){
        if(seen.contains(num)) continue;
        seen.insert(num); tempVector.push_back(num);
        permutationHelper(nums, resultVector, tempVector, seen);
        seen.erase(num); tempVector.pop_back();
    }
}
std::vector<std::vector<int>> permute(const std::vector<int>& nums) {
    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(factorial(std::ssize(nums)));
    std::vector<int> tempVector;
    tempVector.reserve(nums.size());
    std::unordered_set<int> seen;
    
    permutationHelper(nums, resultVector, tempVector, seen);

    return resultVector;
}