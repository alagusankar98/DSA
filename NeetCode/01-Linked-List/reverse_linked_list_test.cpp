#include <gtest/gtest.h>
#include "list_node.hpp"
#include "test_utils.hpp"
#include "reverse_linked_list.hpp"

TEST(ReverseLinkedList, StandardList){
    ListNode* head = createListNode({0, 1, 2, 3});

    ListNode* result = reverseList(head);

    std::vector<int> expected = {3, 2, 1, 0};

    EXPECT_EQ(toVector(result), expected);

    freeList(result);
}

TEST(ReverseLinkedList, EmptyList){
    ListNode* head = createListNode({});

    ListNode* result = reverseList(head);

    std::vector<int> expected = {};

    EXPECT_EQ(toVector(result), expected);

    freeList(result);
}