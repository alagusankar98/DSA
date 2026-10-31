struct Interval {
    int start;
    int end;
};
std::vector<std::vector<int>> merge(const std::vector<std::vector<int>>& intervals) {
    if(intervals.empty()) return {};

    std::vector<std::vector<int>> intervalsCopy(intervals);
    std::sort(intervalsCopy.begin(), intervalsCopy.end());

    Interval reference = {intervalsCopy[0][0], intervalsCopy[0][1]};
    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(intervalsCopy.size());

    for(std::size_t i = 1; i < intervalsCopy.size(); i++){
        if(intervalsCopy[i][0] <= reference.end){
            reference.end = std::max(reference.end, intervalsCopy[i][1]);
        } else {
            resultVector.push_back({reference.start, reference.end});
            reference = {intervalsCopy[i][0], intervalsCopy[i][1]};
        }
    }
    resultVector.push_back({reference.start, reference.end});
    return resultVector;
}