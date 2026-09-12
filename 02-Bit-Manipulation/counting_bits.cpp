vector<int> countBits(int n) {
    std::vector<int> resultVector(n + 1, 0);
    for(int i = 0; i <= n; i++){
        resultVector[i] = resultVector[i >> 1] + (i & 1);
    }
    return resultVector;
}