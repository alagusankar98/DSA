std::vector<int> rightSideView(TreeNode* root) {
    std::vector<int> resultVector;
    std::queue<TreeNode*> levelQueue;
    if(root) levelQueue.push(root);

    while(!levelQueue.empty()){
        const size_t n = levelQueue.size();
        TreeNode* node;
        for(size_t i = 0; i < n; i++){
            node = levelQueue.front(); levelQueue.pop();
            if(node->left) levelQueue.push(node->left);
            if(node->right) levelQueue.push(node->right);
        }
        resultVector.push_back(node->val);
    }

    return resultVector;
}