#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstring>
#include "psim.h"

using namespace std;

struct proc{

  int start;
  int length;
  int priority; //high priority is lower number

  bool operator<(const proc& other )const
  {
    return start<other.start;
  }

};

int psim(int argc, char*argv[])
{
  int alg;
  string file;
  int quantum;
  ifstream fin;
  proc process;
  vector<proc> processes;
  
  if(argc < 3)
  {
    //disp help
    return -1;
  }

  if( strcasecmp(argv[2], "rr" ) )
  {
    //prompt for quantum <q>
    alg = 0;

    if(argc < 4)
    {
      //disp error
      return -1;
    }

    quantum = int( strtol( argv[3], NULL, 10 ) );
  }

  else if( strcasecmp(argv[2], "p" ) )
  {
    alg = 1;
  }

  else if( strcasecmp(argv[2], "sjf" ) )
  {
    alg = 2;
  }

  else
  {
    //error, usage
    return -2;
  }

  fin.open( argv[1] );

  if( !fin )
  {
    //error opening file
    //disp usage
    return -2;
  }
  
  while(fin >> process.start >> process.length >> process.priority)
  {
    processes.push_back(process);  
  }

  sort( processes.begin(), processes.end() ); 
  
  return 0;
}

void psim_sjf( vector<proc> &processes )
{
  vector<proc> queue;
  proc current;
  int t = 0;

  for( t = 0; !processes.empty() || !queue.empty() || current.length > 0; t++)
  {

    if( !processes.empty() && t == processes[0].start )
    {
      while(processes[0].start == t)
      {
        queue.push_back( processes[0] );
        processes.erase( processes.begin() );
      }

      sort( queue.begin(), queue.end(), [](const proc& p1, const proc& p2 ) -> bool
      {
        return( p1.length < p2.length );
      });

    }
  }
  return;
}


