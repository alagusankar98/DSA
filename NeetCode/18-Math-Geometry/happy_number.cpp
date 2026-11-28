int sumSquares(int n) {
    int sum = 0;
    while(n > 0) {
        int rem = n % 10;
        sum += (rem * rem);
        n = n / 10;
    }
    return sum;
}
bool isHappy(int n) {
    int slow = n;
    int fast = sumSquares(n);

    while(slow != fast) {
        slow = sumSquares(slow);
        fast = sumSquares(sumSquares(fast));
    }

    return (slow == 1);
}