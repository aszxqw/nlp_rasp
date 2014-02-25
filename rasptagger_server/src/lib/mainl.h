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

 Statistical labeller - revised version.

 07-12-92	Created
 24-12-92	Restructured
 01-04-93	Change from using FSMs to parser
 14-04-93	FSMs reinstated
 05-04-93	Tag inference option added
 27-01-95     Unknown word handler added

 Changes by Guido Minnen:

 23-03-99     Changed initialization of datastructure to store unknown word 
 statistics
 24-03-99     Additions and adaptions to allow for gdbm 
 lexical lookup and reading in of unknown 
 word statistics
 25-03-99     Adaption of set_up_options to avoid 
 calls with incompatible option specification
 30-06-99     Changes to ensure that transition files are 
 written out and read in in ascii format
 13-12-00	Use Berkeley BD - Chris Hadley

Usage:
label corpus options
*/
#ifndef MAINL_H
#define MAINL_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <assert.h>
#include "common.h"
#include "diction.h"
#include "trans.h"
#include "label.h"
#include "map.h"
#include "low.h"
#include "unkcommon.h"
#include "unkdiction.h"
#include <sys/types.h>
#include "../../database/common/db.h"
#ifdef Analyse
#include "analyse.h"
#endif
#ifdef Use_Parser
#include "parser.h"
#endif
#ifdef Use_FSM
#include "fsm.h"
#endif

/*add by wyy*/
#include<time.h>
#include "options.h"

#define DICTLEN (60000)
#define O_RDONLY        0
#define O_CREAT         0x200

/* Initialisation codes (input option 'I') */
#define Init_d_ntag	(1)
#define	Init_d_tagmax	(2)
#define	Init_t_tagmax	(4)
#define	Init_d_1	(8)
#define	Init_t_1	(16)
#define Init_d_s	(32)

/* For reading in */
#define LineLength 40000

/* File name and structure for unknown word features. */
extern Features  features;

/* Get a word structure */
BOOL fetch_word(FILE *fp, Dict *dict, Dict *skip_dict, Word word, 
			Trans *trans, DB *dbp, Dict *asc);
void init_trans(Trans *trans, Score s, BOOL mul);
void unknown_word_handling_initialization(void);

void gather_unigram_freqs( Dict *dict );
void analyze_features(Dict *dict, char *ofeaturesname, char *obadwordname, Trans *trans, DB *dbp, Dict *asc, char *runkstatname);

#ifdef __cplusplus
}
#endif
#endif
