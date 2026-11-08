int ladderLength(std::string beginWord, std::string endWord, const std::vector<std::string>& wordList) {
    if (beginWord.size() != endWord.size()) return 0;

    std::unordered_set<std::string> wordListSet;
    wordListSet.reserve(wordList.size());

    for(const auto& word : wordList){
        wordListSet.insert(word);
    }

    if(!wordListSet.contains(endWord)) return 0;

    std::vector<std::string> bfsVector;
    bfsVector.reserve(wordList.size() + 1);
    bfsVector.push_back(beginWord);
    if(wordListSet.contains(beginWord)) wordListSet.erase(beginWord);

    std::size_t read = 0;
    int level = 1;
    while(read < bfsVector.size()){
        std::size_t levelEnd = bfsVector.size();

        while(read < levelEnd){
            std::string currentStr = bfsVector[read];
            read++;

            for(size_t i = 0; i < currentStr.size(); i++){
                for(char j = 0; j < 26; j++){
                    std::string newStr = currentStr;
                    newStr[i] = 'a' + j;
                    if(wordListSet.contains(newStr)){
                        if(newStr == endWord) return (level + 1);
                        wordListSet.erase(newStr);
                        bfsVector.push_back(newStr);
                    }
                }
            }
        }
        level++;
    }
    return 0;
}