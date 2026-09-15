std::vector<int> twoSum(const vector<int>& numbers, int target) {
    int left = 0;
    int right = std::ssize(numbers) - 1;
    while(left < right){
        int difference = numbers[left] + numbers[right] - target;
        if(difference == 0) return {left+1, right+1};
        if(difference > 0){
            right--;
        } else {
            left++;
        }
    }
    return {};
}