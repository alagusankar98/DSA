int change(int amount, const std::vector<int>& coins) {
    const int n = static_cast<int>(coins.size());
    std::vector<unsigned int> current(amount + 1, 0);
    current[0] = 1;
    
    for(int i = n - 1; i >= 0; i--) {
        for(int fillAmount = coins[i]; fillAmount <= amount; fillAmount++) {
            current[fillAmount] += current[fillAmount - coins[i]];
        }
    }
    
    return static_cast<int>(current[amount]);
}