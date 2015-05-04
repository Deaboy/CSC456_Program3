#include "mmu.h"

int mmu(int argc, char*argv[])
{
  vector<string> args;
  string input;
  int temp;
  unsigned int i;
  unsigned int j;
  
  unsigned int frame_size;
  unsigned int frame_count;
  unsigned int phys_frame_count;
  unsigned char* memory;
  
  // Validate number of arguments
  if (argc < 4)
  {
    cout << "Usage:\n\tmmu <frame size> <virtual frames> <physical frames>"
         << endl;
    return 0;
  }
  
  // Parse and validate frame size
  temp = (int) strtol(argv[1], NULL, 10);
  if (temp < 1 || (int) log2((double) temp) != log2((double) temp))
  {
    cout << "Invalid frame size " << temp
         << ": Expected positive power of 2" << endl;
    return 0;
  }
  else
  {
    frame_size = (unsigned int) temp;
  }
  
  // Parse and validate frame count
  temp = (int) strtol(argv[2], NULL, 10);
  if (temp < 1)
  {
    cout << "Invalid number of virtual frames " << temp
         << ": Expected positive integer" << endl;
    return 0;
  }
  else
  {
    frame_count = (unsigned int) temp;
  }
  
  // Parse and validate physical frame count
  temp = (int) strtol(argv[3], NULL, 10);
  if (temp < 1)
  {
    cout << "Invalid number of physical frames " << temp
         << ": Expected positive integer" << endl;
    return 0;
  }
  else
  {
    phys_frame_count = (unsigned int) temp;
  }
  
  // Print header
  cout << "Welcome to the Memory Management Unit (MMU) simulator.\n"
          "Please note that this is simulated command prompt with limited "
          "functionality.\nType 'help' for available commands." << endl;
  
  
  do
  {
    args.clear();
    
    cout << "dash/mmu> ";
    
    getline(cin, input);
    
    // Split input by spaces
    j = 0;
    for (i = 0; i <= input.size(); i++)
    {
      // If we find a space or reach end of string
      if (i == input.size() || input[i] == ' '
          || input[i] == '\t' || input[i] == '\n')
      {
        // Make sure substring isn't empty
        if (i - j > 0)
        {
          args.push_back(input.substr(j, i-j));
        }
        j = i + 1;
      }
    }
    
    // Act on arguments
    if (args.size() == 0)
    {
      // Do nothing, just redisplay prompt
    }
    else if (args[0] == "help")
    {
      cout << "Available commands:\n"
              "\n"
              "exit\n"
              "\tExits the MMU simulator\n";
      cout << endl;
    }
    else if (args[0] == "exit")
    {
      break;
    }
    else
    {
      cout << "Unknown command '" << args[0] << "'" << endl;
    }
    
  }
  while(true);
  
  return 0;
}

