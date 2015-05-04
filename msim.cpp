#include "msim.h"

int msim(int argc, char*argv[])
{
  ifstream fin;
  vector<int> ref_string;
  int reference;
  int alg;
  int num_frames;

  // Display help if no arguments are received
  if (argc < 4)
  {
    cout << "Usage:\n\tmsim <file> <frames> <fifo|opt|lru|lfu|mfu|sc|c>" << endl;
    return 0;
  }
  
  // Attempt to open file
  fin.open(argv[1]);
  if (!fin)
  {
    cout << "Failed to open " << argv[1] << " for input" << endl;
    return 1;
  }
  
  // Parse memory sizes
  num_frames = (int) strtol(argv[2], NULL, 10);
  
  // Make sure number of frames is positive integer
  if (num_frames < 1)
  {
    cout << "Invalid number of frames " << num_frames;
    cout << ": expected positive integer" << endl;
    return 1;
  }
  
  // Parse algorithm argument
  if (strcmp(argv[3], "fifo") == 0)
    alg = 0;
  else if (strcmp(argv[3], "opt") == 0)
    alg = 1;
  else if (strcmp(argv[3], "lru") == 0)
    alg = 2;
  else if (strcmp(argv[3], "lfu") == 0)
    alg = 3;
  else if (strcmp(argv[3], "mfu") == 0)
    alg = 6;
  else if (strcmp(argv[3], "sc") == 0)
    alg = 4;
  else if (strcmp(argv[3], "c") == 0)
    alg = 5;
  else
  {
    cout << "Unknown page replacement algorithm " << argv[2] << endl;
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
    msim_fifo(ref_string, (unsigned int) num_frames);
    break;
    
  case 1:
    msim_opt(ref_string, (unsigned int) num_frames);
    break;
    
  case 2:
    msim_lru(ref_string, (unsigned int) num_frames);
    break;
    
  case 3:
    msim_lfu(ref_string, (unsigned int) num_frames);
    break;
    
  case 4:
    msim_sc(ref_string, (unsigned int) num_frames);
    break;
    
  case 5:
    msim_c(ref_string, (unsigned int) num_frames);
    break;
    
  case 6:
    msim_mfu(ref_string, (unsigned int) num_frames);
    break;
  }

  return 0;
}

void msim_fifo(vector<int>& ref_string, unsigned int num_frames)
{
  vector<int> frames;
  auto frame = frames.end();
  bool fault;
  int faults = 0;
  int padding = 0;
  int temp;
  unsigned int i;
  unsigned int j;
  
  vector<int> queue;          // FIFO vector that drives the algorithm
  
  // For formatting reasons, find 'widest' number
  for (i = 0; i < ref_string.size(); i++)
  {
    // Handle positive and negative numbers differently
    if (ref_string[i] < 0)
    {
      temp = (int) (ceil(log10(-ref_string[i] + 1))) + 1;
    }
    else
    {
      temp = (int) (ceil(log10(ref_string[i] + 1)));
    }
    
    if (temp > padding) padding = temp;
  }
  
  cout << right;
  
  // Work through reference string
  for (i = 0; i < ref_string.size(); i++)
  {
    frame = find(queue.begin(), queue.end(), ref_string[i]);
    fault = (frame == queue.end());
    
    // Check for page faults
    if (fault)
    {
      // Handle page faults
      faults++;
        
      // Check if we have spare space
      if (frames.size() < num_frames)
      {
        // We have plenty of space, just stick it in there (lennyface.jpg)
        frames.push_back(ref_string[i]);
      }
      else
      {
        // Out of space, pop first item off queue and replace that frame
        frame = find(frames.begin(), frames.end(), queue[0]);
        *frame = ref_string[i];
        
        // Update queue
        queue.erase(queue.begin());
      }
      queue.push_back(ref_string[i]);
    }
    else
    {
      // There's no fault... WHAT MORE DO YOU WANT FROM ME?!?!
    }
    
    // Output frame state
    cout << setw(padding) << ref_string[i] << " -> ";
    for (j = 0; j < num_frames; j++)
    {
      cout << "| ";
      if (j < frames.size())
      {
        cout << setw(padding) << frames[j] << " ";
      }
      else
      {
        cout << setw(padding) << " " << " ";
      }
    }
    cout << "|";
    if (fault)
    {
      cout << " FAULT";
    }
    cout << endl;
  }
  
  // Final output
  cout << "\n";
  cout << "page faults: " << faults << endl;
}

