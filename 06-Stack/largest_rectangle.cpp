int largestRectangleArea(vector<int>& heights) {
    std::vector<int> numStack;
    numStack.reserve(heights.size());
    int maxArea = 0;
    const int n = std::ssize(heights);

    for(int i = 0; i <= n; i++){ // 7, 1, 7, 2
        // Handle strictly decreasing stack
        int currentHeight = (i == n) ? 0 : heights[i];
        while(!numStack.empty() && currentHeight < heights[numStack.back()]){
            int rightBoundary = i;
            int poppedHeight = heights[numStack.back()]; numStack.pop_back();
            int leftBoundary = (!numStack.empty()) ? numStack.back() : -1;
            int width = (rightBoundary - leftBoundary) - 1;
            maxArea = std::max(maxArea, (poppedHeight * width));
        }
        numStack.push_back(i); // 7, 1, 7, 2
    }
    
    return maxArea;
}