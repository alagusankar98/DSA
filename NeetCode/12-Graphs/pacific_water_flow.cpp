std::vector<std::vector<int>>
pacificAtlantic(const std::vector<std::vector<int>> &heights) {
  if (heights.empty() ||heights[0].empty())
    return {};

  int rows = static_cast<int>(heights.size());
  int columns = static_cast<int>(heights[0].size());
  static constexpr std::array<std::pair<int, int>, 4> directionArray = {
      {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

  std::vector<uint8_t> seen((rows * columns),
                            0); // Bit 0 for Pacific and Bit 1 for Atlantic

  std::vector<std::pair<int, int>> bfsVector;
  bfsVector.reserve(rows * columns);
  std::vector<std::vector<int>> resultVector;
  resultVector.reserve(rows * columns);

  static constexpr uint8_t PACIFIC = 0b01;
  static constexpr uint8_t ATLANTIC = 0b10;
  static constexpr uint8_t BOTH = PACIFIC | ATLANTIC;

  // Left Column for Pacific
  for (int i = 0; i < rows; i++) {
    seen[i * columns] |= PACIFIC;
    bfsVector.emplace_back(i, 0);
  }

  // Top Row for Pacific
  for (int j = 1; j < columns; j++) {
    seen[j] |= PACIFIC;
    bfsVector.emplace_back(0, j);
  }

  auto markOceanCoverage = [&, rows, columns](uint8_t bitMask) {
    // BFS for Pacific(0th bit)/ Atlantic(1st bit)
    std::size_t read = 0;
    while (read < bfsVector.size()) {
      auto [r, c] = bfsVector[read];
      read++;
      if ((seen[r * columns + c] & BOTH) == BOTH)
        resultVector.push_back({r, c});

      for (auto const &[rowOffset, colOffset] : directionArray) {
        if (int rowIdx = (r + rowOffset), colIdx = (c + colOffset);
            (rowIdx >= 0 && colIdx >= 0 && rowIdx < rows && colIdx < columns &&
             (heights[r][c] <= heights[rowIdx][colIdx]) &&
             (seen[rowIdx * columns + colIdx] & bitMask) == 0)) {
          bfsVector.emplace_back(rowIdx, colIdx);
          seen[rowIdx * columns + colIdx] |= bitMask;
        }
      }
    }
    bfsVector.clear();
  };

  // BFS for Pacific
  markOceanCoverage(PACIFIC);

  // Right Column for Atlantic
  for (int i = 0; i < rows; i++) {
    seen[i * columns + (columns - 1)] |= ATLANTIC;
    bfsVector.emplace_back(i, (columns - 1));
  }

  // Bottom Row for Atlantic
  for (int j = 0; j < columns - 1; j++) {
    seen[(rows - 1) * columns + j] |= ATLANTIC;
    bfsVector.emplace_back((rows - 1), j);
  }

  // BFS for Atlantic
  markOceanCoverage(ATLANTIC);

  return resultVector;
}