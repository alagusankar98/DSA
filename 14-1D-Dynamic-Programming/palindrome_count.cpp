int countSubstrings(std::string_view s) {
    const int n = static_cast<int>(s.size());

    std::vector<std::vector<uint8_t>> dp(n, std::vector<uint8_t>(n, 0));
    bool previousAny = false;
    int count = 0;

    for(int difference = 0; difference < n; difference++){
        bool currentAny = false;
        for(int i = 0; (i + difference) < n; i++){
            int j = i + difference;
            dp[i][j] = (s[i] == s[j]) && ((difference < 2) || (dp[i+1][j-1]));
            count += dp[i][j];
            currentAny = currentAny || dp[i][j];
        }
        if(!(currentAny || previousAny)) return count;
        previousAny = currentAny;
    }
    return count;
}