int search(const vector<int>& nums, int target) {
    const int n = static_cast<int>(nums.size());
    int left = 0;
    int right = n - 1;

    while(left <= right){
        int mid = left + ((right - left) / 2);

        if(nums[mid] == target){
            return static_cast<int>(mid);
        } else if (nums[mid] > target){
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return -1;
}