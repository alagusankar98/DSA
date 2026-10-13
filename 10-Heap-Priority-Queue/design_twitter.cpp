class Twitter {
private:
    std::unordered_map<int, std::unordered_set<int>> followersMap_;
    std::unordered_map<int, std::vector<std::pair<int, int>>> tweetListMap_;
    unsigned int time;
public:
    Twitter() {
        time = 0;
    }
    
    void postTweet(int userId, int tweetId) {
        tweetListMap_[userId].emplace_back(time, tweetId);
        time++;
    }
    
    vector<int> getNewsFeed(int userId) {
        std::vector<int> relevantUsers;
        relevantUsers.reserve(followersMap_[userId].size() + 1);
        relevantUsers.push_back(userId);

        for(const int followerId : followersMap_[userId]){
            relevantUsers.push_back(followerId);
        }

        std::priority_queue<std::tuple<unsigned int, int, int, int>> maxHeap;
        for(const int user : relevantUsers){
            if(auto it = tweetListMap_.find(user); it != tweetListMap_.end()){
                int currentIdx = std::ssize(it->second) - 1;
                maxHeap.emplace(it->second[currentIdx].first, it->second[currentIdx].second, user, currentIdx);
            }
        }

        std::vector<int> tweetList;
        tweetList.reserve(10);

        while(tweetList.size() < 10 && !maxHeap.empty()){
            auto [_, tweetId, user, currentIdx] = maxHeap.top(); maxHeap.pop();
            tweetList.push_back(tweetId);
            if(currentIdx > 0){
                auto& tweetVector = tweetListMap_[user];
                maxHeap.emplace(tweetVector[currentIdx - 1].first, tweetVector[currentIdx - 1].second,
                user, currentIdx - 1);
            }
        }

        return tweetList;
    }
    
    void follow(int followerId, int followeeId) {
        followersMap_[followerId].insert(followeeId);
    }
    
    void unfollow(int followerId, int followeeId) {
        if(auto it = followersMap_.find(followerId); it != followersMap_.end()){
            it->second.erase(followeeId);
        }
    }
};
