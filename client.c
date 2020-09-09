#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


int main(int argc, char *argv[])
{
  	char msg[255];
	char *fname;
	char *cmd;
	char *content;
  	int server, portNumber, pid;
  	int fsize,p=0,q=0,n,s,c,size,f1,f2;
  	struct sockaddr_in servAdd;     

 	if(argc != 3)
	{
    		printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
    		exit(0);
  	}

  	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
     		fprintf(stderr, "Cannot create socket\n");
     		exit(1);
  	}

  	servAdd.sin_family = AF_INET;
  	sscanf(argv[2], "%d", &portNumber);
  	servAdd.sin_port = htons((uint16_t)portNumber);

  	if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0)
	{
  		fprintf(stderr, " inet_pton() has failed\n");
  		exit(2);
	}

 	if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0)
	{
  		fprintf(stderr, "connect() has failed, exiting\n");
  		exit(3);
 	}
	printf("Established connection with Server!!\n");
  	read(server, msg, sizeof(msg));
  	fprintf(stderr, "%s\n", msg);

	while(1)
      	{
		if(n=read(0, msg, 255))
		{
        		msg[n]='\0';
         		write(server, msg, strlen(msg)+1);
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
					f1=open(fname, O_RDONLY);
					if(f1==-1)
					{
						fprintf(stderr,"Given file doesn't exist in client\n");
						p=1;
						write(server,&p,sizeof(int));
						p=0;
					}
					else
					{
						write(server,&p,sizeof(int));
						fsize = lseek(f1, 0L, SEEK_END);
						lseek(f1, 0L, SEEK_SET);
						content=(char *) malloc(fsize);
						read(f1,content,fsize);
						write(server,&fsize,sizeof(int));
						write(server,content,fsize);
						fprintf(stderr,"File Uploaded\n");
						free(content);
					}
				}
				if(strcmp(cmd,"get")==0)
                        	{
					read(server,&q,sizeof(int));
					if(q==0)
					{
						f2 = open(fname, O_CREAT | O_WRONLY | O_TRUNC, 0777);
						read(server,&fsize,sizeof(int));
						content = (char *)malloc(fsize);
						if(s=read(server,content,fsize))
						{
							content[s]='\0';
							write(f2,content,fsize);
						}
						fprintf(stderr,"File Downloaded\n");
						free(content);
					}
					else
					{
						fprintf(stderr,"Given file doesn't exist in server\n");
					}
				}
			}
      		}
	}
}

