# Linked List - Topic Guide

## 1. Core Fundamentals

### Standard Node Structure (C++)
```cpp
struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

```

### Doubly Linked List Node (C++)
```cpp
class Node {
    public:
        int val;
        int key;
        Node* next;
        Node* prev;
        Node() : val(-1), key(-1), next(nullptr), prev(nullptr) {}
        Node(int val_, int key_) : val(val_), key(key_), next(nullptr), prev(nullptr) {}
};
```

### Time & Space Complexity

| Operation | Time Complexity | Space Complexity | Notes |
| --- | --- | --- | --- |
| **Access (Index)** | O(N) | O(1) | Must traverse from the head node. |
| **Search (Value)** | O(N) | O(1) | Must traverse until value is found. |
| **Insert / Remove (Head)** | O(1) | O(1) | Just update the head pointer. |
| **Insert / Remove (Tail)** | O(1) or O(N) | O(1) | O(1) only if a `tail` pointer is explicitly maintained. Otherwise, O(N) to traverse to the end. |
| **Insert / Remove (Middle)** | O(N) | O(1) | O(N) to reach the insertion/deletion point, but the actual pointer swap is O(1). |

---

## 2. General Summary / Quick Reference

When tackling Linked List problems, keep these core patterns in mind:

* **Dummy Node:** Use when the `head` might change or when merging/creating lists. It prevents complex `nullptr` checks for the first element.
* **Two Pointers (Fast/Slow):** Used for cycle detection (Tortoise and Hare) and finding the middle of a list.
* **Two Pointers (Fixed-Distance):** Used for finding elements at a specific offset from the end. Maintain a gap of $n$ between two pointers.
* **Three Pointers (`prev`, `curr`, `next`):** The standard for reversing links in place.
* **Mandatory Guard Clauses:** Defensive programming is critical. Always check for `!head` and/or `!head->next` at the very top of your function, especially before initializing advanced pointer setups (like `fast = head->next`) to prevent segmentation faults.
* **Unified Loop Conditions:** When processing multiple lists or tracking a carry, combine conditions (e.g., `while(l1 || l2 || carry)`) to avoid redundant cleanup loops.

---

## 3. Problem Strategies & Patterns

### [1] Reverse Linked List

* **The Core Pattern:** The 3-pointer technique (`prev`, `current`, `next`).
* **The "Gotcha":** `next` is purely a temporary placeholder to prevent losing the rest of the list. Also, beware of **pass-by-value pointer reassignment**: reassigning `head = dummyNode.next` at the very end of a `void` function only updates the local copy of the pointer, leaving the caller's pointer completely unchanged.
* **Time & Space Complexity:** O(N) Time / O(1) Space.
* **The Struggle & Insights:** The mental model of the three pointers was solid. Initialization: `prev = nullptr`, `curr = head`. Termination: Loop until `curr == nullptr`. Return `prev` as the new head.

### [2] Merge Two Sorted Lists

* **The Core Pattern:** The Dummy Node pattern.
* **The "Gotcha":** Forgetting to append the remainder of the list outside the loop. Once the loop `while (list1 != nullptr && list2 != nullptr)` breaks, attach the remaining non-empty nodes with a single pointer assignment: `tail->next = list1 ? list1 : list2;`.
* **Time & Space Complexity:** O(M + N) Time / O(1) Space.
* **The Struggle & Insights:** Recognized the dummy node approach early to track the changing head node and avoid empty list initialization checks.

### [3] Linked List Cycle

