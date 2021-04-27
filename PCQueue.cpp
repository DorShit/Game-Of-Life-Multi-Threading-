#include "PCQueue.hpp"
#include <pthread.h>
#include <vector.h>
#include <cassert>
#include "PCQueue.hpp"

PCQueue<T>::PCQueue() {
    this->size = 0;
    this->items = new queue<T>;
    pthread_mutex_init(&this->mtx, NULL);
    pthread_cond_init(this->cons, NULL);
    pthread_cond_init(this->prod, NULL);
}

PCQueue<T>::~PCQueue() {
    for (auto p : this->items) delete p;
    delete items;
    pthread_mutex_destroy(&this->mtx);
    pthread_cond_destroy(&this->cons);
    pthread_cond_destroy(&this->prod);
}

template <typename T>
void PCQueue<T>::push(const T &item) {
    pthread_mutex_lock(&(this->mtx));
    this->items.push(item);
    this->size++;
    pthread_cond_signal(&(this->cons));
    pthread_mutex_unlock(&(this->mtx));

}

template <typename T>
T PCQueue<T>::pop() {
    pthread_mutex_lock(&(this->mtx));
    while (this->size == 0) {
        pthread_cond_wait(&(this->prod), &(this->mtx));
    }
    assert(this->size != 0);
    T pop_item = this->items.pop();
    this->size--;
    pthread_cond_signal(&(this->cons));
    pthread_mutex_unlock(&(this->mtx));

    return pop_item;
}

