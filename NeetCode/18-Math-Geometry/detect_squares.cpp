class DetectSquares {
private:
    using Point = std::pair<int, int>;
    std::unordered_map<int, std::unordered_map<int, int>> frequencyMap;
public:
    DetectSquares() {
        
    }
    
    void add(const std::vector<int> point) {
        frequencyMap[point[0]][point[1]]++;
    }
    
    int count(const std::vector<int> point) const {
        int x1 = point[0];
        int y1 = point[1];

        auto xIt = frequencyMap.find(x1);
        if(xIt == frequencyMap.end()) return 0; // No matching point to form (x1, y')

        int squaresCount = 0;
        for(const auto& [y2, y2Count] : xIt->second) {
            int side = std::abs(y2 - y1); // Length between (x1, y1) and (x1, y2)
            if(side == 0) continue; // Duplicate point (y1 == y2)

            // Potentially found a square that has one side between (x1, y1) and (x1, y2)
            // Check if matching (x3, y1) & (x3, y2) exists (or) (x4, y1) & (x4, y2) exists
            int x3 = x1 + side;
            if(auto x3It = frequencyMap.find(x3); x3It != frequencyMap.end()) {
                if(x3It->second.contains(y1) && x3It->second.contains(y2)) {
                    squaresCount += (y2Count * x3It->second.at(y1) * x3It->second.at(y2));
                }
            }

            int x4 = x1 - side;
            if(auto x4It = frequencyMap.find(x4); x4It != frequencyMap.end()) {
                if(x4It->second.contains(y1) && x4It->second.contains(y2)) {
                    squaresCount += (y2Count * x4It->second.at(y1) * x4It->second.at(y2));
                }
            }
        }
        return squaresCount;
    }
};