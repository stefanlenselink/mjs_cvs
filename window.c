#include "top.h"
#include "defs.h"
#include "colors.h"
#include "struct.h"
#include "window.h"
#include "misc.h"
#include "tokens.h"
#include "files.h"
#include "extern.h"

static void	 clear_info(void);
static u_char	*parse_title(Window *, u_char *, int);

int
show_list(Window *window)
{
	int top, x = window->width-4, y = window->height-1, i;
	u_char buf[BUFFER_SIZE+1];
	const u_char *line;
	u_int32_t color;
	WINDOW *win = window->win;
	flist *ftmp;
	wlist *list = window->contents.list;

	if (!list)
		return 0;
	top = list->where - (y/2);
	list->top=list->head;

	if (!(conf->c_flags & C_ALT_SCROLL)) {
		if (list->where-3 == list->wheretop)
			list->wheretop-=1;
		else if ((list->where+4) == list->wheretop+y-1)
				if ((list->wheretop+=1)>list->length)
					list->wheretop=list->length;
		if (list->where-3 < list->wheretop)
			list->wheretop = list->where-((y-1)/2);
		else if (list->where > list->wheretop + y-5)
			list->wheretop = list->where-((y-1)/2);
		top = list->wheretop-1;
	}
	if ((top>0)&(list->length > (y-1))) {
		for (i = 0; i < top; i++)
			list->top = list->top->next;
	}

	ftmp = list->top;

	for (i = 1; i < y; i++)
		if (ftmp && *ftmp->filename) {
			if (window->format) {
				memset(buf, 0, sizeof(buf));
				line = parse_tokens(window,ftmp, buf, BUFFER_SIZE, window->format);
			} else 	
				line = ftmp->filename;
			if ((window->flags & W_ACTIVE) && (ftmp->flags & F_PAUSED))
				my_mvwnaddstr(win, i, 2, colors[UNSEL_PLAYING] | A_BLINK, x, line);
			else {
				if (window==play)
					if (ftmp->flags & F_PLAY)
						if ((ftmp->flags & F_SELECTED) && (window->flags & W_ACTIVE))
							color = colors[SEL_PLAYING];
						else
							color = colors[UNSEL_PLAYING];
					else			
						if ((ftmp->flags & F_SELECTED) && (window->flags & W_ACTIVE))
							color = colors[SELECTED];
						else
							color = colors[PLAYLIST];
				else	// window==files
					if (ftmp->flags & F_DIR)
						if ((ftmp->flags & F_SELECTED) && (window->flags & W_ACTIVE))
							color = colors[SELECTED];
						else
							color = colors[DIRECTORY];
					else
						if ((ftmp->flags & F_SELECTED) && (window->flags & W_ACTIVE))
							color = colors[SELECTED];
						else 	
							color = colors[UNSELECTED];
				my_mvwnaddstr(win, i, 2, color, x, line);
			}
			ftmp = ftmp->next;
		} else /* blank the line */
			my_mvwnaddstr(win, i, 2, colors[FILE_BACK], x, "");
	if (active->flags & W_LIST)
		do_scrollbar(active);
	update_title(window);
	update_panels();
	return 1;
}

