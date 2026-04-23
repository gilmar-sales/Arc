#pragma once

#include "SharedState.hpp"

template <typename T> class Arc;

template <> class Arc<void> {
public:
  constexpr Arc() noexcept : m_ptr(nullptr) {}
  constexpr Arc(std::nullptr_t) noexcept : m_ptr(nullptr) {}

  Arc(const Arc &other) noexcept : m_ptr(other.m_ptr) {
    if (m_ptr)
      m_ptr->m_ref_count.fetch_add(1, std::memory_order_relaxed);
  }

  Arc(Arc &&other) noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }

  Arc &operator=(Arc other) noexcept {
    std::swap(m_ptr, other.m_ptr);
    return *this;
  }

  ~Arc() {
    if (m_ptr &&
        m_ptr->m_ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
      delete m_ptr;
  }

  void reset() noexcept {
    if (m_ptr &&
        m_ptr->m_ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
      delete m_ptr;
    m_ptr = nullptr;
  }

  bool is_valid() const noexcept { return m_ptr != nullptr; }
  template <typename U> static Arc<void> cast_from(Arc<U> &arc) noexcept {
    return arc.erase_type();
  }

  operator bool() const noexcept { return is_valid(); }

  bool operator==(const Arc<void> &other) const noexcept {
    return m_ptr == other.m_ptr;
  }
  bool operator==(std::nullptr_t) const noexcept { return m_ptr == nullptr; }
  template <typename U> bool operator==(const Arc<U> &other) const noexcept {
    return m_ptr == other.m_ptr;
  }

private:
  template <typename T> friend class Arc;

  explicit Arc(SharedState<void> *base) noexcept : m_ptr(base) {
    if (m_ptr)
      m_ptr->m_ref_count.fetch_add(1, std::memory_order_relaxed);
  }

  SharedState<void> *m_ptr;
};
