std::vector<int> plusOne(const vector<int>& digits) {
    std::vector<int> result(digits);
    const int n = static_cast<int>(digits.size());
    uint8_t carry = 1;
    for(int i = n - 1; i >= 0; i--) {
        int sum = carry + digits[i];
        result[i] = (sum % 10);
        carry = (sum / 10);
    }

    if(carry) result.insert(result.begin(), 1);

    return result;
}