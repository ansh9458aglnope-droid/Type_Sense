#ifndef EXERCISE_QUEUE_H
#define EXERCISE_QUEUE_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration of opaque struct */
typedef struct ExerciseQueue ExerciseQueue;

/* Node type used internally – not exposed to C++ code */
struct QueueNode {
    void *data;            /* pointer to exercise data (opaque) */
    struct QueueNode *next;
};

/* Create a new empty queue.  Returns NULL on allocation failure. */
ExerciseQueue* exercise_queue_create(void);

/* Enqueue an item at the back of the queue. */
void exercise_queue_enqueue(ExerciseQueue *q, void *item);

/* Dequeue from the front; returns NULL if queue empty. */
void* exercise_queue_dequeue(ExerciseQueue *q);

/* Return number of items currently in the queue. */
size_t exercise_queue_size(const ExerciseQueue *q);

/* Check whether the queue is empty. */
int exercise_queue_is_empty(const ExerciseQueue *q);

/* Destroy the queue and free all memory. */
void exercise_queue_destroy(ExerciseQueue *q);

#ifdef __cplusplus
}
#endif

#endif /* EXERCISE_QUEUE_H */
