vector<int> topKFrequent(const vector<int>& nums, int k) {
    std::unordered_map<int, int> frequencyMap;
    frequencyMap.reserve(nums.size()); // Worse case

    for(const int num : nums){
        frequencyMap[num]++;
    }

    using IntPair = std::pair<int, int>;
    std::priority_queue<IntPair, std::vector<IntPair>, std::greater<IntPair>> minQueue;
    for(const auto& [num, frequency] : frequencyMap){
        minQueue.emplace(frequency, num);
        if(std::ssize(minQueue) > k) minQueue.pop();
    }

    std::vector<int> resultVector;
    while(!minQueue.empty()){
        resultVector.push_back(minQueue.top().second);
        minQueue.pop();
    }
    return resultVector;
}