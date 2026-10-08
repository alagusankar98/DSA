// Preorder traversal
void serializeString(TreeNode* root, std::string& resultString){
    if(!root){
        resultString.append("#,");
        return;
    }
    resultString.append(std::to_string(root->val));
    resultString.push_back(',');
    serializeString(root->left, resultString);
    serializeString(root->right, resultString);
}

// Encodes a tree to a single string.
string serialize(TreeNode* root) {
    std::string serializedTreeString = "";
    serializeString(root, serializedTreeString);

    return serializedTreeString;
}

TreeNode* constructTree(const char*& startPtr, const char* end){
    if(startPtr >= end) return nullptr;

    if(*startPtr == '#'){
        startPtr += 2; // To skip "#" and ","
        return nullptr;
    }

    // Get node value
    int nodeVal = 0;
    auto [nonNumberPos, ec] = std::from_chars(startPtr, end, nodeVal);
    assert(ec == std::errc() && "Error during number parsing");
    startPtr = nonNumberPos + 1; // To skip ","
    auto node = new TreeNode(nodeVal);

    node->left = constructTree(startPtr, end);
    node->right = constructTree(startPtr, end);

    return node;
}

// Decodes your encoded data to tree.
TreeNode* deserialize(std::string_view data) {
    const char* beginPtr = data.data();
    return constructTree(beginPtr, beginPtr + data.size());
}