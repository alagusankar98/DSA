bool checkBST(TreeNode* root, TreeNode* floorNode, TreeNode* ceilingNode){
    if(!root) return true;

    if((ceilingNode && root->val >= ceilingNode->val) || (floorNode && root->val <= floorNode->val)) return false;

    return checkBST(root->left, floorNode, root) && checkBST(root->right, root, ceilingNode);
}
bool isValidBST(TreeNode* root) {
    return checkBST(root, nullptr, nullptr);
}