bool isInterleaveHelper(std::string_view s1, std::string_view s2, std::string_view s3, int i, int j, std::vector<std::vector<uint8_t>>& dp) {
    if(i == s1.size() && j == s2.size()) return true;
    
    if (dp[i][j] != std::numeric_limits<uint8_t>::max()) return dp[i][j];

    bool resultFlag = false;
    if(i < s1.size() && s3[i + j] == s1[i]) {
        // Still have characters left in s1
        resultFlag = isInterleaveHelper(s1, s2, s3, i + 1, j, dp);
    }
    if(j < s2.size() && s3[i + j] == s2[j]) {
        // Still have characters left in s2
        resultFlag = resultFlag || isInterleaveHelper(s1, s2, s3, i, j + 1, dp);
    }

    return dp[i][j] = resultFlag;
}
bool isInterleave(std::string_view s1, std::string_view s2, std::string_view s3) {
    const int n1 = static_cast<int>(s1.size());
    const int n2 = static_cast<int>(s2.size());
    const int n3 = static_cast<int>(s3.size());

    if((n1 + n2) != n3) return false;

    std::vector<std::vector<uint8_t>> dp(s1.size() + 1, std::vector<uint8_t>(s2.size() + 1, std::numeric_limits<uint8_t>::max()));

    return isInterleaveHelper(s1, s2, s3, 0, 0, dp);
}