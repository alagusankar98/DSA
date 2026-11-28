double myPow(double x, int n) {
    if(x == 0) return 0;
    if(x == 1 || n == 0) return 1;
    bool isNegative = (n < 0);
    long long cycles = static_cast<long long>(n);
    cycles = std::abs(cycles);
    double result = 1;

    for(long long i = 0; i < cycles; i++) {
        result *= (x);
    }
    return isNegative ? (1 / result) : result;
}