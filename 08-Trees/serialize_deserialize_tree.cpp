// Preorder traversal
void serializeString(TreeNode* root, std::string& resultString){
    if(!root){
        resultString.append("#,");
        return;
    }
    resultString.append(std::to_string(root->val) + ",");
    serializeString(root->left, resultString);
    serializeString(root->right, resultString);
}

// Encodes a tree to a single string.
string serialize(TreeNode* root) {
    std::string serializedTreeString = "";
    serializeString(root, serializedTreeString);

    return serializedTreeString;
}

TreeNode* constructTree(std::string_view data, size_t& currentPos){
    if(data[currentPos] == '#'){
        currentPos += 2; // To skip "#" and ","
        return nullptr;
    }

    // Get node value
    int nodeVal = 0;
    auto [nonNumberPos, _] = std::from_chars(data.data() + currentPos, data.data() + data.size(), nodeVal); // Accumulates "-100," to "nodeVal = -100" and "nonNumberPos = pointer to ,"
    currentPos = (nonNumberPos - data.data()) + 1; // To skip ","
    auto node = new TreeNode(nodeVal);

    node->left = constructTree(data, currentPos);
    node->right = constructTree(data, currentPos);

    return node;
}

// Decodes your encoded data to tree.
TreeNode* deserialize(string data) {
    size_t currentPos = 0;
    return constructTree(data, currentPos);
}