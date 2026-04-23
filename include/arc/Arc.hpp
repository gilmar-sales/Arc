#pragma once

#include <atomic>

#include "ArcVoid.hpp"
#include "SharedState.hpp"
#include "Weak.hpp"

template <typename T> class Arc {
  template <typename U> friend class Arc;
  template <typename U> friend class Weak;

public:
  using type = T;

  explicit Arc(SharedState<T> *ptr) : m_ptr(ptr) {}
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
        m_ptr->m_ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      delete m_ptr->m_object;
      m_ptr->m_object = nullptr;
      if (m_ptr->m_weak_count.load(std::memory_order_relaxed) == 0) {
        delete m_ptr;
      }
    }
  }

  template <typename... Args> static Arc<T> make(Args &&...args) {
    return Arc<T>(new SharedState<T>(new T(std::forward<Args>(args)...)));
  }

  Arc<void> erase_type() const noexcept;

  void reset() noexcept {
    if (m_ptr) {
      if (m_ptr->m_ref_count.fetch_sub(1, std::memory_order_release) == 1) {

        std::atomic_thread_fence(std::memory_order_acquire);

        delete m_ptr->m_object;
        m_ptr->m_object = nullptr;

        if (m_ptr->m_weak_count.load(std::memory_order_relaxed) == 0) {
          delete m_ptr;
        }
      }
      m_ptr = nullptr;
    }
  }

  bool is_valid() const noexcept { return m_ptr != nullptr; }

  bool expired() const noexcept { return m_ptr == nullptr; }

  template <typename Func>
  auto map(Func &&f) const -> std::invoke_result_t<Func, T &> {
    return f(*m_ptr->m_object);
  }

  template <typename Func>
  auto and_then(Func &&f) const -> std::invoke_result_t<Func, T &> {
    return f(*m_ptr->m_object);
  }

  operator Arc<void>() const noexcept;

  bool operator==(const Arc<T> &other) const noexcept {
    return m_ptr == other.m_ptr;
  }
  bool operator==(std::nullptr_t) const noexcept { return m_ptr == nullptr; }

  template <typename U> bool operator==(const Arc<U> &other) const noexcept {
    return m_ptr == other.m_ptr;
  }

  T &operator*() const noexcept {
    return *m_ptr->m_object;
  }

  T *operator->() const noexcept {
    return m_ptr->m_object;
  }

private:
  friend class Arc<void>;

  void lock() noexcept {
    while (true) {
      uint32_t expected = 0;
      if (m_ptr->m_state.compare_exchange_weak(expected, WRITE_LOCK_BIT,
                                               std::memory_order_acquire,
                                               std::memory_order_relaxed))
        return;
      while (m_ptr->m_state.load(std::memory_order_relaxed) != 0)
        cpu_relax();
    }
  }

  bool try_lock() noexcept {
    uint32_t expected = 0;
    return m_ptr->m_state.compare_exchange_strong(expected, WRITE_LOCK_BIT,
                                                  std::memory_order_acquire,
                                                  std::memory_order_relaxed);
  }

  void unlock() noexcept { m_ptr->m_state.store(0, std::memory_order_release); }

  void lock_shared() noexcept {
    while (true) {
      uint32_t s = m_ptr->m_state.load(std::memory_order_relaxed);
      if (s & WRITE_LOCK_BIT) {
        cpu_relax();
        continue;
      }
      if (m_ptr->m_state.compare_exchange_weak(
              s, s + 1, std::memory_order_acquire, std::memory_order_relaxed))
        return;
    }
  }

  bool try_lock_shared() noexcept {
    uint32_t s = m_ptr->m_state.load(std::memory_order_relaxed);
    if (s & WRITE_LOCK_BIT)
      return false;
    return m_ptr->m_state.compare_exchange_strong(
        s, s + 1, std::memory_order_acquire, std::memory_order_relaxed);
  }

  void unlock_shared() noexcept {
    m_ptr->m_state.fetch_sub(1, std::memory_order_release);
  }

  static constexpr uint32_t WRITE_LOCK_BIT = 1u << 31;

  static void cpu_relax() noexcept {
#if defined(__x86_64__) || defined(__i386__)
    __asm__ volatile("pause" ::: "memory");
#elif defined(__aarch64__) || defined(__arm__)
    __asm__ volatile("yield" ::: "memory");
#else
    std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
  }

  SharedState<T> *m_ptr;
};

template <typename T> Arc<void> Arc<T>::erase_type() const noexcept {
  return Arc<void>(reinterpret_cast<SharedState<void>*>(m_ptr));
}

template <typename T> Arc<T>::operator Arc<void>() const noexcept {
  return erase_type();
}
