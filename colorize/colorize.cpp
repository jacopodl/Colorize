/*
    <colorize, simple bash highlighter>
    Copyright (C) <2013> <Jacopo De Luca>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
// -----------------
#include "ArgsX.h"
#include "Color.h"

/* CONSTANT */

#define __BASENAME "colorize"
#define __VERSION "1.00"
#define __FATAL "[FATAL]\t"
#define __WARNING "[WARNING]\t"

/* Prototype: */

void usage(FILE *);

void printv(FILE *);

bool rMatch(regex_t *,char *);

int Highlighter(struct Data_Opt *);

// ---------------------------------

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

struct Data_Opt
{
	bool background;
	bool foreground;
	enum Color back_color;
	enum Color fore_color;
	bool extended;
	bool insensitive;
	char *regex;
	bool pipedInput;
	char *file_path;
};

int main(int argc, char **argv)
{
	if(argc<2)
	{
		usage(stdout);
		return -1;
	}

	struct Data_Opt Option={false,false,red,red,false,false,NULL,false,NULL};
	
	// Check piped content
	if(!isatty(fileno(stdin)))
		Option.pipedInput=true;

	_ArgsX_LongOpt Lopt[]={{(char*)"help",_ARGSX_OPT_NO_ARGUMENTS,'h'},
						   {(char*)"version",_ARGSX_OPT_NO_ARGUMENTS,'v'}};
	int p_argc=1,
		r_opt=0;

	while((r_opt=ArgsX(argc,argv,&p_argc,(char*)"b+ef+hir+v",Lopt,sizeof(Lopt),'-'))!=_ARGSX_FINISH)
	{
		switch(r_opt)
		{
			case 'b':
				if(strcmp(argv[ArgsX_ArgPtr],"black")==0)
					Option.back_color=black;
				else if(strcmp(argv[ArgsX_ArgPtr],"red")==0)
					Option.back_color=red;
				else if(strcmp(argv[ArgsX_ArgPtr],"green")==0)
					Option.back_color=green;
				else if(strcmp(argv[ArgsX_ArgPtr],"yellow")==0)
					Option.back_color=yellow;
				else if(strcmp(argv[ArgsX_ArgPtr],"blue")==0)
					Option.back_color=blue;
				else if(strcmp(argv[ArgsX_ArgPtr],"violet")==0)
					Option.back_color=violet;
				else if(strcmp(argv[ArgsX_ArgPtr],"cyan")==0)
					Option.back_color=cyan;
				else if(strcmp(argv[ArgsX_ArgPtr],"white")==0)
					Option.back_color=white;
				else
				{
					printf("%sBad color!\n",__WARNING);
					return -1;
				}
				Option.background=true;
			break;
			case 'e':
				Option.extended=true;
			break;
			case 'f':
				if(strcmp(argv[ArgsX_ArgPtr],"black")==0)
					Option.fore_color=black;
				else if(strcmp(argv[ArgsX_ArgPtr],"red")==0)
					Option.fore_color=red;
				else if(strcmp(argv[ArgsX_ArgPtr],"green")==0)
					Option.fore_color=green;
				else if(strcmp(argv[ArgsX_ArgPtr],"yellow")==0)
					Option.fore_color=yellow;
				else if(strcmp(argv[ArgsX_ArgPtr],"blue")==0)
					Option.fore_color=blue;
				else if(strcmp(argv[ArgsX_ArgPtr],"violet")==0)
					Option.fore_color=violet;
				else if(strcmp(argv[ArgsX_ArgPtr],"cyan")==0)
					Option.fore_color=cyan;
				else if(strcmp(argv[ArgsX_ArgPtr],"white")==0)
					Option.fore_color=white;
				else
				{
					printf("%sBad color!\n",__WARNING);
					return -1;
				}
				Option.foreground=true;
			break;
			case 'h':
				usage(stdout);
				return 0;
			break;
			case 'i':
				Option.insensitive=true;
			break;
			case 'r':
				Option.regex=argv[ArgsX_ArgPtr];
			break;
			case 'v':
				printv(stdout);
				return 0;
			break;
			case _ARGSX_BadArg:
				printf("%sBad Option!\n",__WARNING);
				return -1;
			break;
			case _ARGSX_LowArg:
				printf("%sLow arguments!\n",__WARNING);
				return -1;
			break;
			default:
				if(!Option.pipedInput)
					Option.file_path=argv[ArgsX_ArgPtr];
			break;
		}
		p_argc++;
	}
	if(Option.background&&Option.foreground)
	{
		printf("%sSelect -b or -f\n",__WARNING);
		return -1;
	}

	if(!Option.background&&!Option.foreground)
	{
		printf("%sSelect color with -b [color] or -f [color]\n",__WARNING);
		return -1;
	}

	if(Option.file_path==NULL&&!Option.pipedInput)
	{
		printf("%sMissing File\n",__WARNING);
		return -1;
	}
	
	if(Option.regex==NULL)
	{
		printf("%sMissing Regex\n",__WARNING);
		return -1;
	}

	return Highlighter(&Option);
}

