int numDecodingsHelper(std::string_view s, int n, int start, std::vector<int>& dp){
        if(dp[start] != -1) return dp[start];
        if(start == n) {
            return 1;
        }
        int count1 = 0, count2 = 0;
        if(start < n && s[start] != '0'){
            count1 = numDecodingsHelper(s, n, start + 1, dp);
            if((start + 1) < n && (s[start] == '1' || (s[start] == '2' && s[start + 1] <= '6') ) ){
                count2 = numDecodingsHelper(s, n, start + 2, dp);
            }
        }
        return dp[start] = count1 + count2;
    }
    int numDecodings(std::string_view s) {
        const int n = static_cast<int>(s.size());
        std::vector<int> dp(n + 1, -1);
        return numDecodingsHelper(s, n, 0, dp);;
    }