Window *
move_selector(Window *window, int c)
{
	flist *file;
	wlist *wlist = window->contents.list;
	int j, maxx, maxy, length;
	
	if (!wlist)
		return NULL;
	
	getmaxyx(window->win, maxy, maxx);
	length = maxy - 2;

	if (!wlist->selected)
		return NULL;
	switch (c) {
		case KEY_HOME: 
			wlist->selected->flags &= ~F_SELECTED;
			wlist->selected = next_valid(window, wlist->head, c);
			wlist->selected->flags |= F_SELECTED;
			wlist->where = 1;
		wlist->wheretop = 1;
			return window;
		case KEY_END: 
			wlist->selected->flags &= ~F_SELECTED;
			wlist->selected = next_valid(window, wlist->tail, c);
			wlist->selected->flags |= F_SELECTED;
			wlist->where = wlist->length;
			return window;
		case KEY_DOWN:
			if ((file = next_valid(window, wlist->selected->next, c))) {
				wlist->selected->flags &= ~F_SELECTED;
				file->flags |= F_SELECTED;
				wlist->selected = file;
				wlist->where++;
				return window;
			}
			break;
		case KEY_UP:
			if ((file = next_valid(window, wlist->selected->prev, c))) {
				wlist->selected->flags &= ~F_SELECTED;
				file->flags |= F_SELECTED;
				wlist->selected = file;
				wlist->where--;
				return window;
			}
			break;
		case KEY_NPAGE:
			wlist->selected->flags &= ~F_SELECTED;
			for (j = 0; wlist->selected->next && j < length-1; j++) {
				wlist->selected = wlist->selected->next;
				wlist->where++;
				wlist->wheretop++;
				}
			wlist->selected = next_valid(window, wlist->selected, c);
			wlist->selected->flags |= F_SELECTED;
			return window;
		case KEY_PPAGE:
			wlist->selected->flags &= ~F_SELECTED;
			for (j = 0; wlist->selected->prev && j < length-1; j++) {
				wlist->where--;
				wlist->wheretop--;
				wlist->selected = wlist->selected->prev;
				}
			wlist->selected = next_valid(window, wlist->selected, c);
			wlist->selected->flags |= F_SELECTED;
			return window;
		default:
			break;
	}
	return NULL;
}

int
update_info(Window *window)
{
	WINDOW *win = info->win;
	int i = info->width;
	flist *file = NULL;

	if (!window || !window->contents.list)
		return 0;

	if (window->flags & W_LIST)
		file = window->contents.list->selected;
	else
		file = window->contents.play;

	clear_info();

	if (file) {
		my_mvwnprintw(win, 1, 9, colors[UNSELECTED], i-10, "%s", (file->flags & F_DIR) ? "(Directory)" : file->filename);
		if (file->artist)
			my_mvwnprintw(win, 2, 9, colors[UNSELECTED], i-10, "%s", file->artist);
		if (file->album)
			my_mvwnprintw(win, 3, 9, colors[UNSELECTED], i-10, "%s", file->album);
	}
	update_title(info);
	update_panels();
	return 1;
}

void change_active(Window *new)
{
	active->deactivate(active);
	active->update(active);
	active = new;
	active->activate(active);
	active->update(active);
	info->update(active);
	doupdate();
}

static void
clear_info(void)
{
	int i = info->height - 2;
	for (; i; i--)
		my_mvwnclear(info->win, i, 9, info->width - 10);

}

int
active_win(Window *window)
{
	WINDOW *win = window->win;
	PANEL *panel = window->panel;

	int i, x, y;
	if (conf->c_flags & C_FIX_BORDERS)
		wborder(win, 0, 0, '=', '=', 0, 0, 0, 0);
	else
		wborder(win, '�', '�', '�', '�', '�', '�', '�', '�');
	getmaxyx(win, y, x);
	mvwchgat(win, 0, 0, x, A_ALTCHARSET | colors[ACTIVE], 0, NULL);
	mvwchgat(win, y-1, 0, x, A_ALTCHARSET | colors[ACTIVE], 0, NULL);
	for (i = 0; i < y; i++) {
		mvwchgat(win, i, 0, 1, A_ALTCHARSET | colors[ACTIVE], 0, NULL);
		mvwchgat(win, i, x-1, 1, A_ALTCHARSET | colors[ACTIVE], 0, NULL);
	}	
	window->flags |= W_ACTIVE;
	update_title(window);
	top_panel(panel);
	update_panels();
	return 1;
}

int
inactive_win(Window *window)
{
	WINDOW *win = window->win;
	int i, x, y;
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
	getmaxyx(win, y, x);
	mvwchgat(win, 0, 0, x, A_ALTCHARSET | colors[INACTIVE], 0, NULL);
	mvwchgat(win, y-1, 0, x, A_ALTCHARSET | colors[INACTIVE], 0, NULL);
	for (i = 0; i < y; i++) {
		mvwchgat(win, i, 0, 1, A_ALTCHARSET | colors[INACTIVE], 0, NULL);
		mvwchgat(win, i, x-1, 1, A_ALTCHARSET | colors[INACTIVE], 0, NULL);
	}	
	window->flags &= ~W_ACTIVE;
	update_title(window);
	update_panels();
	return 1;
}

int 
clear_menubar(Window *window)
{
	wmove(window->win, 0, 0);
	wclrtoeol(window->win);
	wbkgd(window->win, colors[MENU_BACK]);
	return 1;
}

