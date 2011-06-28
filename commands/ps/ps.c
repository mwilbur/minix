/* ps - print status			Author: Peter Valkenburg */
/* Modified for ProcFS by Alen Stojanov and David van Moolenbroek */

/* Ps.c, Peter Valkenburg (valke@psy.vu.nl), january 1990.
 *
 * This is a V7 ps(1) look-alike for MINIX >= 1.5.0.
 * It does not support the 'k' option (i.e. cannot read memory from core file).
 * If you want to compile this for non-IBM PC architectures, the header files
 * require that you have your CHIP, MACHINE etc. defined.
 * Full syntax:
 *	ps [-][aeflx]
 * Option `a' gives all processes, `l' for detailed info, `x' includes even
 * processes without a terminal.
 * The `f' and `e' options were added by Kees Bot for the convenience of 
 * Solaris users accustomed to these options. The `e' option is equivalent to 
 * `a' and `f' is equivalent to  -l. These do not appear in the usage message.
 *
 * VERY IMPORTANT NOTE:
 *	To compile ps, the kernel/, fs/ and pm/ source directories must be in
 *	../../ relative to the directory where ps is compiled (normally the
 *	tools source directory).
 *
 *	If you want your ps to be useable by anyone, you can arrange the
 *	following access permissions (note the protected memory files and set
 *	*group* id on ps):
 *	-rwxr-sr-x  1 bin   kmem       11916 Jul  4 15:31 /bin/ps
 *	crw-r-----  1 bin   kmem      1,   1 Jan  1  1970 /dev/mem
 *	crw-r-----  1 bin   kmem      1,   2 Jan  1  1970 /dev/kmem
 */

/* Some technical comments on this implementation:
 *
 * Most fields are similar to V7 ps(1), except for CPU, NICE, PRI which are
 * absent, RECV which replaces WCHAN, and PGRP that is an extra.
 * The info is obtained from the following fields of proc, mproc and fproc:
 * ST	- kernel status field, p_rts_flags; pm status field, mp_flags (R if
 *        p_rts_flags is 0; Z if mp_flags == ZOMBIE; T if mp_flags == STOPPED;
 *        else W).
 * UID	- pm eff uid field, mp_effuid
 * PID	- pm pid field, mp_pid
 * PPID	- pm parent process index field, mp_parent (used as index in proc).
 * PGRP - pm process group field, mp_procgrp
 * SZ	- memory size, including common and shared memory
 * RECV	- kernel process index field for message receiving, p_getfrom
 *	  If sleeping, pm's mp_flags, or fs's fp_task are used for more info.
 * TTY	- fs controlling tty device field, fp_tty.
 * TIME	- kernel user + system times fields, user_time + sys_time
 * CMD	- system process index (converted to mnemonic name by using the p_name
 *	  field), or user process argument list (obtained by reading the stack
 *	  frame; the resulting address is used to get the argument vector from
 *	  user space and converted into a concatenated argument list).
 */

#include <minix/config.h>
#include <minix/com.h>
#include <minix/sysinfo.h>
#include <minix/endpoint.h>
#include <paths.h>
#include <limits.h>
#include <timers.h>
#include <sys/types.h>

#include <minix/const.h>
#include <minix/type.h>
#include <minix/ipc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <minix/com.h>
#include <fcntl.h>
#include <a.out.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdio.h>
#include <ttyent.h>

#include <machine/archtypes.h>
#include "../../kernel/const.h"
#include "../../kernel/type.h"
#include "../../kernel/proc.h"

#include "../../servers/pm/mproc.h"
#include "../../servers/pm/const.h"
#include "../../servers/vfs/fproc.h"
#include "../../servers/vfs/const.h"
#include "../../servers/mfs/const.h"


/*----- ps's local stuff below this line ------*/


#define mindev(dev)	(((dev)>>MINOR) & 0377)	/* yield minor device */
#define majdev(dev)	(((dev)>>MAJOR) & 0377)	/* yield major device */

#define	TTY_MAJ		4	/* major device of console */

/* Structure for tty name info. */
typedef struct {
  char tty_name[NAME_MAX + 1];	/* file name in /dev */
  dev_t tty_dev;		/* major/minor pair */
} ttyinfo_t;

ttyinfo_t *ttyinfo;		/* ttyinfo holds actual tty info */
size_t n_ttyinfo;		/* Number of tty info slots */

/* Macro to convert memory offsets to rounded kilo-units */
#define	off_to_k(off)	((unsigned) (((off) + 512) / 1024))


/* Number of tasks and processes and addresses of the main process tables. */
int nr_tasks, nr_procs;		
extern int errno;

