TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if(p && q && (p->val > q->val)) return lowestCommonAncestor(root, q, p);
    
    std::stack<TreeNode*> treeStack;
    treeStack.push(root);

    while(!treeStack.empty()){
        auto node = treeStack.top(); treeStack.pop();

        if(node->val >= p->val && node->val <= q->val) return node;

        if(node->val <= p->val){
            // Move to right side (higher  value side)
            if(node->right) treeStack.push(node->right);
        } else {
            // Move to left side (lower value side)
            if(node->left) treeStack.push(node->left);
        }
    }
    return nullptr;
}