int
std_menubar(Window *window)
{
	char version_str[128];
	int x = window->width-2;
	clear_menubar(window);
	snprintf(version_str, 128, "%s   v%s", window->title_dfl, VERSION);
	my_mvwaddstr(window->win, 0, (x-strlen(version_str))/2, colors[MENU_TEXT], version_str);
	return 1;
}

__inline__ void
printf_menubar(Window *window, char *text) 
{
	clear_menubar(window);
	my_mvwaddstr(window->win, 0, 10, colors[MENU_TEXT], text);
	update_panels();
	doupdate();

}

int
update_title(Window *window)
{
	WINDOW *win = window->win;
	u_char title[BUFFER_SIZE+1], *p = NULL, horiz = ACS_HLINE;
	u_int32_t color = colors[INACTIVE];
	int i = 0, left, right, center, x = window->width-2;

	if (window->flags & W_ACTIVE) {
		if (conf->c_flags & C_FIX_BORDERS)
			horiz = '=', color = colors[ACTIVE];
		else
			horiz = '�', color = colors[ACTIVE];
	}

	memset(title, 0, sizeof(title));
	p = parse_title(window, title, BUFFER_SIZE);
	i = strlen(p)+4;

	if (i > x) { 
		if ((i = strlen(window->title_dfl)) < x) {
			i += 4;
			p = (u_char *)window->title_dfl;
		} else {
			p = "";
			i = 4;
		}
	}

	center = x-i-(x-i)/2;
	right = (x-i)/2;
	left = x-i-right;

	if (p && (i <= x)) {
		mvwhline(win, 0, 1, horiz | A_ALTCHARSET | color, left);
		my_mvwprintw(win, 0, 1+center, colors[TITLE], "[ %s ]", p);
		mvwhline(win, 0, 1+center+i, horiz | A_ALTCHARSET | color, right);
		return 1;
	}
	return 0;
}

void
do_scrollbar(Window *window)
{
	int i = 1, offscreen, x, y; /* window dimensions, etc */
	int top, bar, bottom; /* scrollbar portions */
	double value; /* how much each notch represents */
	wlist *wtmp = window->contents.list;
	flist *ftmp = NULL, *selected = wtmp->selected;
	WINDOW *win = window->win;
	if (wtmp->length < 1)
		return;
	getmaxyx(win, y, x);
	y -= 2;
	x -= 1;
	value = wtmp->length / (double) y;
	for (ftmp = wtmp->top; ftmp != selected; ftmp = ftmp->next)
		i++;
	/* now we calculate the number preceeding the screenfull */
	offscreen = wtmp->where - i;

	/* calculate the sizes of our status bar */
	if (value < 1) {
		top = 0;
		bar = y;
		bottom = 0;
	} else {
		/* drago's code */
		double toptmp;
		toptmp=offscreen / value;
		top = (int)(double)(int)toptmp/1 == toptmp ? toptmp : (double)(int)toptmp+(double)1;
		bar = (int)((y / value)+(double).5);
		bottom = y - top - bar;
		/* end drago's code =) */
	}
	/* because of rounding we may end up with too much, correct for that */
	if (bottom < 0)
		top += bottom;
	mvwvline(win, 1, x, ACS_BOARD | A_ALTCHARSET | colors[SCROLL], top);
	mvwvline(win, 1+top, x, ACS_BLOCK | A_ALTCHARSET | colors[SCROLLBAR], bar);
	if (bottom > 0)
		mvwvline(win, 1+top+bar, x, ACS_BOARD | A_ALTCHARSET | colors[SCROLL], bottom);
	update_panels();
}

u_char *
parse_title(Window *win, u_char *title, int len)
{
	u_char *p = (u_char *)win->title_dfl;
		
	if (win->title_fmt) {
		if (win->flags & W_LIST && win->contents.list && win->contents.list->selected)
			p = (u_char *)parse_tokens(win, win->contents.list->selected, title, len, win->title_fmt);
		else if (!(win->flags & W_LIST) && win->contents.play)
			p = (u_char *)parse_tokens(win, win->contents.play, title, len, win->title_fmt);
		else if (info->contents.play)
			p = (u_char *)parse_tokens(win, info->contents.play, title, len, win->title_fmt);
	}

	return p;
}
