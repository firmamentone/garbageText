#include "RPI.h"
#include <stdio.h>
#include <time.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr

void DelayMicrosecondsNoSleep (int);
int distanceUltra(void);
int sendLevel(int);
int gpiobitset(int);
int gpiobitclr(int);
 
int main()
{
#if 0	/* ultra sonic sample */
  int rec;
  int entwhi;
  int echocnt=0;
  
  if(map_peripheral(&gpio) == -1) 
  {
    printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
    return -1;
  }
 
 
  INP_GPIO(4);
  
  INP_GPIO(3);
  OUT_GPIO(3);

  printf("INP 4\n");



	gpiobitclr(3);
	DelayMicrosecondsNoSleep(10);
	gpiobitset(3);
	DelayMicrosecondsNoSleep(10);
	gpiobitclr(3);

    
	
	echocnt=0;
	entwhi=1;
	while(entwhi)
	{
		
		entwhi++;
		
		rec=GPIO_READ(4);

		if(0<rec)
		{
			echocnt++;
			
		}
		else
		{
			if((0!=echocnt)||(6000<entwhi))
			{
				entwhi=0;
			}
		
			
		}
		DelayMicrosecondsNoSleep(100);
		

		
	}
	
	printf("echocnt=%d\n",echocnt);
	

	unmap_peripheral(&gpio);

#endif	/* ultra sonic sample */
	
  
	if(map_peripheral(&gpio) == -1) 
	{
		printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
		return -1;
	}
 
 
	INP_GPIO(4);
  
	INP_GPIO(3);
	OUT_GPIO(3);

	printf("INP 4\n");
	
	while(1)
	{
		int level=0;
		int keyFllow1;
		printf("press 1 to continue or 2 to exit");
		scanf("%d",&keyFllow1);
	
		switch(keyFllow1)
		{
			case 1:
			{
				level=distanceUltra();
				printf("%d\n",level);
				
				if(20>level)
				{
					level=3;
				}
				else if((20<=level)&&(30>=level))
				{
					level=2;
				}
				else if((30<level)&&(40>=level))
				{
					level=1;
				}
				else if(40<level)
				{
					level=0;
				}
				
				sendLevel(level);
			}break;
			default:
			{
				unmap_peripheral(&gpio);
				return 0;
			
			}
	
		}
	
	}
  return 0; 
}

int distanceUltra(void)
{
	int rec;
	int entwhi;
	int echocnt=0;
	
	gpiobitclr(3);
	DelayMicrosecondsNoSleep(10);
	gpiobitset(3);
	DelayMicrosecondsNoSleep(10);
	gpiobitclr(3);
	
	
	echocnt=0;
	entwhi=1;
	while(entwhi)
	{
		
		entwhi++;
		
		rec=GPIO_READ(4);

		if(0<rec)
		{
			echocnt++;
			
		}
		else
		{
			if((0!=echocnt)||(6000<entwhi))
			{
				entwhi=0;
			}
		
			
		}
		DelayMicrosecondsNoSleep(100);
	
	}
	
	return echocnt;
}

int sendLevel(int level)
{
	int socket_desc;
    struct sockaddr_in server;
    char *message;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
         
    server.sin_addr.s_addr = inet_addr("114.34.21.85");
    server.sin_family = AF_INET;
    server.sin_port = htons( 5000 );
 
    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
        return 1;
    }
     
    puts("Connected\n");
     
    //Send some data
	switch(level)
	{
		case 0:
		{
			message = "GET /garbageText/edit_update?statusPHP=0 HTTP/1.0\n\n";
		}
		break;
		case 1:
		{
			message = "GET /garbageText/edit_update?statusPHP=1 HTTP/1.0\n\n";
		}
		break;
		case 2:
		{
			message = "GET /garbageText/edit_update?statusPHP=2 HTTP/1.0\n\n";
		}
		break;
		case 3:
		{
			message = "GET /garbageText/edit_update?statusPHP=3 HTTP/1.0\n\n";
		}
		break;
	}
    if( send(socket_desc , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return 1;
    }
    puts("Data Send\n");
    close(socket_desc); 
    return 0;

}

int gpiobitset(int bitnumber)
{	
	*(gpio.addr + 7)=(1<<bitnumber);
	return 0;
}

int gpiobitclr(int bitnumber)
{	
	*(gpio.addr + 10)=(1<<bitnumber);

	return 0;
}

//Using delayMicroseconds lets the linux scheduler decide to jump to another process.  Using this function avoids letting the
//scheduler know we are pausing and provides much faster operation if you are needing to use lots of delays.
void DelayMicrosecondsNoSleep (int delay_us)
{
	long int start_time;
	long int time_difference;
	struct timespec gettime_now;

	clock_gettime(CLOCK_REALTIME, &gettime_now);
	start_time = gettime_now.tv_nsec;		//Get nS value
	while (1)
	{
		clock_gettime(CLOCK_REALTIME, &gettime_now);
		time_difference = gettime_now.tv_nsec - start_time;
		if (time_difference < 0)
			time_difference += 1000000000;				//(Rolls over every 1 second)
		if (time_difference > (delay_us * 1000))		//Delay for # nS
			break;
	}
}
