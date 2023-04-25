// client code wiht interactive menu.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include<sys/wait.h>
#include <stdbool.h>


#define SHM_SIZE 1024 // Shared memory size
#define SHM_KEY 12345


typedef struct {
    int connected;
    char request[256];
    char response[256];
    int server_write;
}ConnectionInfo;


typedef struct
{
    int request[5];
    int response;
    int rw_lock;
} CommChannel;

char clientsName[10][256];
int num_clients = 0;
	
int array_check(char client_name[256]){
	int size = sizeof(clientsName)/sizeof(clientsName[0]);
	if (size == 0)
		return 1;
	for (int i = 0 ; i < size ; i++){
	   if(strcmp(client_name,clientsName[i])==0)
	   	return 0;}
	   	
	return 1;}
	
int main() {
        
        FILE *fptr;
	fptr = fopen("fileOut.txt", "a");
	fclose(fptr);
		
	ConnectionInfo *connectionInfo;
	
	// The same shared memory key used by the server
	key_t connect_key;
	connect_key = 12345;
	
	// Get the shared memory segment created by the server
	int shmid;
	shmid = shmget(connect_key,1024,0666);
		    if (shmid == -1) {
			perror("Shmget");
			printf("Shmget. \n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Error, Line number - 64 ]\n\n", getpid(), gettid());
			exit(1); }

	// Attach shared memory segment to process address space
	connectionInfo = (ConnectionInfo*) shmat(shmid,0,0);
		
	while(true){
			
		printf( "You are using the client verison \n MENU : \n 1. Register new client option \n 2. Send service request\n 3. Unregister the new client \n.[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Interactive menu, Line number - 72 ]\n\n", getpid(), gettid());
		
		printf( "Please enter the option \n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Input, Line number - 74 ]\n\n", getpid(), gettid());
		int option;
		scanf("%d", &option);
		
		if(option == 1){
		
		    key_t connect_key;
		    connect_key = 12345;
			
			// Get the shared memory segment created by the server
			int shmid;
			shmid = shmget(connect_key,1024,0666);
				    if (shmid == -1) {
					perror("Shmget");
					printf("Shmget.\n [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Error, Line number - 87 ]\n\n", getpid(), gettid());
					exit(1); }

			// Attach shared memory segment to process address space
			connectionInfo = (ConnectionInfo*) shmat(shmid,0,0);
		
		    fptr = fopen("fileOut.txt", "a");
	            fprintf(fptr,"%s","The client makes register request to the server on the connect channel\n");
	            fclose(fptr);
	            
	            printf("\nClient has sent register request .\n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Register request, Line number - 98 ]\n\n", getpid(), gettid());
		    
		    printf("Please enter the client name :( No spaces and < 256 characters) \n [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Input name, Line number - 99 ]\n\n", getpid(), gettid());
		
		    char client_name[256];
		    scanf("%s", client_name);
		    
		    while(strlen(client_name) > 256){
		    	printf("\nName entered by the client is longer than 256 characters. Please enter a different name. \n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Invalid Name, Line number - 105 ]\n", getpid(), gettid() );
		    	scanf("%s", client_name);}
		    	
		    
		    if (array_check(client_name) == 0){
		    	printf("\nClient name should be unique . \n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Error, Line number - 110 ]\n\n", getpid(), gettid());
		    	continue;}
		    	
		   for (int i = 0; i < 256 ; i++){
		   	clientsName[num_clients][i] = client_name[i];}
		   num_clients = num_clients +1 ;
		   
		   for (int i = 0; i < 256 ; i++){
		   	connectionInfo->request[i] = client_name[i];}

		    connectionInfo->connected = 1;
		    
	            while( connectionInfo->server_write == 0 );
			    
	            printf("\nClient: Response from server: %s    . \n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Response from server, Line number - 124 ]\n\n",connectionInfo->response, getpid(), gettid());
	            
	            fptr = fopen("fileOut.txt", "a");
	            fprintf(fptr,"%s","The client connects to the server via the communication channel with key \n");
	            fclose(fptr);
	            
	            printf("Server has sent register response to client.\n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Server has sent response to client , Line number - 130 ]\n\n", getpid(), gettid()); 
	            
	            printf("Client connects to the server on the comm channel.\n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Comm channel connection , Line number - 132 ]\n\n", getpid(), gettid());
	            
                    connectionInfo->connected = 0;
                    connectionInfo->server_write = 0;
	            continue;} 
			     	    
		if (option == 2) {

			int client_key;
			char name[256];
			printf("\nEnter client key:       \n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Input, Line number - 142]\n\n", getpid(), gettid());
			scanf("%d", &client_key);
			
			fptr = fopen("fileOut.txt", "a");
	                fprintf(fptr,"%s","The client requests to the server via the communication channel with key : ");
	                fprintf(fptr,"%d\n",client_key);
	                fclose(fptr);
	                
		     	shmid = shmget(client_key,1024,0666);
			    if (shmid == -1) {
				perror("\nThe shared memory isnt present, client not registered to server.");
				printf("[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Error, Line number - 153 ]\n\n", getpid(), gettid());
				continue;
			    }
			
			printf("\nThe client sends service request .\n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Service request, Line number - 157 ]\n\n", getpid(), gettid());
			
			CommChannel *commchannel;  
			commchannel = (CommChannel*) shmat(shmid,0,0);
		     	commchannel->request[0] = 1;
		     	commchannel->rw_lock = 0;
		    
		    int optionf;
		    
		    printf(" The following actions can be perfomed: \n 1.Arithmetic \n 2.EvenOrOdd \n 3.IsPrime \n 4.IsNegative \n Please enter the required option \n [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Interactive menu, Line number - 166 ]\n\n", getpid(), gettid());
		    
		    scanf("%d", &optionf);
		    commchannel->request[1] = optionf;
			   
	         if (optionf == 1){
		 
			    int num1, num2, operator;
			    printf("You have chosen Arithmetic. Please enter two integers followed by an arithmetic operator (Type:number + = 0 , - = 1 , * = 2, / = 3)\n [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Input , Line number - 174 ]\n\n", getpid(), gettid());
		    	
			    scanf("%d", &num1);
			    commchannel->request[2] = num1;
			    scanf("%d", &num2);
			    commchannel->request[3] = num2;
			    scanf("%d", &operator);
			    commchannel->request[4] = operator;
			    commchannel->rw_lock = 1;
			   
			    while(commchannel->rw_lock == 1);
			    
			    printf("\n\nThe server side output is %d    .\n ",commchannel->response);
			    printf("[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Response from server, Line number - 187 ]\n\n",getpid(), gettid());
			    
			    }
			    
			    
		 else if (optionf == 2){
		        int num1;		        
		    	printf("You have chosen EvenOrOdd. Please enter one integer\n [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Input , Line number - 194 ]\n\n", getpid(), gettid());
		        scanf("%d", &num1);
		        
		        commchannel->request[2]= num1;
		        commchannel->rw_lock = 1;
		       

		        while(commchannel->rw_lock == 1);
		        
		        if (commchannel->response){
		        	printf("The server side output is : the number is odd \n  [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Response from server, Line number - 204 ]\n\n", getpid(), gettid());}
		        else 
		        {printf("The server side output is : the number is even \n  [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Response from server, Line number - 206 ]\n\n", getpid(), gettid());}}
			

		  else if (optionf == 3){

			int num1;
		    	printf("You have chosen IsPrime. Please enter one integer. \n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Input , Line number - 212]\n\n", getpid(), gettid());
		        scanf("%d", &num1);
		        commchannel->request[2] = num1;
		        commchannel->rw_lock = 1;
		    
			while(commchannel->rw_lock == 1);
			if (commchannel->response){
		        	printf("The server side output is : the number is not a prime number \n  [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Response from server, Line number - 218 ]\n\n", getpid(), gettid());}
		        else 
		        {printf("The server side output is : the number is a prime number \n  [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Response from server, Line number - 221 ]\n\n", getpid(), gettid());}}
	
		   if (optionf == 4){printf("Not supported \n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Response from server, Line number - 223 ]\n\n", getpid(), gettid());}
		   
		   fptr = fopen("fileOut.txt", "a");
	           fprintf(fptr,"%s","The client recieves response from the server on the communication channel key\n");
	           fclose(fptr);
	           
	           printf("Client has responded to service request .\n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Service request response, Line number - 229 ]\n\n", getpid(), gettid()); 
	           
	           }
		    
		  
	        if (option == 3){	           	 
			    int client_key;
			    
			    // attaching the shared memory segment
			    printf("Enter client key:       \n[ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Input, Line number - 238]\n\n", getpid(), gettid());
			    scanf("%d", &client_key);
			    // fetch client's communication channel
			    	CommChannel *commchannel;
			     	shmid=shmget(client_key,1024,0666);
			     	commchannel = (CommChannel*) shmat(shmid,0,0);
			     	
			     	
			    if (shmdt(connectionInfo) == -1) {
				perror("shmdt\n");
				printf("  [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Error, Line number - 248]\n\n", getpid(), gettid());
				exit(1);
			    }
	           	    
			    commchannel->request[0] = 0;
			    commchannel->rw_lock = 1;
			    
			    while(commchannel->rw_lock == 1);
			    
			     // Detach shared memory segment
			    int shmid = shmget(client_key, 1024, 0666);

			    // Remove shared memory segment
			   int result = shmctl(shmid, IPC_RMID, NULL);
			    if ( result == -1) {
				perror("Shmctl\n");
				printf("  [ PID - %d , ThreadID - %d , SourceFile - client.c , Function - Error, Line number - 264 ]\n\n", getpid(), gettid());
				exit(1);}}}
	
	return 0;
	}
