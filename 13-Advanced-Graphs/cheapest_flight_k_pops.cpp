struct DistanceFlightIdxPair {
    int distanceSoFar;
    int edgeCount;
    int idx;
};

struct DistanceEdgePair {
    int distance;
    int idx;
};
class Solution {
public:
    int findCheapestPrice(int n, const std::vector<std::vector<int>>& flights, int src, int dst, int k) {
        std::vector<int> offset(n + 1, 0);

        for(const auto& flight : flights){
            offset[flight[0]]++;
        }

        for(size_t i = 0; i < n; i++){
            offset[i + 1] += offset[i];
        }

        std::vector<DistanceEdgePair> edges(flights.size());
        for(const auto& flight : flights){
            edges[--offset[flight[0]]] = {flight[2], flight[1]}; // source = {distance, destination} pair
        }

        auto DistanceComparator = [](const DistanceFlightIdxPair& a, const DistanceFlightIdxPair& b){
            return a.distanceSoFar > b.distanceSoFar;
        };

        std::vector<DistanceFlightIdxPair> minQueueVector;
        minQueueVector.reserve(flights.size());
        std::priority_queue<DistanceFlightIdxPair, std::vector<DistanceFlightIdxPair>, decltype(DistanceComparator)> minQueue(DistanceComparator , std::move(minQueueVector));
        std::vector<int> costVector(n, std::numeric_limits<int>::max());

        // Push initial node into minQueue
        minQueue.emplace(0, 0, src);
        // costVector[src] = 0;
        while(!minQueue.empty()){
            auto [distanceSoFar, edgeCount, sourceIdx] = minQueue.top(); minQueue.pop();
            if(edgeCount >= costVector[sourceIdx]) continue;
            costVector[sourceIdx] = edgeCount;

            if(sourceIdx == dst) return distanceSoFar;

            if(edgeCount >= (k + 1)) continue; // Discard node since we can't travel further to absorb more than k. 

            for(int i = offset[sourceIdx]; i < offset[sourceIdx + 1]; i++){
                auto [destinationDistance, destinationNodeIdx] = edges[i];
                int totalDistance = destinationDistance + distanceSoFar;
                minQueue.emplace(totalDistance, edgeCount + 1, destinationNodeIdx);
            }
        }

        return -1;
    }
};