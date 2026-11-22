int uniquePaths(int m, int n) {
    std::vector<std::vector<int>> dp(m, std::vector<int>(n, 0));
    
    // Initialize States for both m == 1 || n == 1
    for(int i = 0; i < m; i++) dp[i][0] = 1;
    for(int i = 0; i < n; i++) dp[0][i] = 1;

    for(int i = 1; i < m; i++) {
        for(int j = 1; j < n; j++) {
            dp[i][j] = dp[i-1][j] + dp[i][j-1];
        }
    }
    return dp[m-1][n-1];
}

// Padded version where dp[idx][0] and dp[0][idx] is dummy
// int uniquePaths(int m, int n) {
//     std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
//     // Initialize States for both m == 1 || n == 1
//     dp[0][1] = 1;

//     for(int i = 1; i <= m; i++) {
//         for(int j = 1; j <= n; j++) {
//             dp[i][j] = dp[i-1][j] + dp[i][j-1];
//         }
//     }
//     return dp[m][n];
// }

// int dfs(int m, int n, std::vector<std::vector<int>>& dp) {
//     if(m == 1 || n == 1) return 1;
//     if (dp[m - 1][n - 1] != -1) return dp[m - 1][n - 1]; 
//     return dp[m - 1][n - 1] = dfs(m-1, n, dp) + dfs(m, n - 1, dp);
// }
// int uniquePaths(int m, int n) {
//     std::vector<std::vector<int>> dp(m, std::vector<int>(n, -1));
//     return dfs(m, n, dp);
// }


// Without offset and extra state
// int dfs(int m, int n, std::vector<std::vector<int>>& dp) {
//     if(m == 1 || n == 1) return 1;
//     if (dp[m][n] != -1) return dp[m][n]; 
//     return dp[m][n] = dfs(m-1, n, dp) + dfs(m, n - 1, dp);
// }
// int uniquePaths(int m, int n) {
//     std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, -1));
//     return dfs(m, n, dp);
// }