int orangesRotting(const std::vector<std::vector<int>>& grid) {
    if(grid.empty()) return 0;
        std::vector<uint8_t> gridCopy;
        static constexpr std::array<std::pair<int, int>, 4> directionArray = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

        int rows = static_cast<int>(grid.size());
        int columns = static_cast<int>(grid[0].size());
        std::vector<std::pair<int, int>> bfsVector;
        bfsVector.reserve(rows * columns);
        int freshFruit = 0;

        gridCopy.reserve(rows * columns);
        for(int i = 0; i < rows; i++){
            for (int j = 0; j < columns; j++){
                gridCopy.push_back(grid[i][j]);
                if(grid[i][j] == 2){
                    bfsVector.emplace_back(i, j);
                } else if(grid[i][j] == 1){
                    freshFruit++;
                }
            }
        }

        if(freshFruit == 0) return 0;

        int lastMinute = 0;
        std::size_t read = 0;
        while(read < bfsVector.size()){
            std::size_t levelEnd = bfsVector.size();
            while(read < levelEnd){
                auto [r, c] = bfsVector[read];
                read++;

                for(const auto& [rowOffset, colOffset] : directionArray){
                    if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); 
                    (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && gridCopy[rowIdx * columns + colIdx] == 1)){
                        gridCopy[rowIdx * columns + colIdx] = 2;
                        bfsVector.emplace_back(rowIdx, colIdx);
                        freshFruit--;
                    }
                }
            }
            lastMinute++;
            if(freshFruit == 0) return lastMinute; // Return early if no more fresh fruits even if bfs vector isn't empty
        }
        return -1;
}