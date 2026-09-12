int hammingWeight(uint32_t n) {
    int countOnes = 0;
    while(n > 0){
        countOnes += (n & 1);
        n = n >> 1;
    }
    return countOnes;
}