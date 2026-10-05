#include "correction_stack.h"

#include <stdlib.h>

/* ---------- Stack implementation ---------- */
struct CorrectionStack {
    struct StackNode *top;
};

CorrectionStack* correction_stack_create(void)
{
    CorrectionStack *s = (CorrectionStack*)malloc(sizeof(*s));
    if (!s) return NULL;
    s->top = NULL;
    return s;
}

void correction_stack_push(CorrectionStack *s, void *item)
{
    if (!s) return;
    struct StackNode *node = (struct StackNode*)malloc(sizeof(*node));
    if (!node) return; // ignore failure
    node->data = item;
    node->next = s->top;
    s->top = node;
}

void* correction_stack_pop(CorrectionStack *s)
{
    if (!s || !s->top) return NULL;
    struct StackNode *node = s->top;
    void *data = node->data;
    s->top = node->next;
    free(node);
    return data;
}

size_t correction_stack_size(const CorrectionStack *s)
{
    size_t count = 0;
    for (const struct StackNode *p = s ? s->top : NULL; p; p = p->next) {
        ++count;
    }
    return count;
}

int correction_stack_is_empty(const CorrectionStack *s)
{
    return !s || !s->top;
}

void correction_stack_destroy(CorrectionStack *s)
{
    if (!s) return;
    while (s->top) {
        struct StackNode *tmp = s->top;
        s->top = tmp->next;
        free(tmp);
    }
    free(s);
}