/* Process tables of the kernel, MM, and FS. */
struct proc *ps_proc;
struct mproc *ps_mproc;
struct fproc *ps_fproc;

/* Where is INIT? */
int init_proc_nr;
#define low_user init_proc_nr

#define	KMEM_PATH	"/dev/kmem"	/* opened for kernel proc table */
#define	MEM_PATH	"/dev/mem"	/* opened for pm/fs + user processes */

int kmemfd, memfd;		/* file descriptors of [k]mem */

/* Short and long listing formats:
 *
 *   PID TTY  TIME CMD
 * ppppp tttmmm:ss cccccccccc...
 *
 *   F S UID   PID  PPID  PGRP   SZ       RECV TTY  TIME CMD
 * fff s uuu ppppp ppppp ppppp ssss rrrrrrrrrr tttmmm:ss cccccccc...
 */
#define S_HEADER "  PID TTY  TIME CMD\n"
#define S_FORMAT "%5s %3s %s %s\n"
#define L_HEADER "  F S UID   PID  PPID  PGRP     SZ         RECV TTY  TIME CMD\n"
#define L_FORMAT "%3o %c %3d %5s %5d %5d %6d %12s %3s %s %s\n"


struct pstat {			/* structure filled by pstat() */
  dev_t ps_dev;			/* major/minor of controlling tty */
  uid_t ps_ruid;		/* real uid */
  uid_t ps_euid;		/* effective uid */
  pid_t ps_pid;			/* process id */
  pid_t ps_ppid;		/* parent process id */
  int ps_pgrp;			/* process group id */
  int ps_flags;			/* kernel flags */
  int ps_mflags;		/* mm flags */
  int ps_ftask;			/* fs suspend task */
  int ps_blocked_on;		/* what is the process blocked on */
  char ps_state;		/* process state */
  vir_bytes ps_tsize;		/* text size (in bytes) */
  vir_bytes ps_dsize;		/* data size (in bytes) */
  vir_bytes ps_ssize;		/* stack size (in bytes) */
  phys_bytes ps_vtext;		/* virtual text offset */
  phys_bytes ps_vdata;		/* virtual data offset */
  phys_bytes ps_vstack;		/* virtual stack offset */
  phys_bytes ps_text;		/* physical text offset */
  phys_bytes ps_data;		/* physical data offset */
  phys_bytes ps_stack;		/* physical stack offset */
  int ps_recv;			/* process number to receive from */
  time_t ps_utime;		/* accumulated user time */
  time_t ps_stime;		/* accumulated system time */
  char *ps_args;		/* concatenated argument string */
  vir_bytes ps_procargs;	/* initial stack frame from MM */
};

/* Ps_state field values in pstat struct above */
#define	Z_STATE		'Z'	/* Zombie */
#define	W_STATE		'W'	/* Waiting */
#define	S_STATE		'S'	/* Sleeping */
#define	R_STATE		'R'	/* Runnable */
#define	T_STATE		'T'	/* stopped (Trace) */

_PROTOTYPE(void disaster, (int sig ));
_PROTOTYPE(int main, (int argc, char *argv []));
_PROTOTYPE(char *get_args, (struct pstat *bufp ));
_PROTOTYPE(int pstat, (int p_nr, struct pstat *bufp, int Eflag ));
_PROTOTYPE(int addrread, (int fd, phys_clicks base, vir_bytes addr, 
						    char *buf, int nbytes ));
_PROTOTYPE(void usage, (const char *pname ));
_PROTOTYPE(void err, (const char *s ));
_PROTOTYPE(int gettynames, (void));


/*
 * Tname returns mnemonic string for dev_nr. This is "?" for maj/min pairs that
 * are not found.  It uses the ttyinfo array (prepared by gettynames).
 * Tname assumes that the first three letters of the tty's name can be omitted
 * and returns the rest (except for the console, which yields "co").
 */
PRIVATE char *tname(dev_t dev_nr)
{
  unsigned int i;

  if (major(dev_nr) == TTY_MAJOR && minor(dev_nr) == 0) return "co";

  for (i = 0; i < n_ttyinfo && ttyinfo[i].tty_name[0] != '\0'; i++)
	if (ttyinfo[i].tty_dev == dev_nr)
		return ttyinfo[i].tty_name + 3;

  return "?";
}

/* Return canonical task name of task p_nr; overwritten on each call (yucch) */
PRIVATE char *taskname(int p_nr)
{
  int n;
  n = _ENDPOINT_P(p_nr) + nr_tasks;
  if(n < 0 || n >= nr_tasks + nr_procs) {
	return "OUTOFRANGE";
  }
  return ps_proc[n].p_name;
}

