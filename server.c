// server code with interactive menu.

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include<unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>


#define CONNECT_CH_KEY 12345
#define SHM_SIZE 1024 // Shared memory size
#define MAX_CLIENTS 15


typedef struct {
    int connected;
    char request[256];
    char response[256];
    int server_write;
} ConnectionInfo;


typedef struct
{
    int request[5];
    int response;
    int rw_lock;
} CommChannel;


int num_clients = 0;
int num_response = 0;
char clientsName[MAX_CLIENTS][256];
int requests_serviced[MAX_CLIENTS];
int shared_memory_key[MAX_CLIENTS];

pthread_t tid[MAX_CLIENTS];
pthread_t del;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void sigint_handler (int signo)
{
  printf("\n\n\n\nClient name   -   Request serviced  -   shared memory  \n");
  
  FILE *fptr;
  fptr = fopen("fileOut.txt", "a");
  fprintf(fptr,"%s","\n\n\n\nClient name   -   Request serviced  -   shared memory  \n");	   
  
  int sum = 0;
  for (int i = 0; i < num_clients; i++ ){
  	printf("%s  -   %d   -   %d \n", clientsName[i], requests_serviced[i], shared_memory_key[i]);
  	  fprintf(fptr,"%s   ",clientsName[i]);
  	  fprintf(fptr,"%d   ",requests_serviced[i]);
  	  fprintf(fptr,"%d\n",shared_memory_key[i]);
  	sum = sum + requests_serviced[i]; }
  	
  printf("\nServer side summary info.\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function -Summary info, Line number - 62 ]\n\n", getpid(), gettid()); 
  
  printf("Total no of request_serviced = %d " , sum); 
  printf("\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Total Requests, Line number - 64 ]\n\n", getpid(), gettid()); 
    
  fprintf(fptr,"%s  ","Total no of request_serviced  : ");
  fprintf(fptr,"%d\n",sum);
  fclose(fptr);
  exit (EXIT_SUCCESS);
}

void *client_thread(void *arg)
{   
    FILE *fptr;
    int key = (int) arg; 
    
    int index;
    for ( index = 0; index <num_clients ; index++){
    	if (shared_memory_key[index] == key){
    		break;}}		
    requests_serviced[index] = 1;
    int shmid = shmget(key, 1024, 0666);

    
    if (shmid == -1)
    {   
    	printf("Shmget. \n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Error, Line number - 88 ]\n\n", getpid(), gettid());
    	
        exit(1);}
    
    CommChannel *msg;
    msg = (CommChannel *)shmat(shmid, 0, 0);
    
   while(1){

	    while( msg->rw_lock == 0);
	    pthread_mutex_lock(&mutex);	
	     
	    num_response = num_response + 1 ;
	    requests_serviced[index] = requests_serviced[index] + 1;

	    
	    if (msg->request[0] == 0)
	    {    
	           fptr = fopen("fileOut.txt", "a");
		   fprintf(fptr,"%s","The server has received a unregister request on the comm channel with key : ");	   
		   fprintf(fptr,"%d\n",key);
		   fclose(fptr);
	           
	           printf("Server has received unregister request\n.[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Server has recieved unregsiter , Line number - 111 ]\n\n", getpid(), gettid()); 
	           
	           msg->rw_lock = 0;
	           
		   fptr = fopen("fileOut.txt", "a");
		   fprintf(fptr,"%s","The server has unregistered the client with comm channel with key \n");	   
		   fclose(fptr);
		   
		   printf("Server done cleanup of the comm channel.\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Clean up of comm channel, Line number - 119 ]\n\n", getpid(), gettid()); }
		
		
	    else
	    {      fptr = fopen("fileOut.txt", "a");
		   fprintf(fptr,"%s","The server has received a service request on the comm channel with key : ");	   
		   fprintf(fptr,"%d\n",key);
		   fclose(fptr);
		   
		   printf("Server has received a service request.\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Service request, Line number - 128 ]\n\n", getpid(), gettid()); 
		   
		if (msg->request[1] == 2)
		{
		    if (msg->request[2] % 2 == 0)
		    {
		        msg->response = 0;
		        msg->rw_lock = 0;}
		        
		    else
		    {
		        msg->response = 1;
		        msg->rw_lock = 0;}}
		        
		else if (msg->request[1] == 3)
		{
		    int output = 0;
		    
		    // Check from 2 to square root of n
		    for (int i = 2; i < msg->request[2]; i++)
			{if (msg->request[2] % i == 0)
			    {output = 1; break;}}
			    
		    msg->response = output;
		    msg->rw_lock = 0;}
	
		else if (msg->request[1] == 4)
		{
		    msg->response = 0;
		    msg->rw_lock = 0;}
		    
		else
		{
		    int a = msg->request[2];
		    int b = msg->request[3];
		    int c = msg->request[4];
		    
		    if (c == 0)
		    {   msg->response = a + b; /*addition operation*/ }
		    
		    else if (c == 1)
		    {   msg->response = a - b; /* subtraction operation */}
		    
		    else if (c == 2)
		    {   msg->response = a * b; /*multiplication operation  */ }
		    
		    else
		    {   msg->response = a / b; /* division */ }

		    msg->rw_lock = 0;}}
		    
		   fptr = fopen("fileOut.txt", "a");
		   fprintf(fptr,"%s","The server has responded to a service request on the comm channel with key : ");	   
		   fprintf(fptr,"%d\n",key);
		   fclose(fptr);
		   
		   printf("Server has responded to service request.\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Service request response, Line number - 184 ]\n\n", getpid(), gettid()); 
		      
    pthread_mutex_unlock(&mutex);
    }};