void msim_opt(vector<int>& ref_string, unsigned int num_frames)
{
  vector<int> frames;
  auto frame = frames.end();
  bool fault;
  int faults = 0;
  int padding = 0;
  int temp;
  unsigned int i;
  unsigned int j;
  
  vector<int> unfound;        // used for searching for last used item
  
  // For formatting reasons, find 'widest' number
  for (i = 0; i < ref_string.size(); i++)
  {
    // Handle positive and negative numbers differently
    if (ref_string[i] < 0)
    {
      temp = (int) (ceil(log10(-ref_string[i] + 1))) + 1;
    }
    else
    {
      temp = (int) (ceil(log10(ref_string[i] + 1)));
    }
    
    if (temp > padding) padding = temp;
  }
  
  cout << right;
  
  for (i = 0; i < ref_string.size(); i++)
  {
    frame = find(frames.begin(), frames.end(), ref_string[i]);
    fault = (frame == frames.end());
    
    // Check for and handle page faults
    if (fault)
    {
      // Increment counter
      faults++;
      
      // Figure out how to add new frame to our frame space
      if (frames.size() < num_frames)
      {
        // We have room, just append to end of frame list
        frames.push_back(ref_string[i]);
      }
      else
      {
        // We don't have room, so figure out who to replace
        
        // Build "unfound" list, remove items as they're found
        unfound = frames;
        
        // If a loaded frame is found in reference string's future, try not to
        // replace it. This is a greedy algorithm.
        for (j = i + 1; j < ref_string.size() && unfound.size() > 1; j++)
        {
          frame = find(unfound.begin(), unfound.end(), ref_string[j]);
          if (frame != unfound.end())
          {
            unfound.erase(frame);
          }
        }
        
        // Replace first item left in unfound array
        frame = find(frames.begin(), frames.end(), unfound[0]);
        *frame = ref_string[i];
      }
    }
    else
    {
      // There's no fault... WHAT MORE DO YOU WANT FROM ME?!?!
    }
    
    
    
    // Output frame state
    cout << setw(padding) << ref_string[i] << " -> ";
    for (j = 0; j < num_frames; j++)
    {
      cout << "| ";
      if (j < frames.size())
      {
        cout << setw(padding) << frames[j] << " ";
      }
      else
      {
        cout << setw(padding) << " " << " ";
      }
    }
    cout << "|";
    if (fault)
    {
      cout << " FAULT";
    }
    cout << endl;
  }
  
  // Final output
  cout << "\n";
  cout << "page faults: " << faults << endl;
}

void msim_lru(vector<int>& ref_string, unsigned int num_frames)
{
  vector<int> frames;
  auto frame = frames.end();
  bool fault;
  int faults = 0;
  int padding = 0;
  int temp;
  unsigned int i;
  unsigned int j;
  
  vector<int> queue;          // LRU vector that drives the algorithm
  
  // For formatting reasons, find 'widest' number
  for (i = 0; i < ref_string.size(); i++)
  {
    // Handle positive and negative numbers differently
    if (ref_string[i] < 0)
    {
      temp = (int) (ceil(log10(-ref_string[i] + 1))) + 1;
    }
    else
    {
      temp = (int) (ceil(log10(ref_string[i] + 1)));
    }
    
    if (temp > padding) padding = temp;
  }
  
  cout << right;
  
  // Work through reference string
  for (i = 0; i < ref_string.size(); i++)
  {
    frame = find(queue.begin(), queue.end(), ref_string[i]);
    fault = (frame == queue.end());
    
    // Check for page faults
    if (fault)
    {
      // Handle page faults
      faults++;
        
      // Check if we have spare space
      if (frames.size() < num_frames)
      {
        // We have plenty of space, just stick it in there (lennyface.jpg)
        frames.push_back(ref_string[i]);
      }
      else
      {
        // Out of space, pop first item off queue and replace that frame
        frame = find(frames.begin(), frames.end(), queue[0]);
        *frame = ref_string[i];
        queue.erase(queue.begin());
      }
    }
    else
    {
      // Remove item from queue so we can insert it at beginning again
      queue.erase(frame);
    }
    
    // Add item to back of queue no matter what case
    queue.push_back(ref_string[i]);
    
    // Output frame state
    cout << setw(padding) << ref_string[i] << " -> ";
    for (j = 0; j < num_frames; j++)
    {
      cout << "| ";
      if (j < frames.size())
      {
        cout << setw(padding) << frames[j] << " ";
      }
      else
      {
        cout << setw(padding) << " " << " ";
      }
    }
    cout << "|";
    if (fault)
    {
      cout << " FAULT";
    }
    cout << endl;
  }
  
  // Final output
  cout << "\n";
  cout << "page faults: " << faults << endl;
}

