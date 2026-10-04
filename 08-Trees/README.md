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
