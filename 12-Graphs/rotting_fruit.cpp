class Solution {
public:
    struct IndexMinute{
        int i;
        int j;
        int minutes;
    };
public:
    int orangesRotting(const std::vector<std::vector<int>>& grid) {
        if(grid.empty()) return -1;
        std::vector<std::vector<int>> gridCopy(grid);
        std::array<std::pair<int, int>, 4> directionArray = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

        int rows = static_cast<int>(grid.size());
        int columns = static_cast<int>(grid[0].size());
        std::vector<IndexMinute> bfsVector;
        bfsVector.reserve(rows * columns);

        int minuteOffset = 10;

        for(int i = 0; i < rows; i++){
            for (int j = 0; j < columns; j++){
                if(gridCopy[i][j] == 2){
                    bfsVector.emplace_back(i, j, minuteOffset);
                }
            }
        }

        std::size_t read = 0;
        while(read < bfsVector.size()){
            auto [r, c, minutes] = bfsVector[read];
            read++;

            for(const auto& [rowOffset, colOffset] : directionArray){
                if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); 
                (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns && gridCopy[rowIdx][colIdx] == 1)){
                    gridCopy[rowIdx][colIdx] = minutes + 1;
                    bfsVector.emplace_back(rowIdx, colIdx, (minutes + 1));
                }
            }
        }

        int maxMinutes = 0;
        for(int i = 0; i < rows; i++){
            for (int j = 0; j < columns; j++){
                if(gridCopy[i][j] == 1) return -1;
                maxMinutes = std::max(maxMinutes, gridCopy[i][j]);
            }
        }

        return (maxMinutes <= 2) ? 0 : (maxMinutes - minuteOffset);
    }
};