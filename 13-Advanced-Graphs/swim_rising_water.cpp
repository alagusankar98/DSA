struct TimeIdxPair {
	int maxTime;
	int i;
	int j;
};
class Solution {
public:
    int swimInWater(const std::vector<std::vector<int>>& grid) {
        const int n = static_cast<int>(grid.size());
		static constexpr std::array<std::pair<int, int>, 4> directionArray = {{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}};
		
		auto TimeIdxPairComparator = [](const TimeIdxPair& a, const TimeIdxPair& b) {
			return a.maxTime > b.maxTime;
		};
		
		std::vector<TimeIdxPair> vec;
		vec.reserve(4*n); // ~4n edges for 4 directional n nodes
		std::priority_queue<TimeIdxPair, std::vector<TimeIdxPair>, decltype(TimeIdxPairComparator)> minQueue(TimeIdxPairComparator, std::move(vec));
		std::vector<std::uint8_t> seen(n*n, 0);

		// Mark (0,0) as initial node
		seen[0] = 1;
		minQueue.emplace(grid[0][0], 0, 0);
		
		int maxTimeSoFar = std::numeric_limits<int>::min();
		while(!minQueue.empty()){
			auto [maxTime, r, c] = minQueue.top(); minQueue.pop();
			maxTimeSoFar = std::max(maxTimeSoFar, maxTime);
			
			if((r == c) && (r == (n - 1))) return maxTimeSoFar; // Reached terminal node (n-1, n-1)
			
			for(const auto& [rowOffset, colOffset] : directionArray){
				if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); (rowIdx >= 0 && rowIdx < n && colIdx >= 0 && colIdx < n && !seen[rowIdx * n + colIdx])) {
					seen[rowIdx * n + colIdx] = 1;
					minQueue.emplace(std::max(maxTime, grid[rowIdx][colIdx]), rowIdx, colIdx);
				}
			}
		}
		return -1; // Should be unreachable
    }
};