* **The Core Pattern:** Two Pointers (Floyd’s Cycle-Finding / Tortoise and Hare).
* **The "Gotcha":** **Missing Guard Clauses** with `fast = head->next`. Initializing a pointer to `head->next` without first verifying `head != nullptr` will cause a segmentation fault on empty lists. If using this initialization, you must use `if (!head) return false;`. Starting `slow = head` and `fast = head` and checking equality *after* moving avoids this entirely.
* **Time & Space Complexity:** O(N) Time / O(1) Space.
* **The Struggle & Insights:** Forgot the algorithm name (Floyd's). Debated between checking equality before or after moving pointers depending on initialization (Approach A vs Approach B).

### [4] Reorder List

* **The Core Pattern:** Two Pointers (Fast/Slow) to find middle + Reverse second half + Interweave.
* **The "Gotcha":** **Using Dummy Nodes for In-Place Interweaving**. Dummy nodes are excellent for merging, but overcomplicate in-place zipping. It's cleaner to use temporary pointers to hold `next` nodes and directly rewire existing nodes. Also, missing the `if (!head || !head->next) return;` guard clause will cause segfaults here.
* **Time & Space Complexity:** O(N) Time / O(1) Space.
* **The Struggle & Insights:** Struggled with the separation and weaving points, specifically trying to sever the connection of the first half and tracking a `prev` node. Solved it by changing initialization to perfectly land `slow` on the exact tail of the first half:
* **Initialization:** `slow = head` and `fast = head->next`.
* **The Split:** When `while (fast && fast->next)` finishes, `slow` points exactly to the tail of the first half.
* *Even Length (n):* First half = n/2, Second half = n/2.
* *Odd Length (n):* First half = (n/2) + 1, Second half = n/2.
* The second half is *always* equal to or exactly one element shorter.
* **Severing:** `ListNode* second = slow->next; slow->next = nullptr;`
* **Interweaving:** Because of the length split, simply loop `while (second != nullptr)`. The first half's final node is already correctly pointing to `nullptr`.

### [5] Remove Nth Node From End of List

* **The Core Pattern:** Two Pointers (Fixed-Distance) + Dummy Node.
* **The "Gotcha":**
* **Memory Leaks vs Stack Allocation:** Allocating a dummy node on the stack (`ListNode dummyNode;`) is optimal for automatic cleanup, but explicitly calling `delete nodeToDelete;` is mandatory in production C++ to prevent heap memory leaks.
* **Type Mismatch Warnings:** Iterating with `for (size_t i = 0; i < n; ...)` against a signed `int n` triggers `-Wsign-compare` compiler warnings. Always match types.
* **Post-increment vs. Pre-increment:** Using `i++` creates unnecessary temporary copies under the hood. Standardize on `++i` in `for` loops.
* **Redundant Pointer Unlinking:** Setting `nodeToDelete->next = nullptr` immediately before calling `delete nodeToDelete` is unnecessary computation.
* **Time & Space Complexity:** O(N) Time / O(1) Space.
* **The Struggle & Insights:** Started moving `current` forward $n$ steps, then ran another loop. Tracked `prev` node in a temp variable, but realized the corner case of `prev` being the head or uninitialized. Moving the starting point of `prev` to a dummy node perfectly caught the edge case of deleting the head node itself and simplified getting the `nodeToDelete`.

### [6] Add Two Numbers

* **The Core Pattern:** Unified Iteration + Dummy Node (Building a new list).
* **The "Gotcha":** **Implicit Type Conversions**. Avoid mixing types like `uint8_t` for carries with `int` for sums. While functionally fine for small values, it triggers implicit integer promotion in C++. Sticking to standard `int` for mathematical operations keeps the code clean and avoids compiler warnings.
* **Time & Space Complexity:** O(max(M, N)) Time / O(max(M, N)) Space.
* **The Struggle & Insights:** Solved the case of carry by adding till carry is 0 using a combined `while(carry || l1 || l2)` loop. Used dummy node to avoid complex cases for the first insertion. Extracted values using ternary operators (`l1 ? l1->val : 0`) when one list ran out faster.

### [7] Copy List with Random Pointer

* **The Core Pattern:** Hash Map (Two-Pass) OR Interweaving (Three-Pass for O(1) Space).
* **The "Gotcha":** **Infinite Loops & Null Dereferences.** Forgetting to advance loop pointers (`current = current->next`) at the end of while loops is a classic logical trap. Additionally, explicitly checking `if(current->random)` is required before map lookups to avoid inserting and mapping `nullptr` keys.
* **Time & Space Complexity:** O(N) Time / O(N) Space (Hash Map) or O(1) Space (Interweaving).
* **The Struggle & Insights:** Creating the copy was straightforward with a dummy node, but initially tried running the random-mapping loop on the copy list. Realized the map strictly goes Original -> Copy, requiring the second loop to iterate on the original list. Caught the edge cases of missing loop increments and null random pointers. Successfully conceptualized the optimal O(1) space approach: 1) Interweave copies after originals, 2) Map randoms (`copy->random = orig->random->next`), 3) Unweave to restore both lists cleanly.

