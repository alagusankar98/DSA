# Tries - Topic Guide

## 1. Core Fundamentals

A **trie** (pronounced "try", from re*trie*val; also called a **prefix tree**) is a tree built for one job: storing a set of **strings** so that everything sharing a prefix also shares the path that spells it. It's the first data structure in the run that builds directly on Trees: same nodes and pointers, same `nullptr` base cases, same DFS. The difference is what the shape *means*.

### The Mental Model: the letters live on the edges
Insert `car`, `cat`, `cap`, `do`, `dog`:
```text
                 (root)            ← represents the empty string ""
                /      \
             'c'        'd'
              |          |
             'a'        'o'  ★     ← ★ = "a word ends here" (do)
           /  |  \       |
         'p' 'r' 't'    'g'  ★     (dog)
          ★   ★   ★
       (cap)(car)(cat)
```
* **A node doesn't store "its" letter. The letter is *which child slot* you took to get there.** The root is the empty string, and every node stands for **the prefix spelled by the path from the root to it**. The node reached by `c → a` *is* the prefix `"ca"`.
* **Shared prefixes are stored once.** `car`/`cat`/`cap` share the single `c → a` path. That's the whole point. A hash set stores `"car"`, `"cat"`, `"cap"` as three unrelated strings, with nothing in common.
* **The end-of-word flag is not optional.** In the picture, `"do"` is a word *and* a prefix of `"dog"`. Reaching a node only proves the *prefix* exists. Whether a word *ends* there is a separate fact that must be stored on the node (a `bool`). Without it, inserting `"dog"` would make `search("do")` true. This is the #1 trie bug.

### Standard Node Structure (C++)
The LeetCode alphabet is usually lowercase `a–z`, so the classic node is a fixed array of 26 child pointers:
```cpp
struct TrieNode {
    TrieNode* children[26] = {};   // value-initialized: all nullptr
    bool isEnd = false;            // does a word END at this prefix?
};
```
* **Index = character:** `children[c - 'a']`. `'a'..'z'` are contiguous in ASCII, so subtracting `'a'` maps them to `0..25`. It's the same offset trick as the `[-3000, 3000]` lookup array in Trees `[13]`, and the `count[26]` arrays from Arrays & Hashing.
* **Compared with `TreeNode`:** `left`/`right` become 26 numbered slots. A binary tree is a trie over a 2-letter alphabet.
* **`= {}` matters.** Without it, a `new TrieNode` member array is uninitialized garbage, and `if (children[i])` follows a wild pointer. Default member initializers fix it for every constructor.

### The One Loop Every Trie Operation Is Built From
Almost every trie operation is the same **walk**: start a cursor at the root, and for each character of the input, move to `children[c - 'a']`. The operations differ only in **what you do when the slot is empty** and **what you check when the string runs out**:

