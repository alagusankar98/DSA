int numDistinctHelper(std::string_view s, std::string_view t, size_t i, size_t j) {
    if(j == t.size()) return 1;
    if(i == s.size()) return 0;

    int count = numDistinctHelper(s, t, i + 1, j);

    if(s[i] == t[j]) count += numDistinctHelper(s, t, i + 1, j + 1);

    return count;
}
int numDistinct(std::string_view s, std::string_view t) {
    const int n1 = static_cast<int>(s.size());
    const int n2 = static_cast<int>(t.size());

    if(n2 > n1) return 0;
    return numDistinctHelper(s, t, 0, 0);
}