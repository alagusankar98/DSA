int calculateDepth(TreeNode* root){
    if(!root) return 0;
    
    if(int leftLength = calculateDepth(root->left); leftLength != -1){
        if(int rightLength = calculateDepth(root->right); rightLength != -1){
            if(std::abs(leftLength - rightLength) <= 1){
                return 1 + std::max(rightLength, leftLength);
            }
        }
    }
    return -1;
}
bool isBalanced(TreeNode* root){
    bool resultFlag = true;
    return (calculateDepth(root) != -1);
}