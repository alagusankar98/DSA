std::vector<int> partitionLabels(std::string_view s) {
    const int n = static_cast<int>(s.size());
    std::array<int, 26> lastIdxMap;
    for(int i = 0; i < n; i++) {
        lastIdxMap[s[i] - 'a'] = i;
    }

    std::vector<int> resultVector;
    resultVector.reserve(26); // Possible English lower characters

    int startIdx = 0;
    int endIdx = startIdx;
    for(int i = 0; i < n; i++) {
        endIdx = std::max(endIdx, lastIdxMap[s[i] - 'a']);

        if(i == endIdx) {
            resultVector.push_back(endIdx - startIdx + 1);
            startIdx = i + 1;
            endIdx = startIdx;
        }
    }
    return resultVector;
}