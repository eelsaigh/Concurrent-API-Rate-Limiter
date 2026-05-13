#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct {
    int max;
    int rate;
    double tokens;
    long last;
    pthread_mutex_t lock;
} Bucket;

typedef struct {
    int id;
    Bucket* b;
} Request;

long get_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void init(Bucket* b, int max, int rate) {
    b->max = max;
    b->rate = rate;
    b->tokens = max;
    b->last = get_ms();
    pthread_mutex_init(&b->lock, NULL);
}

void refill(Bucket* b) {
    long now = get_ms();
    double diff = (now - b->last) / 1000.0;
    b->tokens += diff * b->rate;
    if (b->tokens > b->max) b->tokens = b->max;
    b->last = now;
}

bool allow(Bucket* b) {
    pthread_mutex_lock(&b->lock);
    refill(b);
    bool ok = false;
    if (b->tokens >= 1.0) {
        b->tokens -= 1.0;
        ok = true;
    }
    pthread_mutex_unlock(&b->lock);
    return ok;
}

void* run(void* arg) {
    Request* r = (Request*)arg;
    if (allow(r->b)) printf("Req %d: OK\n", r->id);
    else printf("Req %d: 429\n", r->id);
    free(r);
    return NULL;
}

int main() {
    Bucket b;
    init(&b, 5, 2);
    pthread_t t[10];

    for (int i = 0; i < 10; i++) {
        Request* r = malloc(sizeof(Request));
        r->id = i + 1;
        r->b = &b;
        pthread_create(&t[i], NULL, run, r);
        usleep(100000);
    }

    for (int i = 0; i < 10; i++) pthread_join(t[i], NULL);
    pthread_mutex_destroy(&b.lock);
    return 0;
}