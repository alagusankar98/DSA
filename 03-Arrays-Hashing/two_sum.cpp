vector<int> twoSum(const vector<int>& nums, int target) {
    std::unordered_map<int, int> indexMap;
    for(size_t i = 0; i < nums.size(); i++){
        if(auto it = indexMap.find(target - nums[i]); it != indexMap.end()){
            return {it->second, static_cast<int>(i)};
        }
        indexMap.insert(std::make_pair(nums[i], static_cast<int>(i)));
    }
    return {};
}