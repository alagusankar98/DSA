bool countIslands(const std::vector<std::vector<char>>& grid, std::vector<std::vector<bool>>& seen, int i, int j){
    int rows = std::ssize(grid);
    int columns = std::ssize(grid[0]);

    if((i < 0) || (j < 0) || (i >= rows) || (j >= columns) || seen[i][j] || grid[i][j] != '1'){
        return false;
    }

    seen[i][j] = true;

    bool result = countIslands(grid, seen, i - 1, j) ||
                    countIslands(grid, seen, i + 1, j) ||
                    countIslands(grid, seen, i, j - 1) ||
                    countIslands(grid, seen, i, j + 1);

    return result;
}

int numIslands(const std::vector<std::vector<char>>& grid) {
    if(grid.empty()) return 0;
    std::vector<std::vector<bool>> seen(grid.size(), std::vector<bool>(grid[0].size(), false));
    int count = 0;
    for(std::size_t i = 0; i < grid.size(); i++){
        for(std::size_t j = 0; j < grid[i].size(); j++){
            if (!seen[i][j] && grid[i][j] =='1') {
                countIslands(grid, seen, i, j);
                count++;
            }
        }
    }
    return count;
}