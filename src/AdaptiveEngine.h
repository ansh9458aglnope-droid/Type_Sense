#pragma once
#include "c_ds/exercise_queue.h"

class AdaptiveEngine {
public:
    AdaptiveEngine() : queue_(exercise_queue_create()) {}
    ~AdaptiveEngine(){ exercise_queue_destroy(queue_); }

    void enqueueExercise(void* ex) { exercise_queue_enqueue(queue_, ex); }
    void* nextExercise()          { return exercise_queue_dequeue(queue_); }
private:
    ExerciseQueue* queue_;
};