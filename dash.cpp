/* File:    dash.c
 * Purpose: Create DiAgnostic SHell for students to learn POSIX environment
 *
 * Input:   cmdnm <pid>, pid <command>, systat, exit
 * Output:  Determined by input entered
 *
 * Compile: gcc -g -Wall -o dash dash.c
 * Usage:   ./dash
 *
 * Note:    
 * 1.)      Portions of structure based on code provided by Dr. Karlsson
 *
 */

#include "dash.h"

/*-----------------------------------------------------------------------------
 * Function:    main
 * Purpose:     Entry point to program
 * Input args:  
 * Return:      0 on normal termination, 1 if signal handler fails
 */
int main(int argc, char** argv)
{

  //List of variables for main
  string cmd;
  int parser;
  
  //Setup signal handler
  for(int i = 1; i < 32; i++)
  {
    
    signal(i, SignalHandler);
    
  }

  //Read-Eval loop
  do
  {

    CmdPrompt(cmd);

    parser = Parser(cmd);

  }while(parser);

  return 0;

}

/*-----------------------------------------------------------------------------
 * Function:    CmdPrompt
 * Purpose:     Read input from user, and minor checking to make sure input is
 *              nonempty
 * Input args:  cmd
 * Output args: cmd
 * Return:      N/A
 */
void CmdPrompt(string &cmd)
{
  //Initialize buffer
  cmd = "";

  //Prompt user until valid input is presented
  do
  {
    
    cout << "dash> ";

    getline(cin, cmd);
    
  }while((cmd == " ") || (cmd == "") || (cmd == "\t"));

  return;
  
}

/*-----------------------------------------------------------------------------
 * Function:    Parser
 * Purpose:     Parse user input to decide which function needs to be executed.
 *              Returns false only on exit to break Read-Eval loop
 * Input args:  cmd
 * Output args: N/A
 * Return:      False for exit, true for all else
 */
bool Parser(string &cmd)
{

  //Variable list
  char *cCmd;
  char *cArgs[256];
  int i, j = 1;

  //Immediately check if we need to exit
  if(cmd == "exit")
  {
 
    return 0;

  }
  //Otherwise, create a c-style string
  else
  {
    
    char cStr[256*100] = {'\0'};
    bool isRedirect = false;
    bool isPipe = false;
    //Just copy each character over to the c-string
    for(i = 0; i < (int) cmd.length(); i++)
    {
      
      cStr[i] = cmd[i];
      //If we have a redirect, set the flag
      if(('<' == cmd[i]) || ('>' == cmd[i]))
      {
        
        isRedirect = true;
        
      }
      //If we have a pipe, set the flag
      else if('|' == cmd[i])
      {
        
        isPipe = true;
        
      }
      
    }
    
    //Handle redirect case
    if(isRedirect)
    {

      CommandRedirect(cStr);
      return 1;
      
    }
    //Handle pipe case
    else if(isPipe)
    {
      
      CommandPipe(cStr);
      return 1;
      
    }

    //Tokenize input
    cArgs[0] = cCmd = strtok(cStr, " \t\n\r");
    while((cArgs[j] = strtok(NULL, " \t\n\r")) != NULL)
      j++;

    //If there was no command, just ignore it and Read again
    if(cCmd == NULL)
    {

      return 1;
      
    }

    //If they're running cmdnm...
    if(!strcmp(cCmd, "cmdnm"))
    { 
      
      //...but gave no argument, print usage
      if(cArgs[1] == NULL)
      {
        
        CmdnmUsage();
        return 1;
        
      }

      //...try to parse their command to an int, and execute
      int pid = atoi(cArgs[1]);
      if(pid)
      {
        
        Cmdnm(cArgs[1]);
        return 1;
        
      }
      
    }
    //If the command was pid...
    else if(!strcmp(cCmd, "pid"))
    {
     
      //...but they gave no argument, print usage
      if(cArgs[1] == NULL)
      {
        
        PidUsage();
        return 1;
        
      }
      //...try to find it
      else
      {
        
        Pid(cArgs[1]);
        return 1;
        
      }
      
    }
    //If the command was systat...
    else if(!strcmp(cCmd, "systat"))
    {
     
      //...and they gave an argument, print usage
      if(cArgs[1] != NULL)
      {
        
        SystatUsage();
        return 1;
        
      }
      //...print the statistics
      else
      {
        
        Systat();
        return 1;
        
      }
      
    }
    //If they're trying to change directories...
    else if(!strcmp(cCmd, "cd"))
    {
     
      //...call that function
      ChangeDir(cArgs[1]);
      return 1;
      
    }
    //If they're sending a signal...
    else if(!strcmp(cCmd, "signal"))
    {
     
      //...but they didn't give argument(s), print usage
      if((cArgs[1] == NULL) || (cArgs[2] == NULL))
      {
        
        SignalUsage();
        return 1;
          
      }
      //...try to parse, send signal
      else
      {
      
        int sig = atoi(cArgs[1]);
        int pid = atoi(cArgs[2]);
         
        Signal(pid, sig);
        return 1;
          
      }

    }
    //If they want to simulate process scheduler...
    else if(!strcmp(cCmd, "psim"))
    {
      psim(j, cArgs);
      return 1;
    }
    //Otherwise, we have an external command, so run it
    else
    {
      
      ExecuteExternalCommand(cCmd, cArgs);
      return 1;
      
    }
    
  }

  return 1;
  
}

