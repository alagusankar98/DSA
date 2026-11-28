class DetectSquares {
private:
    using Point = std::pair<int, int>;
    std::map<Point, int> frequencyMap;
public:
    DetectSquares() {
        
    }
    
    void add(const std::vector<int> point) {
        Point current(point[0], point[1]);
        frequencyMap[current]++;
    }
    
    int count(const std::vector<int> point) const {
        Point current(point[0], point[1]);

        int squaresCount = 0;
        for(auto& [potentialDiagonal, diagonalCount] : frequencyMap) {
            int dx = std::abs(potentialDiagonal.first - current.first);
            int dy = std::abs(potentialDiagonal.second - current.second);

            if(dx == dy && dx > 0 && dy > 0) {
                // Diagonals match. Check for other two points
                Point left(current.first, potentialDiagonal.second), right(potentialDiagonal.first, current.second);
                if(frequencyMap.contains(left) && frequencyMap.contains(right)) {
                    squaresCount += (frequencyMap.at(left) * frequencyMap.at(right) * diagonalCount);
                }
            }
        }
        return squaresCount;
    }
};