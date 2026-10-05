# Trees - Topic Guide

## 1. Core Fundamentals

A tree is a hierarchical, acyclic structure of nodes. Each node holds a value and pointers to its children. The whole topic is really just **recursion + traversal** — master those two and the rest is variations. Everything you learn here directly unlocks Tries, Heaps, Backtracking, and Graphs.

### What Is a Tree? (The Recursive Definition)
A tree is defined **in terms of itself**: a tree is a single **root** node plus zero or more disjoint **subtrees**, where *each subtree is itself a tree* with its own root — and this nesting continues until you reach **leaves**, nodes whose subtrees are all empty.
```text
            (root)
           /      \
     (subtree)   (subtree)   ← each is a full tree in its own right
       /   \         \
    (...) (...)      (...)    ← ...recursing down until no subtrees remain
```
* **This is the single most important idea in the topic.** It's *why* tree code is recursive: "do X to the tree" almost always decomposes into "do X to the root, then do X to its left subtree and its right subtree." Each recursive call is handed a smaller tree (a subtree) that looks exactly like the original problem.
* **A binary tree** specializes this: every node has at most two subtrees — a **left** and a **right** — either of which may be empty.
* **The empty tree (`nullptr`) is the base case.** It's the "no further subtrees" terminator that stops the descent. This is precisely why nearly every tree function opens with `if (!node) return ...;` — you're handling the empty subtree at the bottom of the recursion.

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

### Height vs Depth (and the edge-vs-node trap)
A single node `x` has **both** a depth and a height, and in general they are *different numbers* — this is the part that trips people up.

* **Depth(x)** — measured **top-down**: edges on the path from the **root** down to `x`. Because a tree has no cycles, the root→`x` path is **unique**, so depth is never ambiguous. The root itself has depth `0`.
* **Height(x)** — measured **bottom-up**: edges on the **longest** path from `x` down to its *deepest* leaf. Since `x` can reach many leaves, you take the deepest one. A leaf has height `0`.

```text
            A        depth 0,  height 2      ← root: deepest leaf is 2 edges away
           / \
          B   C      depth 1,  height 1
         /
        D            depth 2,  height 0      ← leaf: depth and height differ here
```

**The tree-level identity.** When people say *"the height of the tree"* and *"the depth of the tree,"* they mean the **same number**:
> height of tree = height of root = (max depth over all nodes) = depth of the deepest node = depth of tree

So the common slip — "the depth of the tree is `0` because the root's depth is `0`" — is **wrong**. The *root's* depth is 0, but the *tree's* depth is the depth of its **deepest** node (the max), which equals the root's height. Height and depth only coincide at the whole-tree level; at an individual node they usually don't.

**The edge-vs-node counting trap.** The definitions above count **edges** (single node → height `0`, empty tree → `-1`). But many problems — including LeetCode's **Maximum Depth** (`[2]` below) — count **nodes/levels** instead: `null → 0`, a leaf `→ 1`. The two conventions are off by one:
> node-count = edge-count + 1

That's exactly why `maxDepth` returns `1` for a single node even though its *edge-height* is `0`. Neither is "more correct" — just **pin down which one the interviewer wants** before you write the base case, because it decides whether `null` returns `0` or `-1` and whether a leaf returns `0` or `1`.

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
* **Recursion is NOT "free" space — it uses the thread's call stack in RAM.** It only *looks* like you allocated nothing. Every recursive call executes a hardware `call` that pushes a **stack frame** onto your thread's execution stack: the **return address** (where to resume), the **parameters** (the node pointer), and the **saved caller registers / local variables**. A straight-line tree of 10,000 nodes means 10,000 frames physically stacked in memory. So the space cost is genuinely `O(H)` — you're just not the one typing the pushes.
* **Recursion (call stack) vs explicit `std::stack` (heap) — the real tradeoff:**
    * **Per-node overhead:** recursion is forced to store the return address *and* saved register state at every level. An explicit `std::stack<TreeNode*>` stores **only the pointer** — much leaner per node.
    * **The hard limit:** the OS call stack is small and fixed — typically **~8 MB** on a Linux desktop thread. Blow past it on a deep skewed tree and you get a **Stack Overflow → segfault**, with no graceful exception. An explicit stack lives on the **heap**, bounded only by physical RAM (gigabytes) — realistically un-overflowable.
    * **The embedded reality:** on bare-metal (e.g. an STM32 / ARM Cortex-M), the linker script may cap the *entire* call stack at 2–4 KB. There, deep recursion is a guaranteed fatal crash, and iterative loops with an explicit (often pre-sized fixed-array) stack are mandatory.
    * **Which to prefer:** **Interviews / bounded depth → recursion** (4 lines, proves you understand the traversal). **Production over untrusted or deeply-nested data (user graphs, nested JSON), or memory-constrained embedded → iterative** to eliminate the overflow risk. Balanced trees with mathematically bounded depth (e.g. the Red-Black trees behind `std::map`) are safe to recurse.
* **Backing container for an explicit DFS stack:** use the default **`std::stack<TreeNode*>`** (deque-backed) when depth is unknown. Same reasoning as the Stack topic guide: a `std::vector` reallocates and copies the whole buffer on growth (latency spikes), while a `std::deque` just links a new fixed-size chunk and never moves existing elements. On embedded you'd instead pre-allocate a fixed-size array sized to the hardware's safe bound.

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
* **Sentinel bounds vs. legal data (`[11]`):** if the sentinel (`INT_MIN`/`INT_MAX`) is itself a legal node value, widen the bounds to `long long`. Bare `long` is only 32 bits on Windows and on ARM Cortex-M. Also, `numeric_limits<double>::min()` is the smallest *positive* double; the most negative one is `lowest()`.
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
* *Use for:* Path Sum (carry remaining target), max-so-far comparisons, depth tracking, "good node" counts where an ancestor value matters, Right-Side View (right-first DFS + `size() == depth` lock).
* *Parameter discipline:* per-path state (depth, running sum) goes in **by value**, so each frame has its own copy and siblings can't corrupt each other. The shared answer goes in **by reference**. If nothing flows back up, the helper is `void`.

### Pattern 3: BFS Level-Order (Queue + Snapshot)
Process the tree one level at a time using the level-snapshot trick.
* *Use for:* Level-Order Traversal, Right-Side View (last node per level), level averages/max, minimum depth (first leaf found = shallowest), zig-zag traversal.

### Pattern 4: BST-Specific Search (Exploit the Ordering)
Because a BST is sorted by structure, you binary-search it: at each node compare against `val` and descend left or right — **never both**. This is where all your Binary Search intuition transfers directly.
* *Use for:* Search in a BST, Insert/Delete, Lowest Common Ancestor in a BST (the split point where target values diverge), Kth Smallest (in-order traversal stops at the k-th emit), validating a BST (in-order must be strictly increasing).
* **The space insight — single-path descent needs NO stack and NO recursion (`O(1)` space).** This is the part that's easy to miss. The generic tree template reaches for a stack/recursion because at each node you must explore *both* children, so you need somewhere to remember the branch you haven't walked yet. A BST **search-style** operation never branches: the ordering tells you to go *either* left *or* right, so there is nothing to remember and nothing to backtrack to. You just reassign a single `current` pointer in a loop and walk one root-to-node path. **This is literally iterative binary search on a sorted array** — the same `O(1)`-space, no-auxiliary-structure walk, just following child pointers instead of adjusting `lo`/`hi` indices. Reaching for a `std::stack` here is dead weight: it would never hold more than one element.
* **The crucial caveat — this only applies to single-path descent.** The moment a BST problem must *visit many/all nodes* (Kth Smallest, Validate BST, range-sum spanning both sides), you **do** branch again, and the stack/recursion comes back — because now you have a left side to resume after finishing a right side. So the rule is **not** "BSTs never need recursion"; it's: **single root-to-target path → `O(1)` iterative; traversal that touches both subtrees → `O(H)` stack/recursion.** Search, Insert, Delete, LCA, floor/ceil, closest-value are single-path; in-order-based queries are traversals.

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

