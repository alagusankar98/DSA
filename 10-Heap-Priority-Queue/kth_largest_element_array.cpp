int findKthLargest(const std::vector<int>& nums, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    for(const int num : nums){
        if(minHeap.size() < k){
            minHeap.push(num);
        } else if(num > minHeap.top()){
            minHeap.pop();
            minHeap.push(num);
        }
    }

    return minHeap.empty() ? std::numeric_limits<int>::min() : minHeap.top();
}