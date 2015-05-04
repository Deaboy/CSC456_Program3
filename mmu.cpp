#include "mmu.h"

int mmu(int argc, char*argv[])
{
  vector<string> args;
  string input;
  unsigned int i;
  unsigned int j;
  
  // Print intro
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