void msim_lfu(vector<int>& ref_string, unsigned int num_frames)
{
  vector<int> frames;
  auto frame = frames.end();
  bool fault;
  int faults = 0;
  int padding = 0;
  int temp;
  unsigned int i;
  unsigned int j;
  
  vector<int> counts;
  
  // For formatting reasons, find 'widest' number
  for (i = 0; i < ref_string.size(); i++)
  {
    // Handle positive and negative numbers differently
    if (ref_string[i] < 0)
    {
      temp = (int) (ceil(log10(-ref_string[i] + 1))) + 1;
    }
    else
    {
      temp = (int) (ceil(log10(ref_string[i] + 1)));
    }
    
    if (temp > padding) padding = temp;
  }
  
  cout << right;
  
  // Work through reference string
  for (i = 0; i < ref_string.size(); i++)
  {
    frame = find(frames.begin(), frames.end(), ref_string[i]);
    fault = (frame == frames.end());
    
    // Check for page faults
    if (fault)
    {
      // Handle page faults
      faults++;
        
      // Check if we have spare space
      if (frames.size() < num_frames)
      {
        // We have plenty of space, just stick it in there (lennyface.jpg)
        frames.push_back(ref_string[i]);
        counts.push_back(1);
      }
      else
      {
        // Out of space, choose item to replace
        frame = counts.begin();
        
        // Choose item with lowest count to replace
        for (j = 0; j < counts.size(); j++)
        {
          if (counts[j] < *frame)
          {
            frame = counts.begin() + j;
          }
        }
        
        // Replace element at position frame
        counts[frame - counts.begin()] = 1;
        frames[frame - counts.begin()] = ref_string[i];
      }
    }
    else
    {
      counts[frame - frames.begin()]++;
    }
    
    // Output frame state
    cout << setw(padding) << ref_string[i] << " -> ";
    for (j = 0; j < num_frames; j++)
    {
      cout << "| ";
      if (j < frames.size())
      {
        cout << setw(padding) << frames[j] << " ";
      }
      else
      {
        cout << setw(padding) << " " << " ";
      }
    }
    cout << "|";
    if (fault)
    {
      cout << " FAULT";
    }
    cout << endl;
  }
  
  // Final output
  cout << "\n";
  cout << "page faults: " << faults << endl;
}

void msim_mfu(vector<int>& ref_string, unsigned int num_frames)
{
  vector<int> frames;
  auto frame = frames.end();
  bool fault;
  int faults = 0;
  int padding = 0;
  int temp;
  unsigned int i;
  unsigned int j;
  
  vector<int> counts;
  
  // For formatting reasons, find 'widest' number
  for (i = 0; i < ref_string.size(); i++)
  {
    // Handle positive and negative numbers differently
    if (ref_string[i] < 0)
    {
      temp = (int) (ceil(log10(-ref_string[i] + 1))) + 1;
    }
    else
    {
      temp = (int) (ceil(log10(ref_string[i] + 1)));
    }
    
    if (temp > padding) padding = temp;
  }
  
  cout << right;
  
  // Work through reference string
  for (i = 0; i < ref_string.size(); i++)
  {
    frame = find(frames.begin(), frames.end(), ref_string[i]);
    fault = (frame == frames.end());
    
    // Check for page faults
    if (fault)
    {
      // Handle page faults
      faults++;
        
      // Check if we have spare space
      if (frames.size() < num_frames)
      {
        // We have plenty of space, just stick it in there (lennyface.jpg)
        frames.push_back(ref_string[i]);
        counts.push_back(1);
      }
      else
      {
        // Out of space, choose item to replace
        frame = counts.begin();
        
        // Choose item with lowest count to replace
        for (j = 0; j < counts.size(); j++)
        {
          if (counts[j] > *frame)
          {
            frame = counts.begin() + j;
          }
        }
        
        // Replace element at position frame
        counts[frame - counts.begin()] = 1;
        frames[frame - counts.begin()] = ref_string[i];
      }
    }
    else
    {
      counts[frame - frames.begin()]++;
    }
    
    // Output frame state
    cout << setw(padding) << ref_string[i] << " -> ";
    for (j = 0; j < num_frames; j++)
    {
      cout << "| ";
      if (j < frames.size())
      {
        cout << setw(padding) << frames[j] << " ";
      }
      else
      {
        cout << setw(padding) << " " << " ";
      }
    }
    cout << "|";
    if (fault)
    {
      cout << " FAULT";
    }
    cout << endl;
  }
  
  // Final output
  cout << "\n";
  cout << "page faults: " << faults << endl;
}

