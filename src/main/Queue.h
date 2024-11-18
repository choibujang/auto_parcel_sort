#ifndef QUEUE_H
#define QUEUE_H

template <typename T>
class Queue {
private:
    int front;
    int end;       
    T queue[50];   

public:
    Queue() : front(0), end(0) {}

    bool isFull() const {
        return (end + 1) % 50 == front;
    }

    bool isEmpty() const {
        return front == end;
    }

    bool enqueue(const T& item) {
        if (isFull()) {
            return false; // 큐가 가득 찼음
        }
        queue[end] = item;
        end = (end + 1) % 50;
        return true;
    }

    bool dequeue(T& item) {
        if (isEmpty()) {
            return false; // 큐가 비었음
        }
        item = queue[front];
        front = (front + 1) % 50;
        return true;
    }

    bool peek(T& item) const {
        if (isEmpty()) {
            return false; // 큐가 비었음
        }
        item = queue[front];
        return true;
    }
};

#endif