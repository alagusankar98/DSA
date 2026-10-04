int calculateDiameter(TreeNode* root, int& maxDiameter){
    if(!root) return 0;

    int leftLength = calculateDiameter(root->left, maxDiameter);
    int rightLength = calculateDiameter(root->right, maxDiameter);
    maxDiameter = std::max(maxDiameter, (leftLength + rightLength));

    return 1 + std::max(leftLength, rightLength);
}
int diameterOfBinaryTree(TreeNode* root) {
    int maxDiameter = std::numeric_limits<int>::min();
    calculateDiameter(root, maxDiameter);
    return maxDiameter;
}