/*-----------------------------------------------------------------------------
 * Function:    Cmdnm
 * Purpose:     Takes a pid, prints the process name
 * Input args:  pid
 * Output args: N/A
 * Return:      N/A
 */
void Cmdnm(char *pid)
{
  
  //Initialize buffer
  string cmdline = "";

  //Call helper
  if(FileReader(pid, cmdline))
  {
    
    cout << cmdline << endl;
    
  }

}

/*-----------------------------------------------------------------------------
 * Function:    Pid
 * Purpose:     Takes a cmdnm, prints the pid
 * Input args:  pid
 * Output args: N/A
 * Return:      N/A
 */
void Pid(char *cmd)
{

  //Variable list
  ifstream fin;
  DIR *proc = opendir("/proc");
  dirent *curEntry;
  string pid;
  
  //While the directory stream hasn't broken
  while(NULL != proc)
  {
    
    //Read the next
    curEntry = readdir(proc);
    
    //If we hit the end of the directory, print error and return false
    if(NULL == curEntry)
    {
      
      closedir(proc);
      cout << "Entered cmdnm not running currently" << endl;
      break;
      
    }
    //If the item was hidden, move on
    else if(curEntry->d_name[0] == '.')
    {
      
      continue;
      
    }
    //If the current item is a directory AND a number...
    else if((curEntry->d_type == DT_DIR) && (atoi(curEntry->d_name)))
    {
     
      //Create path and open the relevant file
      pid = "/proc/";
      pid += curEntry->d_name;
      pid += "/cmdline";
      fin.open(pid.c_str());

      //Call helper
      if(FileChecker(cmd, fin))
      {
       
        //Cleanup and exit if we found it
        fin.close();
        closedir(proc);
        pid = curEntry->d_name;
        cout << "Process ID: " << pid << endl;
        return;
          
      }

      //Close and try again next time
      fin.close();
      
    }
    
  }
  
}

/*-----------------------------------------------------------------------------
 * Function:    Systat
 * Purpose:     Prints CPU and memory info
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void Systat()
{
  
  //Variable list. Open first file...
  string temp, output = "";
  int processor = 1;
  ifstream fin("/proc/version");
  if(!fin)
  {
    
    cerr << "Failed to open /proc/version" << endl;
    return;
    
  }
  //...read to proper location...
  while(getline(fin, temp))
  {
    
    output += temp;
    output += "\n";
    
  }
  
  //...and output
  cout << "Version:\n" << output << endl;

  fin.close();
  output = "";
  
  //Open...
  fin.open("/proc/uptime");
  if(!fin)
  {
    
    cerr << "Failed to open /proc/uptime" << endl;
    return;
    
  }
  
  //...read to proper location...
  fin >> output;
  //...and output
  cout << "Uptime:\n" << output << endl << endl;

  fin.close();
  output = "";

  //Open...
  fin.open("/proc/meminfo");
  if(!fin)
  {
    
    cerr << "Failed to open /proc/meminfo" << endl;
    return;
    
  }

  //...read to proper location...
  getline(fin, temp);
  output += temp;
  output += "\n";
  getline(fin, temp);
  output += temp;

  //...and output
  cout << "Memory:\n" << output << endl << endl;

  fin.close();
  output = "";

  //Open...
  fin.open("/proc/cpuinfo");
  if(!fin)
  {
    
    cerr << "Failed to open /proc/version" << endl;
    return;
    
  }
  
  //...read to proper location...
  while(getline(fin, temp))
  {
    
    if(-1 != (int) temp.find("vendor"))
    {
      
      output += "Processor ";
      output += (char) ('0'+ processor);
      output += ":\n";
      processor++;

      do
      {
        
        output += temp;
        output += "\n";
        
      }while(getline(fin, temp) && ((int) temp.find("cache") == -1));

      output += temp + "\n\n";
      
    }
    
  }

  //...and output
  cout << output << endl;

  fin.close();
  
}

/*-----------------------------------------------------------------------------
 * Function:    Signal
 * Purpose:     Takes a pid and a signal, and signals the given pid
 * Input args:  pid, sig
 * Output args: N/A
 * Return:      N/A
 */
