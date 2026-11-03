int maxAreaOfIsland(const std::vector<std::vector<int>>& grid) {
    if(grid.empty()) return 0;
    int rows = static_cast<int>(grid.size());
    int columns = static_cast<int>(grid[0].size());

    std::array<std::pair<int, int>, 4> directionArray = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    std::vector<uint8_t> seen(rows*columns, 0);
    std::vector<std::pair<int, int>> bfsVector;
    bfsVector.reserve(rows*columns);
    int maxArea = 0;

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if(grid[i][j] != 1 || seen[i * columns + j]) continue;
            
            bfsVector.emplace_back(i, j); seen[i * columns + j] = 1;
            std::size_t read = 0;
            int currentArea = 0;

            while(read < bfsVector.size()){
                auto [r, c] = bfsVector[read];
                read++;
                currentArea++;

                for(const auto& [rowDelta, colDelta] : directionArray){
                    if(int rowIdx = r + rowDelta, colIdx = c + colDelta; (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && grid[rowIdx][colIdx] && !seen[rowIdx*columns + colIdx])){
                        seen[rowIdx*columns + colIdx] = 1;
                        bfsVector.emplace_back(rowIdx, colIdx);
                    }
                }
            }
            bfsVector.clear();
            maxArea = std::max(maxArea, currentArea);
        }
    }
    return maxArea;
}