int change(int amount, const std::vector<int>& coins) {
    const int n = static_cast<int>(coins.size());
    std::vector<unsigned int> current(amount + 1, 0);
    current[0] = 1;
    std::vector<unsigned int> next(current);
    
    for(int i = n - 1; i >= 0; i--) {
        for(int fillAmount = 0; fillAmount <= amount; fillAmount++) {
            current[fillAmount] = next[fillAmount];
            if(fillAmount >= coins[i]) current[fillAmount] += current[fillAmount - coins[i]];
        }
        std::swap(current, next);
    }
    
    return static_cast<int>(next[amount]);
}