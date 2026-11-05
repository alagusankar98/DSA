void wallsAndGates(std::vector<std::vector<int>>& grid) {
    if(grid.empty()) return;

    int rows = static_cast<int>(grid.size());
    int columns = static_cast<int>(grid[0].size());
    static constexpr std::array<std::pair<int, int>, 4> directionArray = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    std::vector<std::pair<int, int>> bfsNodes;
    bfsNodes.reserve(rows * columns);
    int maxVal = std::numeric_limits<int>::max();

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if(grid[i][j] == 0) bfsNodes.emplace_back(i, j);
        }
    }

    std::size_t read = 0;
    int currentLevel = 1;
    while(read < bfsNodes.size()){
        std::size_t levelEnd = bfsNodes.size();
        while(read < levelEnd){
            auto [r, c] = bfsNodes[read];
            read++;
            for(const auto& [rowOffset, colOffset] : directionArray){
                if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && grid[rowIdx][colIdx] == maxVal)){
                    grid[rowIdx][colIdx] = currentLevel;
                    bfsNodes.emplace_back(rowIdx, colIdx);
                }
            }
        }
        currentLevel++;
    }
}