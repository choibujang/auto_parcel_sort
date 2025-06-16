#ifndef MyQueue_H
#define MyQueue_H

template <typename T, int QUEUE_SIZE>
class MyQueue {
private:
    int front;
    int end;       
    T items_[QUEUE_SIZE];   

public:
    MyQueue() : front(0), end(0) {}

    bool isFull() const {
        return (end + 1) % QUEUE_SIZE == front;
    }

    bool isEmpty() const {
        return front == end;
    }

    bool enqueue(const T& item) {
        if (isFull()) {
            return false; // 큐가 가득 찼음
        }
        items_[end] = item;
        end = (end + 1) % QUEUE_SIZE;
        return true;
    }

    bool dequeue(T& item) {
        if (isEmpty()) {
            return false; // 큐가 비었음
        }
        item = items_[front];
        front = (front + 1) % QUEUE_SIZE;
        return true;
    }

    bool updateLatest(const T& item) {
        if (isEmpty()) {
            return false; // 큐가 비었음
        }
        // 'end' points to the next empty spot, so the last item is at 'end - 1'.
        // We need to handle the circular buffer case where end is 0.
        int last_item_index = (end == 0) ? (QUEUE_SIZE - 1) : (end - 1);
        items_[last_item_index] = item;
        return true;
    }

    bool peekLatest(T& item) const {
        if (isEmpty()) {
            return false;
        }
        int last_item_index = (end == 0) ? (QUEUE_SIZE - 1) : (end - 1);
        item = items_[last_item_index];
        return true;
    }

    bool peek(T& item) const {
        if (isEmpty()) {
            return false; // 큐가 비었음
        }
        item = items_[front];
        return true;
    }

    int getSize() const {
        return (end - front + QUEUE_SIZE) % QUEUE_SIZE;
    }

    T& get(int index) {
        if (index < 0 || index >= getSize()) {
            // This is a simplified error handling. In a real scenario,
            // you might want to throw an exception or handle it more gracefully.
            return items_[front];
        }
        return items_[(front + index) % QUEUE_SIZE];
    }

    T& front() {
        return items_[front];
    }
};

#endif