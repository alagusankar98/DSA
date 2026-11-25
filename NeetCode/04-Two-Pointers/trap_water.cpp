int trap(const vector<int>& height) {
    if(height.empty()) return 0;

    int left = 0;
    int right = std::ssize(height) - 1;
    int leftMax = height[left];
    int rightMax = height[right];
    int totalWater = 0;

    while(left < right){
        if(height[left] < height[right]){
            leftMax = std::max(leftMax, height[left]);
            totalWater += (leftMax - height[left]);
            left++;
        } else {
            rightMax = std::max(rightMax, height[right]);
            totalWater += (rightMax - height[right]);
            right--;
        }
    }
    return totalWater;
}