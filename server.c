#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include<sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>


void serviceClient(int);

int main(int argc, char *argv[])
{
  	int sd,st, client,portNumber;
  	struct sockaddr_in servAdd;    

 	if(argc != 2)
	{
    		printf("Call model: %s <Port Number>\n", argv[0]);
    		exit(0);
  	}
  	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
    		fprintf(stderr, "Cannot create socket\n");
    		exit(1);
  	}
  	sd = socket(AF_INET, SOCK_STREAM, 0);
  	servAdd.sin_family = AF_INET;
  	servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  	sscanf(argv[1], "%d", &portNumber);
  	servAdd.sin_port = htons((uint16_t)portNumber);

  	bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
  	listen(sd, 5);
	printf("Waiting for the Connection\n");

    	client = accept(sd, NULL, NULL);
    	printf("Received client\n");
    	if(!fork())
	{
      		serviceClient(client);
		close(client);
	}
	else
    		wait(&st);
	exit(0);
}

void serviceClient(int sd)
{
	char *cmd;
	char *fname;
	char *content;
	char msg[255];
	int n,s,fd1,fd2,pid;
	int c,size=0,p=0,q=0,fsize;
	
        write(sd, "Give any commands get,put or quit:", 35);
        while(1)
	{
	   	if(n=read(sd, msg, 255))
	   	{
	     		msg[n]='\0';
	     		if(!strcasecmp(msg, "quit\n"))
			{
	        		exit(0);
	     		}
			else
			{
				c = 0;
    				char *tokens = strtok(msg, " \n");
    				while (tokens != NULL && c < 2)
    				{
        				if (c == 0)
            					cmd = tokens;
        				else
            					fname = tokens;
        				c++;
        				tokens = strtok(NULL, " \n");
    				}
				if(strcmp(cmd,"put")==0)
				{
					read(sd,&p,sizeof(int));
					if(p==0)
					{
						fd1 = open(fname, O_CREAT | O_WRONLY | O_TRUNC, 0777);
						read(sd,&fsize,sizeof(int));

						content = (char *)malloc(fsize);
						if(s=read(sd,content,fsize))
						{
							content[s]='\0';
                            write(fd1,content,fsize);
						}
						fprintf(stderr,"Success: Client has uploaded the file %s\n",fname);
						free(content);
					}
				}
				if(strcmp(cmd,"get")==0)
                        	{
                        	      
					fd2 = open(fname,O_RDONLY);
					 if(fd2==-1)
                                        {
                                                q=1;
                                                write(sd,&q,sizeof(int));
                                                q=0;
                                        }
                                        else
                                        {
                                        	write(sd,&q,sizeof(int));
						fsize = lseek(fd2,0L, SEEK_END);
						lseek(fd2,0L, SEEK_SET);
						content = (char *)malloc(fsize);
						read(fd2,content,fsize);
						write(sd,&fsize,sizeof(int));
						write(sd,content,fsize);
						fprintf(stderr,"Success: Client has downloaded the file %s\n",fname);
						free(content);
					}
				}
			}
	   	}
	}
}

