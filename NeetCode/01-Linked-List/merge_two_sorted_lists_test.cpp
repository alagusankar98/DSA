#include <gtest/gtest.h>
#include "list_node.hpp"
#include "test_utils.hpp"
#include "merge_two_sorted_lists.hpp"

struct MergeArguments {
    std::vector<int> list1;
    std::vector<int> list2;
    std::vector<int> expected;
    MergeArguments(const std::vector<int>& list1, const std::vector<int>& list2, const std::vector<int>& expected) : list1(list1), list2(list2), expected(expected) {}
};

class MergeTwoSortedLists : public ::testing::TestWithParam<MergeArguments> {};

TEST_P(MergeTwoSortedLists, DifferentInputs){
    const auto& params = GetParam();

    ListNode* list1 = createListNode(params.list1);
    ListNode* list2 = createListNode(params.list2);

    ListNode* result = mergeTwoLists(list1, list2);

    EXPECT_EQ(toVector(result), params.expected);

    freeList(result);
}

INSTANTIATE_TEST_CASE_P(Merge_Test_Suite, MergeTwoSortedLists, 
    ::testing::Values(
        MergeArguments({1,2,4}, {1,3,5}, {1,1,2,3,4,5}),
        MergeArguments({}, {1,2}, {1,2}),
        MergeArguments({}, {}, {})
    ));