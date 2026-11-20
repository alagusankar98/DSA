int coinChangeHelper(const std::vector<int>& coins, int amount, std::vector<int>& dp){
    if (amount == 0) return 0;

    if(dp[amount] != -1) return dp[amount];

    int minCoinCount = std::numeric_limits<int>::max();
    for(const int coin : coins){
        if(int difference = (amount - coin); difference >= 0){
            int count = coinChangeHelper(coins, difference, dp);
            if(count < minCoinCount) {
                minCoinCount = count;
            }
        }
    }
    return dp[amount] = (minCoinCount == std::numeric_limits<int>::max()) ? minCoinCount : 1 + minCoinCount;
}
int coinChange(const std::vector<int>& coins, int amount) {
    std::vector<int> dp(amount + 1, -1);
    int minCount = coinChangeHelper(coins, amount, dp);
    return (minCount == std::numeric_limits<int>::max()) ? -1 : minCount;
}

// Normal recursive solution

// class Solution {
// public:
//     int coinChangeHelper(const std::vector<int>& coins, int amount){
//         if (amount == 0) return 0;
//         if (amount < 0) return std::numeric_limits<int>::max();

//         int minCoinCount = std::numeric_limits<int>::max();
//         for(const int coin : coins){
//             int difference = (amount - coin);
//             if(int count = coinChangeHelper(coins, difference); difference >= 0 && count < minCoinCount) {
//                 minCoinCount = count;
//             }
//         }
//         return minCoinCount == std::numeric_limits<int>::max() ? minCoinCount : 1 + minCoinCount;
//     }
//     int coinChange(const std::vector<int>& coins, int amount) {
//         int minCount = coinChangeHelper(coins, amount);
//         return (minCount == std::numeric_limits<int>::max()) ? -1 : minCount;
//     }
// };