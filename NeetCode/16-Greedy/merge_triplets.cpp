bool mergeTriplets(const std::vector<vector<int>>& triplets, const std::vector<int>& target) {
    int currentA = std::numeric_limits<int>::min();
    int currentB = std::numeric_limits<int>::min();
    int currentC = std::numeric_limits<int>::min();

    for(const auto& triplet : triplets) {
        if(std::max(currentA, triplet[0]) <= target[0] && std::max(currentB, triplet[1]) <= target[1] && std::max(currentC, triplet[2]) <= target[2]) {
            currentA = std::max(currentA, triplet[0]);
            currentB = std::max(currentB, triplet[1]);
            currentC = std::max(currentC, triplet[2]);
        }
    }

    return (currentA == target[0]) &&  (currentB == target[1]) && (currentC == target[2]);
}