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
#define MAXLINE 1024
#define MAXHDR 2048

/* You won't lose style points for including these long lines in your code */
static const char *user_agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *acceptHdr = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static const char *accept_encoding = "Accept-Encoding: gzip, deflate\r\n";
static const char *connction = "Connection: close\r\n";
static const char *pconnection = "Proxy-Connection: close\r\n";


int main(int argc,char** argv){
    
    int listenfd, connfd, portSelf, clientlen;
    
    char reqBuf[MAX_REQUEST_SIZE];
    //char response[MAX_OBJECT_SIZE];
    
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp; //host addr pointer a string

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
      
      printf("now parsing request\n");
      
      //allocate mem for the reqest and the header
      char* reqheader = malloc(MAXHDR);
      char* newReq = malloc(MAXLINE);
      char* host = malloc(MAXLINE);
      char* response = malloc(MAX_OBJECT_SIZE);

      //parse the inputs and put it in the allocated space
      parseReq(connfd,newReq,reqheader,host);
      
      //make new request
      makeReq(host,newReq,reqheader,response);
      
      //return the content to the client
      Rio_writen(connfd,response, strlen(reqponse));

      Close(connfd);
    }
    //forward request to server*
    //get result and forward to client
    //cache result*
    return 0;
}

void parseReq (int connfd,newReq,reqheader,host) {
    //int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    char oldhost[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio,connfd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, url, version);
    
    //takes care of the header and returns the host header
    //from the request 
    oldHost = read_requesthdrs(&rio,reqheader);

    //write the new request
    if *oldHost != 

    return;

}

/* This function reads the header and returns to parseReq
 * and determine what should be ignored and what should be forwarded
 */
char* read_requesthdrs(rio_t *rp){
    char buf[MAXHDR];
    
    Rio_readlineb(rp, buf, MAXHDR);
    while(strcmp(buf, "\r\n")) {
        Rio_readlineb(rp, buf, MAXHDR);
    }
    return buf;
}


/* This function makes new request to the designated server
 * and retrieve the desired data, put the data in response
 * reqheader need to have the \r\n end response char sequence
 * detects how big the response is and write \0 at the end
 */

void makeReq(host,newReq,reqheader,response){
    
    int clientfd, port;
    char *host;
    rio_t rio;

    port = atoi("80");
    
    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    //write the request
    Rio_writen(clientfd, newReq, strlen(newReq));
    Rio_writen(clientfd, reqheader, strlen(reqheader));
    
    Rio_readlineb(&rio, response, MAX_OBJECT_SIZE);

    Close(clientfd);
}



