#include "msim.h"

int msim(int argc, char*argv[])
{
  ifstream fin;
  vector<int> ref_string;
  int reference;
  int alg;
  int virt_mem;
  int phys_mem;

  // Display help if no arguments are received
  if (argc < 5)
  {
    cerr << "Usage:\n\tmsim <file> <virtual pages> <physical pages> <fifo|opt|lru|lfu|sc|c>" << endl;
    return 0;
  }
  
  // Attempt to open file
  fin.open(argv[1]);
  if (!fin)
  {
    cerr << "Failed to open " << argv[1] << " for input" << endl;
    return 1;
  }
  
  // Parse memory sizes
  virt_mem = int(strtol(argv[2], NULL, 10));
  phys_mem = int(strtol(argv[3], NULL, 10));
  
  // Parse algorithm argument
  if (strcmp(argv[4], "fifo") == 0)
    alg = 0;
  else if (strcmp(argv[4], "opt") == 0)
    alg = 1;
  else if (strcmp(argv[4], "lru") == 0)
    alg = 2;
  else if (strcmp(argv[4], "lfu") == 0)
    alg = 3;
  else if (strcmp(argv[4], "sc") == 0)
    alg = 4;
  else if (strcmp(argv[4], "c") == 0)
    alg = 5;
  else
  {
    cerr << "Unknown page replacement algorithm " << argv[2] << endl;
    alg = -1;
    return 1;
  }
  
  // Read in values from file
  while (fin >> reference)
  {
    ref_string.push_back(reference);
  }

  // Call appropriate algorithm function
  switch (alg)
  {
  case 0:
    // TODO FIFO
    break;
    
  case 1:
    // TODO Optimal
    break;
    
  case 2:
    // TODO LRU
    break;
    
  case 3:
    // TODO LFU
    break;
    
  case 4:
    // TODO Second Chance
    break;
    
  case 5:
    // TODO Clock
    break;
  }

  return 0;
}

