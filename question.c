#include "top.h"
#include "defs.h"
#include "colors.h"
#include "struct.h"
#include "mjs.h"
#include "window.h"
#include "misc.h"
#include "config.h"
#include "inputline.h"
#include "extern.h"

void
ask_question(char *title, char *prompt, char *initial, Window (*callback)(Window *))
{
	question = calloc(1, sizeof(Window));
	question->win = newwin(3, 60, 9, 15);
	question->width = 60;
	question->height = 3;
	question->y = 9;
	question->x = 15;
	question->activate = active_win;
	question->deactivate = inactive_win;
	question->title = strdup(title); /* dont forget to free this */
	question->flags |= W_RDONLY;
}
