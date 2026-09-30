# Trees - Topic Guide

## 1. Core Fundamentals

A tree is a hierarchical, acyclic structure of nodes. Each node holds a value and pointers to its children. The whole topic is really just **recursion + traversal** — master those two and the rest is variations. Everything you learn here directly unlocks Tries, Heaps, Backtracking, and Graphs.

### Standard Node Structure (C++)
Mirrors the `ListNode` from Linked List, but with two forward pointers instead of one:
```cpp
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* left, TreeNode* right) : val(x), left(left), right(right) {}
};
```

### Vocabulary (get these exact before starting)
* **Root:** the single topmost node (no parent). **Leaf:** a node with no children (`!left && !right`).
* **Height of a node:** number of edges on the *longest downward path* to a leaf. **Height of the tree** = height of the root. A single node has height `0`; an empty tree is conventionally height `-1`.
* **Depth of a node:** number of edges from the *root* down to that node. (Height counts *down*, depth counts *from the top*.)
* **Binary Tree:** every node has at most 2 children.
* **Binary Search Tree (BST):** an *ordered* binary tree where for **every** node, all values in the left subtree are `<` the node and all values in the right subtree are `>` the node. The single most important consequence: **an in-order traversal of a BST yields a fully sorted sequence.**
* **Balanced Tree:** height stays `O(log N)` (left/right subtree heights differ by at most a constant at every node). A **skewed** tree degenerates into a linked list with height `O(N)` — this is the worst case that blows up both time *and* recursion depth.
* **Complete / Full / Perfect:** shape guarantees you'll care about later for Heaps. *Complete* = every level full except possibly the last, filled left-to-right. *Full* = every node has 0 or 2 children. *Perfect* = full **and** all leaves on the same level.

### The Two Traversal Families

**Depth-First Search (DFS)** — dive to the bottom before backtracking. Three orderings, defined by *when you process the current node relative to its children*:
* **Pre-order** (`Node → Left → Right`): process root first. Used for *copying/serializing* a tree top-down.
* **In-order** (`Left → Node → Right`): process root in the middle. **On a BST this emits sorted order** — the most exploited property in the whole topic.
* **Post-order** (`Left → Right → Node`): process root last. Used when a node's answer *depends on its children's answers* (height, deletion, subtree sums) — you must compute the children before you can resolve the parent.

**Breadth-First Search (BFS)** — process level by level, left to right. Also called **level-order**.

### DFS: Recursive vs Iterative
Recursion is the natural fit — the call stack *is* your traversal stack, and `nullptr` is your base case:
```cpp
void inorder(TreeNode* node) {
    if (!node) return;          // base case — the empty child
    inorder(node->left);        // Left
    /* process node->val */     // Node
    inorder(node->right);       // Right
}
```
* **The base case is always the null child.** Recursing into `nullptr` and immediately returning is cleaner than checking `if (node->left)` before every call — let the child handle its own emptiness.
* **The Stack-Overflow Reality:** recursion depth equals tree *height*. On a balanced tree that's `O(log N)` — trivial. On a **skewed** tree it's `O(N)`; with ~10⁵ nodes you can blow the real call stack and segfault. The iterative form (an explicit `std::stack<TreeNode*>`) trades recursion for heap-backed stack space and sidesteps the limit. Know both; reach for iterative when depth could be pathological.

### BFS: The Queue + Level-Snapshot Trick
Level-order uses a **`std::queue<TreeNode*>`** (FIFO — first node in is first processed, which is what keeps you moving across a level before descending):
```cpp
std::queue<TreeNode*> q;
if (root) q.push(root);
while (!q.empty()) {
    int levelSize = static_cast<int>(q.size());   // SNAPSHOT the count before the loop
    for (int i = 0; i < levelSize; ++i) {          // process exactly one level
        TreeNode* node = q.front(); q.pop();
        /* process node->val */
        if (node->left)  q.push(node->left);
        if (node->right) q.push(node->right);
    }
    /* here you know one full level just finished */
}
```
* **The Level-Snapshot:** capture `q.size()` *into a variable* before the inner loop. That count freezes how many nodes belong to the current level; the children you push during the loop belong to the *next* level and are correctly excluded. Reading `q.size()` live inside the loop condition would bleed levels together.
* `std::queue` is backed by a `std::deque` by default — chunked contiguous memory, amortized `O(1)` push/pop at both ends. Don't reach for `std::list`.

