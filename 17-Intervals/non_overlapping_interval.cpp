int eraseOverlapIntervals(const vector<vector<int>>& intervals) {
    if(intervals.empty()) return 0;
    std::vector<std::pair<int, int>> intervalCopy;

    for(const auto& interval : intervals){
        intervalCopy.emplace_back(interval[0], interval[1]);
    }

    std::sort(intervalCopy.begin(), intervalCopy.end());

    int count = 0;
    std::pair<int, int> reference = intervalCopy[0];
    for(std::size_t i = 1; i < intervalCopy.size(); i++){
        if(intervalCopy[i].first >= reference.first && intervalCopy[i].first < reference.second){
            count++;
            reference.second = std::min(reference.second, intervalCopy[i].second);
        } else {
            reference = intervalCopy[i];
        }
    }
    return count;
}