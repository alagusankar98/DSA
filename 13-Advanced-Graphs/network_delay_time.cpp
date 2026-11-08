struct EdgeTime{
    int destinationNode;
    int time;
};
int networkDelayTime(const std::vector<std::vector<int>>& times, int n, int k) {
    std::vector<int> offset(n + 1, 0);
    static constexpr int MAX_TIME = std::numeric_limits<int>::max();

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
    std::vector<int> distanceVector(n, MAX_TIME);

    distanceVector[k - 1] = 0; // Mark kth node as initial node
    int lastSettledTime = 0;
    for(int read = 0; read < n; read++){
        // Find unseen node with minimum distance
        int minIdx = -1;
        int minDistance = MAX_TIME;
        for(int i = 0; i < n; i++){
            if(!seen[i]) {
                if (distanceVector[i] < minDistance){
                    minDistance = distanceVector[i];
                    minIdx = i;
                }
            }
        }

        if(minIdx == -1) return -1; // Return early if we can't find a node, means there is atleast one non-reachable node
        seen[minIdx] = 1; // Mark the index as seen
        lastSettledTime = minDistance; // minDistance increases over time and this is the max value seen so far
        // Loop through connected edges of minIdx
        for(int i = offset[minIdx]; i < offset[minIdx + 1]; i++){
            int distanceToDestination = minDistance + edges[i].time;
            if(distanceVector[edges[i].destinationNode] > distanceToDestination){
                distanceVector[edges[i].destinationNode] = distanceToDestination;
            }
        }
    }
    return lastSettledTime;
}