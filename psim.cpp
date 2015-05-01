#include "psim.h"

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
    cout << "Usage:\n\tpsim <file> <sjf | p | rr <quantum>>" << endl;
    return -1;
  }

  if( strcasecmp(argv[2], "rr" ) )
  {
    //prompt for quantum <q>
    alg = 0;

    if(argc < 4)
    {
      //disp error
      cerr << "Missing quantum" << endl;
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
    cerr << "Unknown algorithm " << argv[2] << endl;
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
    process.id = processes.size() + 1; 
    processes.push_back(process);  
  }

  sort( processes.begin(), processes.end() ); 
  
  switch (alg)
  {
  case 2:
    psim_sjf(processes);
  }
  
  return 0;
}

void psim_sjf( vector<proc> &processes )
{
  // declare variables
  vector<proc> queue;
  proc current;
  int t = 0;

  // run time simulation
  for( t = 0; !processes.empty() || !queue.empty() || current.length > 0; t++)
  {

    // handle incoming processes
    if( !processes.empty() && t == processes[0].start )
    {
      // add all incoming processes at this time to queue
      while(processes[0].start == t)
      {
        cerr << t << ": incoming process " << processes[0].id << endl;
        
        queue.push_back( processes[0] );
        processes.erase( processes.begin() );
      }

      // sort queue by shortest job
      // note fanceh lambda function
      sort( queue.begin(), queue.end(), [](const proc& p1, const proc& p2 ) -> bool
      {
        return( p1.length < p2.length );
      });
    }
    
    // handle outgoing processes
    if( current.length == 0 && !queue.empty() )
    {
      cerr << t << ": " << current.id << " replaced by ";
      
      current = queue.front();
      queue.erase( queue.begin() );
      
      cerr << current.id << endl; 
    }
    
    // statistics keeping
    // TODO
  }
  
  cerr << t-1 << ": end" << endl;
  
  return;
}


