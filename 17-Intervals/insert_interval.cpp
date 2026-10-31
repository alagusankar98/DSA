std::vector<std::vector<int>> insert(const std::vector<std::vector<int>>& intervals, const std::vector<int>& newInterval) {
    std::vector<std::array<int, 2>> mergeVector;
    mergeVector.reserve(intervals.size() + 1);

    bool insertionDone = false;

    for(const auto& currentInterval : intervals){
        if(!insertionDone && (newInterval.front() < currentInterval.front())){
            mergeVector.push_back({newInterval.front(), newInterval.back()});
            insertionDone = true;
        }
        mergeVector.push_back({currentInterval.front(), currentInterval.back()});
    }

    if(!insertionDone) mergeVector.push_back({newInterval.front(), newInterval.back()});

    // Merge overlaps
    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(mergeVector.size());
    resultVector.push_back({mergeVector.front()[0], mergeVector.front()[1]});

    for(size_t i = 1; i < mergeVector.size(); i++){
        if(mergeVector[i][0] <= resultVector.back().back()){
            // We have an overlap
            resultVector.back().back() = std::max(mergeVector[i][1], resultVector.back().back());
        } else {
            resultVector.push_back({mergeVector[i][0], mergeVector[i][1]});
        }
    }

    return resultVector;
}