/* Prrecv prints the RECV field for process with pstat buffer pointer bufp.
 * This is either "ANY", "taskname", or "(blockreason) taskname".
 */
PRIVATE char *prrecv(struct pstat *bufp)
{
  char *blkstr, *task;		/* reason for blocking and task */
  static char recvstr[20];

  if (bufp->ps_recv == ANY) return "ANY";

  task = taskname(bufp->ps_recv);
  if (bufp->ps_state != S_STATE) return task;

  blkstr = "?";
  if (bufp->ps_recv == PM_PROC_NR) {
	if (bufp->ps_mflags & PAUSED)
		blkstr = "pause";
	else if (bufp->ps_mflags & WAITING)
		blkstr = "wait";
	else if (bufp->ps_mflags & SIGSUSPENDED)
		blkstr = "sigsusp";
  } else if (bufp->ps_recv == FS_PROC_NR) {
	  switch(bufp->ps_blocked_on) {
		  case FP_BLOCKED_ON_PIPE:
			  blkstr = "pipe";
			  break;
		  case FP_BLOCKED_ON_POPEN:
			  blkstr = "popen";
			  break;
		  case FP_BLOCKED_ON_DOPEN:
			  blkstr = "dopen";
			  break;
		  case FP_BLOCKED_ON_LOCK:
			  blkstr = "flock";
			  break;
		  case FP_BLOCKED_ON_SELECT:
			  blkstr = "select";
			  break;
		  case FP_BLOCKED_ON_OTHER:
			  blkstr = taskname(bufp->ps_ftask);
			  break;
		  case FP_BLOCKED_ON_NONE:
			  blkstr = "??";
			  break;
	  }
  }
  (void) sprintf(recvstr, "(%s) %s", blkstr, task);
  return recvstr;
}

/* If disaster is called some of the system parameters imported into ps are
 * probably wrong.  This tends to result in memory faults.
 */
void disaster(sig)
int sig;
{
  fprintf(stderr, "Ooops, got signal %d\n", sig);
  fprintf(stderr, "Was ps recompiled since the last kernel change?\n");
  exit(3);
}

/* Main interprets arguments, gets system addresses, opens [k]mem, reads in
 * process tables from kernel/pm/fs and calls pstat() for relevant entries.
 */
