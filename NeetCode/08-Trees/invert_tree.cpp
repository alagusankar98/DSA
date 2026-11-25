TreeNode* invertTree(TreeNode* root) {
    if(!root) return nullptr; // Empty tree

    std::swap(root->left, root->right);

    invertTree(root->left);
    invertTree(root->right);

    return root;
}

// Iterative approach
// TreeNode* invertTree(TreeNode* root) {
//     if(!root) return nullptr; // Empty tree

//     // Iterative approach
//     std::stack<TreeNode*> treeStack;
//     treeStack.push(root);
    
//     while(!treeStack.empty()){
//         TreeNode* node = treeStack.top();
//         treeStack.pop();

//         std::swap(node->left, node->right);

//         if(node->left) treeStack.push(node->left);
//         if(node->right) treeStack.push(node->right);
//     }

//     return root;
// }