int search(const std::vector<int>& nums, int target) {
    if(nums.empty()) return -1;
    const int n = static_cast<int>(nums.size());
    int left = 0;
    int right = n -1;

    while(left <= right){
        int mid = left + (right - left) / 2;

        if (nums[mid] == target){
            return mid;
        }
        
        if((nums[mid] < nums[right])){
            // Right half is sorted. mid COULD NOT be in further search space
            // since we are in else if proving (nums[mid] != target)
            if(target > nums[mid] && target <= nums[right]){
                // Target found in right half, discard left half
                left = mid + 1;
            } else {
                // Target not in right half, discard it
                right = mid - 1;
            }
        } else {
            // Left half is sorted.
            if(target >= nums[left] && target < nums[mid]){
                // Target found in left half, discard right half
                right = mid - 1;
            } else {
                // Target not in left half, discard it
                left = mid + 1;
            }
        }
    }

    return -1;
}