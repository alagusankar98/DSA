int leastInterval(const std::vector<char>& tasks, int n) {
    std::array<uint16_t, 26> taskCount{};

    for(const char task : tasks){
        taskCount[task - 'A']++;
    }

    std::priority_queue<uint16_t, std::vector<uint16_t>, std::less<uint16_t>> maxHeap;

    for(const int num : taskCount){
        if(num) maxHeap.push(num);
    }

    std::vector<int> taskStack;
    taskStack.reserve(26);
    int minCount = 0;

    while(!maxHeap.empty()){
        int executedCount = 0;
        for(int i = 0; !maxHeap.empty() && i < (n + 1); i++){
            uint16_t taskFrequency = maxHeap.top(); maxHeap.pop();
            if(taskFrequency > 1) taskStack.push_back(taskFrequency - 1);
            executedCount++;
        }

        if(!taskStack.empty()){
            // Tasks still in heap after timeout. No idle time needed
            minCount += (n + 1);
        } else {
            // No more tasks in queue
            minCount += executedCount;
        }

        while(!taskStack.empty()){
            maxHeap.push(taskStack.back());
            taskStack.pop_back();
        }
    }

    return minCount;
}