int main(argc, argv)
int argc;
char *argv[];
{
  int i;
  struct pstat buf;
  int db_fd;
  int uid = getuid();		/* real uid of caller */
  char *opt;
  int opt_all = FALSE;		/* -a */
  int opt_long = FALSE;		/* -l */
  int opt_notty = FALSE;	/* -x */
  int opt_endpoint = FALSE;	/* -E */
  char *ke_path;		/* paths of kernel, */
  char *mm_path;		/* mm, */
  char *fs_path;		/* and fs used in ps -U */
  char pid[2 + sizeof(pid_t) * 3];
  unsigned long ustime;
  char cpu[sizeof(clock_t) * 3 + 1 + 2];
  struct kinfo kinfo;
  int s;
  u32_t system_hz;

  if(getsysinfo_up(PM_PROC_NR, SIU_SYSTEMHZ, sizeof(system_hz), &system_hz) < 0) {
	exit(1);
  }

  (void) signal(SIGSEGV, disaster);	/* catch a common crash */

  /* Parse arguments; a '-' need not be present (V7/BSD compatability) */
  for (i = 1; i < argc; i++) {
	opt = argv[i];
	if (opt[0] == '-') opt++;
	while (*opt != 0) switch (*opt++) {
		case 'a':	opt_all = TRUE;			break;
		case 'E':	opt_endpoint = TRUE;		break;
		case 'e':	opt_all = opt_notty = TRUE;	break;
		case 'f':
		case 'l':	opt_long = TRUE;		break;
		case 'x':	opt_notty = TRUE;		break;
		default:	usage(argv[0]);
	}
  }

  /* Open memory devices and get PS info from the kernel */
  if ((kmemfd = open(KMEM_PATH, O_RDONLY)) == -1) err(KMEM_PATH);
  if ((memfd = open(MEM_PATH, O_RDONLY)) == -1) err(MEM_PATH);
  if (gettynames() == -1) err("Can't get tty names");

  getsysinfo(PM_PROC_NR, SI_KINFO, &kinfo);

  nr_tasks = kinfo.nr_tasks;	
  nr_procs = kinfo.nr_procs;

  /* Allocate memory for process tables */
  ps_proc = (struct proc *) malloc((nr_tasks + nr_procs) * sizeof(ps_proc[0]));
  ps_mproc = (struct mproc *) malloc(nr_procs * sizeof(ps_mproc[0]));
  ps_fproc = (struct fproc *) malloc(nr_procs * sizeof(ps_fproc[0]));
  if (ps_proc == NULL || ps_mproc == NULL || ps_fproc == NULL)
	err("Out of memory");

	if(minix_getkproctab(ps_proc, nr_tasks + nr_procs, 1) < 0) {
		fprintf(stderr, "minix_getkproctab failed.\n");
		exit(1);
	}

	if(getsysinfo(PM_PROC_NR, SI_PROC_TAB, ps_mproc) < 0) {
		fprintf(stderr, "getsysinfo() for PM SI_PROC_TAB failed.\n");
		exit(1);
	}

	if(getsysinfo(VFS_PROC_NR, SI_PROC_TAB, ps_fproc) < 0) {
		fprintf(stderr, "getsysinfo() for VFS SI_PROC_TAB failed.\n");
		exit(1);
	}

  /* We need to know where INIT hangs out. */
  for (i = FS_PROC_NR; i < nr_procs; i++) {
	if (strcmp(ps_proc[nr_tasks + i].p_name, "init") == 0) break;
  }
  init_proc_nr = i;

  /* Now loop through process table and handle each entry */
  printf("%s", opt_long ? L_HEADER : S_HEADER);
  for (i = -nr_tasks; i < nr_procs; i++) {
	if (pstat(i, &buf, opt_endpoint) != -1 &&
	    (opt_all || buf.ps_euid == uid || buf.ps_ruid == uid) &&
	    (opt_notty || majdev(buf.ps_dev) == TTY_MAJ)) {
		if (buf.ps_pid == 0 && i != PM_PROC_NR) {
			sprintf(pid, "(%d)", i);
		} else {
			sprintf(pid, "%d", buf.ps_pid);
		}

		ustime = (buf.ps_utime + buf.ps_stime) / system_hz;
		if (ustime < 60 * 60) {
			sprintf(cpu, "%2lu:%02lu", ustime / 60, ustime % 60);
		} else
		if (ustime < 100L * 60 * 60) {
			ustime /= 60;
			sprintf(cpu, "%2luh%02lu", ustime / 60, ustime % 60);
		} else {
			sprintf(cpu, "%4luh", ustime / 3600);
		}

		if (opt_long) printf(L_FORMAT,
			       ps->ps_state,
			       ps->ps_euid, pid, ps->ps_ppid, 
			       ps->ps_pgrp,
			       off_to_k(ps->ps_memory),
			       (ps->ps_recv != NONE ? prrecv(ps) : ""),
			       tname((dev_t) ps->ps_dev),
			       cpu,
			       ps->ps_args != NULL ? ps->ps_args : ps->ps_name
			       );
		else
			printf(S_FORMAT,
			       pid, tname((dev_t) ps->ps_dev),
			       cpu,
			       ps->ps_args != NULL ? ps->ps_args : ps->ps_name
			       );
	}
  }
  return(0);
}

/* Get_args obtains the command line of a process. */
char *get_args(struct pstat *ps)
{
  char path[PATH_MAX], buf[4096];
  ssize_t i, n;
  int fd;

  /* Get a reasonable subset of the contents of the 'cmdline' file from procfs.
   * It contains all arguments, separated and terminated by null characters.
   */
  sprintf(path, "%d/cmdline", ps->ps_pid);

  fd = open(path, O_RDONLY);
  if (fd < 0) return NULL;

  n = read(fd, buf, sizeof(buf));
  if (n <= 0) {
	close(fd);

	return NULL;
  }

  close(fd);

  /* Replace all argument separating null characters with spaces. */
  for (i = 0; i < n-1; i++)
	if (buf[i] == '\0')
		buf[i] = ' ';

  /* The last character should already be null, except if it got cut off. */
  buf[n-1] = '\0';

  return strdup(buf);
}

/* Pstat obtains the actual information for the given process, and stores it
 * in the pstat structure. The outside world may change while we are doing
 * this, so nothing is reported in case any of the calls fail.
 */
