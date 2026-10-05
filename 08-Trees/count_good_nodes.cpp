void countGoodNodes(TreeNode* root, int maxSoFar, int& goodNodeCount){
    if(!root) return;

    if(root->val >= maxSoFar){
        goodNodeCount++;
        maxSoFar = root->val;
    }

    countGoodNodes(root->left, maxSoFar, goodNodeCount);
    countGoodNodes(root->right, maxSoFar, goodNodeCount);
}
int goodNodes(TreeNode* root) {
    int goodNodeCount = 0;
    countGoodNodes(root, std::numeric_limits<int>::min(), goodNodeCount);
    return goodNodeCount;
}