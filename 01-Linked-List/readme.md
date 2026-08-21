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

### Time & Space Complexity

| Operation | Time Complexity | Space Complexity | Notes |
| --- | --- | --- | --- |
| **Access (Index)** | $O(N)$ | $O(1)$ | Must traverse from the head node. |
| **Search (Value)** | $O(N)$ | $O(1)$ | Must traverse until value is found. |
| **Insert / Remove (Head)** | $O(1)$ | $O(1)$ | Just update the head pointer. |
| **Insert / Remove (Tail)** | $O(1)$ or $O(N)$ | $O(1)$ | $O(1)$ only if a `tail` pointer is explicitly maintained. Otherwise, $O(N)$ to traverse to the end. |
| **Insert / Remove (Middle)** | $O(N)$ | $O(1)$ | $O(N)$ to reach the insertion/deletion point, but the actual pointer swap is $O(1)$. |

---

## 2. General Summary / Quick Reference

When tackling Linked List problems, keep these core patterns in mind:

* **Dummy Node:** Use when the `head` might change or when merging/creating lists. It prevents complex `nullptr` checks for the first element.
* **Two Pointers (Fast/Slow):** Used for cycle detection (Tortoise and Hare) and finding the middle of a list.
* **Two Pointers (Fixed-Distance):** Used for finding elements at a specific offset from the end. Maintain a gap of $n$ between two pointers.
* **Three Pointers (`prev`, `curr`, `next`):** The standard for reversing links in place.
* **Mandatory Guard Clauses:** Defensive programming is critical. Always check for `!head` and/or `!head->next` at the very top of your function, especially before initializing advanced pointer setups (like `fast = head->next`) to prevent segmentation faults.

---

## 3. Problem Strategies & Patterns

### [1] Reverse Linked List

**Core Concept:** The 3-pointer technique (`prev`, `current`, `next`).

* **Execution:** `next` is purely a temporary placeholder to prevent losing the rest of the list.
* **Initialization:** `prev = nullptr`, `curr = head`.
* **Termination:** Loop until `curr == nullptr`. Return `prev` as the new head.

### [2] Merge Two Sorted Lists

**Core Concept:** The Dummy Node pattern.

* **Execution:** Loop `while (list1 != nullptr && list2 != nullptr)`. Once the loop breaks, attach the remaining nodes with a single pointer assignment: `tail->next = list1 ? list1 : list2;`.

### [3] Linked List Cycle

**Algorithm Name:** **Floyd’s Cycle-Finding Algorithm** (Tortoise and Hare).

* **Initialization Debate:**
* **Approach A:** `slow = head`, `fast = head`. Move pointers *before* checking equality. Preferred because `while (fast && fast->next)` safely handles `nullptr` lists.
* **Approach B:** `slow = head`, `fast = head->next`. Check equality *before* moving. Requires a mandatory guard clause `if (!head) return false;` to prevent segfaults.



### [4] Reorder List

This problem combines three distinct sub-problems: Finding the middle, Reversing the second half, and Interweaving.

**1. Finding the Middle & Severing:**

* **Guard Clause is Mandatory:** Before anything, you *must* have `if (!head || !head->next) return;` to prevent segfaults on 0 or 1 node lists. (Using `!head->next->next` to skip 2-node lists is also optimal).
* **Initialization:** `slow = head` and `fast = head->next`.
* **The Split:** When the loop `while (fast && fast->next)` finishes, `slow` points exactly to the tail of the first half.
* *Even Length ($n$):* First half = $n/2$, Second half = $n/2$.
* *Odd Length ($n$):* First half = $(n/2) + 1$, Second half = $n/2$.
* The second half is *always* equal to or exactly one element shorter.


* **Severing:**
```cpp
ListNode* second = slow->next; // The start of the second half
slow->next = nullptr;          // Sever the first half cleanly!

```



**2. Interweaving:**

* Since the second half is either equal in size or exactly one element shorter, you can simply loop while the second half exists: `while (second != nullptr)`.
* Inside the loop, rewire the pointers. No need to worry about appending leftovers outside the loop, as the first half's final node is already correctly pointing to `nullptr`.

### [5] Remove Nth Node From End of List

**Core Concept:** Fixed-Distance Two Pointers + Dummy Node.

* **Execution:** Create a stack-allocated dummy node pointing to `head`. Advance a `current` pointer $n$ steps ahead. Then advance both `current` and a `prevNode` pointer (starting at dummy) until `current` hits `nullptr`. `prevNode` will safely land exactly on the node *before* the target, even if the target is the head itself.
* **Memory Management:** Always explicitly `delete` the removed node in C++ to avoid heap memory leaks.

---

## 4. Common Pitfalls & Mistakes Log

* **Missing Guard Clauses with `fast = head->next`:** Initializing a pointer to `head->next` without first verifying `head != nullptr` will cause a segmentation fault on empty lists. Always use `if (!head || !head->next) return;` at the top of the function for these patterns.
* **Using Dummy Nodes for In-Place Interweaving:** Dummy nodes are excellent for *merging* lists into a new structure, but they overcomplicate *in-place* zipping. It's much cleaner to use temporary pointers to hold the `next` nodes and directly rewire the existing nodes.
* **Pass-by-Value Pointer Reassignment:** In C++, function parameters like `ListNode* head` are passed by value. Reassigning `head = dummyNode.next` at the very end of a `void` function only updates the local copy of the pointer, leaving the caller's pointer completely unchanged. This is a common logic trap.
* **Memory Leaks vs. Stack Allocation:** When creating a dummy node, allocating it on the stack (`ListNode dummyNode;`) is optimal because it automatically cleans up when out of scope. However, for nodes removed from a heap-allocated linked list, explicitly calling `delete nodeToDelete;` is mandatory in production C++ to prevent memory leaks, even if competitive programming platforms do not enforce it.
* **Type Mismatch Warnings (`size_t` vs `int`):** Iterating with `for (size_t i = 0; i < n; ...)` when `n` is a signed `int` triggers `-Wsign-compare` compiler warnings. Always match types in loops.
* **Post-increment (`i++`) vs. Pre-increment (`++i`):** Standardize on using `++i` in `for` loops. While identical for primitive types due to compiler optimization, `i++` creates an unnecessary temporary copy under the hood. For C++ iterators or complex objects, this temporary copy cannot always be optimized away and introduces performance overhead. If `i` is just a primitive `int`, modern compilers are smart enough to optimize away the temporary copy, so the resulting assembly code is identical. However, in C++, you frequently loop using iterators (e.g., `std::vector<int>::iterator`) or custom classes. For these complex objects, the compiler often *cannot* optimize away the temporary copy because the copy constructor might have side effects. Because of this, using `++i` is universally taught as the standard C++ best practice to ensure you never accidentally introduce unnecessary overhead
* **Redundant Pointer Unlinking:** Setting `node->next = nullptr` immediately before calling `delete node` is unnecessary computation. Once the memory is freed, the pointer's previous state is irrelevant.