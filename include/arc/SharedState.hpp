#pragma once

#include <atomic>

template <typename T> struct SharedState {
  std::atomic<uint32_t> m_ref_count;
  std::atomic<uint32_t> m_weak_count;
  T *m_object;

  explicit SharedState(T *ptr)
      : m_ref_count(1), m_weak_count(0), m_object(ptr) {}
};
