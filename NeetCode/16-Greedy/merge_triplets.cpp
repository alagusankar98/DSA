bool mergeTriplets(const std::vector<std::vector<int>>& triplets, const std::vector<int>& target) {
    int currentA = std::numeric_limits<int>::min();
    int currentB = std::numeric_limits<int>::min();
    int currentC = std::numeric_limits<int>::min();

    for(const auto& triplet : triplets) {
        if(triplet[0] <= target[0] && triplet[1] <= target[1] && triplet[2] <= target[2]) {
            currentA = std::max(currentA, triplet[0]);
            currentB = std::max(currentB, triplet[1]);
            currentC = std::max(currentC, triplet[2]);
            if ((currentA == target[0]) &&  (currentB == target[1]) && (currentC == target[2])) return true;
        }
    }

    return false;
}