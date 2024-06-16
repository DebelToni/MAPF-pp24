#ifndef QUEUE_H
#define QUEUE_H

typedef struct Queue {
    Node** nodes;
    int front;
    int rear;
    int capacity;
} Queue;

Queue* create_queue(int capacity);
void enqueue(Queue* queue, Node* node);
Node* dequeue(Queue* queue);
int is_queue_empty(Queue* queue);
void free_queue(Queue* queue);

#endif // QUEUE_H
