int maxDepth(TreeNode* root) {
    if (!root) return 0;

    int leftNodeDepth = 1 + maxDepth(root->left);
    int rightNodeDepth = 1 + maxDepth(root->right);

    return std::max(leftNodeDepth, rightNodeDepth);
}