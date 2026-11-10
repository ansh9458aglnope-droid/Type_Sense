#include "exercise_queue.h"
#include <stdlib.h>

struct QueueNode { void *data; struct QueueNode *next; };
struct ExerciseQueue {
    struct QueueNode *head;
    struct QueueNode *tail;
    size_t count;
};

ExerciseQueue* exercise_queue_create(void) {
    ExerciseQueue *q = (ExerciseQueue*)malloc(sizeof(*q));
    if (!q) return NULL;
    q->head = q->tail = NULL;
    q->count = 0;
    return q;
}

int exercise_queue_enqueue(ExerciseQueue *q, void *item) {
    if (!q || !item) return 0;
    struct QueueNode *node = (struct QueueNode*)malloc(sizeof(*node));
    if (!node) return 0;
    node->data = item;
    node->next = NULL;
    if (!q->tail) q->head = q->tail = node;
    else { q->tail->next = node; q->tail = node; }
    ++q->count;
    return 1;
}

void* exercise_queue_dequeue(ExerciseQueue *q) {
    if (!q || !q->head) return NULL;
    struct QueueNode *node = q->head;
    void *data = node->data;
    q->head = node->next;
    if (!q->head) q->tail = NULL;
    free(node);
    --q->count;
    return data;
}

size_t exercise_queue_size(const ExerciseQueue *q) { return q ? q->count : 0; }
int exercise_queue_is_empty(const ExerciseQueue *q) { return !q || q->head == NULL; }

/* Queue nodes are freed; payload ownership remains with the caller. */
void exercise_queue_destroy(ExerciseQueue *q) {
    if (!q) return;
    while (q->head) {
        struct QueueNode *node = q->head;
        q->head = node->next;
        free(node);
    }
    free(q);
}
