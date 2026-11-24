int changeHelper(const std::vector<int>& coins, size_t startIdx, int remaining){
    if(remaining < 0 || startIdx == coins.size()) return 0;
    if(remaining == 0) return 1;

    return changeHelper(coins, startIdx, remaining - coins[startIdx]) + changeHelper(coins, startIdx + 1, remaining);
}
int change(int amount, const std::vector<int>& coins) {
    return changeHelper(coins, 0, amount);
}