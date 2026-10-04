bool checkTree(TreeNode* node_1, TreeNode* node_2) {
    if(!node_1 && !node_2) return true; // Structural match
    
    if(!node_1 || !node_2) return false; // Structural mismatch

    if(node_1->val != node_2->val) return false; // Value mismatch

    return checkTree(node_1->left, node_2->left) && checkTree(node_1->right, node_2->right);
}
bool isSubtree(TreeNode* root, TreeNode* subRoot) {
    if(checkTree(root, subRoot)) return true;

    if(!root) return false;
    
    return isSubtree(root->left, subRoot) || isSubtree(root->right, subRoot);
}