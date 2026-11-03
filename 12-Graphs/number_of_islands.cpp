int numIslands(const std::vector<std::vector<char>>& grid) {
    if(grid.empty()) return 0;
    std::array<std::pair<int, int>, 4> directionArray = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    int count = 0;
    int rows = static_cast<int>(grid.size());
    int columns = static_cast<int>(grid[0].size());
    std::vector<uint8_t> seen(rows * columns, 0);
    std::vector<std::pair<int, int>> bfsQueue;
    bfsQueue.reserve(rows * columns);
    
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if(grid[i][j] != '1' || seen[(i * columns) + j]) continue;
            seen[(i * columns) + j] = 1;
            bfsQueue.emplace_back(i, j);
            std::size_t read = 0;
            while(read < bfsQueue.size()){
                // Consume all neighbouring ones
                auto [r, c] = bfsQueue[read];
                read++;

                for(const auto& [rowOffset, columnOffset] : directionArray){
                    if(int rowIdx = (r + rowOffset), colIdx = (c + columnOffset); (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && grid[rowIdx][colIdx] == '1' && seen[rowIdx * columns + colIdx] == 0)){
                        seen[rowIdx * columns + colIdx] = 1;
                        bfsQueue.emplace_back(rowIdx, colIdx);
                    }
                }
            }
            bfsQueue.clear();
            count++;
        }
    }
    return count;
}