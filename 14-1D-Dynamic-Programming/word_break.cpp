bool wordBreakHelper(std::string_view s, const std::vector<std::string>& wordDict, size_t i, std::vector<uint8_t>& dp) {
    if(i == s.size()) return true;

    if(dp[i]) return false;

    for(const auto& word : wordDict) {
        // Compare each word against current position 'i'
        if(size_t wordLength = word.size(); (i + wordLength) <= s.size() && s.substr(i, wordLength) == word) {
            if(wordBreakHelper(s, wordDict, (i + wordLength), dp)) return true;
        }
    }
    dp[i] = 1;
    return false;
}
bool wordBreak(std::string_view s, const std::vector<std::string>& wordDict) {
    std::vector<uint8_t> dp(s.size(), 0);
    return wordBreakHelper(s, wordDict, 0, dp);
}