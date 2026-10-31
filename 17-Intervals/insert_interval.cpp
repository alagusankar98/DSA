struct Interval {
    int start;
    int end;
};
std::vector<std::vector<int>> insert(const std::vector<std::vector<int>>& intervals, const std::vector<int>& newInterval) {
    std::vector<Interval> mergeVector;
    mergeVector.reserve(intervals.size() + 1);

    bool insertionDone = false;

    for(const auto& currentInterval : intervals){
        if(!insertionDone && (newInterval[0] < currentInterval[0])){
            mergeVector.emplace_back(newInterval[0], newInterval[1]);
            insertionDone = true;
        }
        mergeVector.emplace_back(currentInterval[0], currentInterval[1]);
    }

    if(!insertionDone) mergeVector.emplace_back(newInterval[0], newInterval[1]);

    // Merge overlaps
    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(mergeVector.size());

    Interval reference = mergeVector[0];

    for(std::size_t i = 1; i < mergeVector.size(); i++){
        if(mergeVector[i].start <= reference.end){
            // We have an overlap
            reference.end = std::max(mergeVector[i].end, reference.end);
        } else {
            resultVector.push_back({reference.start, reference.end});
            reference = mergeVector[i];
        }
    }

    resultVector.push_back({reference.start, reference.end});

    return resultVector;
}