### Time & Space Complexity (the universal baseline)
| Operation | Time | Space | Notes |
| --- | --- | --- | --- |
| **Any full traversal** (DFS or BFS) | O(N) | O(H) DFS / O(W) BFS | Every node visited once. `H` = height (call/explicit stack), `W` = max level width (queue). |
| **BST search / insert / delete** | O(H) | O(H) | `O(log N)` if balanced, `O(N)` if skewed. |
| **Worst-case recursion depth** | — | O(N) | A skewed tree is a linked list. |

For a balanced tree, `H = O(log N)`; for a skewed tree, `H = O(N)`. BFS space is `O(W)` where the widest level can hold up to `N/2` nodes, so worst-case `O(N)`.

### C++ Specifics & Traps
* **`std::queue` for BFS, `std::stack` for iterative DFS.** Both are container adaptors over a `std::deque` by default. `.front()`/`.pop()` on an empty queue (and `.top()`/`.pop()` on an empty stack) is **Undefined Behavior** — always gate on `!q.empty()`.
* **Pass nodes as raw `TreeNode*`.** LeetCode-style problems hand you raw owning pointers; you traverse by pointer, you don't copy nodes.
* **Carrying state through recursion — two directions:**
    * **Top-down:** pass accumulated state *down* as a parameter (e.g., current depth, path sum so far). The leaf reports the finished answer.
    * **Bottom-up:** *return* computed state *up* from children, combine at the parent (e.g., subtree height, node count, validity). This is post-order in disguise and is usually the more powerful pattern.
* **The out-of-band accumulator:** for problems that compute a global answer while returning something else per node (e.g., "diameter" returns height but tracks max diameter), thread a `int&` reference (or a small captured variable) through the recursion instead of trying to cram two return values into one.
* **`std::optional` / sentinels for "no value":** when a subtree can legitimately have no answer, prefer an explicit sentinel or `std::optional<int>` over magic numbers like `-1` that could collide with real values.

---

## 2. General Summary / Quick Reference

When tackling Tree problems, keep these core patterns in mind:

### Pattern 1: Bottom-Up DFS (Return State Up) — *the workhorse*
Solve for the children first, then combine at the parent. Post-order in structure. The recursive call *returns* the fact you need (height, count, sum, is-valid), and each node computes its own answer from its two children's returns.
* *Use for:* Maximum Depth, Balanced Tree check, Diameter, subtree sums, "is this a valid X" checks.
* *Shape:* `int solve(node) { if (!node) return BASE; auto L = solve(node->left); auto R = solve(node->right); return combine(L, R, node); }`

### Pattern 2: Top-Down DFS (Pass State Down)
Carry running state into the recursion as parameters; the leaves produce the terminal answer. Pre-order in structure.
* *Use for:* Path Sum (carry remaining target), max-so-far comparisons, depth tracking, "good node" counts where an ancestor value matters.

### Pattern 3: BFS Level-Order (Queue + Snapshot)
Process the tree one level at a time using the level-snapshot trick.
* *Use for:* Level-Order Traversal, Right-Side View (last node per level), level averages/max, minimum depth (first leaf found = shallowest), zig-zag traversal.

### Pattern 4: BST-Specific Search (Exploit the Ordering)
Because a BST is sorted by structure, you binary-search it: at each node compare against `val` and descend left or right — never both. This is where all your Binary Search intuition transfers directly.
* *Use for:* Search in a BST, Insert/Delete, Lowest Common Ancestor in a BST (the split point where target values diverge), Kth Smallest (in-order traversal stops at the k-th emit), validating a BST (in-order must be strictly increasing).

### Pattern 5: Construct / Serialize from Traversals
Rebuild a tree from traversal orders, or flatten a tree to a string and back. Pre-order gives you roots-first (ideal for rebuilding top-down); in-order locates the split between left and right subtrees.
* *Use for:* Build Tree from Preorder+Inorder, Serialize/Deserialize.

### Pattern 6: Lowest Common Ancestor (LCA)
Recurse; a node is the LCA if the two targets are found in *different* subtrees below it (or one of them *is* the node). In a BST, simpler: walk down until the two targets fall on opposite sides of the current value.

### The Non-Negotiable Discipline
* **`nullptr` is always the base case.** Handle the empty node first, every single time.
* **Decide top-down vs bottom-up before you write a line.** Ask: "does this node's answer depend on its ancestors (pass down) or its descendants (return up)?" Picking the wrong direction is the #1 source of tangled tree code.
* **For anything level-related, reach for BFS.** For anything subtree/height/path-related, reach for DFS.

---

## 3. Problem Strategies & Patterns

*(Entries added one problem at a time as they're solved — same format as the other topic guides: **The Core Pattern**, **The "Gotcha"**, **Time & Space Complexity**, **The Struggle & Insights**.)*
