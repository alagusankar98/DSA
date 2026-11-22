int uniquePaths(int m, int n) {
    static constexpr std::array<std::pair<int, int>, 2> directionArray = {{{0, 1}, {1, 0}}};
    std::vector<std::pair<int, int>> bfs;
    bfs.reserve(m * n);
    bfs.emplace_back(0,0);

    int count = 0;
    int read = 0;
    while(read < static_cast<int>(bfs.size())) {
        auto [r, c] = bfs[read]; read++;
        if(r == m - 1 && c == n - 1) count++;

        for(const auto& [rowOffset, colOffset] : directionArray) {
            if(int rowIdx = (r + rowOffset), colIdx = (c + colOffset); rowIdx < m && colIdx < n) {
                bfs.emplace_back(rowIdx, colIdx);
            }
        }
    }
    return count;
}