### [8] LRU Cache

* **The Core Pattern:** Doubly Linked List + Hash Map. Track both head and tail using dummy nodes to ensure O(1) insertions and removals without complex null-pointer checks. Write generic `removeNode()` and `insertAtHead()` methods.
* **The "Gotcha":**
    * **Tracking the Key:** You must store the `key` (not just the value) inside the doubly linked list node. When the cache exceeds capacity, you need this `key` from the evicted tail node to delete the corresponding entry from the hash map.
    * **The 4-Link Update:** When inserting a node, it is easy to miss a link. You must update exactly 4 pointers to satisfy the insertion criteria (e.g., `newNode->prev`, `newNode->next`, `prevNode->next`, `nextNode->prev`).
    * **DRY Code:** Failing to reuse the generic `removeNode()` method inside your `deleteFromTail()` method leads to unnecessary code repetition and increases the surface area for bugs.
    * **C++ Rule of 3/5:** Forgetting to explicitly delete copy and move semantics (e.g., `LRUCache(const LRUCache&) = delete;`) violates core C++ principles and can lead to memory corruption if the cache object is ever copied.
* **Time & Space Complexity:** O(1) Time for both `get` and `put` / O(C) Space where C is the capacity.
* **The Struggle & Insights:** Realized the absolute necessity of a doubly linked list, since removing a node from the middle of the list requires an O(1) operation. Adding generic `removeNode()` and `insertAtHead()` methods became remarkably easy by ensuring two dummy nodes (head and tail) are always present at a minimum, meaning we never insert anywhere else or deal with empty list edge cases.

### [9] Find the Duplicate Number

* **The Core Pattern:** Floyd's Cycle-Finding Algorithm (Tortoise and Hare) applied to an array. The array values act as `next` pointers, where `next_node = nums[current_node]`.
* **The "Gotcha":**
    * **The Initialization Trap:** If you initialize `slow` and `fast` differently (e.g., `slow = nums[0]` and `fast = nums[nums[0]]`) just to make a `while (slow != fast)` condition work cleanly, you break the exact offset required for the math to work. The intersection point shifts, requiring a reset point behind index `0`. You **must** start both pointers at `nums[0]` (the conceptual head) and use a `do-while` loop or a `while (true)` with an internal break.
    * **Invalid Indexing:** Since the array contains values in the range `[1, n]`, index `0` is mathematically guaranteed to be the start of the list and outside the cycle. Do not use `-1` or other arbitrary numbers as placeholders for indexing; `nums[0]` is your definitive starting node. 
* **Time & Space Complexity:** O(N) Time / O(1) Space.
* **The Struggle & Insights:** Visualizing Phase 2 of the algorithm is a massive mind bend. The math proves why it works:
    * Let $L$ = distance from start to the cycle entry.
    * Let $X$ = distance from the cycle entry to the intersection point.
    * Let $C$ = cycle length.
    * Slow travels: $L + X$
    * Fast travels twice that: $2(L + X)$. It also travels $L + X + kC$ (where $k$ is the number of laps).
    * Equating them: $2(L + X) = L + X + kC \implies L + X = kC \implies L = kC - X$.
    * Conclusion: The distance from the head to the cycle start ($L$) is perfectly equal to the remaining distance from the intersection to the cycle start ($kC - X$). Moving two pointers at 1 step/time from the head and the intersection guarantees they collide exactly at the cycle entry (the duplicate number).

