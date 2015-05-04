#include "dash.h"

//Global variables
key_t baseKey;
int shmid;
int *baseBlock = NULL;
sem_t *lock;

/*-----------------------------------------------------------------------------
 * Function:    MailboxAttach
 * Purpose:     Attempts to attach to a baseBlock
 * Input args:  N/A
 * Output args: N/A
 * Return:      Returns true if block was found to attach to, false otherwise
 */
bool MailboxAttach()
{
  
  //Try to attach to a block if you aren't
  if(NULL == baseBlock)
  {
    
    //Check for a block
    shmid = shmget(baseKey, 1024, 0666);

    //If one didn't exist, return false
    if(-1 == shmid)
      return 0;

    //Otherwise attach to it and return true
    baseBlock = (int*)shmat(shmid, (void*) 0, 0);
    
  }

  return 1;
  
}

/*-----------------------------------------------------------------------------
 * Function:    MailboxBeginSem
 * Purpose:     Attempts to attach and checkout / create a semaphore
 * Input args:  N/A
 * Output args: N/A
 * Return:      Returns false if no block was found to attach, otherwise true
 */
bool MailboxBeginSem()
{

  //If no block was found, return false
  if(!MailboxAttach())
    return 0;
  
  //Create or link to the semaphore and queue up to use it, then return true
  lock = sem_open("/LOCK", O_CREAT, 0666, 1);
  sem_wait(lock);

  return 1;
  
}

/*-----------------------------------------------------------------------------
 * Function:    MailboxEndSem
 * Purpose:     Posts and destroys the previously used semaphore
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void MailboxEndSem()
{

  //Post and destroy the semaphore
  sem_post(lock);
  sem_close(lock);
  
  return;

}

/*-----------------------------------------------------------------------------
 * Function:    MailboxClientInit
 * Purpose:     Prepares client for use by attaching to an existing block
 *              or printing a warning
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void MailboxClientInit()
{

  //Get the base key
  baseKey = ftok("dash", 0);
  
  //Use the semaphore functions to attach to an existing block if one exists
  if(!MailboxBeginSem())
  {

    cout << "Warning: No mailbox exists; use mboxinit before proceeding" << endl;
    return;

  }

  MailboxEndSem();

  return;
  
}

/*-----------------------------------------------------------------------------
 * Function:    MailboxClientDel
 * Purpose:     Destroys client and - if this was the last client attached - 
 *              destroys any remaining mailboxes
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void MailboxClientDel()
{
  
  //If a block existed to attach to
  if(NULL != baseBlock)
  {

      //Detatch from the block
      shmdt((void*) baseBlock);
      baseBlock = NULL;

      //Get some information about the block we detached from
      shmid = shmget(baseKey, 1024, 0666);
      shmid_ds buffer;
      shmctl(shmid, IPC_STAT, &buffer);

      //If nobody else is attached, destroy the boxes
      if(0 == buffer.shm_nattch)
      {
        
        MailboxDel();
        
      }

      //Grab the semaphore quickly, lock it
      lock = sem_open("/LOCK", O_CREAT, 0666, 1);
      sem_wait(lock);

      //Destroy the semaphore completely
      sem_unlink("/LOCK");
      sem_post(lock);
      sem_close(lock);
    
  }
  
}

/*-----------------------------------------------------------------------------
 * Function:    MailboxClient
 * Purpose:     REPL for the mailbox client
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void MailboxClient()
{

  //Initialize
  MailboxClientInit();
  
  string cmd = "";
    
  //Read-Eval-Print Loop
  do
  {
    
    cout << "mail> ";

    getline (cin, cmd);
    
  }while(((" " == cmd) || ("" == cmd) || ("\t" == cmd))
          || MailboxClientParser(cmd));

  //Destroy the client
  MailboxClientDel();

  return;
  
}

/*-----------------------------------------------------------------------------
 * Function:    MailboxDel
 * Purpose:     Destroys any created mailboxes
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void MailboxDel()
{
  
  //Print error if no box exists
  if(!MailboxBeginSem())
  {

    cout << "Error: No boxes exist to delete" << endl;
    return;

  }

  //Get id of the box
  shmid = shmget(baseKey, 1024, 0666);

  //Iterate over the boxes, destroying them
  for(int i = 1; i < baseBlock[0] + 1; i++)
  {
    
    shmctl(baseBlock[i+1], IPC_RMID, NULL);
    
  }
  
  //Destroy the base block
  shmctl(shmid, IPC_RMID, NULL);

  shmdt((void*) baseBlock);
  baseBlock = NULL;

  //Return the semaphore
  MailboxEndSem();

  return;
  
}


/*-----------------------------------------------------------------------------
 * Function:    MailboxInit
 * Purpose:     Creates mailboxes as specified by the user
 * Input args:  num, size
 * Output args: N/A
 * Return:      Errno on failure (typically EEXIST), 0 otherwise
 */
