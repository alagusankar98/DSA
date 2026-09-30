TreeNode* invertTree(TreeNode* root) {
    if(!root) return nullptr; // Empty tree

    TreeNode* leftNode = root->left;
    root->left = root->right;
    root->right = leftNode;

    TreeNode* _ = invertTree(root->left);
    _ = invertTree(root->right);

    return root;
}