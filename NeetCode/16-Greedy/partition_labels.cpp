std::vector<int> partitionLabels(std::string_view s) {
    const int n = static_cast<int>(s.size());
    std::unordered_map<char, int> lastIdxMap;
    lastIdxMap.reserve(26);
    for(int i = n - 1; i >= 0; i--) {
        lastIdxMap.try_emplace(s[i], i);
    }

    std::vector<int> resultVector;
    resultVector.reserve(26); // Possible English lower characters

    int maxIdx = -1;
    int runningCount = 0;
    for(int i = 0; i < n; i++) {
        maxIdx = std::max(maxIdx, lastIdxMap.at(s[i]));
        runningCount++;

        if(i == maxIdx) {
            resultVector.push_back(runningCount);
            runningCount = 0;
            maxIdx = -1;
        }
    }
    return resultVector;
}