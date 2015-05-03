//Make sure you don't make a recursive include
#ifndef __DASH__H__
#define __DASH__H__

//Include list
#include <iostream>
#include <string>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "psim.h"
#include "msim.h"

using namespace std;

//Input functions
void CmdPrompt(string &cmd);

//Input manipulation functions
bool Parser(string &cmd);

//Input execution functions
void Cmdnm(char *pid);
void Pid(char *cmd);
void Systat();
void Signal(int pid, int sig);
void ExecuteExternalCommand(char *cmd, char *args[]);
void ExecWrapper(char *cmd, char *args[]);
void CommandRedirect(char *cmd);
void CommandPipe(char *cmd);
void ChangeDir(char *arg);

//Usage functions
void PidUsage();
void CmdnmUsage();
void SystatUsage();
void SignalUsage();

//Helper functions
bool FileReader(char *pid, string &cmdline);
bool FileChecker(char *cmd, ifstream &fin);

//Signal Handlers
static void SignalHandler(int sig);

#endif

