#include <stdio.h>
#include <signal.h>
#include <wait.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CHILD 5  // create NUM_CHILD child processes, where NUM_CHILD is defined in the program
#define WITH_SIGNALS // the version of compilation should be drived by the definition or by the lack of definition of the WITH_SIGNALS

#ifdef WITH_SIGNALS
char interrupt = 0; //0-no interrupt | 1-interrupt

void keyboardInterrupt() // the handler should print out info about receiving the keyboard interrupt
{
	printf("\nparent [%i]: The keyboard interrupt received.\n", getpid());
	interrupt = 1; // and set some mark (global variable) which will notify about the fact of keyboard interrupt occurance
}

void TerminationChild()
{
	printf("\nchild [%i]: Termination of the process.\n", getpid()); //set your own handler of the SIGTERM signal, which will only print a message of the termination of this process
}
#endif

int main()
{
	pid_t child_pid = 1;
	int counter = 0; // counter of the number of children
	int status;		 // process statuses

	struct sigaction ignoring_handler, keyboardInterrupt_handler, restoring_handler, terminating_handler;

#ifdef WITH_SIGNALS
	ignoring_handler.sa_handler = SIG_IGN;

	for (int j = 0; j < NSIG; ++j) // NSIG is the total number of signals defined
	{
		sigaction(j, &ignoring_handler, NULL); // force ignoring of all signals with the signal() (or sigaction())
	}
	restoring_handler.sa_handler = SIG_DFL;
	sigaction(SIGCHLD, &restoring_handler, NULL); // but after that at once restore the default handler for SIGCHLD signal

	keyboardInterrupt_handler.sa_handler = keyboardInterrupt; // Set your own keyboard interrupt signal handler (symbol of this interrupt:  SIGINT)
	sigaction(SIGINT, &keyboardInterrupt_handler, NULL);
#endif

	for (int i = 0; i < NUM_CHILD; ++i)
	{
		child_pid = fork(); // use the fork() function

		if (child_pid == 0)
		{
#ifdef WITH_SIGNALS
			sigaction(SIGINT, &ignoring_handler, NULL); // set to ignore handling of the keyboard interrupt signal
			terminating_handler.sa_handler = TerminationChild;
			sigaction(SIGTERM, &terminating_handler, NULL);
#endif
			printf("child [%i]: New child has been created of parent[%i]\n", getpid(), getppid());		 // Print a message about creation of all child processes, Print process identifier of the parent process
			sleep(10);															 // Sleep for 10 seconds
			printf("\nchild [%i]: Execution has been completed.\n", getpid());	 // Print a message about execution completion
			exit(0);
		}
		else if (child_pid < 0) // Check whether each of the child processes has been correctly created.
		{
			printf("\nparent [%i]: Could not create a new child.\n", getpid()); // If not, print an appropriate message.
			kill(0, SIGTERM);												  // Send to all already created child processes SIGTERM signal
			exit(1);														  // and finish with the exit code 1.
		}
		sleep(1); // insert one second delays between consecutive fork() calls

		/*Between the two consequtive creations of new processes 
		check the mark which may be set by the keyboard interrupt handler. 
		If the mark is set the parent process should signal all just 
		created processes with the SIGTERM and, instead of printing the 
		message about creation, print out a message about interrupt of 
		the creation process. After that, the process should continue with wait()'s loop as before.*/

#ifdef WITH_SIGNALS
		if (interrupt)
		{
			printf("parent [%i]: Interrupt of the creation process!\n", getpid());
			kill(0, SIGTERM);
			break;
		}
#endif
	}

	while (1) // Call in a loop the wait() function
	{
		status = wait(NULL); // untill receiving from the system information that there are no more processes to be synchronized with the parent one.
		if (status < 0)
		{
			printf("\nNo more child processes.\n"); //Print a message that there are no more child processes
			break;
		}
		else
		{
			printf("\nchild [%i]: The child has finished.\n", status);
			counter++; // in the loop do count child processes terminations
		}
	}
	printf("\n%d process(es) terminated.\n", counter); //at the very end of the parent process, print a message with the number of just received child processes exit codes

#ifdef WITH_SIGNALS
	for (int j = 0; j < NSIG; j++)
		sigaction(j, &restoring_handler, NULL); // at the very end of the main process, the old service handlers of all signals should be restored.
#endif
	return 0;
}
