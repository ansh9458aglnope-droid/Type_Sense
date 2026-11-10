#ifndef EXERCISE_QUEUE_H
#define EXERCISE_QUEUE_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct ExerciseQueue ExerciseQueue;
ExerciseQueue* exercise_queue_create(void);
/* Returns 1 on success, 0 on invalid input or allocation failure. */
int exercise_queue_enqueue(ExerciseQueue *q, void *item);
void* exercise_queue_dequeue(ExerciseQueue *q);
size_t exercise_queue_size(const ExerciseQueue *q);
int exercise_queue_is_empty(const ExerciseQueue *q);
void exercise_queue_destroy(ExerciseQueue *q);
#ifdef __cplusplus
}
#endif
#endif
