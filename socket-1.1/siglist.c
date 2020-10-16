/* siglist.c -- signal list for those machines that don't have one. */

/* Copyright (C) 1989 Free Software Foundation, Inc.

This file is part of GNU Bash, the Bourne Again SHell.

Modified by Juergen Nickelsen <nickel@cs.tu-berlin.de> for use with
Socket-1.1.

Bash is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 1, or (at your option) any later
version.

Bash is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with Bash; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#if !defined (NSIG)
#  if defined (_NSIG)
#    define NSIG _NSIG
#  else
#    define NSIG 64
#  endif /* !_NSIG */
#endif /* !NSIG */

char *socket_siglist[NSIG];

/* extern *malloc (); */	/* see stdlib.h and/or malloc.h, by apal@szofi */

void initialize_siglist ()
{
  register int i;

  for (i = 0; i < NSIG; i++)
    socket_siglist[i] = (char *)0x0;

  socket_siglist[0] = "Bogus signal";

#if defined (SIGHUP)
  socket_siglist[SIGHUP] = "Hangup signal";
#endif

#if defined (SIGINT)
  socket_siglist[SIGINT] = "Interrupt";
#endif

#if defined (SIGQUIT)
  socket_siglist[SIGQUIT] = "Quit signal";
#endif

#if defined (SIGILL)
  socket_siglist[SIGILL] = "Illegal instruction";
#endif

#if defined (SIGTRAP)
  socket_siglist[SIGTRAP] = "BPT trace/trap";
#endif

#if defined (SIGIOT) && !defined (SIGABRT)
#define SIGABRT SIGIOT
#endif

#if defined (SIGABRT)
  socket_siglist[SIGABRT] = "ABORT instruction";
#endif

#if defined (SIGEMT)
  socket_siglist[SIGEMT] = "EMT instruction";
#endif

#if defined (SIGFPE)
  socket_siglist[SIGFPE] = "Floating point exception";
#endif

#if defined (SIGKILL)
  socket_siglist[SIGKILL] = "Kill signal";
#endif

#if defined (SIGBUS)
  socket_siglist[SIGBUS] = "Bus error";
#endif

#if defined (SIGSEGV)
  socket_siglist[SIGSEGV] = "Segmentation fault";
#endif

#if defined (SIGSYS)
  socket_siglist[SIGSYS] = "Bad system call";
#endif

#if defined (SIGPIPE)
  socket_siglist[SIGPIPE] = "Broken pipe condition";
#endif

#if defined (SIGALRM)
  socket_siglist[SIGALRM] = "Alarm clock signal";
#endif

#if defined (SIGTERM)
  socket_siglist[SIGTERM] = "Termination signal";
#endif

#if defined (SIGURG)
  socket_siglist[SIGURG] = "Urgent IO condition";
#endif

#if defined (SIGSTOP)
  socket_siglist[SIGSTOP] = "Stop signal";
#endif

#if defined (SIGTSTP)
  socket_siglist[SIGTSTP] = "Stopped";
#endif

#if defined (SIGCONT)
  socket_siglist[SIGCONT] = "Continue signal";
#endif

#if !defined (SIGCHLD) && defined (SIGCLD)
#define SIGCHLD SIGCLD
#endif

#if defined (SIGCHLD)
  socket_siglist[SIGCHLD] = "Child signal";
#endif

#if defined (SIGTTIN)
  socket_siglist[SIGTTIN] = "Stop (tty input) signal";
#endif

#if defined (SIGTTOU)
  socket_siglist[SIGTTOU] = "Stop (tty output) signal";
#endif

#if defined (SIGIO)
  socket_siglist[SIGIO] = "I/O ready signal";
#endif

#if defined (SIGXCPU)
  socket_siglist[SIGXCPU] = "CPU limit exceeded";
#endif

#if defined (SIGXFSZ)
  socket_siglist[SIGXFSZ] = "File limit exceeded";
#endif

#if defined (SIGVTALRM)
  socket_siglist[SIGVTALRM] = "Alarm (virtual)";
#endif

#if defined (SIGPROF)
  socket_siglist[SIGPROF] = "Alarm (profile)";
#endif

#if defined (SIGWINCH)
  socket_siglist[SIGWINCH] = "Window change";
#endif

#if defined (SIGLOST)
  socket_siglist[SIGLOST] = "Record lock signal";
#endif

#if defined (SIGUSR1)
  socket_siglist[SIGUSR1] = "User signal 1";
#endif

#if defined (SIGUSR2)
  socket_siglist[SIGUSR2] = "User signal 2";
#endif

#if defined (SIGMSG)
  socket_siglist[SIGMSG] = "HFT input data pending signal";
#endif 

#if defined (SIGPWR)
  socket_siglist[SIGPWR] = "power failure imminent signal";
#endif 

#if defined (SIGDANGER)
  socket_siglist[SIGDANGER] = "system crash imminent signal";
#endif 

#if defined (SIGMIGRATE)
  socket_siglist[SIGMIGRATE] = "Process migration";
#endif 

#if defined (SIGPRE)
  socket_siglist[SIGPRE] = "Programming error signal";
#endif 

#if defined (SIGGRANT)
  socket_siglist[SIGGRANT] = "HFT monitor mode granted signal";
#endif 

#if defined (SIGRETRACT)
  socket_siglist[SIGRETRACT] = "HFT monitor mode retracted signal";
#endif 

#if defined (SIGSOUND)
  socket_siglist[SIGSOUND] = "HFT sound sequence has completed signal";
#endif 

  for (i = 0; i < NSIG; i++)
    {
      if (!socket_siglist[i])
	{
	  socket_siglist[i] =
	    (char *) malloc (10 + strlen ("Unknown Signal #"));

	  sprintf (socket_siglist[i], "Unknown Signal #%d", i);
	}
    }
}
