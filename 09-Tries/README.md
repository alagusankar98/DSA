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

### [1] Implement Trie (Prefix Tree)

> Relatively easy, once the node shape was settled. Designing `TrieNode` was the tipping point; after that, the three operations were the §1 walk, and the rest of the time went into review-level polish. Two versions committed: the first pass (with a `char val` field and three copies of the walk), and the optimized one (no `val`, a shared walk helper, an iterative destructor, `string_view` parameters).

* **The Core Pattern:** The §1 walk, three times over. Start a cursor at the root and, for each character, step to `next[c - 'a']`. The three operations differ only in the empty-slot action and the end-of-string check:
    * `insert`: an empty slot means **create** the child. After the loop, mark `isDone = true` on the node where the word ends.
    * `search`: an empty slot means `false`. At the end, return `isDone`.
    * `startsWith`: an empty slot means `false`. At the end, return `true`.
    * The optimized version moves the shared walk into a private helper, `returnNode(word)`, that returns the node where the string ends, or `nullptr`. Then `search` is `node && node->isDone` and `startsWith` is `node != nullptr`. `insert` keeps its own loop because it *creates* nodes instead of failing.

* **The design question that unlocked it — where does the array of children live?** You weighed keeping a plain `val + next` node and putting the 26-slot array in the outer class. It belongs **in the node**, because *every* node can branch 26 ways, not just the top. The outer class only holds the entry point (the root) and the operations, the same split as `TreeNode` vs the functions that walk it.

* **Your self-review, resolved point by point:**
    * **`char val` is redundant — correct, and it's the key trie idea.** The letter is **which slot you came through**, not something the node stores (§1, "the letters live on the edges"). `val` was 1 byte, padded out to 8 bytes in a 216-byte node. That's small, but it's also a second copy of information the index already holds, and that copy could disagree with it.
    * **"Dummy" node → call it `root`.** It isn't a placeholder; it's the node for the **empty string `""`**, the prefix every word shares. That's why it always exists, even with zero words: `startsWith("")` is `true` on an empty trie because you're already standing at the `""` node. The "dummy" in Linked List was different: it was a *fake* node added to remove the head special case, and it held no meaning. The trie root holds the empty-prefix node and its `isDone` flag (set if `""` was inserted).
    * **"Checking all 26 slots to tell if it's empty seems wasteful."** It's only wasteful if you need that query often. 26 null checks on one contiguous 208-byte block is a few cache lines and essentially free. If `empty()` or `size()` were part of the API, keep a `size_t wordCount_` member and bump it in `insert` when `isDone` goes from `false` to `true`. That's O(1), and the standard answer: **track metadata at write time instead of recomputing it at read time.**
    * **The existence check before `new` — the critical catch.** Without `if (!next[i])`, inserting `"app"` after `"apple"` replaces the existing `a → p → p` nodes with fresh ones, cutting off `l → e` (still allocated, now unreachable: a **leak plus a lost word**). That's the same "check before you overwrite" rule as never reassigning a linked-list `next` before saving the old one.
    * **`isDone = true` after the loop — yes, that's the standard.** After the loop, `current` *is* the node for the full word. Inside the loop it's every prefix along the way, and those must stay `false`, otherwise `insert("apple")` would make `search("app")` true.

