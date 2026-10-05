#ifndef CORRECTION_STACK_H
#define CORRECTION_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration of opaque struct */
typedef struct CorrectionStack CorrectionStack;

/* Node type used internally – not exposed to C++ code */
struct StackNode {
    void *data;            /* pointer to correction data (opaque) */
    struct StackNode *next;
};

/* Create a new empty stack.  Returns NULL on allocation failure. */
CorrectionStack* correction_stack_create(void);

/* Push an item onto the top of the stack. */
void correction_stack_push(CorrectionStack *s, void *item);

/* Pop from the top; returns NULL if stack empty. */
void* correction_stack_pop(CorrectionStack *s);

/* Return number of items currently in the stack. */
size_t correction_stack_size(const CorrectionStack *s);

/* Check whether the stack is empty. */
int correction_stack_is_empty(const CorrectionStack *s);

/* Destroy the stack and free all memory. */
void correction_stack_destroy(CorrectionStack *s);

#ifdef __cplusplus
}
#endif

#endif /* CORRECTION_STACK_H */
