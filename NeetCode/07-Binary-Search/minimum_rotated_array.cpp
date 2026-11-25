int findMin(const std::vector<int> &nums) {
    if (nums.empty()) return -1; // Invalid array
    const int n = static_cast<int>(nums.size());
    int left = 0;
    int right = n - 1;

    while(left <= right){
        if(nums[left] <= nums[right]){
            // we have a sorted segment, left must be the lowest element
            // "<=" needed to exit in case of single element bounday like (left == right)
            return nums[left];
        }
        int mid = left + (right - left) / 2;

        if(nums[mid] < nums[right]){
            // We have perfectly sorted right half. Discard right half, but DO NOT discard
            // the mid element, since it could be the global minimum as well
            right = mid;
        } else {
            // Start of segment greater than end. Means minimum element is here somewhere
            left = mid + 1;
        }
    }
    return -1;
}