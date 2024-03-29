#ifndef _struct_h
#define _struct_h

#ifndef _defs_h
#include "defs.h"
#endif  /* _defs_h */



#define STOPPED 0
#define PLAYING 1
#define PAUSED 2

#define L_APPEND      	0
#define L_NEW		1
#define L_SEARCH	2


typedef struct _flist {
	unsigned short flags;
#define F_DIR      	0x01
#define F_PLAYLIST	0x02
#define F_HTTP		0x04
#define F_PAUSED   	0x40
	char *album;
	char *filename;		// filename without path
	char *path;		// path without filename without mp3path
	char *fullpath;		// the fullpath
	char *relpath;
	char *artist;
	char *genre;
	struct _flist *next;
	struct _flist *prev;
} flist;

typedef struct {
	char *from;
	flist *head;                       /* ptr to the HEAD of the linked list   */
	flist *tail;                       /* ptr to the TAIL of the linked list   */
	flist *top;                        /* ptr to the element at the window top */
	flist *bottom;                        /* ptr to the element at the window bottom */
	flist *selected;                   /* currently selected file              */
	flist *lastadded;	           /* The previously added file         */
	flist *playing;                    /* currently PLAYING file               */
	int length;                        /* length of the list we are tracking   [0..n]*/
	int where;			   /* what position in the list are we?    if length=0 [0] else [1..length]*/
	int wheretop;			   /* at what position is the top */
	int whereplaying;
	unsigned short flags;
#define F_VIRTUAL      	0x01
} wlist;

typedef struct _dirstack {
	struct _dirstack *prev;
	char *fullpath;
	char *filename;
	wlist *list;
} dirstack;

typedef struct _input {
	WINDOW *win;                       /* The window where we are              */
	PANEL *panel;                      /* The panel where we are               */
	int x;                             /* x-coord for the start of our input   */
	int y;                             /* y-coord for the start of our input   */
	int len;                           /* Length of input buffer               */
	int plen;                          /* Length of the input prompt           */
	int pos;                           /* Cursor position inside input buffer  */
	int flen;                          /* Input field length                   */
	int fpos;                          /* Input field position                 */
	int (*parse)
	    (struct _input *, int, int);   /* Function that parses input           */
	int (*update)(struct _input *);    /* Function to call to update the line  */
	int (*finish)(struct _input *);    /* Function to call when input is over  */
	int (*complete)(struct _input *);  /* Function for tab completion          */
	char prompt[PROMPT_SIZE+1];        /* Prompt for the input line            */
	char *anchor;                      /* For wrapped lines, ptr to where the  */
					   /* "anchor" for our wrapped text starts */
	char buf[BUFFER_SIZE+1];           /* The input line itself                */
} Input;

typedef struct _win {
	WINDOW *win;                       /* the actual ncurses window            */
	PANEL *panel;                      /* the panel, duh                       */
	int width;                         /* window/panel width                   */
	int height;                        /* window/panel height                  */
	int x;                             /* x-coord for the upper-left corner    */
	int y;                             /* y-coord for the upper-left corner    */
	Input *inputline;                  /* the input stuff                      */
	int (*input) (struct _win *);      /* Function to parse input              */
	int (*update) (struct _win *);     /* Updates the window                   */
	int (*activate) (struct _win *);   /* activate this window                 */
	int (*deactivate) (struct _win *); /* deactivate the window                */
	short int flags;                   /* various info about the window        */
#define W_ACTIVE   0x01              /* is the window active?                */
#define W_LIST     0x02              /* is it a list, or just 'contents'     */
#define W_RDONLY   0x04              /* is it read-only, ie can we modify it */
	union {
		wlist *list;               /* if the window has contents, use this */
		flist **show;               /* otherwise just this                  */
	} contents;
	const u_char *title_fmt;           /* format string for window title       */
	const u_char *title_dfl;           /* default (if needed) window title     */
	u_char *title;                     /* the window title                     */
	const u_char *format;              /* format for the text in the window    */
	struct _win *next;                 /* next window in the cycle             */
	struct _win *prev;                 /* previous window in the cycle         */
} Window;

typedef struct _config {
	char mpgpath[256];
	char mp3path[256];
	char statefile[256];
	char logfile[256];
	char resultsfile[256];
	char playlistpath[256];
	char bottomtext[256];
	char output[256];
	u_int16_t c_flags;
#define C_PADVANCE 	0x0001
#define C_FADVANCE 	0x0002
#define C_MONO	   	0x0004
#define C_LOOP     	0x0008
#define C_P_TO_F 	0x0010
#define C_FIX_BORDERS	0x0020
#define	C_ALLOW_P_SAVE	0x0040
#define C_TRACK_NUMBERS 0x0080
#define	C_P_SAVE_EXIT	0x0100
#define C_USE_GENRE	0x0200
#define C_SCREENSAVER	0x0400
#define C_RM_AFTER_PLAY 0x0800
	int buffer;
	int jump;
	int pathlen;
	int basepathlen;		/* only used in genre mode */
} Config;

typedef struct {
	int played;			// frames played
	int left;			// frames left in song
	double elapsed;			// time elapsed
	double remaining;		// time remaining in song
} mpgreturn;

/* this struct holds all the info to construct a question/answer input dialog */

typedef struct {
	char **prompts;                 /* array of prompts for our edit box */
	char **inputs;                  /* array of default values           */
	int num_prompts;                /* total # of prompts/defaults       */
	int (*finish) (char **, int);   /* function to parse the final input */
} Inputbox;

#endif /* _struct_h */
