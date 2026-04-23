#include <gtest/gtest.h>

#include <arc/Arc.hpp>

class ArcSpec : public ::testing::Test {};

TEST_F(ArcSpec, ArcShouldBeAbleToCreate) {
  auto arc = Arc<int>::make(42);

  ASSERT_TRUE(arc.is_valid());
}

TEST_F(ArcSpec, ArcShouldBeAbleToRead) {
  auto arc = Arc<int>::make(42);

  arc.and_then([](auto &value) { ASSERT_EQ(value, 42); });
}

TEST_F(ArcSpec, ArcShouldBeAbleToReset) {
  auto arc = Arc<int>::make(42);

  arc.reset();

  ASSERT_FALSE(arc.is_valid());
}

TEST_F(ArcSpec, ArcShouldBeAbleToCopy) {
  auto arc = Arc<int>::make(42);
  auto copy = arc;

  ASSERT_TRUE(copy.is_valid());
  ASSERT_TRUE(arc.is_valid());

  copy.map([](const auto &value) { ASSERT_EQ(value, 42); });

  copy.and_then([](auto &value) {
    value = 43;
    return value;
  });

  copy.map([](const auto &value) { ASSERT_EQ(value, 43); });
  arc.map([](const auto &value) { ASSERT_EQ(value, 43); });
}

TEST_F(ArcSpec, ArcShouldBeAbleToMove) {
  auto arc = Arc<int>::make(42);
  auto moved = std::move(arc);

  ASSERT_TRUE(moved.is_valid());
  moved.and_then([](auto &value) { ASSERT_EQ(value, 42); });
}

TEST_F(ArcSpec, ArcShouldBeThreadSafe) {
  auto arc = Arc<int>::make(42);

  auto threads = std::vector<std::thread>();
  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&]() {
      auto arc1 = arc;
      for (int j = 0; j < 1000; ++j) {
        arc1.and_then([](auto &value) { value += 1; });
      }
    });
  }

  for (auto &t : threads) {
    if (t.joinable())
      t.join();
  }

  ASSERT_TRUE(arc.is_valid());
}

TEST_F(ArcSpec, WeakArcShouldBeCreatedFromArc) {
  auto arc = Arc<int>::make(42);
  Weak<int> weak(arc);

  ASSERT_TRUE(weak.is_valid());
  ASSERT_FALSE(weak.expired());
}

TEST_F(ArcSpec, WeakArcShouldExpireWhenArcDestroyed) {
  Weak<int> weak;
  {
    auto arc = Arc<int>::make(42);
    weak = Weak<int>(arc);
    ASSERT_FALSE(weak.expired());
  }
  ASSERT_TRUE(weak.expired());
}

TEST_F(ArcSpec, WeakArcShouldPromoteToArcWhenValid) {
  auto arc = Arc<int>::make(42);
  Weak<int> weak(arc);

  auto locked = weak.lock();
  ASSERT_TRUE(locked.has_value());
  locked->and_then([](auto &value) { ASSERT_EQ(value, 42); });
}

TEST_F(ArcSpec, WeakArcShouldReturnNulloptWhenExpired) {
  Weak<int> weak;
  {
    auto arc = Arc<int>::make(42);
    weak = Weak<int>(arc);
  }
  auto locked = weak.lock();
  ASSERT_FALSE(locked.has_value());
}

