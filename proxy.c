/* 
 * Xiaofan Li
 * Nov. 21st 
 *
 * 18213 f13
 * Proxy Lab
 */


#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_REQUEST_SIZE 1024

/* You won't lose style points for including these long lines in your code */
static const char *user_agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *acceptHdr = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static const char *accept_encoding = "Accept-Encoding: gzip, deflate\r\n";

int main(int argc,char** argv){
    
    int listenfd, connfd, portSelf, clientlen;
    //int clientfd, portHost;
    //char* host;
    
    char reqBuf[MAX_REQUEST_SIZE];
    //char response[MAX_OBJECT_SIZE];
    
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;

    rio_t rio;

    printf("%s%s%s", user_agent, acceptHdr, accept_encoding);
    
    //set up server side
    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portSelf = atoi(argv[1]);

    //set up listenfd with wrapper function
    listenfd = Open_listenfd(portSelf);
    
    //prefork threads
    //check cache
    while (1) {
      //wait for connection
      clientlen = sizeof(clientaddr);
      connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t*)&clientlen);
      
      /* Determine the domain name and IP address of the client */
      hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
            sizeof(clientaddr.sin_addr.s_addr), AF_INET);
      haddrp = inet_ntoa(clientaddr.sin_addr);
      printf("server connected to %s (%s)\n", hp->h_name, haddrp);
      
      Rio_readinitb(&rio,connfd);


      int n;      
      while((n=Rio_readlineb(&rio, reqBuf, MAX_REQUEST_SIZE))!=0){
        Fputs(reqBuf, stdout);
      }


/*
      //parse request
      parseReq(connfdCli);

      //might be a problem when using threads
      host = "";
      portHost = 0;
      clientfd = Open_clientfd(host,portHost);
      
      sendStuff(clientfd,reqBuff);
      getStuff(clientfd);
      sendStuff(connfd,response);
*/
      Close(connfd);
    }
    //forward request to server*
    //get result and forward to client
    //cache result*
    return 0;
}



