//
// Created by dorat on 08/01/2021.
//
#ifndef WET3_FILES_JOB_H
#define WET3_FILES_JOB_H


class Job {
    protected:
        int_mat current_mat;
        int_mat next_mat;
        uint phase;
        int first_line;
        int last_line;
        uint mat_h;
        uint mat_w;

        Job(int_mat curr, int_mat next, uint phase, int first, int last, uint height, uint width) {
            current_mat = curr;
            next_mat = next;
            phase = phase;
            first_line = first;
            last_line  = last;
            mat_h = height;
            mat_w = width;
        }
};


#endif //WET3_FILES_JOB_H
