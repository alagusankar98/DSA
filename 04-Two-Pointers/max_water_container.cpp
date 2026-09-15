int maxArea(vector<int>& heights) {
    int left = 0;
    int right = std::ssize(heights) - 1;
    int maxArea = 0;
    while(left < right){
        int heightLeft = heights[left];
        int heightRight = heights[right];
        int currentArea = std::min(heightLeft, heightRight) * (right - left);
        maxArea = std::max(maxArea, currentArea);
        if(heightLeft > heightRight){
            while((left < right) && (heights[right] <= heightRight)) right--;
        } else {
            while((left < right) && (heights[left] <= heightLeft)) left++;
        }
    }
    return maxArea;
}