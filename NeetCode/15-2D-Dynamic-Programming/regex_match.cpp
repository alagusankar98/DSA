bool isMatchHelper(std::string_view s, std::string_view pattern, int i, int j, const int n1, const int n2, std::vector<std::vector<uint8_t>>& dp) {
    if(j == n2) return (i == n1);

    if(dp[i][j] != std::numeric_limits<uint8_t>::max()) return dp[i][j];

    // Check if we have an exact character match or if regex pattern is a wild card
    const bool firstMatch = (i < n1) && ((s[i] == pattern[j]) || (pattern[j] == '.'));

    bool resultFlag = false;
    if((j + 1) < n2 && pattern[j + 1] == '*') {
        // Consume 0 or more characters of pattern[j] in s

        // Check both skip branch and take branch (consume one character and let next recursion consume next character)
        // Skip branch : (i, j + 2) to skip pattern and i to let next recursion match ith character  (0 matches, skip '.*' or 'a*')
        // Take branch : Verify if character matches before consuming it (i + 1, j) indicates one character consumed and retaining pattern for next recursion
        resultFlag = (isMatchHelper(s, pattern, i, j + 2, n1, n2, dp) || (firstMatch && isMatchHelper(s, pattern, i + 1, j, n1, n2, dp)));
    }

    resultFlag = resultFlag || firstMatch && isMatchHelper(s, pattern, i + 1, j + 1, n1, n2, dp);

    return dp[i][j] = resultFlag;
}
bool isMatch(std::string_view s, std::string_view p) {
    const int n1 = static_cast<int>(s.size());
    const int n2 = static_cast<int>(p.size());

    std::vector<std::vector<uint8_t>> dp(n1 + 1, std::vector<uint8_t>(n2, std::numeric_limits<uint8_t>::max()));

    return isMatchHelper(s, p, 0, 0, n1, n2, dp);
}