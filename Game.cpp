#include "Game.hpp"
#include "utils.hpp"

static const char *colors[7] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};
/*--------------------------------------------------------------------------------

--------------------------------------------------------------------------------*/


void Game::addJobsToPCQueue(uint phase) {
    int allocation = (this->mat_h) / (this->m_thread_num);
    for(int i = 0 ; i < m_thread_num - 1 ; i++) {
        Job* job_to_add = new Job(this->current_mat, this->next_mat, phase, i*allocation, (i+1)*allocation)-1,
                this->mat_h, this->mat_w);
        this->jobs->push(*job_to_add);
    }
    int leftover = (this->mat_h) % (this->m_thread_num); // we got leftover and put it at the last one
        Job* job_to_add = new Job(this->current_mat, this->next_mat, phase, (m_thread_num - 1)*allocation,
                (m_thread_num)*allocation + leftover - 1, this->mat_h, this->mat_w);
        this->jobs->push(*job_to_add);
    }
}


void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(nullptr);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}

void Game::_init_game() {
    // Create game fields - Consider using utils:read_file, utils::split V
    jobs = new PCQueue<Jobs>;
    pthread_mutex_init(&this->mtx, NULL);
    pthread_cond_init(&this->cond, NULL);
    this->done_jobs = 0;
    vector<uint> cells = vector<uint>();
    vector<string> file_lines = utils::read_lines(this->file_name);
    for(auto& it: file_lines) { // String to int
        vector<string> r = utils::split(it, DEF_MAT_DELIMITER);
        for(int i = 0; i < r.size(); i++) {
            uint s_to_int = stoi(r[i]);
            cells.push_back(s_to_int);
        }
        this->current_mat->push_back(cells);
        this->next_mat->push_back(cells);
    }
    this->mat_h = current_mat->size();
    this->mat_w = current_mat[0].size();

    this->m_thread_num = std::min(m_thread_num, this->mat_h);

    for(int i = 0; i < this->m_thread_num; i++) {
        GameOfLifeThread game = new GameOfLifeThread(i, &(this->mtx), &(this->cond), this->m_tile_hist,
                &(this->done_jobs), this->jobs);
        m_threadpool.push_back(game);
    }

    for(auto &thread; this->m_threadpool) {
        thread->start();
    }

	// Create & Start threads V
	// Testing of your implementation will presume all threads are started here X
}

void Game::_step(uint curr_gen) {
    // Push jobs to queue
    pthread_mutex_lock(&this->mtx);
    this->done_jobs = 0;
    unit phase = 1;
    //need to feel it here
    addJobsToPCQueue(phase);
    while (done_jobs < m_thread_num) {}
    int_mat temp = *(this->current_mat); // Change Matrix
    *(this->current_mat) = *(this->next_mat);
    *(this->next_mat) = temp;
    phase = 2;
    this->done_jobs = 0;
    addJobsToPCQueue(phase);
    while (done_jobs < m_thread_num) {}

}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here
	for (uint i = 0; i < m_thread_num; ++i) {
        m_threadpool[i]->join();
        delete m_threadpool[i];
    }
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

	if(print_on){ 

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;
		
		// TODO: Print the board 

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}


/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters. 

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
                if (field[i][j] > 0)
                    cout << colors[field[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/ 