int Highlighter(struct Data_Opt *opt)
{
	regex_t myregex;
	int ret=0;
	int match_type=0;

	if(opt->extended&&opt->insensitive)
		match_type=REG_ICASE|REG_EXTENDED;
	else if(opt->insensitive)
		match_type=REG_ICASE;
	else
		match_type=REG_EXTENDED;

	ret=regcomp(&myregex,opt->regex,match_type);
	if(ret)
	{
		fprintf(stderr, "%sCould not compile regex!\n",__FATAL);
		return -1;
	}

	FILE *stream=NULL;

	if(opt->pipedInput)
		stream=stdin;
	else
	{
		stream=fopen(opt->file_path,"r");
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
		if(rMatch(&myregex,str))
		{
			if(str[strlen(str)-1]=='\n')
			{
				char *strmod=(char*)calloc((strlen(str)-1),sizeof(char));
				strncpy(strmod,str,strlen(str)-1);

				if(opt->foreground)
					fprintf(stdout, "%s%s%s",__ForeColorList[opt->fore_color],strmod,(char*)"\e[0m\n");
				else
					fprintf(stdout, "%s%s%s",__BackColorList[opt->back_color],strmod,(char*)"\e[0m\n");
				free(strmod);
			}
			else
				if(opt->foreground)
					fprintf(stdout, "%s%s%s",__ForeColorList[opt->fore_color],str,(char*)"\e[0m");
				else
					fprintf(stdout, "%s%s%s",__BackColorList[opt->back_color],str,(char*)"\e[0m");
		}
		else
			fprintf(stdout, "%s", str);
	}
	fclose(stream);
	return 0;
}

bool rMatch(regex_t *regcomp,char *str)
{
	return !regexec(regcomp,str,0,NULL,0)?true:false;
}

void usage(FILE *outstream)
{
	fprintf(outstream, "\n%s V: %s\n",__BASENAME,__VERSION);	
	fprintf(outstream, "Highlights the line that contains the pattern using ANSI escape sequence.\n"
					   "Example: %s [option] [file]\n\n",__BASENAME);
	fprintf(outstream, "Regex Pattern interpretation:\n"
					   "-r\tSET REGEX\n"
					   "-i\tIGNORE CASE\n"
					   "-e\tEXTENDED REGEX\n\n");
	fprintf(outstream, "Highlighting:\n"
					   "-f\tFOREGROUND COLOR [color]\n"
					   "-b\tBACKGROUND COLOR [color]\n"
					   "[color]: black,red,green,yellow,blue,violet,cyan,white\n\n");
	fprintf(outstream, "Miscelaneous:\n"
		  			   "-v, --version\tdisplay program version\n"
		  			   "-h, --help\tdisplay this and exit\n\n");
}

void printv(FILE *outstream)
{
	fprintf(outstream, "%s V: %s\n",__BASENAME,__VERSION);
}
