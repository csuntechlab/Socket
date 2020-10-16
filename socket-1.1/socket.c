/*

$Header: socket.c[1.17] Wed Sep  9 16:23:14 1992 nickel@cs.tu-berlin.de proposed $
This file is part of socket(1).
Copyright (C) 1992 by Juergen Nickelsen <nickel@cs.tu-berlin.de>
Please read the file COPYRIGHT for further details.

*/

#ifdef __linux__
#include <unistd.h>
#endif 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#ifdef SEQUENT
#include <strings.h>
#else
#include <string.h>
#endif
#include "globals.h"
#include <stdint.h> // debfix
#include <stdlib.h>

/* global variables */
int forkflag = 0 ;		/* server forks on connection */
int serverflag = 0 ;		/* create server socket */
int loopflag = 0 ;		/* loop server */
int verboseflag = 0 ;		/* give messages */
int readonlyflag = 0 ;		/* only read from socket */
int writeonlyflag = 0 ;		/* only write to socket */
int quitflag = 0 ;		/* quit connection on EOF */
int crlfflag = 0 ;		/* socket expects and delivers CRLF */
int backgflag = 0 ;		/* put yourself in background */
int active_socket ;		/* socket with connection */
char *progname ;		/* name of the game */
char *pipe_program = NULL ;	/* program to execute in two-way pipe */
int WaitForever = 0;            /* wait forever for socket on connection refused */
long int local_ip=0;           /* IP to bind at local */

void server A((long int local_ip,char *path, int port, char *service_name)) ;
void handle_server_connection A((void)) ;
void client A((long int local_ip,char *host, int port, char *service_name)) ;

int main(argc, argv)
int argc ;
char **argv ;
{
    char *cp ;			/* to point to '/' in argv[0] */
    int opt ;			/* option character */
    int error = 0 ;		/* usage error occurred */
    extern int optind ;		/* from getopt() */
/*    char *host ;  */		/* name of remote host */
    int port ;			/* port number for socket */
    char *service_name ;	/* name of service for port */
    char *path;			/* path to unix domain socket */

    /* print version ID if requested */
    if (argv[1] && !strcmp(argv[1], "-version")) {
	puts(so_release()) ;
	exit(0) ;
    }

    /* set up progname for later use */
    progname = argv[0] ;
    if ((cp = strrchr(progname, '/'))) progname = cp + 1 ;

    /* parse options */
    while ((opt = getopt(argc, argv, "B:bcflp:qrsvWw?")) != -1) {
	switch (opt) {
	  case 'f':
	    forkflag = 1 ;
	    break ;
	  case 'c':
	    crlfflag = 1 ;
	    break ;
          case 'B': 
            {
              struct hostent *dataip;
              char *cad;
              cad=argv[optind-1];
              dataip=gethostbyname(cad);
              if (dataip!=NULL) {
                local_ip=*((long int*)dataip->h_addr_list[0]);
              }
            }
            break;
	  case 'w':
	    writeonlyflag = 1 ;
	    break ;
	  case 'p':
	    pipe_program = argv[optind - 1] ;
	    break ;
	  case 'q':
	    quitflag = 1 ;
	    break ;
	  case 'r':
	    readonlyflag = 1 ;
	    break ;
	  case 's':
	    serverflag = 1 ;
	    break ;
	  case 'v':
	    verboseflag = 1 ;
	    break ;
	  case 'l':
	    loopflag = 1 ;
	    break ;
	  case 'b':
	    backgflag = 1 ;
	    break ;
	  case 'W':
	    WaitForever = 1 ;
	  default:
	    error++ ;
	}
    }
    if ( error || optind >= argc )
     {	usage();
	exit(15);
     }
    if ( strchr(argv[optind],'/') != NULL )
	path=argv[optind];
    else
	path=NULL;

    /* number of args ok? */
    if ( ( path == NULL && argc - optind + serverflag != 2 ) ||
    ( path != NULL && argc - optind != 1 ) )
     {	usage() ;
	exit(15) ;
     }

    /* check some option combinations */
#define senseless(s1, s2) \
    fprintf(stderr, "It does not make sense to set %s and %s.\n", (s1), (s2))

    if (writeonlyflag && readonlyflag) {
	senseless("-r", "-w") ;
	exit(15) ;
    }
    if (loopflag && !serverflag) {
	senseless("-l", "not -s") ;
	exit(15) ;
    }
    if (backgflag && !serverflag) {
 	senseless("-b", "not -s") ;
 	exit(15) ;
    }
    if (forkflag && !serverflag) {
	senseless("-f", "not -s") ;
    }
 
    /* set up signal handling */
    init_signals() ;

    /* get port number */
    if ( path==NULL )
     {    port = resolve_service(argv[optind + 1 - serverflag],
			   "tcp", &service_name) ;
	 if (port < 0) {
		fprintf(stderr, "%s: unknown service\n", progname) ;
		exit(5) ;
	    }
     }
    else
	port=-1;	/* no port for UNIX sockets */

    /* and go */
    if (serverflag) {
	if (backgflag) {
	    background() ;
	}
	server(local_ip,path,port,service_name) ;
    } else if ( path != NULL )
	client(local_ip,path, -1, service_name) ;
    else
    	client(local_ip,argv[optind], port, service_name) ;
    exit(0) ;
}


