int minDistanceHelper(std::string_view word1, std::string_view word2, const int n1, const int n2, int i, int j) {
    if(i == n1 || j == n2) return ((n1 - i) + (n2 - j));
    if(word1[i] == word2[j]) return minDistanceHelper(word1, word2, n1, n2, i + 1, j + 1);

    int count = std::min(minDistanceHelper(word1, word2, n1, n2, i + 1, j + 1), minDistanceHelper(word1, word2, n1, n2, i + 1, j));
    count = std::min(count, minDistanceHelper(word1, word2, n1, n2, i, j + 1));
    return 1 + count;
}
int minDistance(std::string_view word1, std::string_view word2) {
    const int n1 = static_cast<int>(word1.size());
    const int n2 = static_cast<int>(word2.size());
    return minDistanceHelper(word1, word2, n1, n2, 0, 0);
}