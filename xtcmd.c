/* 
xtcmd / Scopia XT API command-line client
2017/2/11 yosuke.sawamura@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MSG_SIZE 255
#define XTPORT 55003

void print_hex_memory(void *mem, void *data)    // Read buff and print out in HEX
{
    int i;
    printf("Sending data:\n");
    size_t mem_length = sizeof(mem)+strlen(data)-2;
    unsigned char *p = (unsigned char *)mem;
    for (i=0; i<mem_length; i++) {
        printf("0x%02x ", p[i]);
        if (i%16==0)
            printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
   int i=0;
   
   int port;
   int num_clients = 0;
   int client_sockfd;
   struct sockaddr_in server_address;
   int addresslen = sizeof(struct sockaddr_in);
   int fd;
   fd_set readfds, testfds, clientfds;
   char msg[MSG_SIZE + 1];
   char input_cmd[256];
   
   /*Client variables =========================================*/
   int sockfd;
   int result;
   char hostname[MSG_SIZE];
   struct hostent *hostinfo;
   struct sockaddr_in address;
   char alias[MSG_SIZE];
   int clientid;
   int length;
   int debug = 0;

   /*Client ==================================================*/
   if(argc==2 || argc==3){
     if(argv[2]){
        if(!strcmp("-d",argv[2])){
           debug = 1;
        }
     }else{
           debug = 0;
     }

     port=XTPORT;
     strcpy(hostname,argv[1]);
       
     printf("*** session starting (enter \"quit\" to stop): \n");
     fflush(stdout);
     
     /* Create a socket for the client */
     sockfd = socket(AF_INET, SOCK_STREAM, 0);

     /* Name the socket, as agreed with the server */
     hostinfo = gethostbyname(hostname);  /* look for host's name */
     address.sin_addr = *(struct in_addr *)*hostinfo -> h_addr_list;
     address.sin_family = AF_INET;
     address.sin_port = htons(port);

     /* Connect the socket to the server's socket */
     if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0){
       perror("connecting");
       exit(1);
     }
     
     fflush(stdout);
     
     FD_ZERO(&clientfds);
     FD_SET(sockfd,&clientfds);
     FD_SET(0,&clientfds);//stdin
     
     /*  Now wait for messages from the endpoint */
     while (1) {
       testfds=clientfds;
       select(FD_SETSIZE,&testfds,NULL,NULL,NULL);
       
       for(fd=0;fd<FD_SETSIZE;fd++){
          if(FD_ISSET(fd,&testfds)){
             if(fd==sockfd){   /* Accept data from open socket */
                
                //read data from open socket
                result = read(sockfd,msg,MSG_SIZE);

                int n = sizeof(msg)-6;
                printf("%.*s\n", n, msg + sizeof(msg)-n);
                
                 if(debug==1){
                     int i;
                     size_t mem_length = sizeof(msg);
                     unsigned char *p = (unsigned char *)msg;
                     for (i=0; i<mem_length; i++) {
                         printf("0x%02x ", p[i]);
                         if (i%16==0)
                             printf("\n");
                     }
                     printf("\n");
                 }
                 
             }
             else if(fd == 0){ /*process input activity*/
                
                fgets(input_cmd,255,stdin);
                 
                if (strcmp(input_cmd, "quit\n")==0) {
                    printf("terminating session\n");
                    close(sockfd);  //close the current socket
                    exit(0);        //end program
                }
                else {

                    char ATHeader[2];
                    ATHeader[0]= 0xAA;
                    ATHeader[1]= 0xAA;
                    
                    char ATNumber[4];
                    ATNumber[0]= 0x00;
                    ATNumber[1]= 0x00;
                    ATNumber[2]= 0x00;
                    ATNumber[3]= strlen(input_cmd);
                    
                    char ATFooter[1];
                    ATFooter[0]= 0x0D;
                    
                    size_t len = strlen(input_cmd);
                    
                    char *buff = malloc(6 + len);           // ex ATHeader(2) + ATNumber(4) + len = 6+len
                    memcpy(buff, ATHeader, 2);              // adding first 2 bytes (ATHeader)
                    memcpy(buff + 2, ATNumber, 6);          // appending next 4 bytes (ATNumber)
                    memcpy(buff + 6, input_cmd, 6 + len-1); // appeding the rest of the buffer to buff
                    memcpy(buff + 6 + len-1, ATFooter, 1);  // adding <CR> (0x0d) at the end (ATFooter)

                    if(debug==1){
                    print_hex_memory(buff, input_cmd);
                    }
                    
                    write(sockfd, buff, sizeof(buff) + strlen(input_cmd)-2);
                }
             }          
          }
       }      
     }
   } else {
       printf("Usage: xtcmd HOSTNAME [-d (debug mode)]\n");
       exit(0);
   } // end client code
   
}//main
