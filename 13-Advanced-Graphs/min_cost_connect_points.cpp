#include <numeric>
struct DistanceIdxPair {
    int distance;
    int u;
    int v;
};
class DisjointSet {
    private:
        std::vector<int> parent_;
        std::vector<int> size_;
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
    int minCostConnectPoints(const std::vector<std::vector<int>>& points) {
        const int n = static_cast<int>(points.size());
        std::vector<DistanceIdxPair> vec;
        vec.reserve(n * (n - 1) / 2);
        auto DistanceIdxPairComparator = [](const DistanceIdxPair& a, const DistanceIdxPair& b){
            return a.distance > b.distance;
        };
        std::priority_queue<DistanceIdxPair, std::vector<DistanceIdxPair>, decltype(DistanceIdxPairComparator)> minQueue(DistanceIdxPairComparator, std::move(vec));

        // Generate edges and store by distance in min-heap
        for(int i = 0; i < n; i++){
            for(int j = i + 1; j < n; j++){
                int distance = std::abs(points[i][0] - points[j][0]) + std::abs(points[i][1] - points[j][1]);
                minQueue.emplace(distance, i, j);
            }
        }

        int totalCost = 0;
        DisjointSet dsu(n);
        while(!minQueue.empty()){
            auto [distance, u, v] = minQueue.top(); minQueue.pop();
            if(dsu.unite(u, v)) totalCost += distance;
        }

        return totalCost;
    }
};