### [1] Invert Binary Tree

* **The Core Pattern:** Recursive DFS (**Pre-order**). At every node, do one local action — swap its two children — then recurse into both subtrees. Because the action at each node is independent of every other node, "invert the whole tree" collapses to "swap children, everywhere."
    ```cpp
    TreeNode* invertTree(TreeNode* root) {
        if (!root) return nullptr;      // base case: empty child
        std::swap(root->left, root->right);
        invertTree(root->left);         // return values intentionally discarded
        invertTree(root->right);
        return root;                    // hand the (now-inverted) root back to the caller
    }
    ```
* **The "Gotcha":**
    * **The Base Case is the `nullptr` node — NOT the leaf.** The instinct was to return `nullptr` for *both* a null pointer and a leaf node. That's a subtle bug: returning `nullptr` for a leaf **deletes the leaf**. On a single-node tree (root *is* a leaf), it wiped the entire tree and returned nothing. The fix is that you never need a special leaf case at all: a leaf's two children are already `nullptr`, so the recursive calls on them hit the null base case and unwind harmlessly. *(The alternate instinct — "return the node itself for a leaf" — would also work, but it's redundant. One base case, the null check, covers everything.)*
    * **Don't `return` the recursive calls.** The temptation is `return invertTree(root->left);`. That's wrong on two counts: it would exit the function after touching only the left subtree (the right never gets processed), and it returns the wrong node. Here the recursion works by **mutating nodes in place** via `std::swap`, so the inner calls' return values carry no information you need — the tree is already being rewired through the pointers. You call them purely for their side effect.
    * **Discarding a return value needs no `_` temporary.** Assigning the result to a throwaway variable just to "consume" it is unnecessary — in C++, calling a function and ignoring its return value makes the compiler discard it automatically. `invertTree(root->left);` on its own line is complete and correct.
    * **`std::swap` over a manual temp.** `std::swap(root->left, root->right)` is clearer and less error-prone than the three-line `TreeNode* tmp = ...` dance.
* **Time & Space Complexity:** $O(N)$ Time (every node visited once) / $O(H)$ Space for the recursion stack, where `H` is tree height — $O(\log N)$ balanced, $O(N)$ worst-case skewed.
* **The Struggle & Insights:**
    * **First real recursion design.** The mechanical parts (base case, swap, repeat) came quickly; the wall was trusting *how* the recursion composes — specifically whether the return value was load-bearing. The unlock: the return value only matters at the **very top**, to hand the caller back the root. Every internal call is fire-and-forget; the actual work happens as a side effect on the pointers.
    * **What the LLM meant by "pre-order":** the approach traverses in **Pre-order** — you process the current node (the swap) *first*, then recurse Left, then Right (`Node → Left → Right`). The name just describes *when the node's own work happens relative to its children*: here, before descending.
    * **The Post-order alternative (and why both work):** instead of swapping on the way *down*, you can recurse first and swap on the way *back up*:
        ```cpp
        if (!root) return nullptr;
        invertTree(root->left);
        invertTree(root->right);
        std::swap(root->left, root->right);   // swap AFTER children are done
        return root;
        ```
        For inversion, pre-order and post-order produce the identical result, because each node's swap is **independent** of what its subtrees look like — the order you visit nodes doesn't change the outcome. (This is *not* true for problems where a node's answer depends on its children's computed results, like height or diameter — those *require* post-order. Invert is just forgiving because the operation is local and order-agnostic.) For the same reason, the left-vs-right recursion order is interchangeable, and even an iterative BFS (swap each node's children as you pop it off a queue) works.

#### Iterative Variant (Explicit Stack — the overflow-proof form)
Same algorithm, but the traversal stack is made explicit on the heap instead of riding the call stack (see §1 for *why* — deep skewed trees and embedded targets need this):
```cpp
TreeNode* invertTree(TreeNode* root) {
    if (!root) return nullptr;                 // early return on empty tree
    std::stack<TreeNode*> st;                  // deque-backed default; depth unknown so no reserve
    st.push(root);
    while (!st.empty()) {
        TreeNode* node = st.top(); st.pop();
        std::swap(node->left, node->right);    // UNCONDITIONAL swap
        if (node->left)  st.push(node->left);  // push only real children
        if (node->right) st.push(node->right);
    }
    return root;
}
```
* **The critical bug — swap UNCONDITIONALLY.** The tempting condition "swap only if *both* children are non-null" is wrong: a node with a left child but a null right child would be skipped, leaving the left child stranded on the left. Inversion requires a left child to *become* the right child **even when it's trading places with `nullptr`**. The children are just memory addresses — swapping a valid address with `0x0` is legal and exactly what's needed. Separate the two concerns: **swap always**, then **push only the non-null children** (pushing `nullptr` would crash on the next `->left` dereference).
* **Container choice:** default `std::stack<TreeNode*>` (deque-backed). You can't `.reserve()` a meaningful size because max depth is unknown up front, so the deque's chunked growth is the right default — no `O(N)` reallocation spikes.
* **Traversal order note:** this explicit-stack version is still a DFS pre-order in spirit (process/swap on pop, then push children). Swapping to a `std::queue` turns it into an iterative BFS — and for invert, both produce the identical tree, per the order-independence above.

### [2] Maximum Depth of Binary Tree

* **The Core Pattern:** Bottom-Up DFS (**Post-order**). A node's depth is `1 + the deeper of its two subtrees`. Each recursive call returns its subtree's depth *up* to the parent, which combines them — the archetype of "a node's answer is built from its children's answers" (the pattern foreshadowed in `[1]`).
* **My Initial Approach:** Got the shape right on the first try — null → return 0, recurse both sides, return the max. My own mental trace was accurate: left branch descends to a leaf, hits the null wall, unwinds computing, then the right side does the same, and *"as I come back up, I carry forward only the max of the two sides."* That sentence **is** the definition of bottom-up post-order — good instinct to trust.
* **Where I Stumbled / What I Lacked:**
    * Wrote `1 + maxDepth(left)` and `1 + maxDepth(right)` as two separate statements → **two additions per node**. Factor the `+1` out: `1 + max(left, right)` adds the current level exactly once. One add, not two.
    * Parked the results in temp locals; unnecessary — inline both calls straight into `max`.
    * (Didn't hit these, but lock them in for next time): base case must return **0**, not 1 — a null subtree has zero depth, and a leaf then correctly resolves to `1 + max(0,0) = 1`. And this traversal is **mandatorily post-order** — unlike Invert, the parent literally can't compute until both children return, so you can't reorder the work.
* **Insights Gained:**
    * **The recursion shift that matters:** Invert used the return value only at the very top (inner calls were fire-and-forget side effects). Here **every** return value is load-bearing — each call hands its depth to its parent. This is the jump from "recursion for side effects" to "recursion that computes a value up the tree," and it's the template for Balanced Tree, Diameter, and subtree-sum problems next.
    * **The decision rule for traversal order:** ask "does this node depend on its *descendants'* computed results?" Yes → post-order. That single question picks the traversal every time.
    * BFS (level-order + level-snapshot from §1) also solves it by counting levels, but DFS is tighter here (`O(H)` vs `O(W)` space).
* **Time & Space Complexity:** $O(N)$ Time / $O(H)$ Space (recursion stack) — $O(\log N)$ balanced, $O(N)$ skewed.

### [3] Diameter of Binary Tree

> The one that cost four days. Not because the code is hard — the final function is `maxDepth` with three extra characters — but because the *mental model* of how data moves through a recursion had to be rebuilt from scratch. The breakthrough: **diameter is a free by-product of computing height**, not a separate traversal.

* **The Core Pattern:** Bottom-Up DFS (**Post-order**) — the exact `maxDepth` engine from `[2]` with a scoreboard **taped to the side of it**. Every node has two jobs:
    * **The main job (report upward):** return `1 + max(left, right)` — the height of the subtree rooted here — so the *parent* can do its own math. This is the fuel that keeps the recursion climbing.
    * **The side hustle (the scoreboard):** the longest path *passing through this node* is `leftHeight + rightHeight` (join the deepest-left reach to the deepest-right reach). Compare that against a global max and keep the biggest ever seen.
    ```cpp
    maxDiameter = std::max(maxDiameter, leftLength + rightLength); // side hustle
    return 1 + std::max(leftLength, rightLength);                   // main job
    ```
    The answer is **not** the value returned from the root — it's whatever the scoreboard holds after the whole tree has reported in.

* **Why `leftLength + rightLength` is the correct edge count.** With the node-count convention (null → 0, leaf → 1), the value a child returns equals *the number of edges from the current node down to that child's deepest leaf*. So adding the two sides counts the edges on the full path that bends through this node — exactly the diameter definition. No `+1`, no off-by-one: a node with two leaf children returns-paths `1` and `1`, giving a diameter of `2` edges, which is correct.

* **The "Gotcha":**
    * **You cannot compute diameter on the way *down*.** The first instinct — push to a stack, keep a `leftCounter`/`rightCounter`, add them — is doomed, and the gut feeling that "something is off" was right. Two reasons: **(1) depth is a bottom-up metric** — standing at a node looking into two dark subtrees, you have *zero* idea how deep they go until you've descended and bounced back. **(2) running counters get corrupted across branches** — in a linear structure (array, linked list) there's one path so a counter works, but a tree has a *different isolated state per node*; a counter mutated by the left branch is garbage by the time an unrelated right branch reads it.
    * **The iterative version needs a second memory structure (a "ledger").** If you refuse the call stack, an explicit `std::stack` for navigation is **not enough** — you also need a hash map `{node → height}` to manually pass results back up. The flow: dive to the leaves, record `ledger[leaf] = 0`, then a parent waits until *both* children are in the ledger before computing its own `left + right` (scoreboard) and `1 + max` (its own ledger entry). This is literally hand-simulating what the recursive call stack does for free — which is *why* recursion makes this problem look trivial and iteration makes it painful.
    * **Where does the global max live? Not on the stack.** A plain `int maxDia` declared inside the recursive function gets a **fresh, isolated copy in every stack frame** — updates deep in the tree never reach the top, and the value is lost on each return. Two correct fixes: **(1) pass by reference** `int& maxDiameter` — every frame then reads/writes the *same* memory address (the systems-engineer choice, thread-safe, no spooky action at a distance); or **(2) a `class Solution` member variable** — it lives *outside* the recursion so it survives the whole traversal (the LeetCode-idiomatic choice, keeps the signature clean). This code took option (1).
    * **Latent bug — initialize the scoreboard to `0`, not `INT_MIN`.** The committed code seeds `maxDiameter = std::numeric_limits<int>::min()`. It passes every LeetCode case because the guaranteed ≥1 node forces at least one `max(INT_MIN, 0)` update — but on a genuinely empty tree it would return `INT_MIN` garbage instead of `0`. A diameter can never be negative; `0` is the honest floor. Harmless here, but the kind of thing a reviewer flags.

* **The Struggle & Insights:**
    * **The wall was vocabulary, not capability.** Four days stuck feeling like DFS/BFS/diameter were alien — when the "start at root, dive to a leaf, compute something coming back up" instinct already built twice *is* DFS. The entire gap was realizing the academic labels described a mental model already in hand.
    * **The plumbing model that cracked it:** picture the tree as PVC pipes hanging from the ceiling. You can't measure a pipe top-down (you're one person, you can't be in both the left and right pipe at once). So you drop a worker to every dead-end; each yells its length up to the joint above. Each joint does two things: **(a)** connect its left and right pipe for a moment and check if that horizontal span beats the record (diameter), and **(b)** report `1 + its longest single pipe` up to *its* boss (height). Diameter isn't a path you walk — it's something each node checks in passing while its real job is reporting height.
    * **Why `return 1 + max(left, right)` and not `left + right`:** the return value feeds the *parent's* calculation, and a parent only cares about the single deepest reach beneath this node (it can only continue *one* path upward), plus `1` for the edge connecting them. Return `left + right` instead and you hand the parent a bent path it can't extend — the chain breaks. The two numbers are deliberately different: `left + right` is for the scoreboard (a path that *ends* here), `1 + max` is for the parent (a path that *continues* up).
* **Time & Space Complexity:** $O(N)$ Time (each node reports exactly once) / $O(H)$ Space (recursion stack) — $O(\log N)$ balanced, $O(N)$ skewed. The iterative hash-map variant is also $O(N)$ time but adds $O(N)$ space for the ledger — strictly worse than recursion here, which is why recursion wins unless depth threatens the call-stack cap.

### [4] Balanced Binary Tree

* **The Core Pattern:** Bottom-Up DFS (**Post-order**) again — same height engine, now used as a *validity check*. A tree is height-balanced iff **every** node's two subtree heights differ by at most 1. The elegant single-pass trick: compute heights bottom-up and, the moment any node is found imbalanced, abort — don't bother finishing the rest of the tree.
* **Two implementations, written in sequence (the journey matters):**
    * **(A) Flag by reference + `&&` masking.** A `bool& resultFlag` threaded through the recursion; each node ANDs in its own local check: `resultFlag = resultFlag && (std::abs(left - right) <= 1)`.
    * **(B) Sentinel `-1` + guard clauses (the committed/optimal form).** Drop the flag entirely; **weaponize the return value**. A real height is always ≥ 0, so `-1` is a free "impossible" value meaning *"imbalance found below — abort."* Each node checks its children's returns and bails to `-1` on any failure.
    ```cpp
    int leftLength = calculateDepth(root->left);
    if (leftLength == -1) return -1;              // short-circuit: skip the right subtree entirely
    int rightLength = calculateDepth(root->right);
    if (rightLength == -1) return -1;             // short-circuit: abort up the chain
    if (std::abs(leftLength - rightLength) > 1) return -1;
    return 1 + std::max(leftLength, rightLength); // happy path, at base indentation
    ```
* **The "Gotcha":**
    * **Why the `&&` is load-bearing — the masking trap.** The first flag version used plain assignment `resultFlag = (abs(left-right) <= 1)`. Fatal: **a perfectly balanced ancestor silently overwrites a `false` from an unbalanced descendant.** Corporate-ladder picture — a terrible manager deep down has 10 reports left, 0 right (wildly unbalanced, flag goes `false`), but the CEO sees a left VP of depth 12 and a right VP of depth 12 (difference 0) and flips the flag back to `true`, declaring the whole company balanced. The `&&` turns the flag into a **permanent latch**: once any node drops it to `false`, no ancestor can ever revive it.
    * **The sentinel beats the flag by short-circuiting — but only if you order the checks right.** The payoff of `-1` is skipping dead work: if the left subtree already failed, you should **never even recurse into the right subtree**. The intermediate attempt computed *both* `left` and `right` before checking either (`if (leftLength == -1 || rightLength == -1 || ...)`) — correct answer, but it **forfeits the early exit**, traversing the entire right side of a tree whose fate was already sealed on the left. The fix is to check `leftLength == -1` *before* the line that computes `rightLength`. (The reason both had to be computed in that attempt: once past the guards, you genuinely need both values for the `abs` difference *and* the `1 + max` return — so the only lever is *ordering*, not merging.)
    * **`-1` is a safe sentinel *here* precisely because the value domain is non-negative.** §1 warns against magic numbers like `-1` that "could collide with real values" — that caution applies when the real answer could itself be negative. Heights never are, so `-1` can't collide. Context decides whether a sentinel is clean or dangerous.
    * **Dead code left behind.** The final `isBalanced` still carried a `bool resultFlag = true;` from version (A) — unused once the sentinel took over. Remove it; a leftover declaration reads as "I didn't finish refactoring."
* **The Struggle & Insights:**
    * **The arrow anti-pattern (pyramid of doom).** Trying to avoid "too many `return`s," an intermediate version nested the checks with C++17 `if (init; cond)` three levels deep. It *works* and the if-initializer syntax was used correctly — but the happy-path (the actual height calc) ends up buried at the deepest indentation, and the reader has to hold every outer condition in working memory to reach it. **Guard clauses (early returns / the "bouncer pattern") are the fix:** failures get kicked out at the door, so the happy path lives flat at the base indentation and the reader can forget each handled case the instant it's past. Flat early-returns are what reviewers want to see — not nesting gymnastics to hit some "one return per function" rule.
    * **The decision rule held:** "does this node depend on its descendants' results?" → yes (it needs both child heights) → post-order. Same engine as `[2]` and `[3]`; the only new idea is *using the return value as an error channel*.
    * **The naive trap this design sidesteps:** the obvious-but-slow approach computes `height()` at every node *and separately* checks balance by recomputing both subtree heights — that re-walks subtrees over and over, costing $O(N \log N)$ balanced / $O(N^2)$ skewed. Folding the balance check *into* the single height pass (returning `-1` as the failure signal) is what collapses it to one $O(N)$ sweep. Going straight to bottom-up avoided ever writing the naive version.
* **Time & Space Complexity:** $O(N)$ Time — single post-order sweep, each node visited once (early-abort only makes it faster, never slower) / $O(H)$ Space for the recursion stack — $O(\log N)$ balanced, $O(N)$ skewed.

### [5] Same Tree

> First Trees problem that is **not** the `maxDepth` height engine. The shape flips: instead of recursing into *one* tree and returning a value *up*, you walk **two trees in lockstep** and short-circuit *down*. Pre-order, not post-order — and the public signature is already the recursive one, so no helper is needed.

* **The Core Pattern:** Parallel dual-pointer DFS (**Pre-order**). Advance a pointer into *both* trees simultaneously; at each paired node, check "do these two agree?" (both exist, same value) **before** descending. Two trees are identical iff every lockstep pair agrees — structure *and* values. Because the check at a node needs nothing from its children (a mismatch anywhere is immediately fatal), it's pre-order: resolve the node first, then recurse.
    ```cpp
    bool isSameTree(TreeNode* p, TreeNode* q) {
        if (!p && !q) return true;    // both empty → this pair agrees
        if (!p || !q) return false;   // exactly one empty → structural mismatch
        if (p->val != q->val) return false;
        return isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
    }
    ```
* **The "Gotcha":**
    * **Base-case *order* is a correctness requirement, not a speed tweak.** `(!p && !q)` must be tested **before** `(!p || !q)`. The OR is a **superset** of the AND — whenever both are null, the OR is *also* true. So if `(!p || !q)` runs first, the both-null case (two matching leaves bottoming out) wrongly returns `false` and the whole comparison breaks. The specific "both empty → true" case has to intercept first; the general "any empty → false" catches only the *remaining* one-sided case. (The initial instinct missed the `(!p && !q)` case entirely, then placed it *after* the OR — the reorder is what makes it correct.)
    * **Deriving the one-sided check.** The sticking point was expressing "one node is empty while the other isn't." Once the both-null case is already handled above, `(!p || !q)` is exactly that: control only reaches this line when the two are *not* both null, so "at least one is null" now means "precisely one is null" — a clean structural-mismatch signal.
    * **No helper function needed here — and knowing *why* matters.** `[3]` Diameter and `[4]` Balanced both needed a helper because the recursion had to return/carry something the public API doesn't expose (a subtree *height*, plus an `int&` accumulator). Same Tree's public signature `(p, q) → bool` **is already the exact signature the recursion wants** — two pointers in, one bool out — so it recurses on itself directly. Rule of thumb: reach for a helper only when the recursive contract differs from the public one.
* **The Struggle & Insights:**
    * **Three versions, and the final refinement is runtime-identical — your instinct was right.** The arc: **(1)** a helper returning `-1`/`1` int sentinels; **(2)** dropped the helper and the sentinels for a plain `bool`, but still wrote two explicit `if (!isSameTree(...)) return false;` guards; **(3)** collapsed those two guards into `return left && right;`. Version (3) is **pure readability — zero runtime difference** from (2). The reason is that `&&` is **short-circuit**: `isSameTree(p->right, …)` is evaluated *only if* the left call returned `true`, which is byte-for-byte the same control flow as "if the left subtree mismatched, return false immediately and never touch the right." Same number of recursive calls, same early exit, cleaner code.
    * **The `-1`/`1` sentinel was overkill here.** In `[4]` Balanced the `-1` sentinel earned its keep because the function *also* had to return a real height — one channel carrying two meanings. Same Tree only ever answers a yes/no question, so `bool` is the honest return type; encoding true/false as `1`/`-1` added a decode step (`!= -1`) for nothing. Match the return type to the actual question.
    * **The recurring `&&` theme.** This is the *same* short-circuit `&&` that latched the flag in `[4]` — there it prevented a balanced ancestor from masking an unbalanced descendant; here it lets the first mismatched pair kill the entire comparison without walking the rest. Boolean `&&` as a propagate-failure-upward mechanism is now a reusable tool.
    * **The traversal-direction flip.** `[2]`–`[4]` were bottom-up (post-order): dive to the leaves, combine results on the way *up*. Same Tree is top-down (pre-order): the verdict is decided at each node *before* descending, and failure propagates *down*-first via short-circuit. Same recursion machinery, opposite flow of information.
* **Time & Space Complexity:** $O(\min(N, M))$ Time — the lockstep walk stops at the first mismatch or the shallower tree's bottom; worst case (identical trees) is $O(N)$, every node paired once / $O(H)$ Space for the recursion stack — $O(\log N)$ balanced, $O(N)$ skewed.

### [6] Subtree of Another Tree

> Directly builds on `[5]`. Recognizing "this is Same Tree, run at every anchor point" is the whole unlock — the identity check is *reused verbatim*; the only new work is wrapping it in a search over every node of the big tree.

* **The Core Pattern:** **Nested DFS** — two recursions, one inside the other:
    * **Outer (the anchor search):** walk *every* node of `root` as a candidate starting point. Pre-order, OR the results together: `isSubtree(left) || isSubtree(right)`.
    * **Inner (the identity check):** at each anchor, ask "is the tree rooted *here* exactly equal to `subRoot`?" — which is **literally `[5]` Same Tree** (`checkTree` *is* `isSameTree`, copied over). First exact match anywhere → the answer is true.
    ```cpp
    bool isSubtree(TreeNode* root, TreeNode* subRoot) {
        if (!subRoot) return true;              // empty pattern matches anything (see gotcha)
        if (!root)    return false;             // walked off the big tree, nothing left to anchor
        if (checkTree(root, subRoot)) return true;   // exact match anchored here
        return isSubtree(root->left, subRoot) || isSubtree(root->right, subRoot);
    }
    ```
* **The "Gotcha" — answering the two questions this raised:**
    * **"Can I just `return true` when `subRoot` is null?"** — **Yes, the reasoning is sound: an empty tree is a subtree of *every* tree** (there's a trivially-matching empty position everywhere). So `if (!subRoot) return true;` is correct unconditionally — it does *not* depend on what `root` is. **But** note the practical caveat: LeetCode guarantees both trees have ≥ 1 node, and the outer recursion only ever changes `root` (never `subRoot`), so **this line never actually fires** on the given constraints. It's a *defensive / semantic* guard — correct and clarifying, but not load-bearing here. Don't expect it to be "doing work."
    * **"Then what is the `if (!root) return false;` actually accomplishing?"** — *This* is the load-bearing guard, and it does two jobs at once: **(1)** it's the **base case that terminates the anchor walk** — when `root` falls off the bottom (a leaf's null child) with a non-empty pattern still to place, there's nowhere left to anchor, so stop; **(2)** it **prevents the null dereference** on the next line's `root->left` / `root->right`. Remove it and a leaf's recursion calls `isSubtree(nullptr, subRoot)`, which would then try `nullptr->left` and crash. So the confusion ("not sure what I'm accomplishing") resolves cleanly: the `!subRoot` line is defensive dead-code under LeetCode's constraints; the `!root` line is the real recursion terminator.
    * **Guard order / why hoisting the null-checks above `checkTree` is the refinement.** The first draft called `checkTree(root, subRoot)` *first*, then checked `if (!root) return false;`. That works (`checkTree` handles a null `root` and returns false), but it wastes a `checkTree` call at every null child. Hoisting both null-guards above `checkTree` means the identity check only ever runs on real nodes — same result, a bit leaner.
    * **The `||` mirrors `[5]`'s `&&`.** Same Tree used `&&` to propagate *failure* up (any mismatch kills it). Subtree uses `||` to propagate *success* laterally (any one anchor matching wins), and it **short-circuits**: the moment the left half finds a match, the right half is never searched. Same first-match early-exit tool, flipped polarity.
* **The Struggle & Insights:**
    * **Reuse recognition is the skill.** Spotting "this is Same Tree run at every node" and lifting `checkTree` wholesale is exactly the pattern-composition that interviews reward — don't re-derive what you already solved one problem ago.
    * **On "is this optimal?" — no, and here's the honest ranking.** This brute-force nested-DFS is **$O(M \times N)$** worst case (`M` = nodes in `root`, `N` = nodes in `subRoot`): you run an $O(N)$ identity check at up to $M$ anchors. The pathological case is two long chains of equal values (e.g. all `1`s) — every anchor's `checkTree` walks deep before mismatching at the tail, and the costs sum to $M \times N$.
    * **Why the string / hash approaches are genuinely faster ($O(M+N)$) — the intuition you were missing.** Your read that "both do the same number of comparisons" isn't right, and *why* is the real lesson: the brute force **re-compares overlapping regions over and over** — `checkTree` at a node and `checkTree` at its child both re-walk the same shared nodes. The linear approaches kill that redundancy by computing a **per-subtree fingerprint exactly once, bottom-up**, then doing a single cheap lookup:
        * **Serialize-and-substring:** flatten both trees to strings (pre-order), then test "is `subRoot`'s string a substring of `root`'s string?" with **KMP** → $O(M + N)$. The classic bug: you **must** insert **delimiters and explicit null-markers** (e.g. `#` for null, `,` between values) — without them `2 # #` vs `12 # #` or value `1,2` vs `12` produce false matches, and a true subtree-string could match mid-value.
        * **Merkle hashing:** hash every subtree bottom-up (`hash(node) = f(val, hash(left), hash(right))`); `subRoot` is a subtree iff its hash appears among `root`'s subtree hashes — then re-verify that one candidate with `checkTree` to rule out a hash collision. Also $O(M + N)$.
        * **The unifying idea:** trade the brute force's *repeated top-down re-comparison* for a *single bottom-up precompute* of a comparable signature. That's the whole "more optimal" claim.
    * **What to actually say in an interview:** lead with this $O(M \times N)$ nested-DFS — it's clean, obviously correct, and the expected first answer. Mention the serialize-KMP / Merkle-hash $O(M+N)$ refinements as the "can we do better?" follow-up, and name the serialization delimiter/null-marker trap to show you know where it bites.
* **Time & Space Complexity:** $O(M \times N)$ Time worst case (identity check at each anchor) / $O(H_{root})$ Space for the recursion stack. The serialize-KMP and Merkle-hash variants reach $O(M + N)$ time at the cost of $O(M + N)$ extra space for the strings/hash store.

### [7] Lowest Common Ancestor of a BST

> First problem that **exploits the BST ordering** instead of treating the tree as a generic bag of nodes. The whole thing is binary search wearing a tree costume — and the real lesson is that it needs **neither recursion nor a stack**: `O(1)` space, one pointer walking one path down.

* **The Core Pattern:** BST-guided descent (**Pattern 4**). Normalize so `p` is the smaller value and `q` the larger, then from the root compare the current node against the `[p, q]` window:
    * `current->val` is **inside `[p, q]`** → `p` and `q` sit on opposite sides (or one *is* this node) → this is the **split point = the LCA**. Return it.
    * `current->val < p` → both targets are larger → the LCA is to the **right**. Descend right.
    * `current->val > q` → both targets are smaller → descend **left**.
    ```cpp
    TreeNode* current = root;
    while (current) {
        if (current->val >= p->val && current->val <= q->val) return current; // split point
        current = (current->val <= p->val) ? current->right : current->left;   // one direction only
    }
    ```
* **The "Gotcha":**
    * **The signature hands you `TreeNode*`, so compare `->val`, not the pointers.** The silly-but-classic slip: writing `p <= current->val` compares a *pointer* against an *int*. BST comparisons are always on `->val`. (This is the tree analogue of comparing iterators instead of the values they point to.)
    * **Normalize `p ≤ q` once, up front.** The `current->val >= p->val && current->val <= q->val` window check only works if `p` is genuinely the lower bound. The one-line `if (p && q && p->val > q->val) return lowestCommonAncestor(root, q, p);` swap at the top guarantees it — a single recursive bounce that re-enters with the arguments ordered, then never fires again. (You *could* instead write a direction test that doesn't assume an order, but normalizing is cleaner and keeps the window check trivial.)
    * **The inverted push guard — and why it couldn't bite the way a generic tree would.** The first draft (stack version) pushed children under `if (!node->left)` / `if (!node->right)` — the negation inverted, pushing *null* children. In a generic branching traversal that's a crash (you'd pop a `nullptr` and dereference it). Here it's masked by a deeper truth: **this walk only ever follows one direction, so the stack never holds more than a single node anyway** — which is the tell that the stack was never needed.
* **The Struggle & Insights:**
    * **The binary-search intuition is the unlock, and it transfers verbatim from the Binary Search topic.** "If the node is below `p`, go to the higher branch (right); otherwise go to the lower branch (left)" is exactly `lo`/`hi` narrowing on a sorted array — except you follow child pointers instead of recomputing a `mid` index. The BST *is* the sorted array, pre-partitioned by structure.
    * **The big realization — the stack was dead weight (answering "do BST problems need a stack?").** The first solution carried a `std::stack`, out of habit from the generic tree template. The template needs a stack because each node has *two* children to explore and you must remember the branch not taken. **A BST search-descent never branches** — the ordering picks exactly one child — so there's nothing to remember, nothing to backtrack to, and the stack never holds more than one element. Drop it for a single `current` pointer and the solution is `O(1)` space. Not foolish to have reached for the stack — it's the right default for trees in general; the skill is *noticing when the ordering collapses the branching* and the auxiliary structure becomes pure overhead. (See the expanded **Pattern 4** in §2.)
    * **But don't over-generalize.** "BSTs don't need recursion/stack" is only true for **single-path** operations (search, insert, delete, LCA, floor/ceil, closest). The moment a BST problem must visit *many* nodes — Kth Smallest, Validate BST, range sums spanning both subtrees — you branch again and the stack/recursion returns. The dividing line is *single root-to-target path vs. full/partial traversal*.
    * **Why BST LCA is strictly simpler than general-tree LCA (`[Pattern 6]`).** In an unordered tree you must *search both subtrees* for `p` and `q` and detect where they split — inherently `O(N)` and recursive. The BST ordering lets you *walk straight to* the split point without exploring anything off-path: `O(H)` time, `O(1)` space.
* **Time & Space Complexity:** $O(H)$ Time — one root-to-split-point descent ($O(\log N)$ balanced, $O(N)$ skewed) / **$O(1)$ Space** — a single reassigned pointer, no stack, no recursion. (The recursive normalization bounce adds at most one extra frame and can be hoisted into an iterative swap if you want strict `O(1)`.)

### [8] Binary Tree Level Order Traversal

> First **BFS** problem — and the first where the *data structure*, not the recursion shape, is the whole answer. The code is the canonical level-snapshot loop from §1; the value here is that the reasoning was derived from first principles (FIFO → queue → per-level count) rather than pattern-matched.

* **The Core Pattern:** BFS level-order (**Pattern 3** — queue + level-snapshot). Push the root, then repeatedly: **snapshot the queue size** (that's exactly how many nodes are on the current level), process precisely that many — popping from the front, recording the value, enqueuing each non-null child to the back — then that batch *is* one level. The children pushed during the batch belong to the next level and are correctly excluded by the frozen count.
    ```cpp
    const size_t n = nodeQueue.size();   // SNAPSHOT: how many nodes on THIS level
    for (size_t i = 0; i < n; ++i) { ... }  // process exactly one level's worth
    ```
* **The "Gotcha":**
    * **The `2^i`-per-level assumption fails on any non-perfect tree — and the snapshot is the fix.** The first instinct ("level `i` has `2^i` nodes, so process `2^i` each iteration") is only true for a *perfect* tree; a lopsided or sparse tree has fewer, and you'd pop into an empty/next-level region and corrupt the grouping. The breakthrough was realizing you don't need a formula: **after finishing level `i`, the queue holds exactly the nodes of level `i+1`**, so `queue.size()` *is* the count — read it fresh each outer iteration. Deriving that independently is the real insight of this problem.
    * **Guard the empty tree at the push, not inside the loop.** `if (root) nodeQueue.push(root);` keeps a `nullptr` out of the queue entirely, so the main loop never has to special-case it and you never pop-then-dereference null. (Same discipline as "push only non-null children.")
    * **Snapshot into a `const` *before* the inner loop.** Reading `nodeQueue.size()` live in the loop condition would bleed the next level's freshly-pushed children into the current level. Freezing it is non-negotiable.
* **The Struggle & Insights:**
    * **The data-structure reasoning was spot-on, and it's the point.** You correctly ruled out stack/recursion (LIFO — dives deep, wrong for level-by-level) and identified you wanted a **FIFO**: insert at the tail, remove from the head, both `O(1)`. That's precisely a queue. Your "a linked list with tail-insert + head-remove would be `O(1)` at both ends" instinct is exactly right in the abstract — and `std::queue` is the concrete realization of it, **default-backed by `std::deque`**, which gives the same `O(1)` both-ends behavior with *far* better cache locality than a real `std::list` (chunked contiguous storage, no per-node heap allocation). Same Big-O, better constants — which is why you reach for `std::queue`, not `std::list` (reinforced in §1).
    * **Should you feel happy or skeptical? Happy — but here's the substance so it's not blind trust.** The LLM's "good to go" is correct: this is the canonical, optimal shape. Concretely, what makes it good: the empty-tree guard, the `const` snapshot, pushing only non-null children, *and* the `levelVector.reserve(n)` — reserving the level vector up front to avoid reallocations as it fills is a systems-minded touch most candidates skip. There's exactly **one** micro-opt left: `resultVector.push_back(levelVector)` copies the finished level's buffer; since `levelVector` is rebuilt next iteration, hand it over instead — `resultVector.push_back(std::move(levelVector))` (or `emplace_back`) transfers the heap buffer by pointer swap (`O(1)`) rather than deep-copying it. Minor, but it's the only thing between "clean" and "nothing left to tighten." Healthy skepticism is good; the way to resolve it is to *audit against a checklist* (guards, snapshot, allocation, moves) rather than take the verdict on faith — which is exactly what you're doing by asking.
    * **Why BFS here and not DFS.** "Group by level" is the textbook BFS signal (§2 Pattern 3). A DFS *can* produce the same grouping if you pass a `depth` parameter and index into `result[depth]`, but BFS maps directly onto the problem's shape — the queue literally holds one frontier at a time.
* **Time & Space Complexity:** $O(N)$ Time — every node enqueued and dequeued exactly once / $O(N)$ Space — the queue holds at most one level, whose width can reach `~N/2` in the last level of a balanced tree, plus the `O(N)` output. (BFS space is `O(W)`, max level width — worst case `O(N)`, versus DFS's `O(H)`.)

### [9] Binary Tree Right Side View

> Two solutions, two paradigms. The BFS version fell out almost instantly (it's `[8]` keeping one value per level). The DFS version is the real lesson: it doesn't skip any work — it **rigs the visiting order** so that "first node to reach a depth" and "rightmost node at that depth" become the same thing.

* **The Core Pattern (two ways):**
    * **(A) BFS — last node of each level (Pattern 3).** Exactly the `[8]` level-snapshot loop, but instead of collecting the whole level, keep only the node popped *last* in each batch. Declaring `TreeNode* node;` *outside* the inner `for` means that when the loop exits, `node` is still holding the last one popped — the rightmost of that level — so push `node->val` right after the inner loop. No extra bookkeeping.
    * **(B) Right-first DFS + depth lock (Pattern 2, top-down).** Recurse **right before left** (`Node → Right → Left`, a mirrored pre-order), carry the current `depth` down as a parameter, and record a node only if it's the **first node ever to reach that depth**:
        ```cpp
        if (resultVector.size() == depth) resultVector.push_back(root->val);  // the lock
        ```
        Because the right side always goes first, the first arrival at any depth is the rightmost node on that level.

* **The "Gotcha":**
    * **"Can I grab the rightmost node per level without processing the others?" — No, in any paradigm. Your instinct was correct.** You can't know where the rightmost node of level `d+1` is until you've seen every node's children on level `d`. The right subtree may stop short, and then a node hanging off the *far left* is what you see from the right. Example: root `1`, left child `2`, right child `3`, and `2` has a child `4` → the view is `[1, 3, 4]`. Here `4` comes from the left side. So BFS has to enqueue every node's children. And **the DFS doesn't skip nodes either**: the lock rejects `2` from the result, but the recursion *still walks through `2`* and that's how it reaches `4`. Both versions are $O(N)$ time; you can't beat that. The DFS trick is about **order, not pruning**.
    * **"Does right-preference help BFS?" — No. It's a DFS idea.** You *can* push `right` before `left` in BFS and take the **first** node of each batch instead of the last, but it's the same work with the roles swapped: every node still gets enqueued. Your BFS is already optimal for BFS. The priority only matters in DFS, because DFS has a "who gets there first" race that BFS doesn't have (BFS reaches a whole level at once).
    * **Why the lock is `size() == depth` (and why `==` is enough).** Invariant: `resultVector.size()` = **number of floors already claimed** (one per depth seen so far). Recursion goes down one level per call, so a node at `depth` can only show up after depths `0 … depth-1` are already claimed. That means `depth` can never be *greater than* `size()`. It's either **equal** (first time anyone has reached this depth → claim it) or **less** (already claimed by a node further right → rejected, but keep descending). Trace on `[1, 2, 3, null, 4]`: `1`@0 (size 0 → claim), `3`@1 (size 1 → claim), back up, `2`@1 (size 2 → reject), `4`@2 (size 2 → claim) → `[1, 3, 4]`.
    * **Signed/unsigned compare.** `resultVector.size()` is `size_t` and `depth` is `int`, so `-Wall` gives a `-Wsign-compare` warning. It's safe here because depth is never negative, but declaring the parameter `size_t depth` removes the warning and says what you mean.

* **The Struggle & Insights:**
    * **"Why is the helper `void`, with depth and result passed in as side-hustles?" This is the key question.** It comes down to *which direction the information flows*:
        * **Return value = data flowing UP** (Pattern 1, bottom-up). In `[2]`–`[4]` the parent couldn't finish until its children *reported back* (heights, `-1` failure). The return value was that report.
        * **Here nothing flows up.** A parent doesn't need anything from its children to do its job: it already knows its depth, and it either claims a floor or doesn't. With nothing to report, the honest return type is **`void`**. (Same as `[1]` Invert, where the inner calls were fire-and-forget.)
        * **`depth` = data flowing DOWN, and it's passed by VALUE, not by reference.** Check the code: `int depth`, no `&`. That's deliberate and it's why the code is correct. Each frame gets **its own copy**, so when `2` (the left child) gets called with `depth + 1`, it sees `1` no matter how deep the right subtree went before it. Returning from a call "restores" depth for free, because the caller's copy was never touched. If you passed `int& depth`, the right subtree's increments would **leak into the left sibling**. That's the same cross-branch corruption that sank the top-down counter in `[3]` Diameter.
        * **`resultVector` = shared state, passed by REFERENCE.** Every frame has to write into the *same* vector, otherwise each frame claims floors in its own private copy and the work disappears on return. This is the same out-of-band accumulator as `[3]`'s `int& maxDiameter`.
        * **The rule:** *per-path state* (depth, path sum, remaining target) → **by value**, flowing down. *Whole-traversal state* (the answer, a global max) → **by reference** (or a class member). *A child's result its parent needs* → **return value**. Pick each channel by asking "who needs this, and in which direction?"
    * **Which is "more optimal" — honestly?** Same $O(N)$ time. Space is where they differ, and neither wins everywhere: BFS uses $O(W)$ (widest level), DFS uses $O(H)$ (height). **Balanced/bushy tree:** `W ≈ N/2` while `H ≈ log N`, so DFS uses far less memory. That's the case the LLM meant. **Skewed tree (a chain):** `W = 1` while `H = N`, so BFS wins, and DFS's `O(N)` is *call stack*, which can overflow (§1). Interview answer: present either one, then explain this `O(H)` vs `O(W)` tradeoff.
    * **Can't `reserve()` — correct, and it doesn't matter.** The result size is the number of levels, which you don't know without a separate $O(N)$ max-depth pass first, and that costs more than the few reallocations it saves. The vector holds only `H` ints, so amortized growth is nothing to worry about.
    * **Uninitialized `node` in the BFS version is safe, barely.** `TreeNode* node;` with no initializer is only read after the inner loop, and that loop always runs at least once (the outer loop only runs on a non-empty queue). It's correct, but `TreeNode* node = nullptr;` costs nothing and keeps reviewers and sanitizers quiet.

* **Time & Space Complexity:** Both: $O(N)$ Time (every node visited once). **BFS:** $O(W)$ Space for the queue (worst ~`N/2`). **DFS:** $O(H)$ Space for the recursion stack ($O(\log N)$ balanced, $O(N)$ skewed). Both have an $O(H)$ output on top.

### [10] Count Good Nodes in Binary Tree

> The first problem where information flows in **both directions at once**: the running max goes *down* as a by-value parameter, and the count comes back *up*. The committed version sent the count up through an `int&` shared counter. The refactor sends it up through the return value. Both are correct, and both have the same Big-O.

* **The Core Pattern:** Top-down DFS (**Pattern 2**, pre-order). A node is "good" if no node on the path **from the root down to it** has a larger value. Carry `maxSoFar`, the largest value seen on the current root-to-node path, down as a parameter. At each node: good iff `node->val >= maxSoFar`, then pass `max(maxSoFar, node->val)` down to both children.

* **The "Gotcha":**
    * **Reading the problem: "greater than" compared to whom?** Not the immediate parent, and not just the topmost root. It's **every ancestor on the path from the root to this node**, which collapses to a single number: the **max on that path**. That's why one `maxSoFar` is all the state you need. Example: path `3 → 1 → 2`: `2` is bigger than its parent `1`, but it's *not* good, because `3` is above it.
    * **"When I backtrack, don't I have to undo the max?" — No, because it's passed by value.** Every frame has its own copy of `maxSoFar`. When the left subtree finishes and control returns, this frame's copy was never touched, so the right child gets the *parent's* max, not whatever the left side raised it to. The "undo" happens for free when the frame pops. Same lesson as `[9]`'s `depth`: per-path state goes **by value**. If you passed `int& maxSoFar`, a big value on the left side would leak across and wrongly make right-side nodes bad.
    * **`>=`, not `>`.** "No node *greater than* X on the path" means an **equal** value doesn't disqualify X: path `3 → 3` has two good nodes. Writing `>` miscounts every node that ties the running max. (This was the one real bug.)
    * **Why `INT_MIN` as the seed is safe, and why `>=` is what makes it safe.** The root has no ancestors, so it must always count as good. Seeding `maxSoFar = INT_MIN` with `>=` guarantees that, even if the root's value is `INT_MIN` itself. With `>` that edge case would fail too. An alternative is to seed with `root->val` (LeetCode guarantees ≥1 node), which avoids the "magic minimum" entirely.

* **The Struggle & Insights — the two versions, and which one to keep:**
    * **(A) Committed: `void` helper + `int& goodNodeCount`.** The count is a shared accumulator; each good node does `++count`. This is the `[3]` Diameter / `[9]` `resultVector` pattern. A neat detail: updating `maxSoFar = root->val` *inside* the `if` is correct and needs no `std::max`, because if the node is good its value is the new max, and if it isn't, the max is unchanged.
    * **(B) Refactor: the helper *returns* the good-node count of its subtree.** This is the version you didn't feel confident about, so here's the model to build it from scratch. **Define the function's contract in one sentence:** *"`countGoodNodes(node, maxSoFar)` = how many good nodes are in the subtree rooted at `node`, given the largest value above it."* Everything else falls out of that sentence:
        * **Empty subtree** → contains 0 good nodes → `return 0;`
        * **Otherwise** → (1 if I'm good, else 0) + (good nodes in my left subtree) + (good nodes in my right subtree). The two recursive calls answer exactly those questions, given the max *including me*.
        * Trace on `3 → (1 → 3), (4)`: the left `3` returns `1` (good, since `3 >= 3`, no children), `1` returns `0 + 1 = 1`, `4` returns `1`, the root returns `1 + 1 + 1 = 3`. Each frame reports its subtree's total, and the parent adds them up. That's exactly Pattern 1 (`[2]` Max Depth's `1 + max(L, R)`, but with `+` instead of `max`).
        * **The hybrid is the new idea:** `maxSoFar` flows **down** (top-down, Pattern 2) *and* the count flows **up** (bottom-up, Pattern 1) in the same function. Nothing new mechanically; you've just never needed both directions at once before.
    * **Which is optimal?** **Identical complexity:** both are one visit per node, $O(N)$ time, $O(H)$ stack. At the hardware level the difference is noise. (A) passes one extra pointer per frame and does a store through it on each good node. (B) returns an int in a register (`eax`) and does two adds per frame. Neither is measurably faster.
    * **Which is preferred? (B), the return-value version, and here's why it's not just taste:**
        * **No hidden shared state.** (B)'s result depends only on its arguments. You can call it on any subtree and get that subtree's answer, test it in isolation, and never have to remember to zero a counter first. (A) only works if the caller set up the counter correctly.
        * **The rule from `[9]` picks it:** "a child's result its parent needs → return value." Here the parent needs its children's counts to compute its own total, so the return value is the natural channel. The `int&` accumulator is the right tool when the answer *isn't* the thing being returned. In `[3]` Diameter, for example, the return slot was already taken by height. Here the return slot is free, so use it.
        * **In an interview:** either version is accepted. Lead with (B), and mention (A) if asked about alternatives. If you blank on (B), (A) is a perfectly correct fallback.
    * **Nits in the current code:** `return countGoodNodes(...);;` has a stray double semicolon. `int currentCount = (root->val >= maxSoFar) ? 1 : 0;` can just be `int currentCount = root->val >= maxSoFar;`: a `bool` converts to `0`/`1`, and the compiler emits the same branchless `setge` instruction either way, so it's purely style.

* **Time & Space Complexity:** $O(N)$ Time (each node visited once) / $O(H)$ Space for the recursion stack ($O(\log N)$ balanced, $O(N)$ skewed). Same for both versions.

### [11] Validate Binary Search Tree

> This one took a long time to see, and the trap you fell into is the most famous one in binary trees. Nearly everyone's first attempt checks each node only against its **parent**. The fix is the same "pass state down by value" tool as `[10]` Good Nodes, but now with **two** values carried down, a floor and a ceiling, and each turn updates only **one** of them.

* **The Core Pattern:** Top-down DFS (**Pattern 2**, pre-order) with a **valid range** `(floor, ceiling)` carried down by value. At each node: if `val` is not strictly inside `(floor, ceiling)`, the tree is invalid. Otherwise:
    * **Going left:** the floor is **inherited unchanged** and the ceiling becomes `node->val`, because everything down here must be smaller than me.
    * **Going right:** the ceiling is **inherited unchanged** and the floor becomes `node->val`, because everything down here must be bigger than me.
    * The root starts with no limits, `(-∞, +∞)`.
    ```cpp
    return checkBST(root->left, floor, root->val) && checkBST(root->right, root->val, ceiling);
    ```

* **Your intuition, tightened by one word.** You wrote: *"When I traverse left, the ceiling should be current node's value, but the floor can be global."* That's right, except the floor isn't **global**. It's **inherited**: whatever floor *this node* received from above. It's only global (`-∞`) at the root. That one word is the whole problem. A concrete way to read it: **a node's floor is the value of the nearest ancestor where the path turned right, and its ceiling is the nearest ancestor where it turned left.** Every node's range is just the two most recent "turns" on its path.

* **The "Gotcha" — why each earlier attempt failed:**
    * **Attempt 1 — checking only against the parent (`isLeft` flag). This is the grandparent trap.** Tree `10 → left 5 → right 15`: `15 > 5`, so it obeys its parent. But `15` sits in `10`'s **left** subtree, where everything must be `< 10`. A BST rule is about the **whole subtree**, not just the edge to the parent. So each node has to know the limits set by *every* ancestor, and the parent alone isn't enough information. Whether a node is a left or right child stops mattering once the range carries that information.
    * **Attempt 2 — running min/max over the whole path.** This was closer, since you saw you needed more than the parent. But `min`/`max` over *all* ancestors mixes both directions together. Going right lowers nothing, yet your running `min` still carries values from left turns higher up. That's the "I'm on the right side, but I still need characteristics of the left side" knot. The fix is not "the path's min and max". It's that **each direction of turn updates only its own limit**: left turns set ceilings, right turns set floors. You don't need both sides' characteristics at once, because the range carries exactly the two that matter.
    * **Attempt 3 — the right logic with arguments in the wrong slots.** The function took `(minVal, maxVal)`, but the call sites passed them in swapped order in three places: the initial call (`max(), min()`), the left call (moved the floor instead of the ceiling), and the right call (moved the ceiling instead of the floor). Same-typed adjacent parameters are a classic place for silent bugs, since the compiler can't catch a swap. Your final code renamed them **`floor, ceiling`** and always passed them in **low-then-high order**, like the interval `(floor, ceiling)`. That naming is what fixed it, and it's a habit worth keeping.
    * **Strict `<`, not `<=`.** LeetCode's BST definition is *strictly* less on the left and *strictly* greater on the right, so a duplicate value makes the tree invalid. That's why the check is `val >= ceiling || val <= floor → false`.

* **The C++ landmines (the part that felt like a minefield):**
    * **The `INT_MIN` / `INT_MAX` collision → widen the bounds.** With `int` bounds seeded to `INT_MIN`/`INT_MAX`, a perfectly valid node whose value *is* `INT_MIN` fails `val <= floor` (equal, not strictly greater). The sentinel collides with real data. That's the same "magic value that could collide" warning from §1, and the opposite of `[4]` Balanced, where `-1` was safe because heights are never negative. Here the full `int` range is legal data, so the sentinels have to live **outside** that range. A 64-bit bound does that.
    * **`long` is not reliably 64-bit. Use `long long` (or `int64_t`).** The C++ standard only guarantees `long ≥ 32 bits`. Its actual width depends on the platform's **data model**:
        * **LP64** (64-bit Linux/macOS, what LeetCode runs on): `long` = 64 bits. Your first pass worked by luck of the platform.
        * **LLP64** (64-bit Windows): `long` = 32 bits. That brings back the `INT_MIN` collision.
        * **ILP32**, which is **your world: ARM Cortex-M / STM32**: `long` = 32 bits.
        `long long` is guaranteed ≥ 64 bits everywhere. If you want the width spelled out, `int64_t` from `<cstdint>` is the embedded-style choice. Either is right. Bare `long` is the trap.
    * **`std::numeric_limits<double>::min()` is NOT negative.** For integer types, `min()` is the most negative value. For floating-point types, `min()` is the **smallest positive normalized** value (~`2.2e-308`, the closest a double can get to zero before losing precision). The most-negative double is **`lowest()`**. Integer limits answer "how far can I count?", and float limits answer "how close to zero can I resolve?". A double bound (`-infinity()`) would *work* here, but mixing float and int comparisons is a bad habit, so stick with a wider integer.
    * **`static_cast<long long>(root->val)` everywhere is overdoing it.** Comparing or passing an `int` against or into a `long long` triggers C++'s **implicit widening conversion**: the `int` is sign-extended to 64 bits automatically, with zero chance of losing data. The compiler emits the same sign-extend instruction with or without your cast, so it adds no safety and no speed, only noise. **Write explicit casts for *narrowing* conversions** (64 → 32, signed ↔ unsigned), where data can be lost and you want both the compiler and the reader to see that you meant it. Widening is free and implicit.

* **The Struggle & Insights:**
    * **Same tool as `[10]`, one level up.** Good Nodes carried *one* value down by value (`maxSoFar`). Validate BST carries *two* and updates them **asymmetrically** depending on which way you turn. Backtracking is still free: the right child receives this frame's untouched `floor`, not whatever the left subtree narrowed it to.
    * **`&&` short-circuit again.** The first violation found on the left aborts everything without touching the right subtree, the same propagate-failure tool from `[4]`/`[5]`.
    * **Alternative worth knowing — in-order traversal.** From §1: an in-order walk of a BST emits a sorted sequence. So the tree is valid iff each value is strictly greater than the **previous value the in-order walk printed**. Keep one `prev` (by reference, or a `TreeNode*` that starts as `nullptr`). Same $O(N)$/$O(H)$. It's elegant, and it's the exact engine for the next problem (Kth Smallest). The range method is the more direct answer to "what rule does each node obey?".
    * **Alternative that skips widening entirely:** pass the bounds as `TreeNode*` (the ancestor that set the limit), with `nullptr` meaning "no limit". No sentinel, so nothing can collide, and it works for any value type. Good to mention if an interviewer asks "what if the values were `int64_t`?", because then there's no wider integer left to step up to.

* **Time & Space Complexity:** $O(N)$ Time (each node checked once, with early exit on the first violation) / $O(H)$ Space for the recursion stack ($O(\log N)$ balanced, $O(N)$ skewed).
