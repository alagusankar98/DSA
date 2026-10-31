int eraseOverlapIntervals(const vector<vector<int>>& intervals) {
    if(intervals.empty()) return 0;
    std::vector<std::pair<int, int>> intervalCopy;
    intervalCopy.reserve(intervals.size());

    for(const auto& interval : intervals){
        intervalCopy.emplace_back(interval[0], interval[1]);
    }

    std::sort(intervalCopy.begin(), intervalCopy.end(),
                [](const std::pair<int, int>& a, const std::pair<int, int>& b){
                    return a.second < b.second;
                });

    int count = 0;
    int lastIntervalEnd = intervalCopy[0].second;
    for(std::size_t i = 1; i < intervalCopy.size(); i++){
        if(intervalCopy[i].first < lastIntervalEnd){
            count++;
        } else{
            lastIntervalEnd = intervalCopy[i].second;
        }
    }
    return count;
}