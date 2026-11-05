struct IndexLevel{
    int i;
    int j;
    int level;
};
void wallsAndGates(std::vector<std::vector<int>>& grid) {
    if(grid.empty()) return;

    int rows = static_cast<int>(grid.size());
    int columns = static_cast<int>(grid[0].size());
    std::array<std::pair<int, int>, 4> directionArray = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    std::vector<IndexLevel> bfsNodes;
    std::unordered_set<int> seen;
    bfsNodes.reserve(rows * columns);

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if(grid[i][j] != 0) continue;
            bfsNodes.emplace_back(i, j, 1);
            int read = 0;

            while(read < static_cast<int>(bfsNodes.size())){
                auto [r, c, l] = bfsNodes[read];
                read++;
                for(const auto& [rowOffset, colOffset] : directionArray){
                    if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && grid[rowIdx][colIdx] > 0 && !seen.contains(rowIdx * columns + colIdx))){
                        grid[rowIdx][colIdx] = std::min(l, grid[rowIdx][colIdx]); // Update level
                        seen.insert(rowIdx * columns + colIdx);
                        bfsNodes.emplace_back(rowIdx, colIdx, l + 1);
                    }
                }
            }
            bfsNodes.clear();
            seen.clear();
        }
    }
}