class TimeMap {
private:
using IntStringPair = std::pair<int, std::string>;
std::unordered_map<std::string, std::vector<IntStringPair>> data_;
public:
    TimeMap() {
        
    }
    
    void set(string key, string value, int timestamp) {
        data_[key].emplace_back(timestamp, value);
    }
    
    string get(string key, int timestamp) {
        if(auto it = data_.find(key); it != data_.end()){
            // Found key. Use Binary search for target timestamp

            auto& searchArray = it->second;
            const int n = static_cast<int>(searchArray.size());
            int left = 0;
            int right = n - 1;

            if(timestamp < searchArray.front().first) return ""; // Can't give out value for a timestamp before recording even started
            if(timestamp >= searchArray.back().first) return searchArray.back().second; // Query for a timestamp that is greater than most recent recording

            while(left <= right){
                int mid = left + (right - left) / 2;

                if(searchArray[mid].first == timestamp){
                    return searchArray[mid].second;
                } else if(searchArray[mid].first > timestamp){
                    // Went too far into right. Search back
                    right = mid - 1;
                } else {
                    // This is the answer we want for last valid index
                    left = mid + 1;
                }
            }

            return searchArray[right].second;
        }

        return "";
    }
};