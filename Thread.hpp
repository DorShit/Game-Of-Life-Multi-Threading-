#ifndef __THREAD_H
#define __THREAD_H

#include "../Part1/Headers.hpp"
#include "../Part1/PCQueue.hpp"
#include "Game.hpp"

class Thread
{
public:
	Thread(uint thread_id, pthread_mutex_t* mtx, pthread_cond_t* cond, vector<double> m_tile_hist,
           int* done_jobs, PCQueue<Job>* jobs) : m_thread_id(thread_id), mtx(mtx), cond(cond), m_tile_hist(m_tile_hist),
           done_jobs(done_jobs), jobs(jobs);

	virtual ~Thread() {} // Does nothing 

	/** Returns true if the thread was successfully started, false if there was an error starting the thread */
	// Creates the internal thread via pthread_create 
	bool start() {
	    if (pthread_create(&this->m_thread, NULL, entry_func, this) == 0) {
            return true;
        }
        return false;
	}

	/** Will not return until the internal thread has exited. */
	void join() {
	    int res = pthread_join(thread_id, NULL);
	}

	/** Returns the thread_id **/
	uint thread_id() {
		return m_thread_id;
	}
protected:
	// Implement this method in your subclass with the code you want your thread to run. 
	virtual void thread_workload() = 0;
	uint m_thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use
	pthread_mutex_t* mtx;
	pthread_cond_t* cond;
	vector<double> m_tile_hist;
	int* done_jobs;
	PCQueue<Job>* jobs;

private:
	static void * entry_func(void * thread) { ((Thread *)thread)->thread_workload(); return NULL; }
	pthread_t m_thread;
};

class GameOfLifeThread: public Thread {
public:
    GameOfLifeThread(uint thread_id, pthread_mutex_t* mtx, pthread_cond_t* cond, vector<double> m_tile_hist,
            int* done_jobs, PCQueue<Job>* jobs) : Thread(thread_id, mtx, cond, m_tile_hist, done_jobs, jobs){};

    void thread_workload() override {
        while(true) {
            Job job_to_run = this->jobs->pop();
            auto start = std::chrono::system_clock::now();
            if(job_to_run.phase == 1) {
                firstPhase(job_to_run);
            }
            else {
                secondPhase(job_to_run);
            }
            auto now = std::chrono::system_clock::now();
            pthread_mutex_lock(&this->mtx);
            *(this->done_jobs)+= 1;
            double elapsed_time = (double)std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
            (this->m_tile_hist).push_back(elapsed_time);
            pthread_mutex_unlock(&this->mtx);
        }
    }

protected:
    int getAverageColor(vector<uint> neighbors) {
        int sum_of_colors = 0;
        int alive_count = 0;
        for (int i = 0; i < neighbors.size(); i++) {
            if (neighbors[i] != 0) {
                sum_of_colors += neighbors[i];
                alive_count++;
            }
        }
        int color_to_return = int(sum_of_colors/alive_count);
        return color_to_return;
    }

    int getDominant(vector<uint> neighbors) {
        int num_of_colors = 7;
        int dominant_to_return = 0;
        int dominant_amount = 0;
        int temp_dominant = 0;
        for(int j = 1; j <= num_of_colors; j++) {
            for (int i = 0; i < neighbors.size(); i++) {
                if (neighbors[i] == j) {
                    temp_dominant++;
                }
            }
            if(temp_dominant > dominant_amount) {
                dominant_to_return = j;
                dominant_amount = temp_dominant;
            }
        }
        return dominant_to_return;
    }

    void changeNeighborsColor(Job& job, int i, int j, int color) {
        int poss_neighbors_up = 3, poss_neighbors_down= 3, poss_neighbors_sides = 2;
        if(i - 1 >= 0) {// upper bound
            int eps = -1;
            for(int k = 0; k < poss_neighbors_up ; k++) {
                if (j + eps >= 0 && j + eps < job.mat_w) {
                    if(*job.current_mat[i - 1][j + eps] != 0){
                        *job.current_mat[i - 1][j + eps] = color;
                    }
                }
                eps++;
            }
        }
        if(i + 1 < job.mat_h) { // lower bound
            int eps2 = -1;
            for(int k = 0; k < poss_neighbors_down ; k++) {
                if (j + eps >= 0 && j + eps < job.mat_w) {
                if(*job.current_mat[i + 1][j + eps2] != 0){
                    *job.current_mat[i + 1][j + eps2] = color;
                }
            }
                eps2++;
            }
        }
        if(j - 1 >= 0) {// left bound
            if(*job.current_mat[i][j - 1] != 0){
                *job.current_mat[i][j - 1] = color;
            }
        }
        if(j + 1 < job.mat_w) { // right bound
            if(*job.current_mat[i][j + 1] != 0){
                *job.current_mat[i][j + 1] = color;
        }
    }
  }

    vector<uint> getLegalNeighbors(Job& job, int i, int j) {
        vector<uint> neighbors;
        int poss_neighbors_up = 3, poss_neighbors_down= 3, poss_neighbors_sides = 2;

        if(i - 1 >= 0) {// upper bound
            int eps = -1;
            for(int k = 0; k < poss_neighbors_up ; k++) {
                if (j + eps >= 0 && j + eps < job.mat_w) {
                    neighbors.push_back((*job.current_mat)[i - 1][j + eps]);
                }
                eps++;
            }
        }
        if(i + 1 < job.mat_h) { // lower bound
            int eps2 = -1;
            for(int k = 0; k < poss_neighbors_down ; k++) {
                if (j + eps2 >= 0 && j + eps2 < job.mat_w) {
                    neighbors.push_back((*job.current_mat)[i + 1][j + eps2]);
                }
                eps2++;
            }
        }
        if(j - 1 >= 0) { // left bound
            neighbors.push_back((*job.current_mat)[i][j - 1]);
        }
        if(j + 1 < job.mat_w) { // right bound
            neighbors.push_back((*job.current_mat)[i][j + 1]);
        }

        return neighbors;

    }

    void firstPhase(Job& job) {
        int first_line = job.first_line;
        int last_line = job.last_line;
        for(int i = first_line; i < last_line ; i++) {
            for(int j = 0; j < job.mat_w; j ++) {
                //find alll neigh
                vector<uint> neighbors = getLegalNeighbors(job, i, j);
                int alive_cells_count;
                for(auto& nei; neighbors){
                    if(nei != 0) {
                        alive_cells_count ++;
                    }
                }
                int dominant = getDominant(neighbors);
                if((*job.current_mat[i][j] == 0 && alive_cells_count == 3)) { // dead but got 3 alive neighbors
                    *job.current_mat[i][j] = dominant;
                }
                else if (!(*job.current_mat[i][j] != 0 && alive_cells_count >= 2)) { // not alive
                    *job.current_mat[i][j] = 0;
                }

            }
        }
    }

    void secondPhase(Job& job) {
        int first_line = job.first_line;
        int last_line = job.last_line;
        for(int i = first_line; i < last_line ; i++) {
            for(int j = 0; j < job.mat_w; j ++) {
                vector<uint> neighbors = getLegalNeighbors(job, i, j);
                int color = getAverageColor(neighbors);
                changeNeighborsColor(job, i, j, color);
            }
        }
    }
};

#endif
