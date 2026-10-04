int checkValues(TreeNode* p, TreeNode* q) {
    if(!p && !q) return 1; // Both the nodes empty, Valid
    if(!p || !q) return -1; // One of the nodes already empty when other is not, Invalid

    if(p->val != q->val) return -1;

    if(checkValues(p->left, q->left) == -1) return -1;
    if(checkValues(p->right, q->right) == -1) return -1;

    return 1;
}
bool isSameTree(TreeNode* p, TreeNode* q) {
    return checkValues(p, q) != -1;
}