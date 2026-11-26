bool isMatch(std::string_view s, std::string_view p) {
    const int n1 = static_cast<int>(s.size());
    const int n2 = static_cast<int>(p.size());

    std::vector<std::vector<uint8_t>> dp(n1 + 1, std::vector<uint8_t>(n2 + 1, 0));
    dp[n1][n2] = 1;

    for(int i = n1; i >= 0; i--) {
        for(int j = n2 - 1; j >= 0; j--) {
            const bool firstMatch = ((i < n1) && ((s[i] == p[j]) || (p[j] == '.')));
            if((j + 1) < n2 && p[j + 1] == '*') {
                // Consume 0 or more characters of pattern[j] in s

                // Check both skip branch and take branch (consume one character and let next recursion consume next character)
                // Skip branch : (i, j + 2) to skip pattern and i to let next recursion match ith character  (0 matches, skip '.*' or 'a*')
                // Take branch : Verify if character matches before consuming it (i + 1, j) indicates one character consumed and retaining pattern for next recursion
                dp[i][j] = dp[i][j + 2] || (firstMatch && dp[i + 1][j]);
            } else {
                dp[i][j] = (firstMatch && dp[i + 1][j + 1]);
            }
        }
    }
    return dp[0][0];
}