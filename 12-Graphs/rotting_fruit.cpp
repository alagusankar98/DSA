class Solution {
public:
    struct IndexMinute{
        int i;
        int j;
        int minutes;
    };
public:
    int orangesRotting(const std::vector<std::vector<int>>& grid) {
        if(grid.empty()) return 0;
        std::vector<std::vector<int>> gridCopy(grid);
        std::array<std::pair<int, int>, 4> directionArray = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

        int rows = static_cast<int>(grid.size());
        int columns = static_cast<int>(grid[0].size());
        std::vector<IndexMinute> bfsVector;
        bfsVector.reserve(rows * columns);
        int freshFruit = 0;

        for(int i = 0; i < rows; i++){
            for (int j = 0; j < columns; j++){
                if(gridCopy[i][j] == 2){
                    bfsVector.emplace_back(i, j, 0);
                } else if(gridCopy[i][j] == 1){
                    freshFruit++;
                }
            }
        }

        if(freshFruit == 0) return 0;

        int maxMinutes = 0;
        std::size_t read = 0;
        while(read < bfsVector.size()){
            auto [r, c, minutes] = bfsVector[read];
            maxMinutes = minutes;
            read++;

            for(const auto& [rowOffset, colOffset] : directionArray){
                if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); 
                (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && gridCopy[rowIdx][colIdx] == 1)){
                    gridCopy[rowIdx][colIdx] = 2;
                    bfsVector.emplace_back(rowIdx, colIdx, (minutes + 1));
                    freshFruit--;
                }
            }
        }
        return (freshFruit > 0) ? -1 :maxMinutes;
    }
};