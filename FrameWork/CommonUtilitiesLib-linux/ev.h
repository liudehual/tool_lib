#ifndef _SYS_EV_H_
#define _SYS_EV_H_

#if !defined(__Win32__) && !defined(__solaris__) && !defined(__sgi__) && !defined(__osf__) && !defined(__hpux__)
    #include <sys/queue.h>
#endif

struct eventreq {
  int      er_type;
#define EV_FD 1    // file descriptor
  int      er_handle;
  void    *er_data;
  int      er_rcnt;
  int      er_wcnt;
  int      er_ecnt;
  int      er_eventbits;
#define EV_RE  1
#define EV_WR  2
#define EV_EX  4
#define EV_RM  8
};

typedef struct eventreq *er_t;

#ifdef _KERNEL

#define EV_RBYTES 0x1
#define EV_WBYTES 0x2
#define EV_RWBYTES (EV_RBYTES|EV_WBYTES)
#define EV_RCLOSED 0x4
#define EV_RCONN  0x8
#define EV_ERRORS  0x10
#define EV_WCLOSED 0x20
#define EV_WCONN   0x40
#define EV_OOBD    0x80
#define EV_OOBM    0x100

struct eventqelt {
  TAILQ_ENTRY(eventqelt)  ee_slist;
  TAILQ_ENTRY(eventqelt)  ee_plist;
  struct eventreq  ee_req;
  struct proc *    ee_proc;
  u_int            ee_flags;
#define EV_QUEUED 1
  u_int            ee_eventmask;
  struct socket   *ee_sp;
};

#endif /* _KERNEL */

#if !MACOSXEVENTQUEUE

int select_watchevent(struct eventreq *req, int which);
int select_modwatch(struct eventreq *req, int which);
int select_waitevent(struct eventreq *req, void* onlyForMOSX);
void select_startevents();
int select_removeevent(int which);

#endif

#endif /* _SYS_EV_H_ */
