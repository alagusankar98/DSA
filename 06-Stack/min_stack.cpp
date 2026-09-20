class MinStack {
private:
    std::vector<int> valStack_;
    std::vector<int> minStack_;
public:
    MinStack() {
        
    }
    
    void push(int val) {
        valStack_.push_back(val);
        // Check if current val is less than already present min element
        if(minStack_.empty() || minStack_.back() > val){
            minStack_.push_back(val);
        } else {
            minStack_.push_back(minStack_.back());
        }
    }
    
    void pop() {
        if(valStack_.empty()) return;
        valStack_.pop_back();
        minStack_.pop_back();
    }
    
    int top() {
        if(valStack_.empty()) return -1;

        return valStack_.back();
    }
    
    int getMin() {
        if(valStack_.empty()) return -1;

        return minStack_.back();
    }
};