### [10] Merge K Sorted Lists

* **The Core Pattern:** Divide and Conquer (Iterative). Write a helper function for `Merge Two Sorted Lists`, then repeatedly apply it to pairs of lists in the array until only one remains.
* **The "Gotcha":**
    * **Redundant Guard Clauses:** If your two-list merge helper is written cleanly (looping `while (l1 && l2)` and appending the remainder), explicit `if (!l1) return l2;` checks at the top are unnecessary overhead.
    * **Index Pairing Complexity:** Pairing the $i$-th and $(n - i - 1)$-th indices inward requires careful management of the boundary $n$. If $n$ becomes odd, halving it to $n / 2$ will skip the middle element.
    * **The Odd-Length Fix:** Updating the boundary using `n = (n + 1) / 2` perfectly rounds up odd lengths, ensuring the leftover list is carried over into the next merging round without being dropped.
* **Alternative Pattern (Min-Heap):** Using a Priority Queue to track the head of every list. (Note: Divide and conquer is arguably better as it uses $O(1)$ auxiliary space, whereas the heap takes $O(K)$ space). Also, can pair neighboring elements for sort, push them to a new vector, assign new vector as original vector and proceed as well.
* **Time & Space Complexity:** O(N log K) Time (where N is total nodes, K is number of lists) / O(1) Space.
* **The Struggle & Insights:** Abstracting the two-list merge into a helper function made the problem significantly easier. The true difficulty was visualizing the recursive/iterative reduction of the array. Initially missed the constraint where an odd number of lists left one list entirely unmerged. Solved this mathematically by rounding the interval up with `(n + 1) / 2` at the end of each pass.

### [11] Reverse Nodes in k-Group

* **The Core Pattern:** Segment Isolation + Reversal. Explicitly pinpoint the boundary nodes before attempting any reversal: the node *before* the group, the *head* of the group, the *last node* of the group, and the *head of the next* group.
* **The "Gotcha":**
    * **Variable Naming Hell:** Using generic names like `prev`, `next`, `prevNode`, and `nextNode` simultaneously will cause your mental model to collapse. Use hyper-explicit names (e.g., `prevNodeToCurrentGroup`, `currentGroupHead`) to maintain sanity.
    * **Traversal Exit Condition:** When finding the $k$-th node, the loop condition simply needs to break or return if the traversing pointer hits `nullptr` (meaning there aren't enough nodes left to form a full $k$-group). Overcomplicating it with combined index bounds and null checks (like `i < k && !lastNode`) leads to off-by-one errors.
    * **The Next Loop Update:** After reversing and reconnecting a group, you must advance your "previous node" pointer for the next iteration. It must be updated to point to the *original* head of the current group (which has now become the tail of the reversed group). A common mistake is accidentally setting it to `nextGroupHead`.
* **Time & Space Complexity:** O(N) Time / O(1) Space.
* **The Struggle & Insights:** Even having solved this twice four months prior, it still took an hour to visualize and polish. The breakthrough came from stepping back from the messy variable soup and writing a strict blueprint in the comments before coding:
    * **Nodes to track:**
        1. Previous Node to current group (`prevNodeToCurrentGroup`)
        2. Current group's head (`currentGroupHead`)
        3. Last Node of current group / Reverse Head (`lastNode` / `revHead`)
        4. Next Group's head (`nextGroupHead`)
    * **Links to update (in exact order):**
        1. Track next group's head and sever the last node of the current group.
        2. Attach `prevNodeToCurrentGroup->next` to the newly reversed head.
        3. Attach `currentGroupHead->next` (which is now the tail) to `nextGroupHead`.
        4. Update `prevNodeToCurrentGroup = currentGroupHead` before the loop ends to set up the next iteration.