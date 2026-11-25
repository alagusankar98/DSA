int carFleet(int target, const std::vector<int>& position, std::vector<int>& speed) {
    int carCount = 0;
    std::vector<std::pair<int, double>> hoursRemaining(position.size());
    for(size_t i = 0; i < position.size(); i++){
        hoursRemaining[i].second = (target - position[i]) / static_cast<double>(speed[i]);
        hoursRemaining[i].first = position[i];
    }

    // Sort based on position (from small to large)
    std::sort(hoursRemaining.begin(), hoursRemaining.end());
    double prevBottleNeckTime = std::numeric_limits<double>::min();

    for(auto it = hoursRemaining.rbegin(); it != hoursRemaining.rend(); it++){
        if(it->second > prevBottleNeckTime){
            prevBottleNeckTime = it->second;
            carCount++;
        }
    }

    return carCount;
}