void Signal(int pid, int sig)
{

  //Send signal, catch return
  int status = kill(pid, sig);

  //If something went wrong, display the errno message
  if(-1 == status)
  {
        
    cerr << "kill() had abnormal termination, with error:\n\t"
      << strerror(errno) << endl;
        
  }
  
}

/*-----------------------------------------------------------------------------
 * Function:    ExecuteExternalCommand
 * Purpose:     Takes command name and arguments, creates a fork to make a call
 *              to to the given command, then presents information on the fork
 * Input args:  cmd, args
 * Output args: N/A
 * Return:      N/A
 */
void ExecuteExternalCommand(char *cmd, char *args[])
{
  
  //Variable list
  int childID;
  struct rusage usage;

  //Fork and check that we didn't fail
  if(0 > (childID = fork()))
  {
        
    cerr << "Unable to create child with fork()" << endl;
    return;
        
  }
  //If you're the child call a helper to make exec call
  else if(0 == childID)
  {
        
    ExecWrapper(cmd, args);
        
  }

  //Catch the child's exit status
  int status;
  waitpid(childID, &status, 0);

  //If the exec call went sideways, output the errno message
  if(-1 == status)
  {
        
    cerr << "Child " << childID << " had abnormal termination, with error:\n\t"
      << strerror(errno) << endl;
        
  }

  //Get information about the child's execution
  getrusage(RUSAGE_CHILDREN, &usage);

  //Put together user time and system time
  double utime = (double) (usage.ru_utime.tv_sec + (usage.ru_utime.tv_usec/1000000.0));
  double stime = (double) (usage.ru_stime.tv_sec + (usage.ru_stime.tv_usec/1000000.0));

  //Output relevant information
  cout << "\nChild ran at id:  " << childID
    << "\nUser time:        " << utime
    << "\nSystem time:      " << stime 
    << "\nPage faults:      " << usage.ru_minflt + usage.ru_majflt 
    << "\nContext switches: " << usage.ru_nvcsw + usage.ru_nivcsw << endl;

  return;
  
}

/*-----------------------------------------------------------------------------
 * Function:    ExecWrapper
 * Purpose:     Just a wrapper for an execvp() call
 * Input args:  cmd, args
 * Output args: N/A
 * Return:      N/A
 */
void ExecWrapper(char *cmd, char *args[])
{
  
  //Seriously, it's just a wrapper function
  exit(execvp(cmd, args));
  
}

/*-----------------------------------------------------------------------------
 * Function:    CommandRedirect
 * Purpose:     A special case of normal external execution calls
 * Input args:  cmd
 * Output args: N/A
 * Return:      N/A
 */
