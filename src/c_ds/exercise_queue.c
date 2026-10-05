#include "exercise_queue.h"

#include <stdlib.h>

/* ---------- Queue implementation ---------- */
struct ExerciseQueue {
    struct QueueNode *head;
    struct QueueNode *tail;
};

ExerciseQueue* exercise_queue_create(void)
{
    ExerciseQueue *q = (ExerciseQueue*)malloc(sizeof(*q));
    if (!q) return NULL;  // allocation failed
    q->head = q->tail = NULL;
    return q;
}

void exercise_queue_enqueue(ExerciseQueue *q, void *item)
{
    if (!q) return;
    struct QueueNode *node = (struct QueueNode*)malloc(sizeof(*node));
    if (!node) return; // silently ignore allocation failure
    node->data = item;
    node->next = NULL;
    if (!q->head) {
        q->head = q->tail = node;
    } else {
        q->tail->next = node;
        q->tail = node;
    }
}

void* exercise_queue_dequeue(ExerciseQueue *q)
{
    if (!q || !q->head) return NULL; // empty
    struct QueueNode *node = q->head;
    void *data = node->data;
    q->head = node->next;
    if (!q->head) q->tail = NULL; // became empty
    free(node);
    return data;
}

size_t exercise_queue_size(const ExerciseQueue *q)
{
    size_t count = 0;
    for (const struct QueueNode *p = q ? q->head : NULL; p; p = p->next) {
        ++count;
    }
    return count;
}

int exercise_queue_is_empty(const ExerciseQueue *q)
{
    return !q || !q->head;
}

void exercise_queue_destroy(ExerciseQueue *q)
{
    if (!q) return;
    while (q->head) {
        struct QueueNode *tmp = q->head;
        q->head = tmp->next;
        free(tmp);
    }
    free(q);
}
