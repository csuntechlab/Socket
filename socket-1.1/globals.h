/*
  
$Header: globals.h[1.10] Sun Aug 30 21:13:41 1992 nickel@cs.tu-berlin.de proposed $
This file is part of socket(1).
Copyright (C) 1992 by Juergen Nickelsen <nickel@cs.tu-berlin.de>
Please read the file COPYRIGHT for further details.

*/

#include "patchlevel.h"
#include <errno.h>

/* globals for socket */

#define IN	0		/* standard input */
#define OUT	1		/* standard output */

#define LLEN	100		/* buffer size fo perror2() */

#ifdef __STDC__
#define A(args) args
#else
#define A(args) ()
#endif

int create_server_socket_inet A((long int local_ip, int port, int queue_length)) ;
int create_server_socket_unix A((char *pathname,int queue_length)) ;
int create_client_socket_inet A((long int local_ip, char **hostname, int port)) ;
int create_client_socket_unix A((char *pathname)) ;
int resolve_service A((char *name_or_number, char *protocol, char **name)) ;
void catchsig A((int sig)) ;
void usage A((void)) ;
int do_read_write A((int from, int to)) ;
int do_write A((char *buffer, int size, int to)) ;
void do_io A((void)) ;
char *so_release A((void)) ;
void open_pipes A((char *prog)) ;
void wait_for_children A((void)) ;
void perror2 A((char *s)) ;
int is_number A((char *s)) ;
void init_signals A((void)) ;
void add_crs A((char *from, char *to, int *sizep)) ;
void strip_crs A((char *from, char *to, int *sizep)) ;
void background A((void)) ;
void initialize_siglist A((void)) ;

//extern int errno ;

/* global variables */
extern int serverflag ;
extern int loopflag ;
extern int verboseflag ;
extern int readonlyflag ;
extern int writeonlyflag ;
extern int quitflag ;
extern int crlfflag ;
extern int active_socket ;
extern char *progname ;
extern char *socket_siglist[] ;
