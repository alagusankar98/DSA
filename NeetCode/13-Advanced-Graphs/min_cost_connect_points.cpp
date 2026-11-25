int minCostConnectPoints(const std::vector<std::vector<int>>& points) {
    const int n = static_cast<int>(points.size());
    std::vector<int> distanceVector(n, std::numeric_limits<int>::max());
    std::vector<uint8_t> seen(n, 0);
    
    int nextNodeIdx = 0;
    int minCost = 0;
    for(int i = 1; i < n; i++){ // First node is already processed
        int minDistance = std::numeric_limits<int>::max();
        int currentNodeIdx = nextNodeIdx;
        seen[currentNodeIdx] = 1; // Mark node as seen before processing it
        for(int j = 0; j < n; j++){
            if(!seen[j]){
                int distance = std::abs(points[currentNodeIdx][1] - points[j][1]) + std::abs(points[currentNodeIdx][0] - points[j][0]);
                distanceVector[j] = std::min(distanceVector[j], distance);
                if(minDistance > distanceVector[j]){
                    minDistance = distanceVector[j];
                    nextNodeIdx = j;
                }
            }
        }
        minCost += minDistance;
    }
    return minCost;
}