int binarySearch(const std::vector<int>& subset, int target) {
    const int n = static_cast<int>(subset.size());
    int left = 0;
    int right = n - 1;
    while(left <= right) {
        int mid = left + (right - left) / 2;
        if(subset[mid] >= target) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return left;
}
int lengthOfLIS(const std::vector<int>& nums) {
    if(nums.empty()) return 0;
    const int n = static_cast<int>(nums.size());
    std::vector<int> subset;
    subset.reserve(n);

    for(int i = 0; i < n; i++) {
        int targetIdx = binarySearch(subset, nums[i]);
        // Add number if current number is greater than largest element in subset
        if(targetIdx >= static_cast<int>(subset.size())) {
            subset.push_back(nums[i]);
        } else {
            // Replace first element that is >= current number
            subset[targetIdx] = nums[i];
        }
    }
    return static_cast<int>(subset.size());
}