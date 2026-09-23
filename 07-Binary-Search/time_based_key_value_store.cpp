class TimeMap {
private:
using IntStringPair = std::pair<int, std::string>;
std::unordered_map<std::string, std::vector<IntStringPair>> personMoodMap_;
public:
    TimeMap() {
        
    }
    
    void set(string key, string value, int timestamp) {
        personMoodMap_[key].emplace_back(timestamp, value);
    }
    
    string get(string key, int timestamp) {
        if(auto it = personMoodMap_.find(key); it != personMoodMap_.end()){
            // Found key. Use Binary search for target timestamp

            auto& searchArray = it->second;
            const int n = static_cast<int>(searchArray.size());
            int left = 0;
            int right = n - 1;

            if(searchArray[left].first > timestamp) return ""; // Can't give out value for a timestamp before recording even started
            if(searchArray[right].first <= timestamp) return searchArray[right].second; // Query for a timestamp that is greater than most recent recording

            int lastValidIndex = -1;
            while(left <= right){
                int mid = left + (right - left) / 2;

                if(searchArray[mid].first == timestamp){
                    return searchArray[mid].second;
                } else if(searchArray[mid].first > timestamp){
                    // Went too far into right. Search back
                    right = mid - 1;
                } else {
                    // This is the answer we want
                    lastValidIndex = mid;
                    left = mid + 1;
                }
            }

            return searchArray[lastValidIndex].second;
        }

        return "";
    }
};