int trap(const vector<int>& height) {
    if(height.empty()) return 0;

    int left = 0;
    int right = std::ssize(height) - 1;
    int leftMax = height[left];
    int rightMax = height[right];
    int totalWater = 0;

    while(left < right){
        if(height[left] < height[right]){
            if(height[left] < leftMax){
                totalWater += (leftMax - height[left]);
            } else {
                leftMax = height[left];
            }
            left++;
        } else {
            if(height[right] < rightMax){
                totalWater += (rightMax - height[right]);
            } else {
                rightMax = height[right];
            }
            right--;
        }
    }
    return totalWater;
}