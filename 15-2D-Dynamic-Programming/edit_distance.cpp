int minDistance(std::string_view word1, std::string_view word2) {
    const int n1 = static_cast<int>(word1.size());
    const int n2 = static_cast<int>(word2.size());
    std::vector<int> current(n2 + 1, 0);
    std::vector<int> next(current);
    for(int j = 0; j <= n2; j++) next[j] = n2 - j;

    for(int i = n1 - 1; i >= 0; i--) {
        current[n2] = n1 - i;
        for(int j = n2 - 1; j >= 0; j--) {
            if(word1[i] == word2[j]){
                current[j] = next[j + 1];
            } else {
                int count = std::min(next[j + 1], next[j]);
                count = std::min(count, current[j + 1]);
                current[j] = 1 + count;
            }
        }
        std::swap(current, next);
    }
    return next[0];
}