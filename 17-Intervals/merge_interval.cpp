struct Interval {
    int start;
    int end;
};

std::vector<std::vector<int>> merge(const std::vector<std::vector<int>>& intervals) {
    if(intervals.empty()) return {};

    std::vector<Interval> intervalsCopy;
    intervalsCopy.reserve(intervals.size());
    for(auto const& interval : intervals) {
        intervalsCopy.emplace_back(interval[0], interval[1]);
    }

    std::sort(intervalsCopy.begin(), intervalsCopy.end(), 
                [](const Interval& a, const Interval &b){
                    return a.start < b.start;
                });

    Interval reference = intervalsCopy[0];
    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(intervalsCopy.size());

    for(std::size_t i = 1; i < intervalsCopy.size(); i++){
        if(intervalsCopy[i].start <= reference.end){
            reference.end = std::max(reference.end, intervalsCopy[i].end);
        } else {
            resultVector.push_back({reference.start, reference.end});
            reference = intervalsCopy[i];
        }
    }
    resultVector.push_back({reference.start, reference.end});
    return resultVector;
}