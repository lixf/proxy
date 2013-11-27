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
#define MAXHDR 2048

/* You won't lose style points for including these long lines in your code */
static const char *user_agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *acceptHdr = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static const char *accept_encoding = "Accept-Encoding: gzip, deflate\r\n";
static const char *connction = "Connection: close\r\n";
static const char *pconnection = "Proxy-Connection: close\r\n";


void parseReq (int connfd,char* newReq,char* reqhdr,char* host);
char* read_reqhdrs(rio_t *rp,char* reqhdr,char* host);
char* geturlHost(char* url, char* host);
void makeReq(char* host,char* newReq,char* reqheader,char* response);


int main(int argc,char** argv){
    
    int listenfd, connfd, portSelf, clientlen;
    //char reqBuf[MAXLINE];
    //char response[MAX_OBJECT_SIZE];
    
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp; //host addr pointer a string


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
        printf("proxy connected to %s (%s)\n", hp->h_name, haddrp);
        
        printf("now parsing request\n");
        
        //allocate mem for the reqest and the header
        char* reqhdr = (char*) malloc(MAXHDR+1);
        char* newReq = (char*) malloc(MAXLINE+1);
        char* host = (char*) malloc(MAXLINE+1);
        char* response = (char*) malloc(MAX_OBJECT_SIZE+1);
        
        reqhdr = "";
        newReq = "";
        host = "";
        response = "";
        //parse the inputs and put it in the allocated space
        parseReq(connfd,newReq,reqhdr,host);
        
        //make new request
        makeReq(host,newReq,reqhdr,response);
        
        //return the content to the client
        Rio_writen(connfd,response, strlen(response));
        
        printf("finsihed wirting\n");
        Close(connfd);
    }
    //forward request to server*
    //get result and forward to client
    //cache result*
    return 0;
}

void parseReq (int connfd,char* newReq,char* reqhdr,char* host) {
    //int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    char* oldHost;
    rio_t rio;

    Rio_readinitb(&rio,connfd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, url, version);

    //takes care of the header and returns the host header
    //from the request 
    oldHost = read_reqhdrs(&rio,reqhdr,host);

    //write the new request
    if (strcmp(oldHost,"")){
        //if can't find host in hdr and the url, return error msg
        oldHost = geturlHost(url,host);
        if(strcmp(oldHost,"")){
            char* retErr = "bad request no host\n";
            Rio_writen(&rio,retErr,strlen(retErr));
            exit(1);
        }
    }

    return;

}

/* This function reads the header and returns to parseReq
 * and determine what should be ignored and what should be forwarded
 */
char* read_reqhdrs(rio_t *rp,char* reqhdr,char* host){
    //first read in all the headers
    Rio_readlineb(rp, reqhdr, MAXHDR);
    while(strcmp(reqhdr, "\r\n")) {
        Rio_readlineb(rp, reqhdr, MAXHDR);
        printf("%s", reqhdr);
    }
   
    host = strstr(reqhdr,"host:");
    //it contains a null char
    host += 5;

    return host;
}

char* geturlHost(char* url, char* host){
    char* end = strstr(url,"/");
    host = end;
    return host;

}


/* This function makes new request to the designated server
 * and retrieve the desired data, put the data in response
 * reqheader need to have the \r\n end response char sequence
 * detects how big the response is and write \0 at the end
 */

void makeReq(char* host,char* newReq,char* reqheader,char* response){
    
    int clientfd, port;
    rio_t rio;
    const char* endStr = "\r\n"; 

    port = atoi("80");
    
    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    //write the request
    Rio_writen(clientfd, newReq, strlen(newReq));
    Rio_writen(clientfd, reqheader, strlen(reqheader));
    Rio_writen(clientfd,endStr,strlen(endStr));
    
    Rio_readlineb(&rio, response, MAX_OBJECT_SIZE);
    
    printf("response: %s\n",response);

    Close(clientfd);
}



