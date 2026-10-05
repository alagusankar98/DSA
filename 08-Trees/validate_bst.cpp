bool checkBST(TreeNode* root, long floor, long ceiling){
    if(!root) return true;

    if(root->val >= ceiling || root->val <= floor) return false;

    return checkBST(root->left, floor, root->val) && checkBST(root->right, root->val, ceiling);
}
bool isValidBST(TreeNode* root) {
    return checkBST(root, std::numeric_limits<long>::min(), std::numeric_limits<long>::max());
}