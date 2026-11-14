// Top Down approach or Memoization
int fibHelper(int n, std::vector<int>& dp){
    if(dp[n] != -1) return dp[n]; // Answer readily available, no need to compute. Return

    return dp[n] = fibHelper(n - 1, dp) + fibHelper(n - 2, dp);
}
int fib(int n) {
    if (n < 2) return n;
    std::vector<int> dp(n + 1, -1);
    dp[0] = 0; dp[1] = 1;
    return fibHelper(dp, n);
}

// Normal Recusive solution
// int fib(int n) {
//     if (n < 2) return n;

//     return fib(n - 1) + fib(n - 2);
// }