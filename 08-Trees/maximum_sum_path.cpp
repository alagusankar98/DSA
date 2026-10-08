int calculateMaxSum(TreeNode* root, int& maxSum){
    if(!root) return 0;

    int leftMax = std::max(0, calculateMaxSum(root->left, maxSum));
    int rightMax = std::max(0, calculateMaxSum(root->right, maxSum));

    maxSum = std::max(maxSum, (root->val + leftMax + rightMax));

    return root->val + std::max(leftMax, rightMax);
}

int maxPathSum(TreeNode* root) {
    int maxSum = std::numeric_limits<int>::min();
    calculateMaxSum(root, maxSum);
    return maxSum;
}