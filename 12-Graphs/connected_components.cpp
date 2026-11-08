#include <numeric>
#include <cassert>
class DisjointSet {
    private:
        std::vector<int> parent_;
        std::vector<int> size_;
    public:
        DisjointSet(int n) : parent_(n), size_(n, 1) {
            std::iota(parent_.begin(), parent_.end(), 0);
        }

        int find(int i){
            if (parent_[i] == i) return i;
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

        int countParents(){
            int count = 0;
            for(int i = 0; i < static_cast<int>(parent_.size()); i++){
                if(parent_[i] == i) count++;
            }
            return count;
        }
};
class Solution {
public:
    int countComponents(int n, const std::vector<std::vector<int>>& edges) {
        DisjointSet dsu(n);

        for(const auto& edge : edges){
            assert(edge[0] >= 0 && edge[0] < n && edge[1] >= 0 && edge[1] < n);
            dsu.unite(edge[0], edge[1]);
        }

        return dsu.countParents();
    }
};