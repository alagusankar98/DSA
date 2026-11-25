std::vector<std::vector<int>> levelOrder(TreeNode* root) {
    std::vector<std::vector<int>> resultVector;
    std::queue<TreeNode*> nodeQueue;
    if(root) nodeQueue.push(root);

    while(!nodeQueue.empty()){
        const size_t n = nodeQueue.size();
        std::vector<int> levelVector;
        levelVector.reserve(n);
        for(size_t i = 0; i < n; i++){
            auto node = nodeQueue.front(); nodeQueue.pop();
            levelVector.push_back(node->val);

            if(node->left) nodeQueue.push(node->left);
            if(node->right) nodeQueue.push(node->right);
        }
        resultVector.push_back(std::move(levelVector));
    }

    return resultVector;
}