std::vector<int> plusOne(const vector<int>& digits) {
    std::vector<int> result(digits);
    const int n = static_cast<int>(digits.size());
    for(int i = n - 1; i >= 0; i--) {
        if(digits[i] < 9) {
            result[i]++;
            return result;
        } else {
            result[i] = 0;
        }
    }
    result.insert(result.begin(), 1);
    return result;
}