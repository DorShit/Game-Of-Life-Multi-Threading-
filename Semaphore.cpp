#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include "Semaphore.hpp"

Semaphore::Semaphore() {
    this->val = 0;
    pthread_mutex_init(&this->mtx, NULL);
    pthread_cond_init(&this->cond_var, NULL);
}

Semaphore::Semaphore(unsigned val) {
    this->all_locks = val;
    this->not_used_locks = val;
    pthread_mutex_init(&this->mtx, NULL);
    pthread_cond_init(&this->cond_var, NULL);
}

Semaphore::~Semaphore() { // not sure
    pthread_cond_destroy(&this->cond_var);
    pthread_mutex_destroy(&this->mtx);
}

void Semaphore::up() {
     pthread_mutex_lock(&this->mtx);
    val++;
    pthread_cond_signal(&(this->cond_var));
    pthread_mutex_unlock(&(this->mtx));
}

void Semaphore::down(){
    pthread_mutex_lock(&this->mtx);
    while(this->not_used_locks == 0) {
        pthread_cond_wait(&this->cond_var, &this->mtx)
    }
    not_used_locks--;
    pthread_mutex_unlock(&(this->mtx));

}