struct EdgeTime{
    int destinationNode;
    int time;
};
int networkDelayTime(const std::vector<std::vector<int>>& times, int n, int k) {
    std::vector<int> offset(n + 1, 0);
    for(const auto& time : times){
        offset[time[0] - 1]++;
    }

    for(int i = 0; i < n; i++){
        offset[i + 1] += offset[i];
    }

    std::vector<EdgeTime> edges(times.size());
    for(const auto& time : times){
        edges[--offset[time[0] - 1]] = {time[1] - 1, time[2]};
    }

    std::vector<std::uint8_t> seen(n, 0);
    using DistanceNodePair = std::pair<int, int>;
    std::vector<DistanceNodePair> vec;
    vec.reserve(times.size() + 1);
    std::priority_queue<DistanceNodePair, std::vector<DistanceNodePair>, std::greater<DistanceNodePair>> minQueue(
        std::greater<DistanceNodePair>(), std::move(vec)
    );
    
    minQueue.emplace(0, (k - 1)); // Mark kth node as initial node with smallest distance
    int lastSettledTime = 0;
    int settledNodes = 0;
    while(!minQueue.empty() && (settledNodes < n)){
        auto [minDistance, minIdx] = minQueue.top(); minQueue.pop();
        if(seen[minIdx]) continue;

        seen[minIdx] = 1; // Mark the index as seen
        settledNodes++;

        lastSettledTime = minDistance;
        // Loop through connected edges of minIdx
        for(int i = offset[minIdx]; i < offset[minIdx + 1]; i++){
            if(!seen[edges[i].destinationNode]){
                int distanceToDestination = minDistance + edges[i].time;
                minQueue.emplace(distanceToDestination, edges[i].destinationNode);
            }
        }
    }
    return (settledNodes == n) ? lastSettledTime : -1;
}