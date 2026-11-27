int canCompleteCircuit(const std::vector<int>& gas, const std::vector<int>& cost) {
    assert(cost.size() == gas.size());
    const int n = static_cast<int>(cost.size());
    int sum = 0;
    int totalSum = 0;
    int validIdx = 0;

    for(int i = 0; i < n; i++) {
        if(sum < 0) {
            // Came across a station that makes us stop
            // Restart again from current node
            validIdx = i;
            sum = 0;
        }
        int gain = gas[i] - cost[i];
        sum += gain;
        totalSum += gain;
    }

    return (totalSum >= 0) ? validIdx : -1;
}