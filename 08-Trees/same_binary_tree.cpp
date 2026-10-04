bool isSameTree(TreeNode* p, TreeNode* q) {
    if(!p && !q) return true; // Both the nodes empty, Valid
    if(!p || !q) return false; // One of the nodes already empty when other is not, Invalid

    if(p->val != q->val) return false;

    if(!isSameTree(p->left, q->left)) return false;
    if(!isSameTree(p->right, q->right)) return false;

    return true;
}