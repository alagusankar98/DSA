int longestCommonSubsequenceHelper(std::string_view text1, std::string_view text2, size_t i, size_t j) {
    if(i >= text1.size() || j >= text2.size()) return 0;

    int count = 0;
    if(text1[i] != text2[j]) {
        count = std::max(longestCommonSubsequenceHelper(text1, text2, i, j + 1), longestCommonSubsequenceHelper(text1, text2, i + 1, j));
    } else {
        count = 1 + longestCommonSubsequenceHelper(text1, text2, i + 1, j + 1);
    }

    return count;
}
int longestCommonSubsequence(string text1, string text2) {
    if(text1.size() > text2.size()) std::swap(text1, text2);
    return longestCommonSubsequenceHelper(text1, text2, 0, 0);
}