| Operation | Slot is `nullptr` mid-walk | String ends — return |
| -- | -- | -- |
| `insert(word)` | **create** the child, keep walking | set `isEnd = true` |
| `search(word)` | stop → `false` | `isEnd` (a prefix alone isn't a word) |
| `startsWith(prefix)` | stop → `false` | `true` (reaching the node is enough) |

* This is **single-path descent**, the Trees §2 Pattern 4 idea again: one cursor, no branching, so it's a plain loop with **O(1) extra space**, no recursion and no stack. Like BST search, it never needs to remember an alternative path.
* **Recursion comes back only when the input branches.** A wildcard like `.` ("any letter") means trying *every* non-null child, and that's a DFS with backtracking (the shape of the `Design Add and Search Words` problem). Same rule as Trees: **one path → loop; many paths → DFS**.

### Complexity: why "O(L)" beats "O(1)" hashing here
Let **L** = length of the word.
* `insert` / `search` / `startsWith`: **O(L) time**, independent of how many words are stored. A hash set lookup is "O(1)" too, but it **hashes all L characters** first, then compares the whole string on a hit, so it's also O(L) in practice.
* **What a hash set *can't* do is prefixes.** "Is there any word starting with `ca`?" costs a hash set a scan over every word. For a trie it's a two-step walk. **Reach for a trie when the question is about prefixes, not whole-word membership.**
* Sorted array + binary search can answer prefix queries too (`lower_bound("ca")`), at O(L log N) per query. A trie trades memory to remove the `log N`, and it shines when a search must **extend letter by letter** (Word Search II), because each step is one array index from where you already are.

### The Hardware Reality: tries are memory-hungry
* **Per-node cost of the array node:** 26 × 8-byte pointers = **208 bytes**, + 1 `bool` → padded to **216 bytes** on a 64-bit machine, mostly `nullptr`s. Deep in the tree a node typically has 1 used child out of 26, so roughly 96% of each node is wasted space. Inserting 10⁴ words of length 10 with little sharing is ~10⁵ nodes ≈ **21 MB**. On a Cortex-M with 128 KB of SRAM that's not happening.
* **Alternatives (trade-offs worth naming in an interview):**
    * **`std::unordered_map<char, TrieNode*>` children:** stores only the children that exist, so it's better for large or sparse alphabets (Unicode). But each entry is its own heap allocation, plus the map's bucket array, and every step pays for hashing. Slower and less cache-friendly than direct indexing for `a–z`.
    * **Pool / arena allocation (the embedded answer):** keep all nodes in one `std::vector<TrieNode>` and store children as `int` indices instead of pointers. One contiguous block, half-size links (4 bytes instead of 8), no per-node `malloc`, and the entire trie is freed in a single deallocation. Same idea as a static memory pool on firmware.
    * **Compressed trie (radix tree):** merges single-child chains into one edge labelled with a string. That's what real routers (IP longest-prefix match) and the Linux kernel's radix trees use. Know it exists; it isn't needed for NeetCode.
* **Ownership:** every `new TrieNode` must be freed. LeetCode won't complain if it leaks; production code should. Either write a recursive destructor (post-order: free the children, then yourself), use `std::unique_ptr<TrieNode> children[26]`, or use the arena above, which avoids the problem entirely.

### Where Tries Show Up in the Real World
Autocomplete and search suggestions, spell checkers, IP routing tables (longest-prefix match on address bits, so a binary trie), T9 predictive text, and word games like Boggle (which is Word Search II).

---

## 2. General Summary / Quick Reference

When tackling Trie problems, keep these core patterns in mind:

### Pattern 1: Build + Walk (Basic Trie Operations)
The table in §1: `insert`, exact `search`, `startsWith`, all as a single iterative walk with one cursor pointer.
* *Use for:* Implement Trie (Prefix Tree), autocomplete, "does any word start with X", word counting (swap `bool isEnd` for `int count`).
* *Discipline:* decide up front what a node *means* (a prefix) and what the flag *means* (a word ends here). Every bug traces back to mixing the two up.

### Pattern 2: Wildcard Search = DFS over Children
When a query character can match **any** letter (`.`), the walk forks. At a `.`, try every non-null child recursively, and return `true` as soon as one branch succeeds. Regular characters still follow just one slot.
* *Use for:* Design Add and Search Words Data Structure, regex-lite matching against a dictionary.
* *Shape:* recursive `(node, index into word)`. Base case: the word is used up → return `node->isEnd`. It's Trees-style DFS where the "children" are 26 slots instead of 2.

### Pattern 3: Trie as a Pruning Index for Another Search
Load the dictionary into a trie, then run a *different* search (grid DFS / backtracking) and walk the trie **in lockstep**, one character per step. The moment the current path isn't a prefix in the trie, abandon that branch. One trie walk replaces running a separate search for every word.
* *Use for:* Word Search II (find all dictionary words in a letter grid).
* *Note:* this leans on grid DFS + backtracking (topic 11). Expect it to be the hardest of the three. Classic optimizations: store the whole word at its end node instead of rebuilding the string, and remove (or mark) a word once found so it isn't reported twice.

### The Non-Negotiable Discipline
* **Initialize children to `nullptr`** (`= {}`), every time.
* **Prefix ≠ word.** "I reached the node" and "a word ends here" are different facts. `search` checks the flag; `startsWith` doesn't.
* **One path → loop; branching (wildcards, grids) → DFS.** Same rule as Trees.
* **Know the memory bill.** Be ready to say "216 bytes per node, mostly null, here's the map / arena / radix alternative."

---

## 3. Problem Strategies & Patterns

*(Entries added one problem at a time as they're solved — same format as the other topic guides: **The Core Pattern**, **The "Gotcha"**, **Time & Space Complexity**, **The Struggle & Insights**.)*

<!-- NeetCode 150 Tries (3): Implement Trie (Prefix Tree) · Design Add and Search Words Data Structure · Word Search II -->
