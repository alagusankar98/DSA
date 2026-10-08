TreeNode* constructTree(const std::vector<int>& preorder, std::unordered_map<int, int>& inorderIdxMap, size_t& currentIdx, int lowIdx, int highIdx){
    if(lowIdx > highIdx) return nullptr;

    TreeNode* node = new TreeNode(preorder[currentIdx]); currentIdx++;

    // Lookup value in inorder array
    int inorderIdx = inorderIdxMap[node->val];

    node->left = constructTree(preorder, inorderIdxMap, currentIdx, lowIdx, inorderIdx - 1);
    node->right = constructTree(preorder, inorderIdxMap, currentIdx, inorderIdx + 1, highIdx);

    return node;
}

TreeNode* buildTree(const std::vector<int>& preorder, const std::vector<int>& inorder) {
    size_t currentIdx = 0;
    std::unordered_map<int, int> inorderIdxMap;
    inorderIdxMap.reserve(inorder.size());

    for(int i = 0; i < inorder.size(); i++){
        inorderIdxMap[inorder[i]] = i;
    }

    return constructTree(preorder, inorderIdxMap, currentIdx, 0, static_cast<int>(inorder.size()) - 1);
}