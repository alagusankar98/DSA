struct Interval {
    int start;
    int end;
};
std::vector<std::vector<int>> insert(const std::vector<std::vector<int>>& intervals, const std::vector<int>& newInterval) {
    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(intervals.size() + 1);
    std::size_t n = intervals.size();

    // Copy elements that are unaffected
    std::size_t i = 0;
    Interval reference = {newInterval[0], newInterval[1]};
    while((i < n) && (intervals[i][0] < newInterval[0])){
        // Exit on overlap
        if(newInterval[0] <= intervals[i][1]){
            reference = {intervals[i][0], std::max(reference.end, intervals[i][1])};
            break;
        }
        resultVector.push_back(intervals[i]);
        i++;
    }

    for(; i < n; i++){
        if(intervals[i][0] <= reference.end){
            // We have an overlap
            reference.end = std::max(intervals[i][1], reference.end);
        } else {
            resultVector.push_back({reference.start, reference.end});
            reference = {intervals[i][0], intervals[i][1]};
        }
    }

    resultVector.push_back({reference.start, reference.end});

    return resultVector;
}