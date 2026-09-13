bool hasDuplicate(const vector<int>& nums) {
    std::unordered_set<int> seenNumbers;
    for(const int num : nums){
        if (seenNumbers.contains(num)) return true;
        seenNumbers.insert(num);
    }
    return false;
}