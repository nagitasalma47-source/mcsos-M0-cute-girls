#ifndef MCSOS_THREAD_H
#define MCSOS_THREAD_H

#include <stdint.h>
#include <stddef.h>

#define MCSOS_SCHED_OK 0
#define MCSOS_SCHED_ERR 1

typedef struct mcsos_context {
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
} mcsos_context_t;

typedef enum {
    MCSOS_THREAD_NEW = 0,
    MCSOS_THREAD_READY,
    MCSOS_THREAD_RUNNING,
    MCSOS_THREAD_BLOCKED,
    MCSOS_THREAD_ZOMBIE
} mcsos_thread_state_t;

typedef struct mcsos_thread {
    const char *name;
    uint64_t id;
    mcsos_thread_state_t state;

    mcsos_context_t context;

    void (*entry)(void *);
    void *arg;

    unsigned char *stack;
    size_t stack_size;

    struct mcsos_thread *next;

    uint64_t ticks;
} mcsos_thread_t;

typedef struct mcsos_scheduler {
    mcsos_thread_t *current;
    mcsos_thread_t *ready_head;
    mcsos_thread_t *ready_tail;

    uint64_t runnable_count;
    uint64_t next_id;
    uint64_t context_switches;
} mcsos_scheduler_t;

int mcsos_scheduler_init(mcsos_scheduler_t *s, mcsos_thread_t *boot);

int mcsos_thread_prepare(
    mcsos_thread_t *t,
    const char *name,
    void (*entry)(void *),
    void *arg,
    unsigned char *stack,
    size_t stack_size,
    uint64_t id
);

int mcsos_sched_enqueue(mcsos_scheduler_t *s, mcsos_thread_t *t);
uint32_t mcsos_sched_ready_count(mcsos_scheduler_t *s);
int mcsos_sched_yield(mcsos_scheduler_t *s);
int mcsos_sched_tick(mcsos_scheduler_t *s);
int mcsos_sched_validate(mcsos_scheduler_t *s);

#endif
