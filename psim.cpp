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

  if( strcasecmp(argv[2], "rr" ) == 0 )
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

  else if( strcasecmp(argv[2], "p" ) == 0 )
  {
    alg = 1;
  }

  else if( strcasecmp(argv[2], "sjf" ) == 0 )
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
  case 0:
    psim_rr(processes, quantum);
    break;
    
  case 1:
    psim_p(processes);
    break;
    
  case 2:
    psim_sjf(processes);
    break;
  }
  
  return 0;
}

void psim_rr( vector<proc> &processes, const int quantum )
{
  vector<proc> queue;
  proc current;
  current.id = 0;
  current.length = -1;
  int t = 0;
  int q = -1;
  
  for( t = 0; !processes.empty() || !queue.empty() || current.length > 0; t++ )
  {
    // handle incoming processes
    if ( !processes.empty() && processes.front().start == t )
    {
      // add all incoming processes at this time to queue
      while( !processes.empty() && processes.front().start == t )
      {
        cerr << t << ": incoming process " << processes.front().id << endl;
        
        queue.push_back( processes.front() );
        processes.erase( processes.begin() );
      }
      
      // no sorting for RR
    }
    
    // Print output for currently running id
    if( current.length == 0 || q == 0 )
    {
      if( current.length == 0 )
      {
        cerr << t << ": finished process " << current.id << endl;
      }
      else if( current.length > 0 && q == 0 )
      {
        cerr << t << ": paused process " << current.id << endl;
      }
    }
    
    // Handle outgoing processes
    if((( current.length <= 0 || q <= 0 ) && !queue.empty() )
       || ( current.length > 0 && q <= 0 ))
    {
      // Add currently running process back into the queue
      if( current.length > 0 && q == 0 )
      {
        queue.push_back( current );
      }
      
      // Start up first process in queue
      current = queue.front();
      queue.erase( queue.begin() );
      q = quantum;
      
      // Display output
      cerr << t << ": started process " << current.id << endl;
    }
    
    // Upkeep
    if( current.length >= 0 && q >= 0 )
    {
      current.length--;
      q--;
    }
  }
  
  // Final output
  cerr << t-1 << ": finished process " << current.id << endl;
  cerr << t-1 << ": end" << endl;
}

void psim_p( vector<proc> &processes )
{
  
}

void psim_sjf( vector<proc> &processes )
{
  // declare variables
  vector<proc> queue;
  proc current;
  current.id = 0;
  current.length = -1;
  int t = 0;

  // run time simulation
  for( t = 0; !processes.empty() || !queue.empty() || current.length > 0; t++ )
  {

    // handle incoming processes
    if( !processes.empty() && processes.front().start == t )
    {
      // add all incoming processes at this time to queue
      while( !processes.empty() && processes.front().start == t )
      {
        cerr << t << ": incoming process " << processes.front().id << endl;
        
        queue.push_back( processes.front() );
        processes.erase( processes.begin() );
      }

      // sort queue by shortest job
      // note fanceh lambda function
      sort( queue.begin(), queue.end(), []( const proc& p1, const proc& p2 ) -> bool
      {
        return( p1.length < p2.length );
      });
    }
    
    // handle outgoing processes, replace with item in front of queue
    if( current.length <= 0 && !queue.empty() )
    {
      if( current.length == 0 )
      {
        cerr << t << ": " << current.id << " replaced by ";
      }
      else
      {
        cerr << t << ": started process ";
      }
      current = queue.front();
      queue.erase( queue.begin() );
      
      cerr << current.id << endl; 
    }
    
    // statistics keeping
    if( current.length >= 0 )
    {
      if( current.length == 0 )
      {
        cerr << t << ": " << current.id << " ends" << endl;
      }
    
      current.length--;
    }
    // TODO
  }
  
  cerr << t-1 << ": end" << endl;
  
  return;
}

