#ifndef _MSIM_H_
#define _MSIM_H_

#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

int msim(int argc, char*argv[]);

void msim_fifo(vector<int>& ref_string, unsigned int num_frames);
void msim_opt(vector<int>& ref_string, unsigned int num_frames);
void msim_lru(vector<int>& ref_string, unsigned int num_frames);
void msim_lfu(vector<int>& ref_string, unsigned int num_frames);
void msim_mfu(vector<int>& ref_string, unsigned int num_frames);
void msim_sc(vector<int>& ref_string, unsigned int num_frames);
void msim_c(vector<int>& ref_string, unsigned int num_frames);

#endif

