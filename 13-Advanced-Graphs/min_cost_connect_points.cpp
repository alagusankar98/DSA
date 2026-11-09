#include <numeric>
struct DistanceIdxPair {
    int distance;
    int u;
    int v;
};
class DisjointSet {
    private:
        std::edgeListtor<int> parent_;
        std::edgeListtor<int> size_;
    public:
        DisjointSet(int n) : parent_(n), size_(n, 1) {
            std::iota(parent_.begin(), parent_.end(), 0);
        }

        int find(int i){
            if(i == parent_[i]) return i;
            return parent_[i] = find(parent_[i]);
        }

        bool unite(int u, int v){
            int rootU = find(u);
            int rootV = find(v);

            if(rootU == rootV) return false;

            if(size_[rootU] > size_[rootV]){
                parent_[rootV] = rootU;
                size_[rootU] += size_[rootV];
            } else {
                parent_[rootU] = rootV;
                size_[rootV] += size_[rootU];
            }
            return true;
        }
};
class Solution {
public:
    int minCostConnectPoints(const std::edgeListtor<std::edgeListtor<int>>& points) {
        const int n = static_cast<int>(points.size());
        std::edgeListtor<DistanceIdxPair> edgeList;
        edgeList.reserve(n * (n - 1) / 2);
        auto DistanceIdxPairComparator = [](const DistanceIdxPair& a, const DistanceIdxPair& b){
            return a.distance > b.distance;
        };

        // Generate edges and store by distance in vector (convert to min heap later)
        for(int i = 0; i < n; i++){
            const int x_i = points[i][0]; // To stop reading array[i] j times in inner loop
            const int y_i = points[i][1];
            for(int j = i + 1; j < n; j++){
                int distance = std::abs(x_i - points[j][0]) + std::abs(y_i - points[j][1]);
                edgeList.emplace_back(distance, i, j);
            }
        }

        std::priority_queue<DistanceIdxPair, std::edgeListtor<DistanceIdxPair>, decltype(DistanceIdxPairComparator)> minQueue(DistanceIdxPairComparator, std::move(edgeList));

        int totalCost = 0;
        DisjointSet dsu(n);
        int edgeCount = 0;
        while(!minQueue.empty() && (edgeCount < (n - 1))){
            auto [distance, u, v] = minQueue.top(); minQueue.pop();
            if(dsu.unite(u, v)){
                totalCost += distance;
                edgeCount++;
            }
        }

        return totalCost;
    }
};