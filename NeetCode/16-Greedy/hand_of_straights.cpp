bool isNStraightHand(const std::vector<int>& hand, int groupSize) {
    const int n = static_cast<int>(hand.size());
    if(n % groupSize != 0) return false;

    std::vector<int> handCopy(hand);
    std::sort(handCopy.begin(), handCopy.end());

    std::unordered_map<int, int> frequencyMap;
    frequencyMap.reserve(hand.size());

    for(const int num : hand) {
        frequencyMap[num]++;
    }

    for(const int num : handCopy) {
        if(frequencyMap[num] == 0) continue;
        
        for(int i = 0; i < groupSize; i++) {
            if(auto it = frequencyMap.find(i + num); (it != frequencyMap.end()) && (it->second > 0)) {
                it->second--;
            } else {
                return false;
            }
        }
    }
    return true;
}