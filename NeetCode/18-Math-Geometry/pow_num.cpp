double myPow(double x, int n) {
    if(x == 0) return 0;
    if(x == 1 || n == 0) return 1;
    bool isNegative = (n < 0);
    long long cycles = static_cast<long long>(n);
    cycles = std::abs(cycles);
    double result = 1;
    double base = x;

    while(cycles > 0) {
        if(cycles & 1) {
            // Include current (x ^ ) to result
            result *= base;
        }
        base = (base * base); // x^2, x^4, x^8, x^16....
        cycles /= 2;
    }
    return isNegative ? (1 / result) : result;
}