void server(local_ip,path,port, service_name)
long int local_ip ;
char *path;
int port ;
char *service_name ;
{
    int socket_handle, alen ;

    /* allocate server socket */
    if ( path != NULL )
	    socket_handle = create_server_socket_unix(path,1);
    else
	    socket_handle = create_server_socket_inet(local_ip,port, 1) ;
    if (socket_handle < 0) {
	perror2("server socket") ;
	exit(1) ;
    }
    if (verboseflag) {
	if ( path==NULL )
	 {	fprintf(stderr, "inet: listening on port %d", port) ;
		if (service_name) {
		    fprintf(stderr, " (%s)", service_name) ;
		}
		fprintf(stderr, "\n") ;
	 }
	else
		fprintf(stderr, "unix: bound to %s\n",path);
    }

    /* server loop */
    do {
	union 
	 {	struct sockaddr_in sa ;
		struct sockaddr_un su ;
	 } peer;
	    
	alen = sizeof(peer) ;

	/* accept a connection */
	if ((active_socket = accept(socket_handle,
			  (struct sockaddr *) &peer,
			  &alen)) == -1) {
	    perror2("accept") ;
	} else {
	    /* if verbose, get name of peer and give message */
	    if (verboseflag)
	     {
		if ( peer.sa.sin_family==AF_INET )
	 	 {	struct hostent *he ;
			// debfix long norder ;
			uint32_t norder;
			char dotted[20] ;

			he = gethostbyaddr((char *) &peer.sa.sin_addr.s_addr,
				   sizeof(peer.sa.sin_addr.s_addr), AF_INET) ;
			if (!he) {
			    norder = htonl(peer.sa.sin_addr.s_addr) ;
			    sprintf(dotted, "%ld.%ld.%ld.%ld",
				    (norder >> 24) & 0xff,
				    (norder >> 16) & 0xff,
				    (norder >>  8) & 0xff,
				    norder & 0xff) ;
			}
			fprintf(stderr, "inet: connection from %s\n",
				(he ? he->h_name : dotted)) ;
		 }
		else if ( peer.su.sun_family==AF_UNIX )
			fprintf(stderr,"unix: connection from %s\n",path);
	     }
	    if (forkflag) {
		switch (fork()) {
		  case 0:
		    handle_server_connection() ;
		    exit(0) ;
		  case -1:
		    perror2("fork") ;
		    break ;
		  default:
		    close(active_socket) ;
		    wait_for_children() ;
		}
	    } else {
		handle_server_connection() ;
	    }
	}
    } while (loopflag) ;

   close(socket_handle);
   if ( path != NULL )
	unlink(path);
	
}


void handle_server_connection()
{
    /* open pipes to program, if requested */
    if (pipe_program != NULL) {
	open_pipes(pipe_program) ;
    }
    /* enter IO loop */
    do_io() ;
    /* connection is closed now */
    close(active_socket) ;
    if (pipe_program) {
	/* remove zombies */
	wait_for_children() ;
    }
}


void client(local_ip,host, port, service_name)
long int local_ip;
char *host ;
int port ;
char *service_name ;
{
    int	is_unix_socket;

    /* get connection */
    if ( port>=0 )
     {    while (1) {
		active_socket = create_client_socket_inet(local_ip,&host, port) ;
		if (active_socket >= 0 || !WaitForever || errno != ECONNREFUSED) 
			break;
		sleep( 60 );
	    }
     }
    else
	   active_socket = create_client_socket_unix(host);	

    if (active_socket == -1) {
	perror2("client socket") ;
	exit(errno) ;
    } else if (active_socket == -2) {
	fprintf(stderr, "%s: unknown host %s\n", progname, host) ;
	exit(13) ;
    }
    if (verboseflag) {
	if ( port>=0 )
	 {	fprintf(stderr, "inet: connected to %s port %d", host, port) ;
		if (service_name) {
		    fprintf(stderr, " (%s)", service_name) ;
		}
		fprintf(stderr, "\n") ;
	 }
	else
		fprintf(stderr, "unix: connected to %s\n", host);
    }

    /* open pipes to program if requested */
    if (pipe_program != NULL) {
	open_pipes(pipe_program) ;
    }
    /* enter IO loop */
    do_io() ;
    /* connection is closed now */
    close(active_socket) ;
}

/*EOF*/
