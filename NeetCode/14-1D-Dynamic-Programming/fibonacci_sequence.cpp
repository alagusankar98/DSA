// Optimized Bottom up approach or Tabulation approach
int fib(int n) {
    if (n < 2) return n;
    int prev1 = 1, prev2 = 0;
    int current = 0;
    for(int i = 2; i <= n; i++){
        current = prev1 + prev2;
        prev2 = prev1;
        prev1 = current;
    }
    return current;
}


// Bottom up approach or Tabulation approach
// int fib(int n) {
//     if (n < 2) return n;
//     std::vector<int> dp(n + 1, 0);
//     dp[1] = 1;
    
//     for(int i = 2; i <= n; i++){
//         dp[i] = dp[i - 1] + dp[i - 2];
//     }

//     return dp[n];
// }



// Top Down approach or Memoization
// int fibHelper(int n, std::vector<int>& dp){
//     if(dp[n] != -1) return dp[n]; // Answer readily available, no need to compute. Return

//     return dp[n] = fibHelper(n - 1, dp) + fibHelper(n - 2, dp);
// }
// int fib(int n) {
//     if (n < 2) return n;
//     std::vector<int> dp(n + 1, -1);
//     dp[0] = 0; dp[1] = 1;
//     return fibHelper(n, dp);
// }



// Normal Recusive solution
// int fib(int n) {
//     if (n < 2) return n;

//     return fib(n - 1) + fib(n - 2);
// }