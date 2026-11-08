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

    using DistanceNodePair = std::pair<int, int>;
    std::vector<DistanceNodePair> vec;
    vec.reserve(times.size() + 1);
    std::priority_queue<DistanceNodePair, std::vector<DistanceNodePair>, std::greater<DistanceNodePair>> minQueue(
        std::greater<DistanceNodePair>(), std::move(vec)
    );
    std::vector<int> distanceVector(n, std::numeric_limits<int>::max());
    
    distanceVector[k - 1] = 0; // Mark kth node as initial node
    minQueue.emplace(0, (k - 1)); // Mark kth node as initial node with smallest distance
    int lastSettledTime = 0;
    int settledNodes = 0;
    while(!minQueue.empty() && (settledNodes < n)){
        auto [minDistance, minIdx] = minQueue.top(); minQueue.pop();
        if(minDistance > distanceVector[minIdx]) continue; // A cheaper path replaced this entry's d[v] after it was originally pushed, Discard this

        settledNodes++;
        lastSettledTime = minDistance;
        // Loop through connected edges of minIdx
        for(int i = offset[minIdx]; i < offset[minIdx + 1]; i++){
            int distanceToDestination = minDistance + edges[i].time;
            if (distanceToDestination < distanceVector[edges[i].destinationNode]){
                distanceVector[edges[i].destinationNode] = distanceToDestination;
                minQueue.emplace(distanceToDestination, edges[i].destinationNode);
            }
        }
    }
    return (settledNodes == n) ? lastSettledTime : -1;
}