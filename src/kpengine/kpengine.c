/* Modified by Paul Goins:
 * This file contains modifications so it will compile without GTK
 * dependencies and to make it better work with wxKanjipad.
 */
/* KanjiPad - Japanese handwriting recognition front end
 * Copyright (C) 1997 Owen Taylor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "jstroke.h"
#include <ctype.h>

#define MAX_STROKES 32
#define BUFLEN 1024
#ifndef PATH_MAX //FIXME: this is not found if debug mode is used (-pedantic ?)
# define PATH_MAX BUFLEN
#endif

static StrokeDic stroke_dicts[MAX_STROKES];
static char *progname;
static char *data_dir;

void
load_database()
{
	char buf[PATH_MAX];
	char fname[PATH_MAX];
	int at_db;
	FILE *fd;
	char *dir=0;

	if(data_dir)
	{
		int len = strlen(data_dir)+1;
		dir = malloc(sizeof(char)*(len));
		strncpy(dir, data_dir, len);
	}
	else
	{
		/* Default to current directory if no data dir is specified. */
		dir = malloc(sizeof(char)*9);
		strcpy(dir, "kanjipad");
		dir[8]=0;
	}
	
	for(at_db=0;at_db<MAX_STROKES;++at_db)
		stroke_dicts[at_db]=0;

	for(at_db=0;at_db<MAX_STROKES;++at_db)
	{
		int len = strlen(dir) + 12; /* dir len + ##.unistrok (11 chars)
									   + sep char = dir len + 12 */
		if(len<1 || len>=PATH_MAX)
		{
			fprintf(stderr,"Failed to format path\n");
			if(dir) free(dir);
			exit(-5);
		}

#ifdef __MSWINDOWS__
		char separator = '\\';
#else
		char separator = '/';
#endif
		sprintf(fname,"%s%c%02d.unistrok",dir,separator,at_db);

		fd = fopen(fname,"r");
		if(fd)
		{
			StrokeDicEntryList **ppnext=&stroke_dicts[at_db];

			while( fgets(buf,PATH_MAX,fd) )
			{
				unsigned long unicode_point;
				char *begin;
				char *next_pchr;
				char *pchr = buf;
				StrokeDicEntryList *plist=0;
				
				if( pchr[0]=='#')
					continue;

				unicode_point = strtol(pchr,&next_pchr,16);
				
				if(pchr==next_pchr)
				{
					fprintf(stderr,"Failed to parse line:\n\"%s\"\n",pchr);
					continue;
				}
				pchr=next_pchr;

				if(unicode_point<0x3300 ||
					unicode_point>0x2FA1F)
				{
					fprintf(stderr,"Unicode out of range for Kanji: 0x%08lX\n"
							"file \"%s\"\n\"%s\" ",
							unicode_point,
							fname,
							buf
							);
					continue;
				}

				plist = malloc(sizeof(StrokeDicEntryList));

				if(!plist)
				{
					mem_err:
					fprintf(stderr,"out of memory\n");
					if(dir) free(dir);
					exit(-1);
				}
				
				plist->m_next=0;
				plist->m_entry.m_character=malloc(20);
				if(!plist->m_entry.m_character)
					goto mem_err;
				//plist->m_entry.m_character[ucs4toutf8(unicode_point,plist->m_entry.m_character)]=0;
				sprintf(plist->m_entry.m_character,"%lu",unicode_point);

				//Strokes
				while(*pchr!='|')
				{
					if( !*pchr || *pchr=='\n')
					{
						fail_no_strokes:
						fprintf(stderr,"Failed to parse line: No Strokes\n\"%s\"\n",buf);
						free(plist->m_entry.m_character);
						free(plist);
						continue;
					}
					++pchr;
				}
				++pchr;

				while(*pchr==' '||*pchr=='\t')
					++pchr;
				begin=pchr;
				while(*pchr && *pchr!='#' && *pchr!='|' && *pchr!='\n')
					++pchr;
				if(pchr==begin)
					goto fail_no_strokes;
				plist->m_entry.m_strokes=malloc(pchr-begin+1);
				if(!plist->m_entry.m_strokes)
					goto mem_err;
				strncpy(plist->m_entry.m_strokes,begin,pchr-begin);
				plist->m_entry.m_strokes[pchr-begin]=0;

				//Filter
				plist->m_entry.m_filters=0;
				while(*pchr!='|')
				{
					if( !*pchr || *pchr=='\n')
						break;
					++pchr;
				}
				if(*pchr=='|')
				{
					++pchr;
					//Possible Filter
					while(*pchr==' '||*pchr=='\t')
						++pchr;
					begin=pchr;
					while(*pchr && *pchr!='#' && *pchr!='\n')
						++pchr;
					if(pchr!=begin)
					{
						plist->m_entry.m_filters=malloc(pchr-begin+1);
						if(!plist->m_entry.m_filters)
							goto mem_err;
						strncpy(plist->m_entry.m_filters, begin,pchr-begin);
						plist->m_entry.m_filters[pchr-begin]=0;
					}
				}

				//line accepted
				*ppnext=plist;
				ppnext=&plist->m_next;
/*
fprintf(stderr,"Loaded %s\n",plist->m_entry.m_character);
fprintf(stderr,"Strokes:--%s--\n",plist->m_entry.m_strokes);
if(plist->m_entry.m_filters)
{
	fprintf(stderr,"Filters:--%s--\n",plist->m_entry.m_filters);
	break;
}
*/
			}

			fclose(fd);
		}

	}
	free(dir);
}

