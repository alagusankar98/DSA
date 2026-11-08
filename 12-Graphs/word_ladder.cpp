int ladderLength(std::string beginWord, std::string endWord, const std::vector<std::string>& wordList) {
    std::unordered_set<std::string> wordListSet(wordList.begin(), wordList.end());
    if(!wordListSet.contains(endWord)) return 0;
    
    std::unordered_set<std::string> setA;
    setA.insert(beginWord);
    wordListSet.erase(beginWord);

    std::unordered_set<std::string> setB;
    setB.insert(endWord);
    wordListSet.erase(endWord);

    int level = 1;
    std::unordered_set<std::string> next;
    while(!setA.empty() && !setB.empty()){
        // Set A is always small
        if(setA.size() > setB.size()){
            std::swap(setA, setB);
        }

        for(std::string originalStr : setA){
            for(size_t i = 0; i < originalStr.size(); i++){
                char originalChar = originalStr[i];
                for(char j = 'a'; j <= 'z'; j++){
                    if(j == originalChar) continue;
                    originalStr[i] = j;

                    if(setB.contains(originalStr)) return (level + 1);
                    if(wordListSet.erase(originalStr)){
                        next.insert(originalStr);
                    }
                }
                originalStr[i] = originalChar;
            }
        }
        std::swap(setA, next);
        level++;
        next.clear();
    }
    return 0;
}