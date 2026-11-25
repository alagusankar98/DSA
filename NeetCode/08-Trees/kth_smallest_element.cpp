int kthSmallest(TreeNode* root, int k) {
    std::stack<TreeNode*> treeStack;
    int i = 0;

    TreeNode* current = root;

    while(current || !treeStack.empty()){

        while(current){
            treeStack.push(current); // All left nodes pushed to stack
            current = current->left;
        }

        auto node = treeStack.top(); treeStack.pop(); // Last left node

        i++;
        if(i == k) return node->val;

        current = node->right;
    }
    
    return -1;
}