* **The "Gotcha" — what the interviewer review got right, and what it missed:**
    * **Got right:** `string` by value copies the argument on every call. That's a heap allocation for anything past the 15-char short-string buffer, and LeetCode words go up to 2000 chars. `std::string_view` is zero-copy. **Duplicated walk → one helper** (DRY). `return p && p->isDone;` short-circuits, so `p->isDone` is never read when `p` is null.
    * **Missed — the Rule of Three (a real crash, not a nitpick).** `PrefixTree` owns raw heap pointers and has a destructor, but no copy constructor or copy assignment. So the compiler generates ones that copy `head_` **as a pointer**. `PrefixTree b = a;` gives two objects pointing at the same nodes, and when both go out of scope, both destructors free them: a **double free**. The rule: *if you write a destructor, a copy constructor, or copy assignment, you almost always need all three.* The minimal fix is to forbid copying:
      `PrefixTree(const PrefixTree&) = delete;` and `PrefixTree& operator=(const PrefixTree&) = delete;`.
      LeetCode never copies the trie, so it never shows up there. In a code review, it's the first thing a senior C++ reviewer flags. **Fixed in the follow-up:** both are now `= delete`. Your comment says "delete copy and move semantics", and that's accurate even though only the copy pair is written. Once you declare a copy constructor or copy assignment yourself, even as `= delete`, the compiler stops generating the move constructor and move assignment. A move attempt then falls back to the deleted copy and fails to compile. So the class can be neither copied nor moved, which is the safe default for a raw-pointer owner.
    * **Missed — the iterative destructor is *more* robust than the `unique_ptr` version, not just equivalent.** The LLM said `std::array<std::unique_ptr<TrieNode>, 26>` means "no destructor needed". True, but the cleanup it generates is **recursive**: each node's destructor destroys its children's `unique_ptr`s, which call *their* destructors, and so on, one stack frame per level. Measured: a 2000-deep chain (LeetCode's max word length) is fine, but a 10⁶-deep chain **segfaults** on an 8 MB stack. Your `std::stack` loop keeps the pending nodes on the heap, so its depth is unlimited. That's the same Trees lesson (iterative over recursive when depth isn't bounded), and on a 4 KB MCU stack the difference matters much sooner.
    * **Small things:**
        * `if (head_)` in the destructor is always true (the constructor always allocates the root). Removed in the follow-up, along with renaming `head_` → `root_`.
        * Storing the root **by value** (`TrieNode root_;`) skips one `new` and the null question entirely. The destructor then starts by pushing `root_`'s children instead of `root_` itself.
            * **Your question: wouldn't every `current` then become a `TrieNode` copy?** No. Only the **root** moves into the object; the cursor stays a pointer. Start it with the root's **address**: `TrieNode* current = &root_;`. Every node below the root is still heap-allocated and reached through `next[i]`, so `current = current->next[i]` doesn't change. The cursor never holds a node by value. It's a 64-bit address that gets reassigned at each step, exactly as before.
            * **The trap you sensed is real, but only with `auto`.** If you keep `auto current = root_;`, `auto` deduces `TrieNode` (a by-value **216-byte copy** of the root), not a pointer or reference. `auto` never adds `*` or `&` on its own. You'd find out immediately, though: `current->next` doesn't compile on a non-pointer. Write `&root_` (or `auto* current = &root_;` to make the pointer-ness explicit).
            * **Destructor change, and it's mandatory:** never `delete &root_`. It wasn't allocated with `new`, so `delete` on it is undefined behavior (heap corruption). Push `root_.next[i]` for each non-null child, and let the root die with the object.
            * **Is it worth it?** It saves one heap allocation and one pointer hop on every operation, and removes "can the root be null?" from the design entirely. Its cost is that `PrefixTree` now holds the 216-byte node itself instead of an 8-byte pointer (fine, since it's not being copied anyway). It's a small win; the pointer version is not wrong. **Owner's decision: keep the heap root.** The by-value root makes the destructor asymmetric: either skip the root inside the loop (`nodeToDelete != &root_`), or seed the stack with its 26 children in a separate loop first. Both add a special case to code that's currently uniform ("every node came from `new`, so every node gets `delete`"). One allocation per trie isn't worth breaking that symmetry. (Terminology: a by-value root isn't necessarily on the *stack*. It's a member, so it lives wherever the `PrefixTree` object lives: on the stack, on the heap, or in static storage.)
            * **Related polish:** `returnNode`, `search` and `startsWith` never modify the trie, so they can be marked `const`. Inside a `const` member, `&root_` has type `const TrieNode*`, so the cursor becomes `const TrieNode*` too, and the compiler guarantees the read paths never write.
        * `c - 'a'` assumes lowercase `a–z` (a LeetCode guarantee). Any other character gives an index outside `0..25`, which is **out-of-bounds UB** on `std::array::operator[]`. Validate the input, or `assert`, if it isn't guaranteed.

* **📝 Parked for later — the `std::unique_ptr` version (owner's note: not implemented yet).** What to try when you come back to it:
    * Children become `std::array<std::unique_ptr<TrieNode>, 26> next;`. Create with `next[i] = std::make_unique<TrieNode>();`, and walk with a **raw, non-owning** cursor: `TrieNode* cur = root_.get(); … cur = cur->next[i].get();`. Owning pointers live in the tree; the cursor only observes.
    * **What you gain:** no hand-written destructor, and the class becomes **move-only automatically** (`unique_ptr` can't be copied), which also solves the Rule-of-Three bug above.
    * **What you lose:** the recursive cleanup depth described above. To keep both benefits, write a destructor that detaches the children into a `std::vector<std::unique_ptr<TrieNode>>` worklist (`std::move` them out) and lets them die one at a time, so no recursion is needed. Worth doing once to really learn ownership transfer.

* **Time & Space Complexity:** `insert` / `search` / `startsWith`: $O(L)$ time per call (L = word / prefix length), $O(1)$ extra space for the walk (one cursor, no recursion). Storage: $O(\text{total characters inserted})$ nodes in the worst case (no shared prefixes), each 216 bytes (26 × 8-byte pointers + `bool`, padded). Destructor: $O(\text{nodes})$ time, with the work stack's peak size bounded by the node count.