void msim_sc(vector<int>& ref_string, unsigned int num_frames)
{
  vector<int> frames;
  auto frame = frames.end();
  bool fault;
  int faults = 0;
  int padding = 0;
  int temp;
  unsigned int i;
  unsigned int j;
  
  vector<int> queue;          // FIFO vector that drives the algorithm
  vector<int> refbit;
  
  // For formatting reasons, find 'widest' number
  for (i = 0; i < ref_string.size(); i++)
  {
    // Handle positive and negative numbers differently
    if (ref_string[i] < 0)
    {
      temp = (int) (ceil(log10(-ref_string[i] + 1))) + 1;
    }
    else
    {
      temp = (int) (ceil(log10(ref_string[i] + 1)));
    }
    
    if (temp > padding) padding = temp;
  }
  
  cout << right;
  
  // Work through reference string
  for (i = 0; i < ref_string.size(); i++)
  {
    frame = find(queue.begin(), queue.end(), ref_string[i]);
    fault = (frame == queue.end());
    
    // Check for page faults
    if (fault)
    {
      // Handle page faults
      faults++;
        
      // Check if we have spare space
      if (frames.size() < num_frames)
      {
        // We have plenty of space, just stick it in there (lennyface.jpg)
        frames.push_back(ref_string[i]);
      }
      else
      {
        // Out of space, find first item in queue not referenced
        for (j = 0; j < refbit.size() && refbit[j] == 1; j++)
        {
          refbit[j] = 0;
        }
        if (j == refbit.size())
        {
          j = 0;
        }
        
        frame = find(frames.begin(), frames.end(), queue[j]);
        *frame = ref_string[i];
        
        // Update queue
        refbit.erase(refbit.begin() + j);
        queue.erase(queue.begin() + j);
      }
      queue.push_back(ref_string[i]);
      refbit.push_back(0);
    }
    else
    {
      refbit[frame - queue.begin()] = 1;
    }
    
    // Output frame state
    cout << setw(padding) << ref_string[i] << " -> ";
    for (j = 0; j < num_frames; j++)
    {
      cout << "| ";
      if (j < frames.size())
      {
        cout << setw(padding) << frames[j] << " ";
      }
      else
      {
        cout << setw(padding) << " " << " ";
      }
    }
    cout << "|";
    if (fault)
    {
      cout << " FAULT";
    }
    cout << endl;
  }
  
  // Final output
  cout << "\n";
  cout << "page faults: " << faults << endl;
}

void msim_c(vector<int>& ref_string, unsigned int num_frames)
{
  vector<int> frames;
  auto frame = frames.end();
  bool fault;
  int faults = 0;
  int padding = 0;
  int temp;
  unsigned int i;
  unsigned int j;
  
  vector<int> refbit;
  auto hand = refbit.end();
  
  // For formatting reasons, find 'widest' number
  for (i = 0; i < ref_string.size(); i++)
  {
    // Handle positive and negative numbers differently
    if (ref_string[i] < 0)
    {
      temp = (int) (ceil(log10(-ref_string[i] + 1))) + 1;
    }
    else
    {
      temp = (int) (ceil(log10(ref_string[i] + 1)));
    }
    
    if (temp > padding) padding = temp;
  }
  
  cout << right;
  
  // Work through reference string
  for (i = 0; i < ref_string.size(); i++)
  {
    frame = find(frames.begin(), frames.end(), ref_string[i]);
    fault = (frame == frames.end());
    
    // Check for page faults
    if (fault)
    {
      // Handle page faults
      faults++;
        
      // Check if we have spare space
      if (frames.size() < num_frames)
      {
        // We have plenty of space, just stick it in there (lennyface.jpg)
        frames.push_back(ref_string[i]);
        refbit.push_back(0);
        hand = refbit.end() - 1;
      }
      else
      {
        // Out of space, move hand until we find unreferenced item
        while (*hand == 1)
        {
          *hand = 0;
          hand++;
          if (hand == refbit.end())
          {
            hand = refbit.begin();
          }
        }
        
        frame = frames.begin() + (hand - refbit.begin());
        *frame = ref_string[i];
      }
    }
    else
    {
      hand = refbit.begin() + (frame - frames.begin());
      *hand = 1;
    }
    
    // Output frame state
    cout << setw(padding) << ref_string[i] << " -> ";
    for (j = 0; j < num_frames; j++)
    {
      cout << "| ";
      if (j < frames.size())
      {
        cout << setw(padding) << frames[j] << " ";
      }
      else
      {
        cout << setw(padding) << " " << " ";
      }
    }
    cout << "|";
    if (fault)
    {
      cout << " FAULT";
    }
    cout << endl;
  }
  
  // Final output
  cout << "\n";
  cout << "page faults: " << faults << endl;
}