int pstat(struct pstat *ps, pid_t pid)
{
  FILE *fp;
  int version, ruid, euid, dev;
  char type, path[PATH_MAX], name[256];

  ps->ps_pid = pid;
  ps->ps_next = NULL;

  sprintf(path, "%d/psinfo", pid);

  if ((fp = fopen(path, "r")) == NULL)
	return -1;

  if (fscanf(fp, "%d", &version) != 1) {
	fclose(fp);
	return -1;
  }

  /* The psinfo file's version must match what we expect. */
  if (version != PSINFO_VERSION) {
	fputs("procfs version mismatch!\n", stderr);
	exit(1);
  }

  if (fscanf(fp, " %c %d %255s %c %d %*d %lu %lu %*u %*u",
	&type, &ps->ps_endpt, name, &ps->ps_state,
	&ps->ps_recv, &ps->ps_utime, &ps->ps_stime) != 7) {

	fclose(fp);
	return -1;
  }

  strncpy(ps->ps_name, name, sizeof(ps->ps_name)-1);
  ps->ps_name[sizeof(ps->ps_name)-1] = 0;

  ps->ps_task = type == TYPE_TASK;

  if (!ps->ps_task) {
	if (fscanf(fp, " %lu %*u %*u %c %d %u %u %u %*d %c %d %u",
		&ps->ps_memory, &ps->ps_pstate, &ps->ps_ppid,
		&ruid, &euid, &ps->ps_pgrp, &ps->ps_fstate,
		&ps->ps_ftask, &dev) != 9) {

		fclose(fp);
		return -1;
	}

	ps->ps_ruid = ruid;
	ps->ps_euid = euid;
	ps->ps_dev = dev;
  } else {
	ps->ps_memory = 0L;
	ps->ps_pstate = PSTATE_NONE;
	ps->ps_ppid = 0;
	ps->ps_ruid = 0;
	ps->ps_euid = 0;
	ps->ps_pgrp = 0;
	ps->ps_fstate = FSTATE_NONE;
	ps->ps_ftask = NONE;
	ps->ps_dev = NO_DEV;
  }

  fclose(fp);

  if (ps->ps_state == STATE_ZOMBIE)
	ps->ps_args = "<defunct>";
  else if (!ps->ps_task)
	ps->ps_args = get_args(ps);
  else
	ps->ps_args = NULL;

  return 0;
}

/* Plist creates a list of processes with status information. */
void plist(void)
{
  DIR *p_dir;
  struct dirent *p_ent;
  struct pstat pbuf;
  pid_t pid;
  char *end;
  unsigned int slot;

  /* Allocate a table for process information. Initialize all slots' endpoints
   * to NONE, indicating those slots are not used.
   */
  if ((ptable = malloc((nr_tasks + nr_procs) * sizeof(struct pstat))) == NULL)
	err("Out of memory!");

  for (slot = 0; slot < nr_tasks + nr_procs; slot++)
	ptable[slot].ps_endpt = NONE;

  /* Fill in the table slots for all existing processes, by retrieving all PID
   * entries from the /proc directory.
   */
  p_dir = opendir(".");

  if (p_dir == NULL) err("Can't open " _PATH_PROC);

  p_ent = readdir(p_dir);
  while (p_ent != NULL) {
	pid = strtol(p_ent->d_name, &end, 10);

	if (!end[0] && pid != 0 && !pstat(&pbuf, pid)) {
		slot = SLOT_NR(pbuf.ps_endpt);

		if (slot < nr_tasks + nr_procs)
			memcpy(&ptable[slot], &pbuf, sizeof(pbuf));
	}

	p_ent = readdir(p_dir);
  }

  closedir(p_dir);
}

void usage(const char *pname)
{
  fprintf(stderr, "Usage: %s [-][aeflx]\n", pname);
  exit(1);
}

void err(const char *s)
{
  extern int errno;

  if (errno == 0)
	fprintf(stderr, "ps: %s\n", s);
  else
	fprintf(stderr, "ps: %s: %s\n", s, strerror(errno));

  exit(2);
}

/* Fill ttyinfo by fstatting character specials in /dev. */
int gettynames(void)
{
  static char dev_path[] = "/dev/";
  struct stat statbuf;
  static char path[sizeof(dev_path) + NAME_MAX];
  unsigned int index;
  struct ttyent *ttyp;

  index = 0;
  while ((ttyp = getttyent()) != NULL) {
	strcpy(path, dev_path);
	strcat(path, ttyp->ty_name);
	if (stat(path, &statbuf) == -1 || !S_ISCHR(statbuf.st_mode))
		continue;
	if (index >= n_ttyinfo) {
		n_ttyinfo= (index+16) * 2;
		ttyinfo = realloc(ttyinfo, n_ttyinfo * sizeof(ttyinfo[0]));
		if (ttyinfo == NULL) err("Out of memory");
	}
	ttyinfo[index].tty_dev = statbuf.st_rdev;
	strcpy(ttyinfo[index].tty_name, ttyp->ty_name);
	index++;
  }
  endttyent();
  while (index < n_ttyinfo) ttyinfo[index++].tty_dev= 0;

  return 0;
}
