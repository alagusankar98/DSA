bool wordBreakHelper(std::string_view s, const std::vector<std::string>& wordDict, size_t i, std::vector<char>& dp) {
    if(i == s.size()) return dp[i] = 1;

    if(dp[i] != -1) return dp[i];

    for(const auto& word : wordDict) {
        // Compare each word against current position 'i'
        if(size_t wordLength = word.size(); (i + wordLength) <= s.size() && s.substr(i, wordLength) == word) {
            if(wordBreakHelper(s, wordDict, (i + wordLength), dp)) return dp[i] = 1;
        }
    }

    return dp[i] = 0;
}
bool wordBreak(std::string_view s, const std::vector<std::string>& wordDict) {
    std::vector<char> dp(s.size() + 1, -1);
    return wordBreakHelper(s, wordDict, 0, dp);
}