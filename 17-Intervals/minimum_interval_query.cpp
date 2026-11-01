struct Interval {
    int start;
    int end;
};

std::vector<int> minInterval(const std::vector<std::vector<int>>& intervals, const std::vector<int>& queries) {

    std::vector<Interval> intervalsCopy;
    intervalsCopy.reserve(intervals.size());
    for(const auto& interval : intervals){
        intervalsCopy.emplace_back(interval[0], interval[1]);
    }

    std::sort(intervalsCopy.begin(), intervalsCopy.end(), 
    [](const Interval& a, const Interval& b){
        return a.start < b.start;
    });

    std::vector<std::pair<int,std::size_t>> queriesCopy;
    queriesCopy.reserve(queries.size());

    for(std::size_t i = 0; i < queries.size(); i++){
        queriesCopy.emplace_back(queries[i], i);
    }

    std::sort(queriesCopy.begin(), queriesCopy.end(),
    [](const std::pair<int,std::size_t>& a, const std::pair<int,std::size_t>& b){
        return a.first < b.first;
    });
    
    using DistanceEndIntervalPair = std::pair<int, int>;
    std::priority_queue<DistanceEndIntervalPair, std::vector<DistanceEndIntervalPair>, std::greater<DistanceEndIntervalPair>> minQueue;
    std::vector<int> resultVector(queries.size(), -1); // Initialize all to '-1' to indicate not found. Update only on valid queries
    std::size_t currentIntervalIdx = 0;

    for(auto& [queryVal, queryIdx] : queriesCopy){

        // Check for push to min queue
        while((currentIntervalIdx < intervalsCopy.size()) && (queryVal >= intervalsCopy[currentIntervalIdx].start)){
            // First push all intervals that may contain query val before checking for min val
            minQueue.emplace((intervalsCopy[currentIntervalIdx].end - intervalsCopy[currentIntervalIdx].start + 1), intervalsCopy[currentIntervalIdx].end);
            currentIntervalIdx++;
        }

        // Once min queue contains all intervals applicable for current query, proceed to access it

        while(!minQueue.empty() && minQueue.top().second < queryVal) minQueue.pop(); // Discard minQueue entries whose end interval exceeds query value

        if(!minQueue.empty()){
            // Top element in min heap will now contain the interval that answers query with min distance
            resultVector[queryIdx] = minQueue.top().first;
        }
    }

    return resultVector;
}