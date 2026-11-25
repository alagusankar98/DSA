class Twitter {
private:
    std::unordered_map<int, std::unordered_set<int>> followersMap_;
    std::unordered_map<int, std::vector<std::pair<int, int>>> tweetListMap_;
    unsigned int time;
    static constexpr int maxTweets = 10;
public:
    Twitter() {
        time = 0;
    }
    
    void postTweet(int userId, int tweetId) {
        tweetListMap_[userId].emplace_back(time, tweetId);
        time++;
    }
    
    vector<int> getNewsFeed(int userId) const {
        std::vector<int> relevantUsers;
        if(auto it = followersMap_.find(userId); it != followersMap_.end() ){
            relevantUsers.reserve(it->second.size() + 1);
            for(const int followerId : it->second){
                relevantUsers.push_back(followerId);
            }
        }

        relevantUsers.push_back(userId);

        std::priority_queue<std::tuple<unsigned int, int, int, int>> maxHeap;
        for(const int user : relevantUsers){
            if(auto it = tweetListMap_.find(user); it != tweetListMap_.end()){
                int currentIdx = std::ssize(it->second) - 1;
                maxHeap.emplace(it->second[currentIdx].first, it->second[currentIdx].second, user, currentIdx);
            }
        }

        std::vector<int> tweetList;
        tweetList.reserve(maxTweets);

        while(tweetList.size() < maxTweets && !maxHeap.empty()){
            auto [_, tweetId, user, currentIdx] = maxHeap.top(); maxHeap.pop();
            tweetList.push_back(tweetId);
            if(currentIdx > 0){
                auto& tweetVector = tweetListMap_.at(user);
                maxHeap.emplace(tweetVector[currentIdx - 1].first, tweetVector[currentIdx - 1].second,
                user, currentIdx - 1);
            }
        }

        return tweetList;
    }
    
    void follow(int followerId, int followeeId) {
        if(followerId != followeeId) followersMap_[followerId].insert(followeeId);
    }
    
    void unfollow(int followerId, int followeeId) {
        if(auto it = followersMap_.find(followerId); it != followersMap_.end()){
            it->second.erase(followeeId);
        }
    }
};