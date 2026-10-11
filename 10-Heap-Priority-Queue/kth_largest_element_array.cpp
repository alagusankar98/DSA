int findKthLargest(const std::vector<int>& nums, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    assert(!nums.empty() || k == 0);

    for(const int num : nums){
        if(std::ssize(minHeap) < k){
            minHeap.push(num);
        } else if(num > minHeap.top()){
            minHeap.pop();
            minHeap.push(num);
        }
    }

    return minHeap.top();
}