struct DistanceIndexPair{
    int Distance;
    int nodeIdx;
};
class Solution {
public:
    int minCostConnectPoints(vector<vector<int>>& points) {
        const int n = static_cast<int>(points.size());
        std::vector<int> distanceVector(n, std::numeric_limits<int>::max());

        auto DistanceComparator = [](const DistanceIndexPair& a, const DistanceIndexPair& b){
            return a.Distance > b.Distance;
        };
        
        std::vector<DistanceIndexPair> minQueueVector;
        minQueueVector.reserve(n * (n - 1) / 2);
        std::priority_queue<DistanceIndexPair, std::vector<DistanceIndexPair>, decltype(DistanceComparator)> minQueue(DistanceComparator, std::move(minQueueVector));
        std::vector<std::uint8_t> seen(n, 0);

        // Marking Node 0 as initial node
        distanceVector[0] = 0; 
        minQueue.emplace(0,0);

        int minCost = 0;
        int nodeCount = 0;

        while(!minQueue.empty() && (nodeCount < n)){
            auto [minDistance, nodeIdx] = minQueue.top(); minQueue.pop();
            
            if(seen[nodeIdx]) continue; // Skip if node is already processed, since (nodeIdx, i) pairs are already in min heap
            seen[nodeIdx] = 1;
            nodeCount++;
            minCost += minDistance;

            const int x_idx = points[nodeIdx][0];
            const int y_idx = points[nodeIdx][1];
            for(int i = 0 ; i < n; i++){
                // Distance between nodeIdx and i
                if(!seen[i]){
                    int distance = std::abs(points[i][0] - x_idx) + std::abs(points[i][1] - y_idx);
                    if(distance < distanceVector[i]){
                        distanceVector[i] = distance;
                        minQueue.emplace(distance, i);
                    }
                }
            }
        }
        return minCost;
    }
};