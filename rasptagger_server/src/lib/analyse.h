/******************************************************************************
 * Copyright 1995, 2002, 2006, 2011, 2012 John Carroll, Guido Minnen,         *
 *                                        Larry Piano, David Elworthy         *
 *                                                                            *
 * This file is part of RASP.                                                 *
 *                                                                            *
 * RASP is free software: you can redistribute it and/or modify it            *
 * under the terms of the GNU Lesser General Public License as published      *
 * by the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                        *
 *                                                                            *
 * RASP is distributed in the hope that it will be useful,                    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Lesser General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with RASP.  If not, see <http://www.gnu.org/licenses/>.              *
 ******************************************************************************

   Statistical labeller - error analysis

   04-01-93	Created
*/
#ifndef ANALYSIS_H
#define ANALYSIS_H
#ifdef __cplusplus
extern "C"{
#endif
/* Print the accumulated analysis */
void print_analysis(FILE *out);

/* Add a word to the analysis; call for each word processed; if word is NULL,
   flushes the internal state. */
void analyse_word(BOOL ambig, char wordkind, BOOL correct, Word word,
			 Tag ctag, Tag chosen);

#ifdef __cplusplus
}
#endif
#endif
