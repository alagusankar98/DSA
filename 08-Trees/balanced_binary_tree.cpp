int calculateDepth(TreeNode* root){
    if(!root) return 0;

    int leftLength = calculateDepth(root->left);
    if(leftLength == -1) return -1;

    int rightLength = calculateDepth(root->right);
    if(rightLength == -1) return -1;
    
    if(std::abs(leftLength - rightLength) > 1) return -1;

    return 1 + std::max(rightLength, leftLength);
}
bool isBalanced(TreeNode* root){
    return (calculateDepth(root) != -1);
}