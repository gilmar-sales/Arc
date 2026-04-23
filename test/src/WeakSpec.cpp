#include <gtest/gtest.h>

#include <arc/Arc.hpp>


class WeakSpec : public ::testing::Test {
};


struct Node {
    Weak<Node> parent;
    Arc<Node> right;
    Arc<Node> left;
    int value;
};

TEST_F(WeakSpec, WeakArcShouldBreakCyclicReferences) {
    // Arrange
    auto root = Arc<Node>::make(Node{.value = 1});
    auto child = Arc<Node>::make(Node{.parent = root, .value = 2});
    root.and_then([&](Node &node) { node.left = child; });

    Weak root_weak(root);
    Weak child_weak(child);

    // Act
    child.reset();
    ASSERT_FALSE(root_weak.expired());
    ASSERT_FALSE(child_weak.expired());

    root.reset();
    auto locked_root = root_weak.lock();
    ASSERT_FALSE(locked_root.has_value());
}
