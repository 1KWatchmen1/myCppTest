#include <pthread.h>
#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>
#include <stdlib.h>
#include "thrd_pool.h"
#include "spinlock.h"

// zero-voice: mark

typedef struct spinlock spinlock_t;

typedef struct task_s {
    void *next;
    handler_pt func;
    void *arg;
} task_t;

typedef struct task_queue_s {
    void *head;
    void **tail;
    int block;
    spinlock_t lock;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} task_queue_t;

struct thrdpool_s {
    task_queue_t *task_queue;
    atomic_int quit;
    int thrd_count;
    pthread_t *threads;
};

static task_queue_t *
__taskqueue_create() {
    task_queue_t *queue = (task_queue_t *) malloc(sizeof(*queue));
    if (!queue) return NULL;

    int ret;
    ret = pthread_mutex_init(&queue->mutex, NULL);
    if (ret == 0) {
        ret = pthread_cond_init(&queue->cond, NULL);
        if (ret == 0) {
            spinlock_init(&queue->lock);
            queue->head = NULL;
            queue->tail = &queue->head;
            queue->block = 1;
            return queue;
        }
        pthread_cond_destroy(&queue->cond);
    }
    pthread_mutex_destroy(&queue->mutex);
    return NULL;
}

static void
__nonblock(task_queue_t *queue) {
    pthread_mutex_lock(&queue->mutex);
    queue->block = 0;
    pthread_mutex_unlock(&queue->mutex);
    pthread_cond_broadcast(&queue->cond); // 让所有休眠的线程的唤醒
}

static inline void 
__add_task(task_queue_t *queue, void *task) {
    // task->next = NULL;
    void **link = (void **) task;
    *link = NULL;
    spinlock_lock(&queue->lock);
    *queue->tail = link;   // 第一个节点：head = task;  tail = task; 第二个节点： node->next = task; tail = task
    queue->tail = link;
    spinlock_unlock(&queue->lock);
    pthread_cond_signal(&queue->cond); // 让一个线程唤醒
}

static inline void * 
__pop_task(task_queue_t *queue) {
    spinlock_lock(&queue->lock);
    if (queue->head == NULL) { // 队列是否为空
        spinlock_unlock(&queue->lock);
        return NULL;
    }
    task_t *task;
    task = queue->head;
    queue->head = task->next;
    if (queue->head == NULL) { // 这里是重点：如果此时从长度为1 的队列中 pop 节点
        queue->tail = &queue->head;
    }
    spinlock_unlock(&queue->lock);
    return task;
}

static inline void * 
__get_task(task_queue_t *queue) { // 如果没有任务的话，让当前线程休眠
    task_t *task;
    while ((task = __pop_task(queue)) == NULL) {
        pthread_mutex_lock(&queue->mutex);
        if (queue->block == 0) {
            pthread_mutex_unlock(&queue->mutex);
            break;
        }
        pthread_cond_wait(&queue->cond, &queue->mutex); // 这里是线程休眠的位置
        pthread_mutex_unlock(&queue->mutex);
    }
    return task;
}

static void
__taskqueue_destroy(task_queue_t *queue) {
    task_t *task;
    while (task = __pop_task(queue)) { // 任务的生命周期由 thrdpool 管理
        free(task);
    }
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->mutex);
    spinlock_destroy(&queue->lock);
    free(queue);
}

static void *
__thrdpool_worker(void *arg) {
    thrdpool_t *pool = (thrdpool_t*) arg;
    task_t *task;
    void *ctx;

    while (atomic_load(&pool->quit) == 0) {
        task = (task_t*)__get_task(pool->task_queue);
        if (!task) break;
        handler_pt func = task->func;
        ctx = task->arg;
        free(task);
        func(ctx);   // 能确保 消费者也可以作为生产者
    }
    
    return NULL;
}

static void 
__threads_terminate(thrdpool_t * pool) {
    atomic_store(&pool->quit, 1);
    __nonblock(pool->task_queue);
    int i;
    for (i=0; i<pool->thrd_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
}

static int 
__threads_create(thrdpool_t *pool, size_t thrd_count) {
    pthread_attr_t attr;
	int ret;

    ret = pthread_attr_init(&attr);
    if (ret == 0) {
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thrd_count);
        if (pool->threads) {
            int i = 0;
            for (; i < thrd_count; i++) {
                if (pthread_create(&pool->threads[i], &attr, __thrdpool_worker, pool) != 0) {
                    // exit(-1);
                    break;
                }
            }
            pool->thrd_count = i;
            pthread_attr_destroy(&attr);
            if (i == thrd_count)
                return 0;
            __threads_terminate(pool);
        }
        ret = -1;
    }
    return ret;
}

void
thrdpool_terminate(thrdpool_t * pool) {
    atomic_store(&pool->quit, 1);
    __nonblock(pool->task_queue);
}

thrdpool_t *
thrdpool_create(int thrd_count) {
    thrdpool_t *pool;

    pool = (thrdpool_t *) malloc(sizeof(*pool));
    if (!pool) return NULL;

    task_queue_t *queue = __taskqueue_create();
    if (queue) {
        pool->task_queue = queue;
        atomic_init(&pool->quit, 0);
        if (__threads_create(pool, thrd_count) == 0) {
            return pool;
        }
        __taskqueue_destroy(pool->task_queue);
    }
    free(pool);
    return NULL;
}

int
thrdpool_post(thrdpool_t *pool, handler_pt func, void *arg) {
    if (atomic_load(&pool->quit) == 1) {
        return -1;
    }
    task_t *task = malloc(sizeof(task_t));
    if (!task) return -1;
    task->func = func;
    task->arg = arg;
    __add_task(pool->task_queue, task);
    return 0;
}

void
thrdpool_waitdone(thrdpool_t *pool) {
    int i;
    for (i=0; i<pool->thrd_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    __taskqueue_destroy(pool->task_queue);
    free(pool->threads);
    free(pool);
}