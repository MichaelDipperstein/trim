/***************************************************************************
*                   Tab Remover and Trailing Space Trimmer
*
*   File    : trim.c
*   Purpose : Replace tabs with spaces and trim trailing spaces
*   Author  : Michael Dipperstein
*   Date    : November 26, 2006
*
****************************************************************************
*   UPDATES
*
*   $Id: trim.c,v 1.2 2007/09/30 04:54:41 michael Exp $
*   $Log: trim.c,v $
*   Revision 1.2  2007/09/30 04:54:41  michael
*   Replace getopt with optlist.
*   Changes required for LGPL v3.
*
*   Revision 1.1.1.1  2006/12/31 06:14:58  michael
*   Initial Release
*
*
****************************************************************************
*
* Trim: A tab removal and trailing space trimmer
* Copyright (C) 2006, 2007 by
*       Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of Trim.
*
* Trim is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the
* Free Software Foundation; either version 3 of the License, or (at your
* option) any later version.
*
* Trim is distributed in the hope that it will be useful, but WITHOUT ANY
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <optlist.h>

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define LINE_BLOCK  80
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
    char *line;
    int c, pos;
    unsigned int lineSize, tabSize;
    option_t *optList, *thisOpt;

    /* initialize variables */
    inFile = NULL;
    outFile = NULL;
    tabSize = DEFAULT_TAB;

    lineSize = LINE_BLOCK;
    line  = (char *)malloc(lineSize * sizeof(char));

    if (NULL == line)
    {
        perror("Memory allocation");
        return EXIT_FAILURE;
    }

    /* parse command line */
    optList = GetOptList(argc, argv, "t:i:o:h?");
    thisOpt = optList;

    while (thisOpt != NULL)
    {
        switch(thisOpt->option)
        {
            case 't':       /* compression mode */
                tabSize = atoi(thisOpt->argument);
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
                printf("  -t : tab size.\n");
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
    else
    {
        fpIn = stdin;
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
    else
    {
        fpOut = stdout;
    }

    free(outFile);

    if (NULL == fpOut)
    {
        perror(outFile);
        return EXIT_FAILURE;
    }

    /* expand tabs and trim 1 line at a time  */
    while(!feof(fpIn))
    {
        /* initialize line */
        pos = 0;
        line[0] = '\0';

        /* look for tabs to expand */
        while ((c = fgetc(fpIn)) != '\n')
        {
            if (EOF == c)
            {
                break;
            }
            else if ('\t' == c)
            {
                /* found a tab, expand it */
                do
                {
                    line[pos] = ' ';
                    pos++;

                    if (pos == lineSize)
                    {
                        /* need a longer line for storage */
                        lineSize += LINE_BLOCK;
                        line  = (char *)realloc(line, lineSize * sizeof(char));

                        if (NULL == line)
                        {
                            perror("Memory allocation");
                            return EXIT_FAILURE;
                        }
                    }
                } while (pos % tabSize);
            }
            else
            {
                line[pos] = c;
                pos++;

                if (pos == lineSize)
                {
                    /* need a longer line for storage */
                    lineSize += LINE_BLOCK;
                    line  = (char *)realloc(line, lineSize * sizeof(char));

                    if (NULL == line)
                    {
                        perror("Memory allocation");
                        return EXIT_FAILURE;
                    }
                }
            }
        }

        /* remove all white space from the end of the line */
        if ((EOF != c) || (0 != pos))
        {
            line[pos] = '\0';
            pos--;

            while (isspace(line[pos]))
            {
                line[pos] = '\0';

                if (pos > 0)
                {
                    pos--;
                }
                else
                {
                    break;
                }
            }

            if ('\n' == c)
            {
                /* write out line + removed '\n' */
                fprintf(fpOut, "%s\n", line);
            }
            else
            {
                /* write out line without '\n' */
                fprintf(fpOut, "%s", line);
            }
        }
    }

    free(line);
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
