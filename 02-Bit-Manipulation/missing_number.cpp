int missingNumber(vector<int>& nums) {
    int result = 0;
    for(int i = 0; i < static_cast<int> (nums.size()); i++){
        result = result ^ (i+1) ^ nums[i];
    }
    return result;
}