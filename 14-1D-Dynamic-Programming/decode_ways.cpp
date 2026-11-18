int numDecodings(std::string_view s) {
    const int n = static_cast<int>(s.size());
    std::vector<int> dp(n + 1);
    dp[n] = 1;
    for(int i = n-1; i >= 0; i--){
        int count1 = 0, count2 = 0;
        if(s[i] != '0'){
            count1 = dp[i+1];
            if((i + 1) < n && (s[i] == '1' || (s[i] == '2' && s[i + 1] <= '6') ) ){
                count2 = dp[i+2];
            }
        }
        dp[i] = count1 + count2;
    }
    return dp[0];
}