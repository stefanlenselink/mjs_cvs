#ifndef _playlist_h
#define _playlist_h

#ifndef _struct_h
#include "struct.h"
#endif

void	 play_next_song(wlist *);
void	 play_prev_song(wlist *);

int	 jump_to_song(wlist *, flist *);
int	 playback_title(Window *);

void	add_to_playlist_recursive(wlist *, flist *, flist *);
void	add_to_playlist(wlist *, flist *, flist *);
void	stop_player(wlist *);
void	pause_player(wlist *);
void	resume_player(wlist *);
wlist	*randomize_list(wlist *);
wlist	*read_playlist(wlist *, const char *);

#endif /* _playlist_h */
