TreeNode* invertTree(TreeNode* root) {
    if(!root) return nullptr; // Empty tree

    std::swap(root->left, root->right);

    invertTree(root->left);
    invertTree(root->right);

    return root;
}