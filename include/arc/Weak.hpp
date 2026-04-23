#pragma once

#include <optional>
#include "SharedState.hpp"

template <typename T> class Arc;

template <typename T> class Weak {
  template <typename U> friend class Arc;

public:
  Weak(Arc<T> &arc) noexcept : m_ptr(arc.m_ptr) {
    if (m_ptr)
      m_ptr->m_weak_count.fetch_add(1, std::memory_order_relaxed);
  }

  Weak(Arc<T> &&arc) noexcept : m_ptr(arc.m_ptr) {
    if (m_ptr)
      m_ptr->m_weak_count.fetch_add(1, std::memory_order_relaxed);
  }

  constexpr Weak() noexcept : m_ptr(nullptr) {}

  Weak(const Weak &other) noexcept : m_ptr(other.m_ptr) {
    if (m_ptr)
      m_ptr->m_weak_count.fetch_add(1, std::memory_order_relaxed);
  }

  Weak(Weak &&other) noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }

  Weak &operator=(Weak other) noexcept {
    std::swap(m_ptr, other.m_ptr);
    return *this;
  }

  ~Weak() {
    if (m_ptr &&
        m_ptr->m_weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      if (m_ptr->m_ref_count.load(std::memory_order_relaxed) == 0) {
        delete m_ptr;
      }
    }
  }

  [[nodiscard]] std::optional<Arc<T>> lock() const noexcept {
    if (!m_ptr)
      return std::nullopt;

    uint32_t ref_count = m_ptr->m_ref_count.load(std::memory_order_relaxed);
    if (ref_count == 0)
      return std::nullopt;

    if (m_ptr->m_ref_count.compare_exchange_strong(
            ref_count, ref_count + 1, std::memory_order_acq_rel,
            std::memory_order_relaxed)) {
      return Arc<T>(m_ptr);
    }
    return std::nullopt;
  }

  uint32_t ref_count() const noexcept {
    return m_ptr->m_ref_count.load(std::memory_order_relaxed);
  }

  bool expired() const noexcept {
    if (!m_ptr)
      return true;
    return m_ptr->m_ref_count.load(std::memory_order_relaxed) == 0;
  }

  bool is_valid() const noexcept { return m_ptr != nullptr && !expired(); }

private:
  SharedState<T> *m_ptr;
};