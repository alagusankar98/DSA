struct Interval {
    int start;
    int end;
};
int minMeetingRooms(vector<vector<int>>& intervals) {
    if(intervals.empty()) return 0;
    std::vector<Interval> intervalCopy;
    intervalCopy.reserve(intervals.size());
    for(const auto& interval : intervals){
        intervalCopy.emplace_back(interval[0], interval[1]);
    }

    std::sort(intervalCopy.begin(), intervalCopy.end(),
    [](const Interval& a, const Interval& b){
        return a.start < b.start;
    });

    int conflictCount = 1;
    std::priority_queue<int, std::vector<int>, std::greater<int>> meetingEndQueue;
    meetingEndQueue.push(intervalCopy.front().end);

    for(std::size_t i = 1; i < intervalCopy.size(); i++){
        if(intervalCopy[i].start >= meetingEndQueue.top()){
            meetingEndQueue.pop();
        }
        meetingEndQueue.push(intervalCopy[i].end);
    }

    return static_cast<int>(meetingEndQueue.size());
}