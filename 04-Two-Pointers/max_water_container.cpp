int maxArea(vector<int>& heights) {
    int left = 0;
    int right = std::ssize(heights) - 1;
    int maxArea = 0;
    while(left < right){
        int currentArea = std::min(heights[left], heights[right]) * (right - left);
        maxArea = std::max(maxArea, currentArea);
        if(heights[left] > heights[right]){
            right--;
        } else {
            left++;
        }
    }
    return maxArea;
}