void CommandRedirect(char *cmd)
{
 
  //Variable list
  //Check whether or not we need to read from a file, or print to one
  bool read = (NULL != strstr(cmd, "<"));
  char *args[256];
  char *file;
  char *cStr;
  int j = 1;
  int filePointer;
  int childID;
  struct rusage usage;
  
  //Tokenize string
  cStr = strtok(cmd, "<>");
  file = strtok(NULL, " \t\n\r");
  args[0] = strtok(cStr, " \t\n\r");
  while((args[j] = strtok(NULL, " \t\n\r")) != NULL)
    j++;

  //Try to create fork
  if(0 > (childID = fork()))
  {

    cerr << "Unable to create child with fork()" << endl;
    return;

  }
  //If you're the child...
  else if(0 == childID)
  {
    
    //...and we're reading from a file, set up redirect
    if(read)
    {
    
      filePointer = open(file, O_RDONLY);
      close(0);

    }
    //...set up redirect to file
    else
    {
    
      filePointer = creat(file, 0644);
      close(1);
    
    }

    //If we couldn't open the file for redirecting input, print error
    if(-1 == filePointer)
    {
    
      cout << "Unable to open file '" << file << "'" << endl;
      return;
    
    }

    //Create redirection, close pointer
    dup(filePointer);
    close(filePointer);

    //Call wrapper
    ExecWrapper(args[0], args);

  }

  //Catch status code
  int status;
  waitpid(childID, &status, 0);

  //If the execvp call went sideways, print errno message
  if(-1 == status)
  {
        
    cerr << "Child " << childID << " had abnormal termination, with error:\n\t"
      << strerror(errno) << endl;
        
  }

  //Gather information, put it together, print it
  getrusage(RUSAGE_CHILDREN, &usage);

  double utime = (double) (usage.ru_utime.tv_sec + (usage.ru_utime.tv_usec/1000000.0));
  double stime = (double) (usage.ru_stime.tv_sec + (usage.ru_stime.tv_usec/1000000.0));

  cout << "\nChild ran at id:  " << childID
    << "\nUser time:        " << utime
    << "\nSystem time:      " << stime 
    << "\nPage faults:      " << usage.ru_minflt + usage.ru_majflt 
    << "\nContext switches: " << usage.ru_nvcsw + usage.ru_nivcsw << endl;

  return;
  
}

/*-----------------------------------------------------------------------------
 * Function:    CommandPipe
 * Purpose:     Special case of external command execution
 * Input args:  cmd
 * Output args: N/A
 * Return:      N/A
 */
void CommandPipe(char *cmd)
{
  
  //Variable list
  int _pipe[2];
  int childID, gChildID;
  int i = 1, j = 1;
  struct rusage usage;

  //Finding our separate commands
  char *cmd1;
  cmd1 = strtok(cmd, "|");
  char *cmd2;
  cmd2 = strtok(NULL, "|");

  //Tokenizing arguments
  char *args1[256];
  args1[0] = strtok(cmd1, " \t\n\r");
  while((args1[i] = strtok(NULL, " \t\n\r")) != NULL)
    i++;

  char *args2[256];
  args2[0] = strtok(cmd2, " \t\n\r");
  while((args2[j] = strtok(NULL, " \t\n\r")) != NULL)
    j++;
  
  //Error if we couldn't fork
  if(0 > (childID = fork()))
  {

    cerr << "Unable to create child process with fork()" << endl;
    return;

  }
  //If you're the child...
  else if(0 == childID)
  {
   
    //Create pipe
    pipe(_pipe);
    
    //Error if child couldn't fork
    if(0 > (gChildID = fork()))
    {

      cerr << "Unable to create grandchild process with fork()" << endl;
      exit(1);

    }
    //If you're the grandchild...
    else if(0 == gChildID)
    {
     
      //Set up redirection into pipe
      close(1);
      dup(_pipe[1]);
      close(_pipe[0]);
      close(_pipe[1]);

      //Call wrapper
      ExecWrapper(args1[0], args1);
      
    }

    //Catch the status of grandchild
    int status;
    waitpid(gChildID, &status, 0);
    
    //Set up redirection from pipe
    close(0);
    dup(_pipe[0]);
    close(_pipe[0]);
    close(_pipe[1]);

    //Call wrapper
    ExecWrapper(args2[0], args2);
    
  }

  //Catch status of the child
  int status;
  waitpid(childID, &status, 0);

  //If something went wrong, print errno message
  if(-1 == status)
  {
        
    cerr << "Child " << childID << " had abnormal termination, with error:\n\t"
      << strerror(errno) << endl;
        
  }

  //Gather information, put it together, print it
  getrusage(RUSAGE_CHILDREN, &usage);

  double utime = (double) (usage.ru_utime.tv_sec + (usage.ru_utime.tv_usec/1000000.0));
  double stime = (double) (usage.ru_stime.tv_sec + (usage.ru_stime.tv_usec/1000000.0));

  cout << "\nChild ran at id:  " << childID
    << "\nUser time:        " << utime
    << "\nSystem time:      " << stime 
    << "\nPage faults:      " << usage.ru_minflt + usage.ru_majflt 
    << "\nContext switches: " << usage.ru_nvcsw + usage.ru_nivcsw << endl;
  
}