int
process_strokes (FILE *file)
{
    RawStroke strokes[MAX_STROKES];
    char *buffer = malloc(BUFLEN);
    int buflen = BUFLEN;
    int nstrokes = 0;

    /* Read in strokes from standard in, all points for each stroke
     *  strung together on one line, until we get a blank line
     */

    while (1)
    {
        char *p,*q;
        int len;

        if (!fgets(buffer, buflen, file))
            return 0;

		len=strlen(buffer);
        while( (buffer[len-1] != '\n') )
        {
//fprintf(stderr,"READ PARTIAL (%d so far)\n",buflen-1);
//fflush(stderr);
			if(buflen < 2*len)
			{
            	buflen += BUFLEN;
	            buffer = realloc(buffer, buflen);
			}
            if (!fgets(buffer+len, buflen-len, file))
                return 0;
			len=strlen(buffer);
        }

//fprintf(stderr,"parsing %d bytes\n",strlen(buffer));
//fflush(stderr);
        len = 0;
        p = buffer;

        while (1) {
            while (isspace ((unsigned char) *p)) p++;
            if (*p == 0)
                break;
            strokes[nstrokes].m_x[len] = strtol (p, &q, 0);
            if (p == q)
                break;
            p = q;

            while (isspace ((unsigned char) *p)) p++;
            if (*p == 0)
                break;
            strokes[nstrokes].m_y[len] = strtol (p, &q, 0);
            if (p == q)
                break;
            p = q;

            len++;
        }

        if (len == 0)
            break;

        strokes[nstrokes].m_len = len;
        nstrokes++;
        if (nstrokes == MAX_STROKES)
            break;
//fprintf(stderr,"added strok (%d)\n",nstrokes);
    }

    if (nstrokes != 0 && nstrokes < MAX_STROKES && stroke_dicts[nstrokes])
    {
//fprintf(stderr,"attempting %d stroke lookup\n",nstrokes);
        int i;
        ListMem *top_picks;
        StrokeScorer *scorer = StrokeScorerCreate (stroke_dicts[nstrokes],
                               strokes, nstrokes);
        if (scorer)
        {
            StrokeScorerProcess(scorer, -1);
            top_picks = StrokeScorerTopPicks(scorer);
            StrokeScorerDestroy(scorer);

            printf("K");
            for (i=0;i<(int)top_picks->m_argc;i++)
            {
                if (i)
                    printf(" ");
                printf("%s",top_picks->m_argv[i]);
            }

            free(top_picks);
        }
        printf("\n");

        fflush(stdout);
    }
	else
	{
		fprintf(stderr,"Invalid number of strokes: %d\n",nstrokes);
	}
    return 1;
}

void
usage ()
{
	fprintf(stderr, "Usage: %s [-f/--data-dir DIR]\n", progname);
	exit (1);
}

int 
real_main(int argc, char **argv)
{
	int i;
	char *p = progname = argv[0];
	while (*p)
    {
		if (*p == '/') progname = p+1;
		p++;
    }
	
	for (i=1; i<argc; i++)
	{
		if (!strcmp(argv[i], "--data-dir") ||
			!strcmp(argv[i], "-d"))
		{
			i++;
			if (i < argc)
				data_dir = argv[i];
			else
				usage();
		}
		else
		{
			usage();
		}
	}

	load_database();
	
	while (process_strokes (stdin))
		;
	
	return 0;
}

#ifdef __MSWINDOWS__
#include <windows.h>

/* To avoid a console window coming up while running our
 * program, on Win32, we act as a GUI app... a WinMain()
 * and no main().
 */
#ifdef __GNUC__
#  ifndef _stdcall
#    define _stdcall  __attribute__((stdcall))
#  endif
#endif

int _stdcall
WinMain (struct HINSTANCE__ *hInstance,
	 struct HINSTANCE__ *hPrevInstance,
	 char               *lpszCmdLine,
	 int                 nCmdShow)
{
  return real_main (__argc, __argv);
}
#else
int 
main(int argc, char **argv)
{
  return real_main (argc, argv);
}
#endif
