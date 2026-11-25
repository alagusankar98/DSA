vector<int> twoSum(const vector<int>& nums, int target) {
    std::unordered_map<int, int> indexMap;
    indexMap.reserve(nums.size());
    for(int i = 0; i < std::ssize(nums); i++){
        if(auto it = indexMap.find(target - nums[i]); it != indexMap.end()){
            return {it->second, i};
        }
        indexMap.try_emplace(nums[i], i);
    }
    return {};
}