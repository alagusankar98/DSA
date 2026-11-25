int calculateDiameter(TreeNode* root, int& maxDiameter){
    if(!root) return 0;

    int leftLength = calculateDiameter(root->left, maxDiameter);
    int rightLength = calculateDiameter(root->right, maxDiameter);
    maxDiameter = std::max(maxDiameter, (leftLength + rightLength));

    return 1 + std::max(leftLength, rightLength);
}
int diameterOfBinaryTree(TreeNode* root) {
    int maxDiameter = 0; // Initializing to min() would fail in case of empty root (nullptr) that fails to check std::max() due to early return
    calculateDiameter(root, maxDiameter);
    return maxDiameter;
}