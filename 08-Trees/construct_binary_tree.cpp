TreeNode* constructTree(const std::vector<int>& preorder, const std::vector<int>& inorder, size_t& currentIdx, int lowIdx, int highIdx){
    if(lowIdx > highIdx) return nullptr;

    TreeNode* node = new TreeNode(preorder[currentIdx]); currentIdx++;

    // Lookup value in inorder array
    int inorderIdx;
    for(inorderIdx = lowIdx; inorderIdx <= highIdx; inorderIdx++){
        if(inorder[inorderIdx] == node->val) break;
    }

    node->left = constructTree(preorder, inorder, currentIdx, lowIdx, inorderIdx - 1);
    node->right = constructTree(preorder, inorder, currentIdx, inorderIdx + 1, highIdx);

    return node;
}

TreeNode* buildTree(const std::vector<int>& preorder, const std::vector<int>& inorder) {
    size_t currentIdx = 0;
    return constructTree(preorder, inorder, currentIdx, 0, static_cast<int>(inorder.size()) - 1);
}