int MailboxInit(int num, int size)
{
  
  //Get the id for the box, creating it if it doesn't exist
  shmid = shmget(baseKey, size*1024, 0666 | IPC_CREAT | IPC_EXCL);

  //If create failed, return errno
  if(-1 == shmid)
  {
    
    return errno;
    
  }

  //Set up the base block
  baseBlock = (int*) shmat(shmid, (void*) 0, 0);
  baseBlock[0] = num;
  baseBlock[1] = size;

  //Loop through and create the boxes
  for(int i = 1; i <= num; i++)
  {
 
    //Get an id for the next box and add it to the baseBlock
    int id = shmget(ftok("dash", i), size*1024, 0666 | IPC_CREAT);
    baseBlock[i+1] = id;

    //Initialize the boxes to empty strings
    char *box = (char*)shmat(id, (void*) 0, 0);
    box[0] = '\0';
    shmdt((void*) box);
    
  }

  return 0;
  
}

/*-----------------------------------------------------------------------------
 * Function:    MailboxWriter
 * Purpose:     Takes user-entered text and saves it to the specified box
 * Input args:  box
 * Output args: N/A
 * Return:      N/A
 */
void MailboxWriter(int box)
{
  
  string newMessage = "";
  string curr;

  //Get input until EOS character is encountered
  while(getline(cin, curr))
  {
    
    newMessage += curr + "\n";
    
  }
  //Clear EOS character
  cin.clear();

  //Attach to the base block, print error if it doesn't exist
  if(!MailboxBeginSem())
  {

    cout << "Error: Unable to write; no boxes exist" << endl;
    return;

  }

  //Bounds check on the entered number
  if((box < 1) || (box > baseBlock[0]))
  {
    
    cout << "dash: Box number must be within [1, " << baseBlock[0] << ']' << endl;
    MailboxEndSem();
    return;
    
  }

  //Set the entered text to the contents of the box, stopping at size kB
  char *dest = (char*) shmat(baseBlock[box+1], (void*) 0, 0);
  strncpy(dest, newMessage.c_str(), baseBlock[1]*1024);

  //Detach from the box
  shmdt((void*)dest);

  //Return semaphore
  MailboxEndSem();

  return;
  
}

/*-----------------------------------------------------------------------------
 * Function:    MailboxReader
 * Purpose:     Retreives the contents of a box and prints it
 * Input args:  box
 * Output args: N/A
 * Return:      N/A
 */
void MailboxReader(int box)
{
 
  //Error message if no boxes exist
  if(!MailboxBeginSem())
  {

    cout << "Error: Unable to read; no boxes exist" << endl;
    return;

  }

  //Bounds checking on box
  if((box < 1) || (box > baseBlock[0]))
  {
    
    cout << "dash: Box number must be within [1, " << baseBlock[0] << ']' << endl;
    MailboxEndSem();
    return;
    
  }

  //Create a string of the correct size
  char *content = new(nothrow) char[baseBlock[1]*1024];

  //Set the contents and print
  char *src = (char*) shmat(baseBlock[box+1], (void*) 0, 0);
  strncpy(content, src, baseBlock[1]*1024);
  cout << content << endl;

  //Delete the string and detach
  delete[] content;
  shmdt((void*) src);

  //Return semaphore
  MailboxEndSem();

  return;
  
}

/*-----------------------------------------------------------------------------
 * Function:    MailboxCopier
 * Purpose:     Copies the contents of one box into the other
 * Input args:  srcNum, destNum
 * Output args: N/A
 * Return:      N/A
 */
void MailboxCopier(int srcNum, int destNum)
{
  
  //Error if no boxes exist
  if(!MailboxBeginSem())
  {

    cout << "Error: Unable to copy; no boxes exist" << endl;
    return;

  }

  //Bounds checking on both source and destination, and make sure they aren't
  //the same box
  if((srcNum < 1) || (srcNum > baseBlock[0])   ||
     (destNum < 1) || (destNum > baseBlock[0]) ||
     (srcNum == destNum))
  {
    
    cout << "dash: Box numbers must be within [1, " << baseBlock[0] 
      << "] and must be different" << endl;

  MailboxEndSem();

    return;
    
  }

  //Get the contents of the first, copy to the second
  char *src = (char*) shmat(baseBlock[srcNum+1], (void*) 0, 0);
  char *dest = (char*) shmat(baseBlock[destNum+1], (void*) 0, 0);
  strncpy(dest, src, baseBlock[1]*1024);

  //Detatch from both boxes
  shmdt((void*)dest);
  shmdt((void*)src);

  //Return semaphore
  MailboxEndSem();

  return;

}

