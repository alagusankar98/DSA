bool checkBST(TreeNode* root, TreeNode*& prev){
    if(!root) return true;

    if(!checkBST(root->left, prev)) return false;

    if(prev && root->val <= prev->val) return false;

    prev = root;

    return checkBST(root->right, prev);
}
bool isValidBST(TreeNode* root) {
    TreeNode* prev = nullptr;
    return checkBST(root, prev);
}