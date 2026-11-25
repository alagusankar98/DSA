std::string longestPalindrome(std::string_view s) {
    const int n = static_cast<int>(s.size());
    int start = 0;
    int length = 0;
    std::vector<std::vector<uint8_t>> dp(n, std::vector<uint8_t>(n, 0));

    auto modifyLength = [&](const int i, const int j){
        if(dp[i][j] && (j - i + 1) > length){
            start = i;
            length = j - i + 1;
        }
    };

    bool previousAny = false;
    for(int i = 0; i < n; i++){
        dp[i][i] = 1;
        modifyLength(i, i);
        if((i + 1) < n) {
            dp[i][i+1] = (s[i] == s[i+1]);
            modifyLength(i, i+1);
            previousAny |= dp[i][i+1];
        }
    }
    
    for(int difference = 2; difference < n; difference++){
        bool currentAny = false;
        for(int i = 0; (i + difference) < n; i++){
            int j = i + difference;
            dp[i][j] = (s[i] == s[j]) && dp[i+1][j-1];
            modifyLength(i, j);
            currentAny |= dp[i][j];
        }
        if(!currentAny && !previousAny) break;
        previousAny = currentAny;
    }
    
    return std::string(s.data() + start, length);
}