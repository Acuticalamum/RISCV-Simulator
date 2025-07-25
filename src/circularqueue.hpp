#ifndef CIRCULAR_QUEUE_HPP
#define CIRCULAR_QUEUE_HPP
#include<stdint.h>

template<typename T, uint8_t N>
class CircularQueue {
  public:
    T queue[N];
    uint8_t head, tail;
    uint8_t size;
    bool empty() {
      return size == 0;
    }
    bool full() {
      return size == N;
    }
    void clear() {
      head = tail = 0;
      size = 0;
    }
    void push(T x) {
      queue[tail++] = x;
      tail %= N;
      ++size;
    }
    void pop() {
      ++head;
      head %= N;
      --size;
    }
    T front() {
      return queue[head];
    }
    T& operator[](uint8_t pos) {
      return queue[pos];
    }
  public:
    CircularQueue() {
      head = tail = 0;
      size = 0;
    }
};
#endif