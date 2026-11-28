double myPow(double x, int n) {
    if(x == 0) return 0;
    if(x == 1 || n == 0) return 1;
    bool isNegative = (n < 0);
    long long cycles = static_cast<long long>(n);
    cycles = std::abs(cycles);
    double result = 1;

    while(cycles > 0) {
        if(cycles & 1) {
            // Include current (x ^ ) to result
            result *= x;
        }
        x = (x * x); // x^2, x^4, x^8, x^16....
        cycles /= 2;
    }
    return isNegative ? (1 / result) : result;
}