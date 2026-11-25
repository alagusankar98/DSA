int countGoodNodes(TreeNode* root, int maxSoFar){
    if(!root) return 0;

    int currentCount = (root->val >= maxSoFar) ? 1 : 0;
    maxSoFar = std::max(maxSoFar, root->val);

    return currentCount + countGoodNodes(root->left, maxSoFar) + countGoodNodes(root->right, maxSoFar);
}
int goodNodes(TreeNode* root) {
    return countGoodNodes(root, std::numeric_limits<int>::min());
}