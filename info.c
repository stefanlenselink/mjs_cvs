/*
 * most of this code is NOT mine. I don't know where it came from, but it
 * sure as hell is ugly! It _really_ needs some cleanup. Someday...
 */

#include "top.h"
#include "defs.h"
#include "struct.h"
#include "info.h"
#include "extern.h"



flist *
mp3_info(char *dir, char *filename, flist *file, u_int32_t size)
{
	char *startp, *endp;
	int length;
	if (!file)
		file = calloc(1, sizeof(flist));
	
	startp=dir+strlen(conf->mp3path)+1;
	if ((endp=strchr(startp,'/')))
		length = endp-startp;
	else
		length = strlen(startp);
	
	file->artist = calloc(length+1, sizeof(char));
	strncpy(file->artist,startp,length);
	file->artist[length]='\0';

	if (endp)
		file->album = strdup(endp+1);
	else
		file->album = strdup("Unknown");

	file->length = 0;
	
	return file;

}
	
