int findCheapestPrice(int n, const std::vector<std::vector<int>>& flights, int src, int dst, int k) {
    static constexpr int INT_MAX_VAL = std::numeric_limits<int>::max();
    std::vector<int> prev(n, INT_MAX_VAL);
    prev[src] = 0;
    std::vector<int> current(prev);
    for(int i = 0; i <= k; i++){
        bool changed = false;
        for(const auto& flight : flights){
            int u = flight[0];
            int v = flight[1];
            if(prev[u] != INT_MAX_VAL && (current[v] > prev[u] + flight[2])){
                changed = true;
                current[v] = prev[u] + flight[2];
            }
        }
        if(!changed) return (current[dst] != INT_MAX_VAL) ? current[dst] : -1;
        prev = current;
    }
    return (prev[dst] != INT_MAX_VAL) ? prev[dst] : -1;
}