std::vector<int> dailyTemperatures(const std::vector<int>& temperatures) {
    std::vector<size_t> temperatureStack;
    temperatureStack.reserve(temperatures.size());
    std::vector<int> resultVector(temperatures.size(), 0);

    for(size_t i = 0; i < temperatures.size(); i++){
        // Use current temperature to find answers for all waiting temperatures if it is larger
        // Ensures that back() is strictly lesser than what we've seen so far till (i - 1)th index
        // For indices still in stack after exit, there were no future hotter temps
        while(!temperatureStack.empty() && (temperatures[i] > temperatures[temperatureStack.back()])){
            auto idx = temperatureStack.back();
            resultVector[idx] = static_cast<int>(i - idx);
            temperatureStack.pop_back();
        }
        // Push current temperature to stack to compare against future indices
        temperatureStack.push_back(i);
    }
    return resultVector;
}