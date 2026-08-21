#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <vector>
#include "list_node.hpp"

// Creates a Linked List from Vector. REMEMBER TO FREE LIST CREATED FROM HERE (ONCE DONE)
ListNode* createListNode(const std::vector<int>& values);

// Converts a Linked List to Vector
std::vector<int> toVector(ListNode* head);

// Free heap used by Linked list
void freeList(ListNode* head);

// Introduce cycle in linked list
void createCycle(ListNode* head, std::pair<int, int> indices);

#endif