/*-----------------------------------------------------------------------------
 * Function:    ChangeDir
 * Purpose:     Takes a directory to attempt to move into
 * Input args:  arg
 * Output args: N/A
 * Return:      N/A
 */
void ChangeDir(char *arg)
{
      
      //If no argument was given...
      if(NULL == arg)
      {
       
        //...get the home directory. If there is no HOME specified, print error
        arg = getenv("HOME");
        if(NULL == arg)
        {
          
          cout << "No home directory specified in current environment" << endl;
          return;
          
        }

        //...try to change directories, if you couldn't print errno message
        bool notChanged = chdir(arg);
        if(notChanged)
        {
          
          cout << "Experienced an error while attempting to change directory:\n\t"
            << strerror(errno) << endl;
          
        }
        
      }
      //If there was an argument...
      else
      {
       
        //...try to change directories, print errno message on failure
        bool notChanged = chdir(arg);
        if(notChanged)
        {
          
          cout << "Experienced an error while attempting to change directory:\n\t"
            << strerror(errno) << endl;
          
        }
        
      }
  
}

/*-----------------------------------------------------------------------------
 * Function:    CmdnmUsage
 * Purpose:     Prints a usage message
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void CmdnmUsage()
{
  
  cout << "dash: Usage: cmdnm <pid>" << endl;
  
}

/*-----------------------------------------------------------------------------
 * Function:    PidUsage
 * Purpose:     Prints a usage message
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void PidUsage()
{
  
  cout << "dash: Usage: pid <command>" << endl;
  
}

/*-----------------------------------------------------------------------------
 * Function:    SystatUsage
 * Purpose:     Prints a usage message
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void SystatUsage()
{
  
  cout << "dash: Usage: systat" << endl;
  
}

/*-----------------------------------------------------------------------------
 * Function:    SignalUsage
 * Purpose:     Prints a usage message
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void SignalUsage()
{
  
  cout << "dash: Usage: signal <signal_num> <pid>" << endl;
  
}

/*-----------------------------------------------------------------------------
 * Function:    FileReader
 * Purpose:     Helper for checking what process belongs to a given id
 * Input args:  pid, cmdnline
 * Output args: cmdline
 * Return:      False if the pid doesn't exist, true otherwise
 */
bool FileReader(char *pid, string &cmdline)
{
  //Variable list
  //Initialize buffer
  cmdline = "";
  string processID = "/proc/";
  processID += pid;
  processID += "/cmdline";

  //Open input stream
  ifstream fin(processID.c_str());

  //Error and exit if it didn't open
  if(!fin)
  {
    
    cerr << "dash: Failed to open " << processID << endl;
    return false;
    
  }

  //Read until you run out of input
  while(getline(fin, processID, '\0'))
  {
    
    cmdline += processID + " ";
    
  }

  //Cleanup and return
  fin.close();
  return true;
  
}

/*-----------------------------------------------------------------------------
 * Function:    FileChecker
 * Purpose:     Checks if a given filestream contains the given cmd
 * Input args:  cmd, fin
 * Output args: N/A
 * Return:      True if fin contains cmd, false otherwise
 */
bool FileChecker(char *cmd, ifstream &fin)
{
  
  //Variable list
  string cmdline;
  string temp;

  //Reconstruct cmdline info
  while(getline(fin, temp, '\0'))
  {
    
    cmdline += temp + " ";
    
  }

  //If you can find cmd in cmdline, return true
  if(-1 != (int) cmdline.find(cmd))
  {
    
    return true;
    
  }

  //This wasn't the right file
  return false;
  
}

/*-----------------------------------------------------------------------------
 * Function:    SignalHandler
 * Purpose:     Dictates actions for signals sent to the process, excepting
 *              9 and 15, which cannot be caught
 * Input args:  sig
 * Output args: N/A
 * Return:      N/A
 */
static void SignalHandler(int sig)
{
  
  cout << "\nReceived signal: " << sig << endl;
  
  if (sig == 11 || sig == 2)
  {
    exit(1);
  }
}

