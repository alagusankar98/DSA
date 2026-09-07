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