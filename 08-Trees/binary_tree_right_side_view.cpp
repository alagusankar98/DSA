void calculateDepth(TreeNode* root, size_t depth, std::vector<int>& resultVector){
    if(!root) return;

    if(resultVector.size() == depth){
        resultVector.push_back(root->val);
    }

    // Going one level deep, prioritize right side
    calculateDepth(root->right, depth + 1, resultVector);
    calculateDepth(root->left, depth + 1, resultVector);
}
std::vector<int> rightSideView(TreeNode* root) {
    std::vector<int> resultVector;
    calculateDepth(root, 0, resultVector);
    return resultVector;
}