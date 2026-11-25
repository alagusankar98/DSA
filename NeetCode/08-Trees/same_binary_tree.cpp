bool isSameTree(TreeNode* p, TreeNode* q) {

    if(!p && !q) return true; // Both the nodes empty, Valid (Structural Match)
    if(!p || !q) return false; // One of the nodes already empty when other is not, Invalid (Structural Mismatch)

    if(p->val != q->val) return false; // (Value mismatch)

    return isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
}