int carFleet(int target, vector<int>& position, vector<int>& speed) {
    int carCount = 0;
    std::vector<std::pair<int, double>> hoursRemaining(position.size());
    for(size_t i = 0; i < position.size(); i++){
        hoursRemaining[i].second = (target - position[i]) / static_cast<double>(speed[i]);
        hoursRemaining[i].first = position[i];
    }

    // Sort based on position (from small to large)
    std::sort(hoursRemaining.begin(), hoursRemaining.end());

    while(!hoursRemaining.empty()){
        double currentTimeRemaining = hoursRemaining.back().second;
        while(!hoursRemaining.empty() && (currentTimeRemaining >= hoursRemaining.back().second)){
            hoursRemaining.pop_back();
        }
        carCount++;
    }
    return carCount;
}