int dfs(int m, int n, std::vector<std::vector<int>>& dp) {
    if(m == 1 || n == 1) return 1;
    if (dp[m - 1][n - 1] != -1) return dp[m - 1][n - 1]; 
    return dp[m - 1][n - 1] = dfs(m-1, n, dp) + dfs(m, n - 1, dp);
}
int uniquePaths(int m, int n) {
    std::vector<std::vector<int>> dp(m, std::vector<int>(n, -1));
    return dfs(m, n, dp);
}


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