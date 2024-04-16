#pragma once
#include <utility>

template<typename T>
class circular_buffer {
 public:
  // types
  typedef T value_type;
  typedef T* pointer;
  typedef value_type& reference;
  typedef const value_type& param_value_type;
  typedef value_type&& rvalue_type;
  typedef const value_type& const_reference;

  explicit circular_buffer(int buffer_capacity) : size_(0) {
    buff_ = new value_type[buffer_capacity];
    end_ = buff_ + buffer_capacity;
    first_ = last_ = buff_;
  }

  ~circular_buffer() { destory(); }

  reference operator [] (const int index) {
    return *add(first_, index);
  }

  bool empty() const { return size() == 0; }

  bool full() const { return capacity() == size(); }

  int reserve() const { return capacity() - size(); }

  int size() const { return size_; }

  int capacity() const { return end_ - buff_; }

  reference push_back_peek() {
    return *last_;
  }

  reference push_front_peek() {
    return *first_;
  }

  void push_back(param_value_type item) {
    push_back_impl<param_value_type>(item);
  }

  void push_back(rvalue_type item) {
    push_back_impl<rvalue_type>(std::forward<rvalue_type>(item));
  }

  void push_front(param_value_type item) {
    push_front_impl<param_value_type>(item);
  }

  void push_front(rvalue_type item) {
    push_front_impl<rvalue_type>(std::forward<rvalue_type>(item));
  }

  reference back() {
    return *((last_ == buff_ ? end_ : last_) - 1);
  }

  reference front() {
    return *first_;
  }


  void pop_back() {
    decrement(last_);
    --size_;
  }

  void pop_front() {
    increment(first_);
    --size_;
  }


 private:
  template<typename ValT>
  void push_back_impl(ValT item) {
    if (full()) {
      // if capacity == 0, do nothing
      if (empty()) return;
      // if capacity != 0, insert at the end
      replace(last_, static_cast<ValT>(item));
      increment(last_);
      first_ = last_;
    } else {
      replace(last_, static_cast<ValT>(item));
      increment(last_);
      ++size_;
    }
  }

  template<typename ValT>
  void push_front_impl(ValT item) {
    if (full()) {
      if (empty()) return;
      decrement(first_);
      replace(first_, static_cast<ValT>(item));
      last_ = first_;
    } else {
      decrement(first_);
      replace(first_, item);
      ++size_;
    }
  }

  void increment(pointer& p) {
    if (++p == end_) {
      p = buff_;
    }
  }

  void decrement(pointer& p) {
    if (p == buff_) p = end_;
    --p;
  }

  void replace(pointer pos, param_value_type item) {
    *pos = item;
  }

  void replace(pointer pos, rvalue_type item) {
    *pos = std::move(item);
  }

  pointer add(pointer p, int n) const {
    return p + (n < (end_ - p) ? n : n - (end_ - buff_));
  }

  void destory() {
    delete [] buff_;
    buff_ = nullptr;
    first_ = nullptr;
    last_ = nullptr;
    end_ = nullptr;
  }

 private:
  int size_;
  pointer buff_;
  pointer end_;
  pointer first_;
  pointer last_;
};