int singleNumber(vector<int>& nums) {
    int result = 0;
    for(const int num : nums){
        result = result ^ num;
    }
    return result;
}