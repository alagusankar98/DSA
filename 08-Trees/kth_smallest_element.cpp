void getKthNode(TreeNode* root, int& k, TreeNode*& kthNode){
    if(!root || k == 0) return;
    
    getKthNode(root->left, k, kthNode);
    
    if(k == 0) return;

    k--;
    if(k == 0){
        kthNode = root;
        return;
    }

    getKthNode(root->right, k, kthNode);
}

int kthSmallest(TreeNode* root, int k) {
    TreeNode* kthNode = nullptr;
    getKthNode(root, k, kthNode);
    return kthNode->val;
}