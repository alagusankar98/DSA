std::vector<std::vector<int>> pacificAtlantic(const std::vector<std::vector<int>>& heights) {
    if(heights.empty()) return {};

    int rows = static_cast<int>(heights.size());
    int columns = static_cast<int>(heights[0].size());
    static constexpr std::array<std::pair<int, int>, 4> directionArray = {{{-1,0}, {1,0}, {0,-1}, {0,1}}};

    std::vector<uint8_t> seen((rows * columns), 0); // Bit 0 for Pacific and Bit 1 for Atlantic

    std::vector<std::pair<int, int>> bfsVector;
    bfsVector.reserve(rows * columns);
    
    // Left Column for Pacific
    for(int i = 0; i < rows; i++){
        seen[i * columns] |= 1;
        bfsVector.emplace_back(i, 0);
    }

    // Top Row for Pacific
    for(int j = 1; j < columns; j++){
        seen[j] |= 1;
        bfsVector.emplace_back(0, j);
    }

    // BFS for Pacific
    std::size_t read = 0;
    while(read < bfsVector.size()){
        auto [r, c] = bfsVector[read];
        read++;

        for(auto const& [rowOffset, colOffset] : directionArray){
            if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); 
            (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && (heights[r][c] <= heights[rowIdx][colIdx]) && (seen[rowIdx * columns + colIdx] & 1) == 0)){
                bfsVector.emplace_back(rowIdx, colIdx);
                seen[rowIdx * columns + colIdx] = 1;
            }
        }
    }

    bfsVector.clear();

    // Right Column for Atlantic
    for(int i = 0; i < rows; i++){
        seen[i * columns + (columns - 1)] |= (1 << 1);
        bfsVector.emplace_back(i, (columns - 1));
    }

    // Bottom Row for Atlantic
    for(int j = 0; j < columns - 1; j++){
        seen[(rows - 1) * columns + j] |= (1 << 1);
        bfsVector.emplace_back((rows - 1), j);
    }

    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(rows * columns);

    // BFS for Atlantic
    read = 0;
    while(read < bfsVector.size()){
        auto [r, c] = bfsVector[read];
        read++;

        for(auto const& [rowOffset, colOffset] : directionArray){
            if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); 
            (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && (heights[r][c] <= heights[rowIdx][colIdx]) && ((seen[rowIdx * columns + colIdx] >> 1) & 1) == 0)){
                bfsVector.emplace_back(rowIdx, colIdx);
                seen[rowIdx * columns + colIdx] |= (1 << 1);
            }
        }
    }

    
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if((seen[i * columns + j] & 0b11) == 0b11) resultVector.push_back({i, j}); 
        }
    }

    return resultVector;
}