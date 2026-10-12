int leastInterval(const std::vector<char>& tasks, int n) {
        std::array<uint16_t, 26> taskCount{};

        for(const char task : tasks){
            taskCount[task - 'A']++;
        }

        // Fill max heap
        std::priority_queue<uint16_t> maxHeap;
        for(size_t i = 0; i < taskCount.size(); i++){
            if(taskCount[i] > 0) maxHeap.push(taskCount[i]);
        }

        int minCount = 0;
        std::vector<uint16_t> waitStack;
        waitStack.reserve(taskCount.size());

        while(!maxHeap.empty()){
            int currentCount = 0;

            for(int  i = 0; !maxHeap.empty() && i <= n; i++){ // Run till all tasks or till 'n'
                uint16_t taskFrequency = maxHeap.top(); maxHeap.pop();
                if(taskFrequency > 1) waitStack.push_back(taskFrequency - 1); // Move to wait room
                currentCount++;
            }

            while(!waitStack.empty()){
                maxHeap.push(waitStack.back());
                waitStack.pop_back();
            }

            minCount += maxHeap.empty() ? currentCount : (n + 1);
        }
        return minCount;
    }