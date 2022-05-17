/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file threadsafequeue.h
 * \brief Implements a thread-safe queue.
 *
 * Simple thread-safe queue.
 */
#pragma once
#include <mutex>
#include <queue>
#include <memory>

namespace util::log {

/** class ThreadSafeQueue threadsafequeue.h "util/threadsafequeue.h"
 * \brief Implements a thread-safe queue.
 *
 * Implements a thread-safe queue based upon a std::queue. The queue
 * uses stores the objects with smart pointers.
 *
 * @tparam T Type of object to store.
 */
template <typename T>
class ThreadSafeQueue
{
 public:
  ThreadSafeQueue() = default;
  virtual ~ThreadSafeQueue();

  ThreadSafeQueue(const ThreadSafeQueue&) = delete;
  ThreadSafeQueue& operator = (const ThreadSafeQueue&) = delete;

  /** \brief Adds a value at the end of the queue.
   *
   * Add a value last in the queue.
   * @param value
   */
  void Put(std::unique_ptr<T>& value);

  /** \brief Fetch the first object from the queue.
   *
   * Gets the first object in the queue. The function may block until a value is available.
   * Deleting the queue will unblock the call and returning false.
   *
   * @param dest Returning object
   * @param block True if the function should block until a value is available.
   * @return True if a value was returned.
   */
  [[nodiscard]] bool Get(std::unique_ptr<T>& dest, bool block);

  /** \brief Returns true if the queue is empty.
   *
   * Returns true if the queue is empty.
   *
   * @return True if list is empty.
   */
  [[nodiscard]] bool Empty() const;

  /** \brief Returns number of items in the queue.
   *
   * @return Number of items in the queue.
   */
  [[nodiscard]] size_t Size() const;
 private:
  mutable std::mutex lock_; ///< Mutex lock for the queue:
  std::queue<std::unique_ptr<T>> queue_; ///< The queue.
  std::atomic<bool> stop_ = false; ///< Set to true to indicate that any blocking call shall end.
  std::condition_variable queue_event_; ///< Condition to speed up waiting calls.
};

template<typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue() {
  stop_ = true;
  queue_event_.notify_one(); // Release any blocking Get()
  std::lock_guard lock(lock_); // Waiting for Get() to release
  while (!queue_.empty() ) {
   queue_.pop();
  }
}

template<typename T>
void ThreadSafeQueue<T>::Put(std::unique_ptr<T>& value) {
  if (stop_) {
    return;
  }
  std::lock_guard lock(lock_);
  queue_.push(std::move(value));
  queue_event_.notify_one();
}

template<typename T>
bool ThreadSafeQueue<T>::Get(std::unique_ptr<T> &dest, bool block) {
  if (stop_) {
    return false;
  }

  if (block) {
    std::unique_lock lock(lock_);
    queue_event_.wait(lock,[&] {
      return !queue_.empty() || stop_.load();
    });
    if (queue_.empty() || stop_) {
      return false;
    }
    dest = std::move(queue_.front());
    queue_.pop();
  } else {
    std::lock_guard lock(lock_);
    if (queue_.empty() || stop_) {
      return false;
    }
    dest = std::move(queue_.front());
    queue_.pop();
  }
  return true;
}

template<typename T>
bool ThreadSafeQueue<T>::Empty() const {
  std::lock_guard lock(lock_);
  return queue_.empty();
}

template<typename T>
size_t ThreadSafeQueue<T>::Size() const {
  std::lock_guard lock(lock_);
  return queue_.size();
}

}