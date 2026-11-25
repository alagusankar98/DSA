bool checkTree(TreeNode* node_1, TreeNode* node_2) {
    if(!node_1 && !node_2) return true; // Structural match
    
    if(!node_1 || !node_2) return false; // Structural mismatch

    if(node_1->val != node_2->val) return false; // Value mismatch

    return checkTree(node_1->left, node_2->left) && checkTree(node_1->right, node_2->right);
}
bool isSubtree(TreeNode* root, TreeNode* subRoot) {
    if(!subRoot) return true; // Empty subtree will match either empty root or a leaf node in original tree

    if(!root) return false; // If root becomes empty when subroot is not, no point checking further

    if(checkTree(root, subRoot)) return true;
    
    return isSubtree(root->left, subRoot) || isSubtree(root->right, subRoot);
}