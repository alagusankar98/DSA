#include <gtest/gtest.h>
#include <utility>
#include "list_node.hpp"
#include "test_utils.hpp"
#include "linked_list_cycle_detection.hpp"

struct CycleDetectionArguments {
    std::vector<int> inputList;
    std::pair<int, int> linkedIndices;
    bool expected;
    CycleDetectionArguments(const std::vector<int>& inputList, std::pair<int, int> linkedIndices, bool expected) : inputList(inputList), linkedIndices(linkedIndices), expected(expected) {}
};

class CycleDetection : public ::testing::TestWithParam<CycleDetectionArguments> {};

TEST_P(CycleDetection, DifferentInputs){
    const auto& params = GetParam();

    ListNode* list1 = createListNode(params.inputList);
    createCycle(list1, params.linkedIndices);

    bool result = hasCycle(list1);

    EXPECT_EQ(result, params.expected);

    // freeList(list1);
}

INSTANTIATE_TEST_CASE_P(CycleDetection_Test_Suite, CycleDetection, 
    ::testing::Values(
        CycleDetectionArguments({1, 2, 3, 4}, {1,3}, true),
        CycleDetectionArguments({1, 2}, {-1,-1}, false)
    ));