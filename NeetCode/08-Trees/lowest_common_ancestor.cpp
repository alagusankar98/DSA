TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if(p && q && (p->val > q->val)) return lowestCommonAncestor(root, q, p);
    
    TreeNode* current = root;

    while(current){
        if(current->val >= p->val && current->val <= q->val) return current;

        if(current->val <= p->val){
            // Move to right side (higher  value side)
            current = current->right;
        } else {
            // Move to left side (lower value side)
            current = current->left;
        }
    }
    return nullptr;
}