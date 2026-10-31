bool canAttendMeetings(const std::vector<Interval>& intervals) {
    if(intervals.empty()) return true;

    std::vector<Interval> intervalsCopy(intervals);

    std::sort(intervalsCopy.begin(), intervalsCopy.end(), 
    [](const Interval& a, const Interval& b){
        return a.end < b.end;
    });

    int lastMeetEnd = intervalsCopy.front().end;
    for(std::size_t i = 1; i < intervalsCopy.size(); i++){
        if(intervalsCopy[i].start < lastMeetEnd){
            return false;
        }
        lastMeetEnd = intervalsCopy[i].end;
    }

    return true;
}