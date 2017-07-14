/***************************************************************************
*                   Tab Remover and Trailing Space Trimmer
*
*   File    : trim.c
*   Purpose : Replace tabs with spaces and trim trailing spaces
*   Author  : Michael Dipperstein
*   Date    : November 26, 2006
*
****************************************************************************
*
* Trim: A tab removal and trailing space trimmer
* Copyright (C) 2006, 2007, 2010, 2017 by
*       Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of Trim.
*
* Trim is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 3 of the License, or (at your
* option) any later version.
*
* Trim is distributed in the hope that it will be useful, but WITHOUT ANY
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "optlist/optlist.h"

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define DEFAULT_TAB 4

/***************************************************************************
*                            GLOBAL VARIABLES
**************************************************************************/

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
char *RemovePath(char *fullPath);

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : main
*   Description: This is the main function for this program, it validates
*                the command line input and, if valid, it will open the
*                specified input file and write out a copy with tabs expaned
*                to spaces and trailing spaces trimmed.
*   Parameters : argc - number of parameters
*                argv - parameter list
*   Effects    : Writes version of input file with tabs expanded and trailing
*                spaces removed.
*   Returned   : EXIT_SUCCESS for success, otherwise EXIT_FAILURE.
****************************************************************************/
int main(int argc, char *argv[])
{
    FILE *fpIn, *fpOut;
    char *inFile, *outFile;
    int c, pos, spaces;
    unsigned int tabSize, keepTabs;
    option_t *optList, *thisOpt;

    /* initialize variables */
    inFile = NULL;
    fpIn = stdin;
    outFile = NULL;
    fpOut = stdout;
    tabSize = DEFAULT_TAB;
    keepTabs = 0;

    /* parse command line */
    optList = GetOptList(argc, argv, "t:ki:o:h?");
    thisOpt = optList;

    while (thisOpt != NULL)
    {
        switch(thisOpt->option)
        {
            case 't':       /* tab size */
                tabSize = atoi(thisOpt->argument);
                break;

            case 'k':       /* keep tabs; don't convert them to spaces */
                keepTabs = 1;
                break;

            case 'i':       /* input file name */
                if (inFile != NULL)
                {
                    fprintf(stderr, "Multiple input files not allowed.\n");
                    free(inFile);

                    if (outFile != NULL)
                    {
                        free(outFile);
                    }

                    FreeOptList(optList);
                    return EXIT_FAILURE;
                }
                else if ((inFile =
                    (char *)malloc(strlen(thisOpt->argument) + 1)) == NULL)
                {
                    perror("Memory allocation");

                    if (outFile != NULL)
                    {
                        free(outFile);
                    }

                    FreeOptList(optList);
                    return EXIT_FAILURE;
                }

                strcpy(inFile, thisOpt->argument);
                break;

            case 'o':       /* output file name */
                if (outFile != NULL)
                {
                    fprintf(stderr, "Multiple output files not allowed.\n");
                    free(outFile);

                    if (inFile != NULL)
                    {
                        free(inFile);
                    }

                    FreeOptList(optList);
                    return EXIT_FAILURE;
                }
                else if ((outFile =
                    (char *)malloc(strlen(thisOpt->argument) + 1)) == NULL)
                {
                    perror("Memory allocation");

                    if (inFile != NULL)
                    {
                        free(inFile);
                    }

                    FreeOptList(optList);
                    return EXIT_FAILURE;
                }

                strcpy(outFile, thisOpt->argument);
                break;

            case 'h':
            case '?':
                printf("Usage: %s <options>\n\n", RemovePath(argv[0]));
                printf("Options:\n");
                printf("  -t : Tab size.\n");
                printf("  -k : Keep tabs.  Do not convert them to spaces.\n");
                printf("  -i <filename> : Name of input file.\n");
                printf("  -o <filename> : Name of output file.\n");
                printf("  -h | ?  : Print out command line options.\n\n");
                printf("Default: %s -t4 -i stdin -o stdout\n",
                    RemovePath(argv[0]));

                FreeOptList(optList);
                return EXIT_SUCCESS;
        }

        optList = thisOpt->next;
        free(thisOpt);
        thisOpt = optList;
    }

    /* open file to be trimmed */
    if (NULL != inFile)
    {
        fpIn = fopen(inFile, "r");
    }

    free(inFile);

    if (NULL == fpIn)
    {
        perror(inFile);
        free(outFile);
        return EXIT_FAILURE;
    }

    /* open output file */
    if (NULL != outFile)
    {
        fpOut = fopen(outFile, "w");
    }

    free(outFile);

    if (NULL == fpOut)
    {
        perror(outFile);
        return EXIT_FAILURE;
    }

    pos = 0;
    spaces = 0;

    /* copy file 1 character at a time, replacing tabs and trimming spaces */
    while ((c = fgetc(fpIn)) != EOF)
    {
        switch (c)
        {
            case '\n':
            case '\r':
                /* end of line (maybe other OS format) */
                pos = 0;
                spaces = 0;
                fputc(c, fpOut);
                break;

            case '\t':
                if (keepTabs)
                {
                    if (0 == spaces)
                    {
                        /* I could use the else clause, but this is faster */
                        fputc('\t', fpOut);
                    }
                    else
                    {
                        /* write out leading spaces too */
                        fprintf(fpOut, "%*c", spaces + 1, c);
                        spaces = 0;
                    }

                    pos++;
                }
                else
                {
                    /* convert tab to spaces; compute width of tab */
                    c = tabSize - (pos % tabSize);
                    spaces += c;
                    pos += c;
                }
                break;

            case ' ':
                spaces++;
                pos++;
                break;

            default:
                if (0 == spaces)
                {
                    /* I could use the else clause, but this is faster */
                    fputc(c, fpOut);
                }
                else
                {
                    /* write out leading spaces too */
                    fprintf(fpOut, "%*c", spaces + 1, c);
                    spaces = 0;
                }

                pos++;
                break;
        }
    }

    fclose(fpIn);
    fclose(fpOut);
    return EXIT_SUCCESS;
}

/****************************************************************************
*   Function   : RemovePath
*   Description: This is function accepts a pointer to the name of a file
*                along with path information and returns a pointer to the
*                character that is not part of the path.
*   Parameters : fullPath - pointer to an array of characters containing
*                           a file name and possible path modifiers.
*   Effects    : None
*   Returned   : Returns a pointer to the first character after any path
*                information.
****************************************************************************/
char *RemovePath(char *fullPath)
{
    int i;
    char *start, *tmp;                          /* start of file name */
    const char delim[3] = {'\\', '/', ':'};     /* path deliminators */

    start = fullPath;

    /* find the first character after all file path delimiters */
    for (i = 0; i < 3; i++)
    {
        tmp = strrchr(start, delim[i]);

        if (tmp != NULL)
        {
            start = tmp + 1;
        }
    }

    return start;
}
