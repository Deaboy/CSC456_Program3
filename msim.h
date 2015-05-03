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

#endif

