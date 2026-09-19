std::vector<int> maxSlidingWindow(const std::vector<int>& nums, int k) {
    if(nums.size() < k) return {};

    std::deque<size_t> maxIdxQueue;
    
    // Initial run for first 'k' sized window
    for(size_t i = 0; i < k; i++){
        // Consume ith index and pop small elements at back
        while(!maxIdxQueue.empty() && (nums[i] > nums[maxIdxQueue.back()])) maxIdxQueue.pop_back();
        maxIdxQueue.push_back(i);
    }

    std::vector<int> maxWindowVector((nums.size() - k) + 1);
    maxWindowVector[0] = nums[maxIdxQueue.front()];

    for(size_t left = 0, right = k; right < nums.size(); right++, left++){
        // Ingest right element
        while(!maxIdxQueue.empty() && (nums[right] > nums[maxIdxQueue.back()])) maxIdxQueue.pop_back();
        maxIdxQueue.push_back(right);

        // Check if max element expired before access
        while(!maxIdxQueue.empty() && (maxIdxQueue.front() <= left)) maxIdxQueue.pop_front();

        // Push max element into vector
        maxWindowVector[left + 1] = nums[maxIdxQueue.front()];
    }
    return maxWindowVector;
}