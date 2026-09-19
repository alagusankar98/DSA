std::vector<int> maxSlidingWindow(const std::vector<int>& nums, int k) {
        size_t k_size = static_cast<size_t>(k);
        if(nums.size() < k) return {};

        std::deque<size_t> maxIdxQueue;

        std::vector<int> maxWindowVector;
        maxWindowVector.reserve((nums.size() - k_size) + 1);

        for(size_t i = 0; i < nums.size(); i++){
            // Ingest right element after deleting all smaller elements from back (including duplicates)
            while(!maxIdxQueue.empty() && (nums[i] >= nums[maxIdxQueue.back()])) maxIdxQueue.pop_back();
            maxIdxQueue.push_back(i);

            // Check if max element expired before access
            if(!maxIdxQueue.empty() && ((maxIdxQueue.front() + k_size) <= i)) maxIdxQueue.pop_front();

            // Push max element into vector
            if((i + 1) >= k_size) maxWindowVector.push_back(nums[maxIdxQueue.front()]);
        }
        return maxWindowVector;
    }