int main() {

    FILE *fptr;
    fptr = fopen("fileOut.txt", "w");    
    fclose(fptr);
    
    if (signal (SIGINT, sigint_handler) == SIG_ERR)
    {
         fprintf (stderr, "Error in SIGINT!");
         exit (EXIT_FAILURE);}
    
    int connectCh = 1; // Connect channel for client registration
    int shmid;
    int client_key = 1000;
    
    // Create shared memory segment
    shmid = shmget(CONNECT_CH_KEY, SHM_SIZE, IPC_CREAT | 0666);

    if (shmid == -1) {
    	fptr = fopen("fileOut.txt", "a");
	fprintf(fptr,"%s","Error in creating shared memory for connect channel\n");
        fclose(fptr);
        
        printf("Error in creating shared memory for connect channel.\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Error in creating connect channel, Line number - 213 ] \n\n ", getpid(), gettid());
        exit(1);}
    
    fptr = fopen("fileOut.txt", "a");
    fprintf(fptr,"%s","The server has initiated and created the connect channel.\n");
    fclose(fptr);
    
    printf("The server has initiated and created the connect channel.\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Initiation and Creation of connect channel, Line number - 220 ] \n\n ", getpid(), gettid());
    
    // Attach shared memory segment to process address space
    ConnectionInfo *connectionInfo;
    connectionInfo = (ConnectionInfo*) shmat(shmid, NULL, 0);

    if (connectionInfo == (void*) -1){
        perror("Shmat");
        exit(1);}

    // Initialize connection info
    connectionInfo->connected = 0;
    connectionInfo->server_write = 0;
    printf("Server started. Waiting for connection requests...\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Status update , Line number - 233]\n\n", getpid(), gettid());

    // Server loop
    while (1) {

        // Check for connection request
        if (strlen(connectionInfo->request )<= 0) {
            printf("No active connections. Waiting...\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Status update , Line number - 240]\n\n", getpid(), gettid());
            sleep(1); // Sleep for 1 second before checking again
            continue;}
             
        else {
            // Connection request received, handle it
            fptr = fopen("fileOut.txt", "a");
	    fprintf(fptr,"%s","The server has recieved a connection request. \n");
	    fclose(fptr);
            
            client_key++;
            char client_commkey[256];
            sprintf(client_commkey, "%d", client_key);

            if (strlen(connectionInfo->request)>0){
            printf("Server has recieved register request : Message: %s .\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Register request, Line number - 255 ]\n\n", connectionInfo->request, getpid(), gettid());
            num_response = num_response + 1 ;
            
            for (int i = 0; i < 256 ; i++){
		clientsName[num_clients][i] = connectionInfo->request[i];}
		
            shmid = shmget(client_key, SHM_SIZE, IPC_CREAT | 0666);
            if (shmid == -1) { perror("Shmget");
            printf("Shmget. \n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Error, Line number - 263 ]\n\n", getpid(), gettid());
			exit(1);}
			
	    printf("Server has created a communication channel : key : %d .\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Creation of communication channel , Line number - 266]\n\n", client_key, getpid(), gettid());
		    
            fptr = fopen("fileOut.txt", "a");
	    fprintf(fptr,"%s","The server has created a communication channel for key :");
	    fprintf(fptr,"%d\n",client_key);
	    fclose(fptr);
            
	    for (int i = 0; i < 256 ; i++){
		connectionInfo->response[i] = client_commkey[i];}
	    shared_memory_key[num_clients] = client_key;
	    num_clients = num_clients + 1;

            
            fptr = fopen("fileOut.txt", "a");
	    fprintf(fptr,"%s","The server has sent response to client register request.key : ");
	    fprintf(fptr,"%d\n",client_key);
	    fclose(fptr);
	    
	    printf("Server has sent response to client.\n[ PID - %d , ThreadID - %d , SourceFile - server.c , Function - Server has sent response to client , Line number - 284 ]\n\n", getpid(), gettid());    
	    
	    connectionInfo->server_write = 1;
            connectionInfo->connected = 0;
            
            pthread_create(&tid[num_clients], NULL, client_thread, (void*)client_key);
            
            //Reset connection info
            memset(connectionInfo->request, 0, sizeof(connectionInfo->request));

	}}}
    
    connectionInfo->connected = 1;
    return 0;
}
