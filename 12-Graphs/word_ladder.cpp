int ladderLength(std::string beginWord, std::string endWord, const std::vector<std::string>& wordList) {
    std::unordered_set<std::string> wordListSet;
    wordListSet.reserve(wordList.size());

    for(const auto& word : wordList){
        wordListSet.insert(word);
    }

    if(!wordListSet.contains(endWord)) return 0;

    std::vector<std::string> bfsVector;
    bfsVector.reserve(wordList.size() + 1);
    bfsVector.push_back(beginWord);
    wordListSet.erase(beginWord);

    std::size_t read = 0;
    int level = 1;
    while(read < bfsVector.size()){
        std::size_t levelEnd = bfsVector.size();

        while(read < levelEnd){
            std::string currentStr = bfsVector[read];
            read++;

            for(size_t i = 0; i < currentStr.size(); i++){
                char origChar = currentStr[i];
                for(char j = 'a'; j <= 'z'; j++){
                    if(origChar != j){
                        currentStr[i] = j;
                        if(wordListSet.erase(currentStr)){
                            if(currentStr == endWord) return (level + 1);
                            bfsVector.push_back(currentStr);
                        }
                    }
                }
                currentStr[i] = origChar;
            }
        }
        level++;
    }
    return 0;
}