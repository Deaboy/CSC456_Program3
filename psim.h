/***************************************************************************//**
 * File:
 * psim.h
 *
 * Author:
 * Daniel Andrus, Joseph Mowery
 * 
 * Description:
 * Contains function headers for functions related to the psim command.
 ******************************************************************************/

#ifndef _PRISM_H_
#define _PRISM_H_

#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstring>

using namespace std;

struct proc
{
  int id;
  int start;
  int length;
  int priority; //high priority is lower number
  int remaining;

  bool operator<(const proc& other )const
  {
    return start<other.start;
  }

};

int psim(int argc, char*argv[]);
void psim_rr ( vector<proc> &processes, int quantum );
void psim_p( vector<proc> &processes );
void psim_sjf( vector<proc> &processes );

#endif

