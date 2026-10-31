std::vector<std::vector<int>> insert(const std::vector<std::vector<int>>& intervals, const std::vector<int>& newInterval) {
    std::vector<std::vector<int>> mergeVector;
    mergeVector.reserve(intervals.size() + 1);

    bool insertionDone = false;

    for(const auto& currentInterval : intervals){
        if(!insertionDone && (newInterval.front() < currentInterval.front())){
            mergeVector.push_back(newInterval);
            insertionDone = true;
        }
        mergeVector.push_back(currentInterval);
    }

    if(!insertionDone) mergeVector.push_back(newInterval);

    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(mergeVector.size());
    resultVector.push_back(std::move(mergeVector.front()));

    for(size_t i = 1; i < mergeVector.size(); i++){
        auto& currentInterval = mergeVector[i];
        if(currentInterval.front() >= resultVector.back().front() && currentInterval.front() <= resultVector.back().back()){
            // We have an overlap
            resultVector.back().back() = std::max(currentInterval.back(), resultVector.back().back());
        } else {
            resultVector.push_back(std::move(currentInterval));
        }
    }
    return resultVector;
}