int longestConsecutive(const std::vector<int>& nums) {
    std::unordered_set<int> uniqueNums;
    uniqueNums.reserve(nums.size());
    
    for(const int num : nums){
        uniqueNums.insert(num);
    }

    int longestSequence = 0;
    for(int num : uniqueNums){
        if(!uniqueNums.contains(num - 1)){
            int count = 1;
            while(uniqueNums.contains(++num)){
                count++;
            }
            longestSequence = std::max(count, longestSequence);
        }
    }

    return longestSequence;
}