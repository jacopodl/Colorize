/*
	* <colorize, simple bash highlighter>
	* Copyright (C) <2013-2015> <Jacopo De Luca>
	*
	* This program is free software: you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation, either version 3 of the License, or
	* (at your option) any later version.
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	* GNU General Public License for more details.
	* You should have received a copy of the GNU General Public License
	* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <regex.h>

#include "argsx.h"

/* CONSTANT */
#define __BASENAME "colorize"
#define __VERSION "2.00"
#define __FATAL "[FATAL]\t"
#define __WARNING "[WARNING]\t"

enum Color
{
	black,
	red,
	green,
	yellow,
	blue,
	violet,
	cyan,
	white
};

struct settings
{
	enum Color back_color;
	enum Color fore_color;
	bool extended;
	bool insensitive;
	bool pipedInput;
	char *regex;
	char *file_path;
};

/* Prototype: */
char *build_color(enum Color back, enum Color fore);
int Highlighter(struct settings *set, char *color);
enum Color set_color(char *cstr);
void usage(void);

int main(int argc, char **argv)
{
	if(argc<2)
	{
		usage();
		return -1;
	}
	struct settings set = {-1,-1,false,false,NULL,false,NULL};

	/* Check piped content */
	if(!isatty(fileno(stdin)))
		set.pipedInput=true;

	ax_lopt lopt[]={{(char*)"help",ARGSX_NOARG,'h'},
					{(char*)"version",ARGSX_NOARG,'v'}};
	int ret;
	while((ret=argsx(argc,argv,(char*)"b!ef!hiv\0",lopt,sizeof(lopt),'-'))!=-1)
	{
		switch(ret)
		{
			case 'b':
				set.back_color=set_color(ax_arg);
			break;
			case 'f':
				set.fore_color=set_color(ax_arg);
				printf("%d\n",set.fore_color);
				if(set.fore_color==-1)
				{
					printf("Invalid color\n");
					exit(0);
				}
			break;
			case 'e':
				set.extended=true;
			break;
			case 'h':
				usage();
				exit(0);
			break;
			case 'i':
				set.insensitive=true;
			break;
			case 'v':
				printf("%s V: %s\n", __BASENAME, __VERSION);
				exit(0);
			break;
			case ARGSX_BAD_OPT:
				exit(0);
			break;
			case ARGSX_FEW_ARGS:
				exit(0);
			break;
			case ARGSX_NONOPT:
				if(!set.pipedInput)
					if(set.regex==NULL)
						set.regex=ax_arg;
					else
						set.file_path=ax_arg;
				else
					if(set.regex==NULL)
						set.regex=ax_arg;
			break;
		}
	}

	if(set.back_color==-1 && set.fore_color==-1)
	{
		printf("%sSelect color with -b [color] or -f [color]\n",__WARNING);
		return -1;
	}
	else if(set.file_path==NULL&&!set.pipedInput)
	{
		printf("%sMissing File\n",__WARNING);
		return -1;
	}
	else if(set.regex==NULL)
	{
		printf("%sMissing Regex\n",__WARNING);
		return -1;
	}
	return Highlighter(&set, build_color(set.back_color,set.fore_color));
}

char *build_color(enum Color back, enum Color fore)
{
	static char fcolor[]={"\e[00;30m\0"};
	static char bcolor[]={"\e[40m\0"};
	if(back != -1 && fore != -1)
	{
		fcolor[2]='4';
		fcolor[3]=(char)(48+back);
		fcolor[6]=(char)(48+fore);
		return fcolor;
	}
	else if(fore!=-1)
	{
		fcolor[6]=(char)(48+fore);
		return fcolor;
	}
	else if(back!=-1)
	{
		bcolor[3]=(char)(48+back);
		return bcolor;
	}
	return NULL;
}

int Highlighter(struct settings *set, char *color)
{
	regex_t myregex;
	int ret=0;
	int match_type=0;

	if(set->extended&&set->insensitive)
		match_type=REG_ICASE|REG_EXTENDED;
	else if(set->insensitive)
		match_type=REG_ICASE;
	else
		match_type=REG_EXTENDED;

	ret=regcomp(&myregex,set->regex,match_type);
	if(ret)
	{
		fprintf(stderr, "%sCould not compile regex!\n",__FATAL);
		return -1;
	}

	FILE *stream=NULL;

	if(set->pipedInput)
		stream=stdin;
	else
	{
		stream=fopen(set->file_path,"r");
		if(stream==NULL)
		{
			fprintf(stderr, "%sFile not found!\n",__FATAL);
			return -1;
		}
	}

	ssize_t bread=0;
	size_t len=0;
	char *str=NULL;
	
	while((bread=getline(&str,&len,stream))!=EOF)
	{
		if(!regexec(&myregex,str,0,NULL,0))
		{
			if(str[bread-1]=='\n')
			{
				char *strmod=(char*)malloc(bread);
				if(strmod==NULL)
					return -1;
				strncpy(strmod,str,bread-1);
				strmod[bread-1]='\0';
				fprintf(stdout, "%s%s%s",color,strmod,(char*)"\e[0m\n");
				free(strmod);
			}
			else
				fprintf(stdout, "%s%s%s",color,str,(char*)"\e[0m");
		}
		else
			fprintf(stdout, "%s", str);
	}
	fclose(stream);
	free(str);
	return 0;
}

enum Color set_color(char *cstr)
{
	enum Color color;
	if(strcmp(cstr,"black")==0)
		color=black;
	else if(strcmp(cstr,"red")==0)
		color=red;
	else if(strcmp(cstr,"green")==0)
		color=green;
	else if(strcmp(cstr,"yellow")==0)
		color=yellow;
	else if(strcmp(cstr,"blue")==0)
		color=blue;
	else if(strcmp(cstr,"violet")==0)
		color=violet;
	else if(strcmp(cstr,"cyan")==0)
		color=cyan;
	else if(strcmp(cstr,"white")==0)
		color=white;
	else
		color = -1;
	return color;
}

void usage(void)
{
	printf("\n%s V: %s\n",__BASENAME,__VERSION);	
	printf("Highlights the line that contains the pattern using ANSI escape sequence.\n"
			"Example:\n"
			"%s [option] [regex] [file]\n"
			"%s [option] [regex]\n\n"
			,__BASENAME,__BASENAME);
	printf("Regex Pattern interpretation:\n"
			"-i\tIGNORE CASE\n"
			"-e\tEXTENDED REGEX\n\n");
	printf("Highlighting:\n"
			"-f\tFOREGROUND COLOR [color]\n"
			"-b\tBACKGROUND COLOR [color]\n"
			"[color]: black,red,green,yellow,blue,violet,cyan,white\n\n");
	printf("Miscelaneous:\n"
		    "-v, --version\tdisplay program version\n"
		  	"-h, --help\tdisplay this and exit\n\n");
}