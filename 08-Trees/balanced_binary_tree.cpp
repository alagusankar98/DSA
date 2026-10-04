int calculateDepth(TreeNode* root, bool& resultFlag){
    if(!root) return 0;
    
    int leftLength = calculateDepth(root->left, resultFlag);
    int rightLength = calculateDepth(root->right, resultFlag);
    resultFlag = resultFlag && (std::abs(leftLength - rightLength) <= 1);
    return 1 + std::max(rightLength, leftLength);
}
bool isBalanced(TreeNode* root) {
    bool resultFlag = true;
    calculateDepth(root, resultFlag);
    return resultFlag;
}