vector<int> topKFrequent(const vector<int>& nums, int k) {
    std::unordered_map<int, int> frequencyMap;
    frequencyMap.reserve(nums.size()); // Worse case

    for(const int num : nums){
        frequencyMap[num]++;
    }

    std::priority_queue<std::pair<int, int>> maxQueue;
    // maxQueue.reserve(frequencyMap.size());
    for(const auto& [num, frequency] : frequencyMap){
        maxQueue.push({frequency, num});
    }

    std::vector<int> resultVector;
    resultVector.reserve(k); // Worse case
    while(!maxQueue.empty()){
        auto [_, num] = maxQueue.top();
        maxQueue.pop();
        resultVector.push_back(num);
        if(--k == 0) break;
    }
    return resultVector;
}