/*-----------------------------------------------------------------------------
 * Function:    MailboxClientParser
 * Purpose:     Decides how to respond to user input (usage, unknown command,
 *              etc.)
 * Input args:  cmd
 * Output args: N/A
 * Return:      false if the command was exit, otherwise true
 */
bool MailboxClientParser(string cmd)
{
  
  char *cCmd;
  char *cArgs[256];
  int i, j = 1;

  //Quit on exit
  if("exit" == cmd)
  {
    
    return 0;
    
  }
  //Otherwise, tokenize the string
  else
  {
    
    char cStr[256*100] = {0};

    for(i = 0; i < (int) cmd.length(); i++)
    {
      
      cStr[i] = cmd[i];
      
    }

    cArgs[0] = cCmd = strtok(cStr, " \t\n\r");
    while(NULL != (cArgs[j] = strtok(NULL, " \t\n\r")))
      j++;

    //If the command was init
    if(!strcmp(cCmd, "mboxinit"))
    {
     
      //Check arguments, print usage if they're wrong
      int num, size;
      if(((NULL == cArgs[1]) || !(num = atoi(cArgs[1])))   ||
         ((NULL == cArgs[2]) || !(size = atoi(cArgs[2]))))
      {
        
        InitUsage();
        return 1;
        
      }

      //Create boxes, catch any possible errno and print it
      if(int exit = MailboxInit(num, size))
      {
        
        cout << "Initialization failed with error code " << exit 
          << " from shmget" <<  endl;
        
      }
      
    }
    //If the command was del
    else if(!strcmp(cCmd, "mboxdel"))
    {
     
      //Print usage if they gave it arguments
      if(NULL != cArgs[1])
      {
        
        DelUsage();
        return 1;

      }

      //Delete boxes
      MailboxDel();
      
    }
    //If the command was write
    else if(!strcmp(cCmd, "mboxwrite"))
    {
      
      //Check arguments, print usage if wrong
      int dest;
      if(((NULL == cArgs[1]) || !(dest = atoi(cArgs[1]))))
      {
        
        WriteUsage();
        return 1;
        
      }
  
      //Write
      MailboxWriter(dest);
      
    }
    //If the command was read
    else if(!strcmp(cCmd, "mboxread"))
    {
      
      //Check arguments, print usage if wrong
      int src;
      if(((NULL == cArgs[1]) || !(src = atoi(cArgs[1]))))
      {
        
        ReadUsage();
        return 1;
        
      }

      //Read given box
      MailboxReader(src);
      
    }
    //If the command was copy
    else if(!strcmp(cCmd, "mboxcopy"))
    {
      
      //Check arguments, print usage if wrong
      int src, dest;
      if(((NULL == cArgs[1]) || !(src = atoi(cArgs[1])))   ||
         ((NULL == cArgs[2]) || !(dest = atoi(cArgs[2]))))
      {
        
        CopyUsage();
        return 1;
        
      }

      //Copy
      MailboxCopier(src, dest);
      
    }
    //Otherwise the command was unrecognized; complain about it
    else
    {
      
      cout << "dash: " << cCmd << " is not a recognized command" << endl;
      
    }
    
  }
 
  return 1;

}

/*-----------------------------------------------------------------------------
 * Function:    InitUsage
 * Purpose:     Usage function for init
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void InitUsage()
{
  
  cout << "dash: Usage: mboxinit <number of mailboxes> <size of mailbox in kB>" 
    << endl;
  
}

/*-----------------------------------------------------------------------------
 * Function:    DelUsage
 * Purpose:     Usage function for delete
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void DelUsage()
{
  
  cout << "dash: Usage: mboxdel" << endl;
  
}

/*-----------------------------------------------------------------------------
 * Function:    WriteUsage
 * Purpose:     Usage function for write
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void WriteUsage()
{
  
  cout << "dash: Usage: mboxwrite <boxnumber> ...message... ^D" << endl;
  
}

/*-----------------------------------------------------------------------------
 * Function:    ReadUsage
 * Purpose:     Usage function for read
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void ReadUsage()
{
  
  cout << "dash: Usage: mboxinit <boxnumber>" << endl;
  
}

/*-----------------------------------------------------------------------------
 * Function:    CopyUsage
 * Purpose:     Usage function for copy
 * Input args:  N/A
 * Output args: N/A
 * Return:      N/A
 */
void CopyUsage()
{
  
  cout << "dash: Usage: mboxinit <boxnumber1> <boxnumber2>" << endl;
  
}

