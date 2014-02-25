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

   Statistical labeller: labeller core.

   25-11-92	Created
   24-03-93	Phrase tags added
   01-04-93	Change from using FSMs to parser
   14-04-93	FSMs reinstated
   21-04-93	Move towards chart data structure
   14-05-93	Merged options (in common.h)

   Note: some of the functions and types declared here are in files other than
   label.c.
*/


/*----------------------------------------------------------------------------

    General types and definitions.

----------------------------------------------------------------------------*/

#ifndef LABEL_H
#define LABEL_H
#ifdef __cplusplus
extern "C" {
#endif
/* Type declarations to reduce use of other headers */
/* Full declarations elsewhere */
typedef struct node_st	NodeSt,  *Node;
typedef struct lexeme_st LexemeSt, *Lexeme;
typedef struct link_st	LinkSt,  *Link;
typedef struct shyp_st	SHypSt,  *SHyp;
typedef struct hyp_st	HypSt,   *Hyp;
#ifdef Use_Parser
typedef struct actedge_st ActEdgeSt, *ActEdge;
#endif
#ifdef Use_FSM
typedef struct fsmstate_st FSMStateSt, *FSMState;
#endif



/*----------------------------------------------------------------------------
    Dictionary and words

    Types and definitions for the dictionary access part of the labeller.

----------------------------------------------------------------------------*/

/* Representation of a word. Has a pointer into the dictionary, the actual
   text (NULL if the same as the dictionary text), a kind indicator, the
   correct tag (for tagged corpora), and the contexts, i.e orthographic
   elements preceding and/or following the word which play no part in the
    tagging, but which must be retained for the output.
*/

/* Kinds of word */
typedef enum
{
    Known,		/* Word is in dictionary */
    Unknown,		/* Word is not present in dictionary */
    Special,		/* Word recevies special treatment (numbers) */
    PhraseStart,	/* Start of phrase brackets: only if lancpars set */
    PhraseEnd		/* End of phrase brackets: only if lancpars set */
} WordKind;

typedef struct
{
    WordKind kind;	/* Kind of word */
    DictWord d;		/* Dictionary entry, if Known */
    char    *text;	/* Text for unknown or special entry */
    char    *left;	/* Left context */
    char    *right;	/* Right context */
    Tag      ctag;	/* The correct tag, if tagged input */
} WordSt, *Word;

/* Find the text from a Word structure w or from a lexeme structure lp */

#define WordText(w) (((w)->text == NULL) ? (w)->d->text : (w)->text)
#define LexemeText(lp) (((lp) == NULL) ? (char *)"" : WordText((lp)->word))

/* Set up anchor word */
void set_anchor(Dict *dict);

/*----------------------------------------------------------------------------
    Main tagging functions

    "c_new" is for re-estimation.
----------------------------------------------------------------------------*/

/* Tag an entire corpus */
void tag_corpus(FILE *corpfile, FILE *outfile,
			Dict *dict, Dict *skip_dict,
			Trans *trans, Trans *c_newtrans, DB *dbp, Dict *asc, char tranname[MAXFN]);

/* Tag from one specified node to another */
Score tag(Node start, Node end, Trans *trans, Trans *c_new);

/*----------------------------------------------------------------------------
    High level I/O.
----------------------------------------------------------------------------*/

/* Statistics handlers */
void init_statistics(void);

/* Type for telling output_statistics what to output */
typedef enum
{
    stats_total,
    stats_ambig,
    stats_known,
    stats_unknown,
    stats_ambig_known,
    stats_special
} StatsKind;

/* Statistics output functions */
void report_results(FILE *outfile);

/* Get a word structure */
BOOL fetch_word(FILE *fp, Dict *dict, Dict *skip_dict, Word word, Trans *trans, 
		       DB *dbp, Dict *asc);

/* Output a range of words */
void output_words(FILE *outfile, Lexeme from, Lexeme to, Score obs);

/* Set output threshold */
void set_output_threshold(Score threshold);

/* Set reestimation threshold */
void set_re_est_threshold(Score threshold);
#ifdef __cplusplus
}
#endif
#endif
