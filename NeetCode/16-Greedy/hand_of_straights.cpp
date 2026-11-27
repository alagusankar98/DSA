bool isNStraightHand(const std::vector<int>& hand, int groupSize) {
    const int n = static_cast<int>(hand.size());
    if(n % groupSize != 0) return false;

    std::unordered_map<int, int> frequencyMap;
    frequencyMap.reserve(n);

    for(const int num : hand) {
        frequencyMap[num]++;
    }

    for(const int num : hand) {
        if(frequencyMap[num] == 0) continue;

        int groupStart = num;
        while(frequencyMap.contains(groupStart - 1) && frequencyMap[groupStart - 1] > 0) groupStart--;
        
        while(groupStart <= num) {
            auto it = frequencyMap.find(groupStart);
            while((it != frequencyMap.end()) && it->second > 0) {
                for(int i = 0; i < groupSize; i++) {
                    if(auto groupIt = frequencyMap.find(i + groupStart); (groupIt != frequencyMap.end()) && (groupIt->second > 0)) {
                        groupIt->second--;
                    } else {
                        return false;
                    }
                }
            }
            groupStart++;
        }
    }
    return true;
}