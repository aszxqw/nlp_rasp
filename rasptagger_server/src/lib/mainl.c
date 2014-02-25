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
#include "mainl.h"

/* File name and structure for unknown word features. */
Features  features;


/*-------------------------------------------------------------------------
  init_trans

  Initialise the trans and pi (but not gamma) arrays.
  --------------------------------------------------------------------------*/

void init_trans(Trans *trans, Score s, BOOL mul)
{
	int size = trans->size;

	set_trans(trans->trans, s, size, mul);
	set_pi(trans->pi, s, size, mul);
}


/*
   ==============================================================================
   Feature memory deallocation functions.
   */

/*-----------------------------------------------------------------------------
  free_transform_list

  Free a transform list's memory.
  -----------------------------------------------------------------------------*/

void free_transform_list( TagTrans *tagtrans )
{
	TagTrans this_tagtrans, next_tagtrans;

	if ( tagtrans != NULL )
	{
		for ( this_tagtrans = *tagtrans, next_tagtrans = this_tagtrans->next; this_tagtrans != NULL; this_tagtrans = next_tagtrans )
		{
			free( this_tagtrans );
			this_tagtrans = NULL;

			if ( next_tagtrans != NULL )
			{
				next_tagtrans = next_tagtrans->next;
			}
		}

		*tagtrans = NULL;
	}
}

/*-----------------------------------------------------------------------------
  free_integrated_transform_list

  Free an integrated transform list's memory.
  -----------------------------------------------------------------------------*/

void free_integrated_transform_list( TagTrans *tagtrans )
{
	TagTrans this_tagtrans, next_tagtrans;

	if ( tagtrans != NULL )
	{
		for ( this_tagtrans = *tagtrans, next_tagtrans = this_tagtrans->next; this_tagtrans != NULL; this_tagtrans = next_tagtrans )
		{
			free_tagscore_list( &(this_tagtrans->source_tag) );
			free_tagscore_list( &(this_tagtrans->transform_tags) );
			free( this_tagtrans );
			this_tagtrans = NULL;

			if ( next_tagtrans != NULL )
			{
				next_tagtrans = next_tagtrans->next;
			}
		}

		*tagtrans = NULL;
	}
}

/*-----------------------------------------------------------------------------
  free_taglist

  Free a tag list's memory.
  -----------------------------------------------------------------------------*/

void free_taglist( TagList *taglist )
{
	int    i;
	TagTag s;

	if ( taglist != NULL )
	{
		/* Free the tag list allocations */
		s  = taglist->s;
		for ( i = 0 ; i < taglist->maxsize ; i++, s++ )
		{
			if ( s->tagtext != NULL )
			{
				free( s->tagtext );
				if ( s->group != NULL )
				{
					free_tagscore_list( &(s->group) );
				}
			}
		}

		free( taglist->s );
		free( taglist->key );

		taglist = NULL;
	}
}

/*-----------------------------------------------------------------------------
  free_afflist

  Free an affix list's memory.
  -----------------------------------------------------------------------------*/

void free_afflist( AffList *afflist )
{
	int    i;
	TagAff s;

	if ( afflist != NULL )
	{
		/* Free the affix list allocations */
		s  = afflist->s;
		for ( i = 0 ; i < afflist->maxsize ; i++, s++ )
		{
			if ( s->affix != NULL )
			{
				free( s->affix );
			}

			if ( s->vanilla_tagscore_list != NULL )
			{
				free_integrated_transform_list( &(s->vanilla_tagscore_list) );
			}

			if ( s->integrated_tagscore_list != NULL )
			{
				free_tagscore_list( &(s->integrated_tagscore_list) );
			}
		}

		free( afflist->s );
		free( afflist->key );

		afflist = NULL;
	}
}

/*-----------------------------------------------------------------------------
  free_cutlist

  Free a cut list's memory.
  -----------------------------------------------------------------------------*/

void free_cutlist( CutList *cutlist )
{
	int    i;
	TagCut s;

	if ( cutlist != NULL )
	{
		/* Free the cut list allocations */
		s  = cutlist->s;
		for ( i = 0 ; i < cutlist->maxsize ; i++, s++ )
		{
			if ( s->cut != NULL )
			{
				free( s->cut );
			}

			if ( s->transform_list != NULL )
			{
				free_transform_list( &(s->transform_list) );
			}

			if ( s->integrated_transform_list != NULL )
			{
				free_integrated_transform_list( &(s->integrated_transform_list) );
			}

			if ( s->special_tags != NULL )
			{
				free_tagscore_list( &(s->special_tags) );
			}
		}

		free( cutlist->s );
		free( cutlist->key );

		cutlist = NULL;
	}
}

/*-----------------------------------------------------------------------------
  free_wordlist

  Free a transform list's memory.
  -----------------------------------------------------------------------------*/

void free_wordlist( IndexWord *start )
{
	IndexWord this_word, next_word;

	if ( start != NULL )
	{
		for ( this_word = *start, next_word = this_word->next; this_word != NULL; this_word = next_word )
		{
			free( this_word );
			this_word = NULL;

			if ( next_word != NULL )
			{
				next_word = next_word->next;
			}
		}
	}
}

/*-----------------------------------------------------------------------------
  free_indexlist

  Free a index list's memory.
  -----------------------------------------------------------------------------*/

void free_indexlist( IndexList *indexlist )
{
	int    i;
	Index  s;

	if ( indexlist != NULL )
	{
		/* Free the index list allocations */
		s  = indexlist->s;
		for ( i = 0 ; i < indexlist->maxsize ; i++, s++ )
		{
			if ( s->prefix != NULL )
			{
				free( s->prefix );
			}

			if ( s->wordlist_start != NULL )
			{
				free_wordlist( &(s->wordlist_start) );
			}
		}

		free( indexlist->s );
		free( indexlist->key );

		indexlist = NULL;
	}
}

/*-----------------------------------------------------------------------------
  free_features

  Free the feature structure memory.
  -----------------------------------------------------------------------------*/

void free_features( Features *features )
{
	if ( features != NULL )
	{
		free_afflist( &((*features)->sufflist ));
		free_afflist( &((*features)->variable_sufflist ));
		free_afflist( &((*features)->separator_sufflist ));
		free_afflist( &((*features)->variable_separator_sufflist ));


		free_cutlist( &((*features)->cut_list) );
		free_cutlist( &((*features)->container_cut_list) );
		free_cutlist( &((*features)->replacement_cut_list) );
		free_cutlist( &((*features)->special_cut_list) );

		free_cutlist( &((*features)->sep_cut_list) );
		free_cutlist( &((*features)->sep_container_cut_list) );
		free_cutlist( &((*features)->sep_replacement_cut_list) );
		free_cutlist( &((*features)->sep_special_cut_list) );

		free_dict( &((*features)->featdict) );
		free_dict( &((*features)->unigramdict) );
		free_dict( &((*features)->unkdict) );
		free_dict( &((*features)->capdict) );
		free_dict( &((*features)->sepdict) );
		free_dict( &((*features)->unkstatdict) );

		free_indexlist( &((*features)->indexlist) );
		free_indexlist( &((*features)->enclosure_indexlist) );
		free_indexlist( &((*features)->partialcap_indexlist) );
		free_indexlist( &((*features)->separator_indexlist) );

		free( features );

		*features = NULL;
	}
}

/*
   ==============================================================================
   Feature print functions.
   */

void print_tags( FILE *ofile, TagScore start, char *heading )
{TagScore     next_tagscore;

	/*  if ( start != NULL ) */
	/*    { */
	if ( heading != NULL )
	{
		fprintf(ofile, "[%s] ", heading);
	}
	for ( next_tagscore = start; next_tagscore != NULL; next_tagscore = next_tagscore->next )
	{
		fprintf(ofile, "%s %.6g ", unmap_tag(next_tagscore->tag), next_tagscore->score);
	}
	/*    } */
}

void print_tag_scores( FILE *ofile, TagTrans start )
{
	TagTrans next_trans;

	for ( next_trans = start; next_trans != NULL; 
				next_trans = next_trans->next )
	{fprintf(ofile, "   ");
		print_tags(ofile,next_trans->source_tag,NULL);
		fprintf(ofile, "* ");
		print_tags( ofile, next_trans->transform_tags, NULL );
		fprintf(ofile, "\n");
	}
}

void print_affixes( FILE *ofile, AffList start )
{
	TagAff   *k = start.key;
	int       i;

	for (i = 0 ; i < start.size ; i++, k++)
	{
		TagAff s = *k;

		if ( s->vanilla_tagscore_list != NULL )
		{
			fprintf(ofile, "%s %g:\n", s->affix, s->total_score);
			print_tag_scores( ofile, s->vanilla_tagscore_list );
		}
	}
}

void print_cuts( FILE *ofile, CutList start )
{
	TagCut   *k = start.key;
	int       i;

	for (i = 0 ; i < start.size ; i++, k++)
	{
		TagCut s = *k;

		if ( s->transform_list != NULL )
		{
			fprintf(ofile, "%s %g:\n", s->cut,
						s->special_total_score);
			print_tag_scores( ofile, s->transform_list );
		}
	}
}

/*-----------------------------------------------------------------------------
  print_words

  Print words to a file.
  -----------------------------------------------------------------------------*/

void print_words( FILE *ofile, IndexWord start )
{
	IndexWord next_word;
	TagScore next_unktag;

	for ( next_word = start; next_word != NULL; next_word = next_word->next )
	{
		fprintf( ofile, "%s ", next_word->word->text );
		for ( next_unktag = next_word->word->unktag; next_unktag != NULL; next_unktag = next_unktag->next)
		{  
			fprintf( ofile, "%s %g ", unmap_tag(next_unktag->tag), next_unktag->score );
		}
		fprintf( ofile, "* " );
	}

	fprintf( ofile, "\n" );
}

/*-----------------------------------------------------------------------------
  print_indices

  Print indexes to a file.
  -----------------------------------------------------------------------------*/

void print_indices( FILE *ofile, IndexList start )
{
	Index   *k = start.key;
	int     i;

	/* Work through the indexes list */
	for (i = 0 ; i < start.size ; i++, k++)
	{
		Index s = *k;

		if ( s->wordlist_start != NULL )
		{
			fprintf(ofile, "%s %d\n", s->prefix, s->wordnum);
			/* 	  (void) printf( "%d: %s %d\n", i, s->prefix, s->wordnum); */

			print_words( ofile, s->wordlist_start );
			/* 	  fprintf( ofile, "\n" ); */
		}
	}
}

/*
   ==============================================================================
   Feature affix gathering functions.
   */

/*----------------------------------------------------------------------------
  add_affix

  Add affixes to an affix list.
  ----------------------------------------------------------------------------*/

BOOL add_affix( BOOL true_capital, BOOL pseudo_capital, BOOL mixed_capital, 
			int mode, AffList *afflist, int suffixlen, char *text, 
			int testpos, TagScore tagscore )
{
	TagAff  s;
	BOOL    success;
	char   *affix = NULL;
	char   modifier[MAX_MODIFIER_LEN], *modified_affix;

	if ( mode == VARIABLE_SUFFIX )
	{
		success = get_variable_suffix( (text+testpos), &affix );
	}
	else
	{
		success = get_affix( mode, (text+testpos), &affix, suffixlen, NULL );
	}

	if ( success )
	{
		set_modifier( modifier, true_capital, pseudo_capital, mixed_capital );

		if ( modifier[0] != '\0' )
		{
			/* Add modifier to the affix: ! indicates a capital, # indicates a pseudo capital. */
			modified_affix = add_chars( affix, modifier );
			s = find_affix( afflist, modified_affix );
			free( modified_affix );
		}
		else
		{
			/* Add the affix to the affix list (or find its hash) */
			s = find_affix(afflist, affix);
		}

		if ( s == NULL )
		  error_exit1("Out of memory creating affix: %s\n", affix);

		/* Add the vanilla tagscore list to the affix record */
		add_transforms( &(s->vanilla_tagscore_list), NULL, tagscore );

		free( affix );
	}

	return success;
}

/*
   ==============================================================================
   Feature index gathering functions.
   */

/*----------------------------------------------------------------------------
  add_word_indexes

  Add word indexes to an index.
  ----------------------------------------------------------------------------*/

void add_word_indexes( DictWord word, IndexWord *start, IndexWord *end )
{
	IndexWord index_list;
	int       testpos;

	/*
	 * If end is empty, add index word to start, else add it to the end.
	 */
	/* Create c_new entry */
	Allocate(index_list, sizeof(IndexWordSt)*sizeof(char), "add word indexes chain");
	index_list->word   = word;
	is_initial( word->text, &testpos );
	index_list->length = (int)strlen((char *)(word->text+testpos));
	index_list->next   = NULL;

	if ( *end == NULL )
	{
		*start = index_list;
		*end   = index_list;
	}
	else
	{
		(*end)->next   = index_list;
		*end           = index_list;
	}

}

/*----------------------------------------------------------------------------
  add_index

  Add indexes to an index list.
  ----------------------------------------------------------------------------*/

BOOL add_index( BOOL downcase_prefix, DictWord word, int testpos, int minlen, 
			IndexList *indexlist )
{
	Index   s;
	char   *prefix, *down_prefix;
	BOOL    success = TRUE;

	success = get_affix( PREFIX, (word->text+testpos), &prefix, minlen, NULL );

	if ( success )
	{
		/* Add the index to the index list (or find its hash) */
		if ( downcase_prefix )
		{
			down_prefix = downcase( prefix );
			s = find_index(indexlist, down_prefix);
			free( down_prefix );
		}
		else
		{
			s = find_index(indexlist, prefix);
		}

		if ( s == NULL )
		  error_exit1("Out of memory creating index prefix: %s\n", prefix);

		(s->wordnum)++;

		/* Add the word indexes to the index record */
		add_word_indexes( word, &(s->wordlist_start), &(s->wordlist_end) );

		free( prefix );
	}

	return success;
}

/*
   ==============================================================================
   Cut gathering functions.
   */

/*-----------------------------------------------------------------------------
  make_special_cuts

  Perform cuts on the specified word for the special affix mode.
  -----------------------------------------------------------------------------*/

void make_special_cuts( BOOL true_capital, BOOL pseudo_capital, BOOL mixed_capital, 
			DictWord d, Dict *dict, char *text, int testpos, 
			CutList *special_cut_list, Trans *trans, DB *dbp, Dict *asc )
{
	int      i, textlen;
	DictWord suffix_dictword = NULL, prefix_dictword = NULL;

	textlen = (int)strlen((char *)(text+testpos));
	for ( i = MinTestLen; (textlen-i >= MinTestLen); i++ )
	{
		BOOL     compound_word_found = FALSE, partial_word = FALSE;

		get_special_cut( dict, i, text, testpos, &prefix_dictword, &suffix_dictword, trans, dbp, asc );

		compound_word_found = (suffix_dictword != NULL) && (prefix_dictword != NULL);
		partial_word = (suffix_dictword != NULL);
		if ( partial_word )
		{
			/* Add the suffix to the special suffix cut list */
			add_cut( true_capital, pseudo_capital, mixed_capital, suffix_dictword->text, 
						special_cut_list, NULL, d->unktag );
		}

		if ( compound_word_found )
		{
			char *prefix = add_chars( "-", prefix_dictword->text );

			/* Add the prefix to the prefix cut list */
			add_cut( true_capital, NO_CAPITAL, mixed_capital, prefix, special_cut_list, 
						suffix_dictword->unktag, d->unktag );

			free( prefix );
		}
	}
}

/*-----------------------------------------------------------------------------
  make_cuts

  Perform cuts on the specified word.
  -----------------------------------------------------------------------------*/

void make_cuts( BOOL true_capital, BOOL pseudo_capital, BOOL mixed_capital, DictWord d, 
			Dict *dict, char *text, int testpos, IndexList indexlist, CutList *cut_list, 
			Trans *trans, DB *dbp, Dict *asc )
{
	DictWord  c_new_dictword = NULL;
	int       i, j, textlen;
	char     *cut = NULL, *smart_cut = NULL;

	/* Perform prefix cuts. */
	if ( (cut = get_cut( PREFIX, indexlist, dict, text, testpos, MinPrefixLen, features->maxprefcut, 
						&c_new_dictword, trans, dbp, asc )) != NULL )
	{
		if ( c_new_dictword != NULL  )
		{
			add_cut( true_capital, pseudo_capital, mixed_capital, cut, cut_list, d->unktag, 
						c_new_dictword->unktag );

			if ( (smart_cut = get_smart_cut( c_new_dictword->text, cut )) != NULL )
			{
				add_cut( true_capital, pseudo_capital, mixed_capital, smart_cut, cut_list, 
							d->unktag, c_new_dictword->unktag );

				free( smart_cut );
			}
		}

		free( cut );
	}

	if ( is_allalpha( (text+testpos) ) )
	{
		/* Perform root cuts. */

		textlen = (int)strlen((char *)(text+testpos));

		for ( i = 1, j = 1; (textlen-(i+j) >= MinRootLen); i++, j++ )
		{
			char *cut = NULL;

			if ( (cut = get_root_cut( dict, i, j, text, testpos, &c_new_dictword, trans, dbp, asc )) 
						!= NULL )
			{
				if ( c_new_dictword != NULL )
				{
					add_cut( true_capital, pseudo_capital, mixed_capital, cut, cut_list, d->unktag, 
								c_new_dictword->unktag );
				}

				free( cut );
			}
		}
	}
}

/*-----------------------------------------------------------------------------
  process_cut

  Compare strings and determine the type of cut, if any.
  -----------------------------------------------------------------------------*/

BOOL process_cut( BOOL true_capital, BOOL pseudo_capital, BOOL mixed_capital, IndexWord scan_word, IndexWord base_word, CutList *cut_list, CutList *container_cut_list, CutList *replacement_cut_list )
{
	BOOL  cut_found = FALSE, scan_compress, base_compress;
	int   i, cutlen;
	int   scan_testpos, base_testpos;
	char *cut = NULL, *reverse_cut = NULL;
	char *scan_text = (scan_word->word)->text, *base_text = (base_word->word)->text;
	char *comptext1, *comptext2;

	scan_compress = ((comptext1 = compress_word( scan_text )) != NULL);
	if ( scan_compress )
	{
		scan_text = comptext1;
	}

	base_compress = ((comptext2 = compress_word( base_text )) != NULL);
	if ( base_compress )
	{
		base_text = comptext2;
	}

	is_initial( base_text, &base_testpos );
	is_initial( scan_text, &scan_testpos );

	cutlen = (int)strlen( (char *)(scan_text+scan_testpos) ) - (int)strlen( (char *)(base_text+base_testpos) );
	if ( (cutlen > 0) && (cutlen <= features->maxsuffcut) )
	{
		cut_found = (strstr( (char *)(scan_text+scan_testpos), (char *)(base_text+base_testpos) ) != NULL);
	}

	if ( cut_found )
	{
		/* Get the word remaining after removing the suffix */
		if ( get_affix( SUFFIX, (scan_text+scan_testpos), &cut, cutlen, NULL ) )
		{
			char *smart_cut = NULL;

			/* Add cut to the container cut list. */
			add_cut( true_capital, pseudo_capital, mixed_capital, cut, container_cut_list, (base_word->word)->unktag, (scan_word->word)->unktag );

			/* Add cut to the cut list. */
			add_cut( true_capital, pseudo_capital, mixed_capital, cut, cut_list, (scan_word->word)->unktag, (base_word->word)->unktag );

			if ( (smart_cut = get_smart_cut( (base_text+base_testpos), cut )) != NULL )
			{
				/* Add cut to the smart cut list. */
				add_cut( true_capital, pseudo_capital, mixed_capital, smart_cut, cut_list, (scan_word->word)->unktag, (base_word->word)->unktag );

				free( smart_cut );
			}

			free( cut );
		}
	}
	else if ( replacement_cut_list != NULL )
	{
		BOOL  comparison_valid = TRUE;
		char *base_suffix, *scan_suffix;
		int   scanlen, baselen;

		for ( i = MinTestLen; (scan_text+scan_testpos)[i] == (base_text+base_testpos)[i]; i++ )
		{
			if ( ((scan_text+scan_testpos)[i] == '\0') || ((base_text+base_testpos)[i] == '\0') )
			{
				comparison_valid = FALSE;
				break;
			}
		}

		scanlen = (scan_word->length-i);
		baselen = (base_word->length-i);

		if ( (scanlen <= 0) || (baselen <= 0) )
		{
			comparison_valid = FALSE;
		}

		if ( (scanlen <= features->maxsuffcut) && (baselen <= features->maxsuffcut) && comparison_valid && (i >= MinCutLen) )
		{
			cut_found = TRUE;

			if ( get_affix( SUFFIX, (base_text+base_testpos), &base_suffix, baselen, NULL ) )
			{
				if ( get_affix( SUFFIX, (scan_text+scan_testpos), &scan_suffix, scanlen, NULL ) )
				{
					cut = add_chars( scan_suffix, base_suffix );
					add_cut( true_capital, pseudo_capital, mixed_capital, cut, replacement_cut_list, (base_word->word)->unktag, (scan_word->word)->unktag );
					free( cut );

					reverse_cut = add_chars( base_suffix, scan_suffix );
					add_cut( true_capital, pseudo_capital, mixed_capital, reverse_cut, replacement_cut_list, (scan_word->word)->unktag, (base_word->word)->unktag );
					free( reverse_cut );

					free( scan_suffix );
				}

				free( base_suffix );
			}
		}
	}

	if ( scan_compress )
	{
		free( comptext1 );
	}

	if ( base_compress )
	{
		free( comptext2 );
	}

	return cut_found;
}

/*-----------------------------------------------------------------------------
  compare_words

  Compare words in a list.
  -----------------------------------------------------------------------------*/

void compare_words( IndexWord base_word, IndexWord word_list, IndexWord *stop_word, CutList *cut_list, CutList *container_cut_list, CutList *replacement_cut_list )
{
	IndexWord scan_word;

	for ( scan_word = word_list; scan_word != NULL; scan_word = scan_word->next )
	{		  
		if ( scan_word == *stop_word )
		{
			break;
		}

		if ( !process_cut( NO_CAPITAL, NO_CAPITAL, NO_CAPITAL, scan_word, base_word, cut_list, container_cut_list, replacement_cut_list ) )
		{
			*stop_word = scan_word;
			break;
		}

	}
}

/*-----------------------------------------------------------------------------
  compare_capital_words

  Compare words in a list.
  -----------------------------------------------------------------------------*/

void compare_capital_words( IndexWord base_word, IndexWord word_list, IndexWord *stop_word, CutList *cut_list, CutList *container_cut_list, CutList *replacement_cut_list )
{
	BOOL      skip = FALSE, scan_capital = FALSE, scan_mixed_capital = FALSE, base_capital = FALSE, base_mixed_capital = FALSE, scan_compress = FALSE, base_compress = FALSE;
	IndexWord scan_word;
	int       scan_testpos, base_testpos;
	char     *scan_text, *base_text;
	char     *comptext1, *comptext2;

	base_text = (base_word->word)->text;
	base_compress = ((comptext2 = compress_word( base_text )) != NULL);
	if ( base_compress )
	{
		base_text = comptext2;
	}

	is_initial( base_text, &base_testpos );
	base_capital = (search_chain( (base_word->word)->tag, features->max_capital->tag ) != 0);
	if ( !base_capital )
	{
		base_mixed_capital = (contains_capitals( (base_text+base_testpos) ) && !contains_numbers( (base_text+base_testpos) ) && (strpbrk( (char *)(base_text+base_testpos), SpecialChars ) != NULL));
	}

	for ( scan_word = word_list; scan_word != NULL; scan_word = scan_word->next )
	{		  
		if ( scan_word == *stop_word )
		{
			break;
		}

		scan_text = (scan_word->word)->text;
		scan_compress = ((comptext1 = compress_word( scan_text )) != NULL);
		if ( scan_compress )
		{
			scan_text = comptext1;
		}

		skip = scan_capital = scan_mixed_capital = FALSE;

		is_initial( scan_text, &scan_testpos );
		scan_capital = (search_chain( (scan_word->word)->tag, features->max_capital->tag ) != 0);
		if ( !(scan_capital || base_mixed_capital) )
		{
			scan_mixed_capital = (contains_capitals( (scan_text+scan_testpos) ) && !contains_numbers( (scan_text+scan_testpos) ) && (strpbrk( (char *)(scan_text+scan_testpos), SpecialChars ) != NULL));
		}


		if ( (base_testpos && !scan_testpos) || (!base_testpos && scan_testpos) )
		{
			skip = (strcmp( (char *)(scan_text+scan_testpos), (char *)(base_text+base_testpos) ) == 0);
		}

		if ( !skip )
		{
			skip = ( (scan_capital && !base_capital) || (!scan_capital && base_capital) );
		}

		if ( !skip )
		{
			if ( !process_cut( base_capital, NO_CAPITAL, (base_mixed_capital || scan_mixed_capital), scan_word, base_word, cut_list, container_cut_list, replacement_cut_list ) )
			{
				if ( !scan_testpos && !base_testpos )
				{
					*stop_word = scan_word;
					break;
				}
				else if ( !scan_testpos )
				{
					if ( (strcmp( (char *)scan_text, (char *)(base_text+base_testpos) ) > 0) )
					{
						break;
					}
				}
			}
		}
	}

	if ( scan_compress )
	{
		free( comptext1 );
	}

	if ( base_compress )
	{
		free( comptext2 );
	}
}

/*-----------------------------------------------------------------------------
  make_other_cuts

  Perform replacement and container cuts on the specified word, using the word indexes.
  -----------------------------------------------------------------------------*/

void make_other_cuts( IndexList indexlist, CutList *cut_list, CutList *container_cut_list, CutList *replacement_cut_list )
{
	BOOL      capital_list;
	Index     *k = indexlist.key;
	IndexWord next_word, stop_word;
	int       i;
	char     *capital_test;

	/* Work through the indexes list */
	for (i = 0 ; i < indexlist.size ; i++, k++)
	{
		Index s = *k;

		capital_list = FALSE;
		capital_test = string_dup( s->prefix );
		capital_test[MinTestLen] = '\0';
		if ( is_capital( capital_test ) || is_allcaps( capital_test ) )
		{
			capital_list = TRUE;
		}

		free( capital_test );


		if ( (s->wordnum > 1) && (s->wordlist_start != NULL) )
		{
			stop_word = NULL;
			for ( next_word = s->wordlist_start; next_word != NULL; next_word = next_word->next )
			{
				if ( next_word == stop_word )
				{
					stop_word = NULL;
				}

				if ( capital_list )
				{
					compare_capital_words( next_word, next_word->next, &stop_word, cut_list, container_cut_list, replacement_cut_list );
				}
				else
				{
					compare_words( next_word, next_word->next, &stop_word, cut_list, container_cut_list, replacement_cut_list );
				}
			}
		}
	}
}

/*
   ==============================================================================
   Feature analysis functions.
   */

/*-----------------------------------------------------------------------------
  log_feature

  Log selected features into the feature structure.
  -----------------------------------------------------------------------------*/

void log_feature( BOOL skip_closed, BOOL feat_exists, char *featname, TagScore tagscore )
{
	if ( feat_exists )
	{
		add_unkword( &(features->featdict), featname, skip_closed, tagscore, NULL );
	}
}

/*-----------------------------------------------------------------------------
  unknown_word_handling_initialization

  Initialize various feature items.
  -----------------------------------------------------------------------------*/

void unknown_word_handling_initialization( void )
{
	features->unigramtotal = features->unigram_open_total = 0;

	/* Create and clear dictonaries. */
	/* Already processed unknown words. */
	InitDict((features->unkdict));
	create_dict(&(features->unkdict), (features->maxunkwords));
	clear_dict(&(features->unkdict));

	/* Unigram tag probabilities. */
	InitDict((features->unigramdict));
	create_dict(&(features->unigramdict), tags_max);
	clear_dict(&(features->unigramdict));
}

/*-----------------------------------------------------------------------------
  initialize_features

  Initialize various feature items.
  -----------------------------------------------------------------------------*/

void initialize_features( void/* Dict *dict */ )
{
	features->badwordfile_open = FALSE;
	features->all_wordnum = features->cap_wordnum = features->aff_wordnum = features->cut_wordnum = features->container_cut_wordnum = features->separator_wordnum = features->separator_aff_wordnum = features->separator_cut_wordnum = features->separator_container_cut_wordnum = 0;

	/* Create and clear dictonaries. */
	/* Word features. */
	InitDict((features->featdict));
	create_dict(&(features->featdict), MAXFEATURES);
	clear_dict(&(features->featdict));

	/* Unknown word feature statistics. */
	InitDict((features->unkstatdict));
	create_dict(&(features->unkstatdict), MAXFEATURES);
	clear_dict(&(features->unkstatdict));

	/* Set up main index list */
	InitList( features->indexlist );
	create_indexlist( &(features->indexlist), DICTSIZE );
	clear_indexlist( &(features->indexlist) );

	InitList( features->enclosure_indexlist );
	create_indexlist( &(features->enclosure_indexlist), DICTSIZE );
	clear_indexlist( &(features->enclosure_indexlist) );

	InitList( features->partialcap_indexlist );
	create_indexlist( &(features->partialcap_indexlist), DICTSIZE );
	clear_indexlist( &(features->partialcap_indexlist) );

}

/*-----------------------------------------------------------------------------
  initialize_other_features

  Initialize various other feature items.
  -----------------------------------------------------------------------------*/

void initialize_other_features( void )
{
	int cutlist_multiplier;

	/* Separator word endings. */
	InitDict((features->sepdict));
	create_dict(&(features->sepdict), (features->separator_wordnum));
	clear_dict(&(features->sepdict));

	/* Pure forms of capital words. */
	InitDict((features->capdict));
	create_dict( &(features->capdict), (features->cap_wordnum) );
	clear_dict(&(features->capdict));

	/* Set up separator index list */
	InitList( features->separator_indexlist );
	create_indexlist( &(features->separator_indexlist), (int)(1.0 * (float)(features->separator_wordnum)) );
	clear_indexlist( &(features->separator_indexlist) );

	/* Set up affix lists */
	InitList( features->sufflist );
	create_afflist( &(features->sufflist), (int)(1.0 * (float)(features->aff_wordnum)) );
	clear_afflist( &(features->sufflist) );

	InitList( features->variable_sufflist );
	create_afflist( &(features->variable_sufflist), (int)(1.0 * (float)(features->aff_wordnum)) );
	clear_afflist( &(features->variable_sufflist) );

	InitList( features->separator_sufflist );
	create_afflist( &(features->separator_sufflist), (int)(1.0 * (float)(features->separator_aff_wordnum)) );
	clear_afflist( &(features->separator_sufflist) );

	InitList( features->variable_separator_sufflist );
	create_afflist( &(features->variable_separator_sufflist), (int)(1.0 * (float)(features->separator_aff_wordnum)) );
	clear_afflist( &(features->variable_separator_sufflist) );

	/* Set up cut lists */
	InitList( features->cut_list );
	InitList( features->container_cut_list );
	InitList( features->replacement_cut_list );
	InitList( features->special_cut_list );

	InitList( features->sep_cut_list );
	InitList( features->sep_container_cut_list );
	InitList( features->sep_replacement_cut_list );
	InitList( features->sep_special_cut_list );

	cutlist_multiplier = (int)(((float)features->maxsuffcut + 0.5) / 2) + 1;

	create_cutlist( &(features->cut_list), (cutlist_multiplier * features->cut_wordnum) );
	create_cutlist( &(features->container_cut_list), (int)(1.0 * (float)(features->cut_wordnum)) );
	create_cutlist( &(features->replacement_cut_list), (cutlist_multiplier * features->cut_wordnum) );
	create_cutlist( &(features->special_cut_list), (int)(0.5 * (float)(features->cut_wordnum)) );

	create_cutlist( &(features->sep_cut_list), (int)(1.0 * (float)(features->separator_cut_wordnum)) );
	create_cutlist( &(features->sep_container_cut_list), (int)(1.0 * (float)(features->separator_cut_wordnum)) );
	create_cutlist( &(features->sep_replacement_cut_list), (int)(1.0 * (float)(features->separator_cut_wordnum)) );
	create_cutlist( &(features->sep_special_cut_list), (int)(0.5 * (float)(features->separator_cut_wordnum)) );

	clear_cutlist( &(features->cut_list) );
	clear_cutlist( &(features->container_cut_list) );
	clear_cutlist( &(features->replacement_cut_list) );
	clear_cutlist( &(features->special_cut_list) );

	clear_cutlist( &(features->sep_cut_list) );
	clear_cutlist( &(features->sep_container_cut_list) );
	clear_cutlist( &(features->sep_replacement_cut_list) );
	clear_cutlist( &(features->sep_special_cut_list) );
}


void print_max_cap( FILE  *wsf )
{TagScore ptr;

	ptr = (TagScoreSt *)malloc(sizeof(TagScoreSt));

	fprintf(wsf, "[max_cap]\n");
	for (ptr = features->max_capital; ptr != NULL; ptr = ptr->next)
	{ fprintf(wsf, "%s %g\n", unmap_tag(ptr->tag), ptr->score);
	}
	fprintf(wsf, "\n\n");
}

void print_dictionary(FILE *wsf, Dict dictionary)
{DictWord     *k = dictionary.key;
	int          i;

	for ( i = 0 ; i < dictionary.size ; i++, k++ )
	{
		DictWord d = *k;
		if (d->tag != NULL)
		{print_tags( wsf, d->tag, d->text );
			fprintf(wsf, "\n");
		}
	}
}

void print_gamma(FILE *wsf)
{int i; 

	for (i = 0; i < 400 ; i++)
	{ fprintf(wsf,"%g ", features->gamma[i]);
	}
	fprintf(wsf, "\n\n");
}

/*-----------------------------------------------------------------------------
  write_features

  Write out features file.
  -----------------------------------------------------------------------------*/

void write_features( char *wunkstatname )
{FILE         *wsf;

	wsf = open_file(wunkstatname, "w");

	fprintf(wsf, "%d %d %d %d\n",
				(features->sepdict).size,
				(features->featdict).size, 
				(features->capdict).size,
				(features->unkstatdict).size);
	fprintf(wsf, "%d %d %d %d\n",
				(features->indexlist).size,
				(features->enclosure_indexlist).size,
				(features->partialcap_indexlist).size,
				(features->separator_indexlist).size);
	fprintf(wsf, "%d %d %d %d %d\n",
				(features->cut_list).size,
				(features->container_cut_list).size, 
				(features->replacement_cut_list).size, 
				(features->special_cut_list).size,
				(features->smart_cut_list).size); 
	fprintf(wsf, "%d %d %d %d\n",
				(features->sep_cut_list).size,
				(features->sep_container_cut_list).size, 
				(features->sep_replacement_cut_list).size, 
				(features->sep_special_cut_list).size);
	fprintf(wsf, "%d %d %d %d\n\n\n",
				(features->sufflist).size,
				(features->variable_sufflist).size,
				(features->separator_sufflist).size,
				(features->variable_separator_sufflist).size);
	fprintf(wsf, "%d %d %d %d\n",
				features->initials_exist, 
				features->badwordfile_open, 
				features->all_wordnum, 
				features->cap_wordnum);
	fprintf(wsf, "%d %d %d %d\n",
				features->aff_wordnum, 
				features->cut_wordnum, 
				features->container_cut_wordnum, 
				features->replacement_cut_wordnum); 
	fprintf(wsf, "%d %d %d %d %d\n",
				features->separator_wordnum, 
				features->separator_aff_wordnum, 
				features->separator_cut_wordnum, 
				features->separator_container_cut_wordnum, 
				features->separator_replacement_cut_wordnum); 
	fprintf(wsf, "%d %d %d %d %d\n",
				features->maxprefix, 
				features->maxsuffix, 
				features->maxprefcut, 
				features->maxsuffcut, 
				features->maxunkwords);
	fprintf(wsf, "%f %f\n",
				features->unigramtotal, 
				features->unigram_open_total);
	fprintf(wsf, "%s\n\n\n",features->type_info);

	print_gamma(wsf); 

	print_max_cap(wsf);

	fprintf(wsf, "[sepdict]\n");
	print_dictionary(wsf,features->sepdict);
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[featdict]\n");
	print_dictionary(wsf,features->featdict);
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[capdict]\n");
	print_dictionary(wsf,features->capdict);
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[unkdict]\n");
	print_dictionary(wsf,features->unkdict);
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[unkstatdict]\n");
	print_dictionary(wsf,features->unkstatdict);
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[unigramdict]\n");
	print_dictionary(wsf,features->unigramdict);
	fprintf(wsf, "\n\n");

	fprintf(wsf, "[indices]\n");
	print_indices( wsf, features->indexlist );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[enclosure indices]\n");
	print_indices( wsf, features->enclosure_indexlist );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[partial capital indices]\n");
	print_indices( wsf, features->partialcap_indexlist );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[separator indices]\n");
	print_indices( wsf, features->separator_indexlist );
	fprintf(wsf, "\n\n");

	fprintf(wsf, "[cuts]\n");
	print_cuts( wsf, features->cut_list );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[container cuts]\n");
	print_cuts( wsf, features->container_cut_list );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[replacement cuts]\n");
	print_cuts( wsf, features->replacement_cut_list );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[special cuts]\n");
	print_cuts( wsf, features->special_cut_list );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[smart cuts]\n");
	print_cuts( wsf, features->smart_cut_list );
	fprintf(wsf, "\n\n");

	fprintf(wsf, "[separator cuts]\n");
	print_cuts( wsf, features->sep_cut_list );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[separator container cuts]\n");
	print_cuts( wsf, features->sep_container_cut_list );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[separator replacement cuts]\n");
	print_cuts( wsf, features->sep_replacement_cut_list );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[separator special cuts]\n");
	print_cuts( wsf, features->sep_special_cut_list );
	fprintf(wsf, "\n\n");

	fprintf(wsf, "[affixes]\n");
	print_affixes( wsf, features->sufflist );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[variable suffixes]\n");
	print_affixes( wsf, features->variable_sufflist );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[separator suffixes]\n");
	print_affixes( wsf, features->separator_sufflist );
	fprintf(wsf, "\n\n");
	fprintf(wsf, "[variable separator suffixes]\n");
	print_affixes( wsf, features->variable_separator_sufflist );
	fprintf(wsf, "\n\n");

	(void) fclose(wsf);
}

/*-----------------------------------------------------------------------------
  check_pure_word

  Determine if an unknown word needs to be store in its "pure" form.
  -----------------------------------------------------------------------------*/

BOOL check_pure_word( BOOL needs_compression, DictWord d, char *vanilla_text, 
			char *down_vanilla_text, int testpos )
{
	BOOL success = FALSE;

	if ( contains_capitals( (vanilla_text+testpos) ) )
	{
		TagScore captags = NULL;

		/* Store pure word form(s) in capital dictionary */ 
		captags = filter_tags( d->tag );
		add_unkword( &(features->capdict), (down_vanilla_text+testpos), SKIP_CLOSED_TAGS, captags, NULL );

		if ( needs_compression )
		{
			char *down_comptext = NULL;

			down_comptext = compress_word( (down_vanilla_text+testpos) );
			add_unkword( &(features->capdict), down_comptext, SKIP_CLOSED_TAGS, captags, NULL );
			free( down_comptext );
		}

		free_tagscore_list( &captags );
	}

	return success;
}

/*-----------------------------------------------------------------------------
  gather_unigram_freqs

  First run through the lexicon to gather unigram frequencies.
  -----------------------------------------------------------------------------*/

void gather_unigram_freqs( Dict *dict )
{
	DictWord *k = dict->key;
	TagScore next_tagscore = NULL;
	int      i;

	/* Work through the dictionary to get feature frequencies */
	for ( i = 0 ; i < dict->size ; i++, k++ )
	{
		DictWord d = *k;

		/* Get unigram frequencies and total tag score */
		for ( next_tagscore = d->tag; next_tagscore != NULL; next_tagscore = next_tagscore->next )
		{
			TagScore tagscore;

			Allocate(tagscore, sizeof(TagScoreSt), "tagscore: gather_unigram_freqs");

			tagscore->tag = next_tagscore->tag;
			tagscore->score = next_tagscore->score;
			tagscore->next = NULL;

			features->unigramtotal += tagscore->score;

			if ( !is_closed( next_tagscore->tag ) )
			{
				features->unigram_open_total += tagscore->score;
			}

			add_unkword( &(features->unigramdict), unmap_tag(next_tagscore->tag), NO_SKIP_CLOSED_TAGS, tagscore, NULL );

			free( tagscore );
		}
	}

	sort_dict( &(features->unigramdict) );
}

/*-----------------------------------------------------------------------------
  gather_initial_freqs

  Second run through the lexicon to gather initial feature frequencies.
  -----------------------------------------------------------------------------*/

void gather_initial_freqs( Dict *dict, Trans *trans, DB *dbp, Dict *asc)
{
	DictWord *k = dict->key, feature_entry = NULL, feature_entry1 = NULL;
	BOOL     initial, capital, allcaps, true_capital, valid_length = FALSE;
	int      i, testpos, textlen;
	char    *text, *vanilla_text;

	/* Work through the dictionary to get feature frequencies */
	for ( i = 0 ; i < dict->size ; i++, k++ )
	{
		DictWord d = *k;
		BOOL     has_numbers = FALSE, all_numbers = FALSE, has_alpha = FALSE, needs_compression = FALSE, has_separator = FALSE;
		char     *compressable_ptr = strpbrk( (char *)d->text, CompressableChars );
		char     *sepptr = strpbrk( (char *)d->text, SeparatorChars );
		char    *repeating_chars = NULL, *c_new_suffix_word = NULL, *c_new_prefix_word = NULL;
		char    *end_text = NULL;
		int      sepcharnum = 0;

		vanilla_text = d->text;
		text = d->text;

		/* Check for sentence-initial marker */
		/* testpos is starting position in text for comparison. */
		initial = is_initial( vanilla_text, &testpos );
		textlen = (int)strlen((char *)(vanilla_text+testpos));
		valid_length = (textlen >= MinTestLen);
		has_numbers = contains_numbers( vanilla_text );
		has_alpha = contains_alpha( vanilla_text );
		all_numbers = is_allnums( (vanilla_text+testpos) );

		if ( sepptr != NULL )
		{
			end_text = (char *)strrchr( vanilla_text, *sepptr );
		}

		if ( end_text != NULL )
		{
			end_text++;
		}

		if ( initial )
		{
			features->initials_exist = TRUE;
		}

		/* Check if there is a separator in the text. */
		if ( (has_alpha || has_numbers) && (sepptr != NULL) )
		{
			has_separator = TRUE;
			sepcharnum = contains_this_char( vanilla_text, *sepptr );
		}

		/* Take snapshot of dictionary tags with unadjusted scores */
		add_chain_tags( ADD, &(d->unktag), NULL, d->tag, NO_SKIP_CLOSED_TAGS, NULL, "gather_initial_freqs" );

		/* Singletons */
		log_feature( SKIP_CLOSED_TAGS, (d->ntag == 1 && (d->tag)->score == 1), "singletons", d->tag );

		/* Add word to the index. */
		if ( !has_numbers && has_alpha )
		{if ( add_index( NO_DOWNCASE, d, testpos, MinTestLen, &(features->indexlist) ) )
			{
				(features->all_wordnum)++;

				if ( (int)strlen( (vanilla_text+testpos) ) >= MinCutLen )
				{
					(features->cut_wordnum)++;
				}

				if ( (int)strlen( (vanilla_text+testpos) ) >= MinAffWord )
				{
					(features->aff_wordnum)++;
				}

				if (  (strchr( MixedCapitalChars, (vanilla_text+testpos)[0] ) != NULL) && contains_capitals( (vanilla_text+testpos) ) )
				{
					add_index( DOWNCASE, d, testpos, MinTestLen, &(features->partialcap_indexlist) );
				}
			}
		}
		else if ( is_enclosure( (vanilla_text+testpos) ) )
		{ add_index( NO_DOWNCASE, d, testpos, MinTestLen, &(features->enclosure_indexlist) );
		}

		/* Count all alpha end words for words with separators. */
		if ( end_text != NULL )
		{
			BOOL  end_text_cut_valid = FALSE, end_text_affix_valid = FALSE;

			end_text_cut_valid = ((int)strlen((char *)end_text) >= MinCutLen) && is_allalpha( end_text );
			end_text_affix_valid = ((int)strlen((char *)end_text) >= MinAffWord) && is_allalpha( end_text );

			if ( is_allalpha( end_text ) )
			{
				(features->separator_wordnum)++;

				if ( (int)strlen( end_text ) >= MinCutLen )
				{
					(features->separator_cut_wordnum)++;
				}

				if ( (int)strlen( end_text ) >= MinAffWord )
				{
					(features->separator_aff_wordnum)++;
				}
			}
		}

		/* Check if a separator character is present */
		if ( has_separator )
		{
			/* A separator character is present */
			if ( make_affix_words( vanilla_text, testpos, &c_new_prefix_word, &c_new_suffix_word ) )
			{
				BOOL pref_capital, suff_capital, pref_allcaps, suff_allcaps, pref_allnum, suff_allnum, suff_lower, goodpreflen, goodsufflen;

				/*Alphanumeric Features */

				goodpreflen  = (int)strlen((char *)c_new_prefix_word) >= MinTestLen;
				goodsufflen  = (int)strlen((char *)c_new_suffix_word) >= MinTestLen;

				pref_allnum  = is_allnums( c_new_prefix_word );
				suff_allnum  = is_allnums( c_new_suffix_word );

				log_feature( NO_SKIP_CLOSED_TAGS, (pref_allnum && (!suff_allnum && goodsufflen)), "number-string", d->tag );

				log_feature( NO_SKIP_CLOSED_TAGS, (pref_allnum && suff_allnum), "number-number", d->tag );

				if ( sepcharnum == 1 )
				{
					/*Alphanumeric Features */
					pref_capital = is_capital( c_new_prefix_word );
					suff_capital = is_capital( c_new_suffix_word );

					pref_allcaps = is_allcaps( c_new_prefix_word );
					suff_allcaps = is_allcaps( c_new_suffix_word );

					suff_lower   = (is_allalpha( c_new_suffix_word ) && !is_allcaps( c_new_suffix_word ) && !is_capital( c_new_suffix_word ));

					log_feature( NO_SKIP_CLOSED_TAGS, (pref_capital && suff_capital), "capital-capital", d->tag );

					log_feature( NO_SKIP_CLOSED_TAGS, (pref_allcaps && suff_allcaps), "allcaps-allcaps", d->tag );

					log_feature( NO_SKIP_CLOSED_TAGS, (pref_allcaps && suff_lower), "allcaps-lowercase", d->tag );

					log_feature( NO_SKIP_CLOSED_TAGS, (pref_allcaps && suff_allnum), "allcaps-numbers", d->tag );
				}
			}
		}

		/* Capitalization */
		if ( valid_length )
		{
			if ( (valid_length && (has_alpha || has_numbers) && (compressable_ptr != NULL)) )
			{
				/* Text needs compression */
				needs_compression = (text = compress_word( vanilla_text )) != NULL;
			}

			/* Count mixed words containing capitals. */
			if ( contains_capitals( (vanilla_text+testpos) ) )
			{
				(features->cap_wordnum)++;

				if ( needs_compression )
				{
					(features->cap_wordnum)++;
				}
			}
		}

		capital         = is_capital( (text+testpos) );
		allcaps         = is_allcaps( (text+testpos) );
		true_capital    = (capital && !initial);

		log_feature( SKIP_CLOSED_TAGS, true_capital, "true capital", d->tag );
		log_feature( SKIP_CLOSED_TAGS, (!(capital || allcaps) && is_allalpha( (text+testpos) )), "not capital", d->tag );

		if ( all_numbers )
		{
			/* All numeric characters */
			log_feature( NO_SKIP_CLOSED_TAGS, TRUE, "contains numbers only", d->tag );
		}
		else if ( (repeating_chars = contains_repeating_consecutives( (vanilla_text+testpos) )) != NULL )
		{
			/* Repeating consecutive characters */
			log_feature( NO_SKIP_CLOSED_TAGS, TRUE, repeating_chars, d->tag );
			free ( repeating_chars );
		}
		else if ( check_time_format( (vanilla_text+testpos) ) )
		{
			/* Time format */
			log_feature( NO_SKIP_CLOSED_TAGS, TRUE, "time format", d->tag );
		}
		else if ( check_currency_format( (vanilla_text+testpos), 2, 2 ) )
		{
			/* Currency format */
			log_feature( NO_SKIP_CLOSED_TAGS, TRUE, "currency format", d->tag );
		}
		else if ( check_ordinal( (vanilla_text+testpos), OrdinalSuffix ) )
		{
			/* Ordinal number */
			log_feature( NO_SKIP_CLOSED_TAGS, TRUE, "ordinal number", d->tag );
		}
		else if ( check_cardinal( (vanilla_text+testpos) ) )
		{
			/* Cardinal number */
			log_feature( NO_SKIP_CLOSED_TAGS, TRUE, "cardinal number", d->tag );
		}

		if ( c_new_suffix_word != NULL )
		{
			free( c_new_suffix_word );
		}

		if ( c_new_prefix_word != NULL )
		{
			free( c_new_prefix_word );
		}

		if ( needs_compression )
		{
			free( text );
		}
	} /* End of working through the dictionary */

	/* Sort the feature dictionaries */
	sort_dict( &(features->featdict) );
	sort_dict( &(features->unigramdict) );

	/* Sort the index lists */
	sort_indexlist( &(features->indexlist) );
	sort_indexlist( &(features->enclosure_indexlist) );
	sort_indexlist( &(features->partialcap_indexlist) );

	/* Save the capital tag which has the maximum score to determine if a word is a "true" capital. */ 
	features->max_capital = NULL;
	feature_entry = search_unkdict( &(features->featdict), "true capital", trans, dbp, asc );
	if ( feature_entry != NULL )
	{
		features->max_capital = get_max_tag( feature_entry->tag );
		features->max_capital->next = NULL;
	}

	/* Filter the non-capital tags to set the initial tag hypotheses for lowercase unknown words. */
	feature_entry1 = search_unkdict( &(features->featdict), "not capital", trans, dbp, asc );
	if ( feature_entry1 != NULL )
	{
		TagScore noncaptags;

		noncaptags = filter_tags( feature_entry1->tag );
		log_feature( SKIP_CLOSED_TAGS, TRUE, "not capital - filtered", noncaptags );

		free_tagscore_list( &noncaptags );
	}
}

/*-----------------------------------------------------------------------------
  gather_other_freqs

  Using the initial feature frequencies, go through the lexicon
  again to get other feature frequencies.

  -----------------------------------------------------------------------------*/

void gather_other_freqs( Dict *dict, Trans *trans, DB *dbp, Dict *asc )
{
	BOOL     initial, capital, allcaps, initial_capital, true_capital, valid_length = FALSE;
	DictWord *k = dict->key;
	int      i, testpos, textlen;
	char    *comptext, *text, *vanilla_text;

	/* Work through the dictionary to get frequencies for selected features */
	for ( i = 0 ; i < dict->size ; i++, k++ )
	{
		DictWord d = *k;
		BOOL     all_alpha, all_numbers, has_numbers = contains_numbers( d->text ), 
				 has_alpha = contains_alpha( d->text ), needs_compression = FALSE, has_separator = FALSE, 
				 pseudo_capital = FALSE, has_special_char = FALSE;
		BOOL     use_separator_suffix = FALSE, separator_suffix_capital = FALSE;
		BOOL     mixed_capital = FALSE;
		char     *compressable_ptr = strpbrk( (char *)d->text, CompressableChars );
		char     *sepptr = strpbrk( (char *)d->text, SeparatorChars );
		char     *specptr;
		char    *c_new_suffix_word = NULL, *c_new_prefix_word = NULL, *down_vanilla_text = NULL, 
				 *down_text = NULL;
		char    *end_text = NULL;
		int      sepcharnum = 0;

		vanilla_text = d->text;
		text = d->text;

		/* Check for sentence-initial marker */
		/* testpos is starting position in text for comparison. */
		initial = is_initial( vanilla_text, &testpos );
		textlen = (int)strlen((char *)(vanilla_text+testpos));
		valid_length = (textlen >= MinTestLen);
		all_alpha = is_allalpha( (vanilla_text+testpos) );
		all_numbers = is_allnums( (vanilla_text+testpos) );
		specptr = strpbrk( (char *)(vanilla_text+testpos), SpecialChars );
		has_special_char = (specptr != NULL);
		if ( sepptr != NULL )
		{
			end_text = (char *)strrchr( vanilla_text, *sepptr );
		}

		if ( end_text != NULL )
		{
			end_text++;
		}

		/* Get a compressed version of the text. */
		if ( (valid_length && (has_alpha || has_numbers) && (compressable_ptr != NULL)) )
		{
			needs_compression = ((comptext = compress_word( vanilla_text )) != NULL);
		}

		if ( needs_compression )
		{
			text = comptext;
		}

		/* Check if there is a separator in the text. */
		if ( (has_alpha || has_numbers) && (sepptr != NULL) )
		{
			has_separator = TRUE;
			sepcharnum = contains_this_char( vanilla_text, *sepptr );
		}

		/* Get a downcased version of the text. */
		down_vanilla_text = downcase( vanilla_text );
		down_text = downcase( text );

		/* Capitalization */
		/* See if word is a "true" capital, i.e., if it contains the max capital tag. */
		true_capital = (search_chain( d->tag, features->max_capital->tag ) != 0);
		allcaps         = is_allcaps( (text+testpos) ) && true_capital;
		capital         = is_capital( (text+testpos) );
		initial_capital = (initial && capital) && true_capital;
		pseudo_capital = ((capital && !initial) && !true_capital);
		mixed_capital = (contains_capitals( (text+testpos) ) && !has_numbers && 
					(strpbrk( (char *)(vanilla_text+testpos), MixedCapitalChars ) != NULL));

		if ( valid_length )
		{
			log_feature( SKIP_CLOSED_TAGS, (initial_capital), "initial capital", d->tag );
			log_feature( SKIP_CLOSED_TAGS, (allcaps), "all capitals", d->tag );
			log_feature( SKIP_CLOSED_TAGS, (initial && allcaps), "initial allcapitals", d->tag );

			check_pure_word( needs_compression, d, vanilla_text, down_vanilla_text, testpos );
		}

		if ( all_alpha )
		{
			int   m;
			char *msg = NULL;

			/* All alpha characters. */
			/* Length features */
			Allocate(msg, MAXFEATNAME*sizeof(char), "msg: gather_initial_freqs");
			for( m = 1; m <= MinTestLen; m++ )
			{
				if ( (int)strlen((text+testpos)) == m )
				{
					sprintf( msg, "%d letters (capital)", m );
					log_feature( SKIP_CLOSED_TAGS, (true_capital), msg, d->tag );
					sprintf( msg, "%d letters (mixed capital)", m );
					log_feature( SKIP_CLOSED_TAGS, (mixed_capital), msg, d->tag );
					sprintf( msg, "%d letters (initial capital)", m );
					log_feature( SKIP_CLOSED_TAGS, (initial_capital), msg, d->tag );
					sprintf( msg, "%d letters (all capital)", m );
					log_feature( SKIP_CLOSED_TAGS, (allcaps), msg, d->tag );
					sprintf( msg, "%d letters", m );
					log_feature( NO_SKIP_CLOSED_TAGS, (!(true_capital || initial_capital) && 
									!allcaps), msg, d->tag );
				}
			}

			free ( msg );
		}
		else if ( has_separator )
		{
			/* A separator character is present */
			if ( make_affix_words( vanilla_text, testpos, &c_new_prefix_word, &c_new_suffix_word ) )
			{
				/* Separator Suffixes */
				if ( end_text != NULL )
				{
					BOOL suff_alpha   = is_allalpha( end_text );
					BOOL goodsepsufflen = ((int)strlen((char *)end_text) >= MinAffWord);

					if ( suff_alpha && goodsepsufflen )
					{
						use_separator_suffix = TRUE;
						separator_suffix_capital = is_capital( end_text ) || is_allcaps( end_text );
					}
				}
			}
		}

		/* Add end words of words with separators to separator dictionary. */
		if ( end_text != NULL )
		{
			if ( is_allalpha( end_text ) )
			{
				TagScore septags = NULL;

				septags = filter_tags( d->tag );
				add_unkword( &(features->sepdict), end_text, NO_SKIP_CLOSED_TAGS, septags, NULL );
				free_tagscore_list( &septags );
			}
		}

		if ( valid_length && has_alpha && !all_alpha) /* Alphanumeric & special char mixture */
		{
			char  *featname;

			if ( needs_compression )
			{
				char *sepchar, *msg = NULL;

				/* Compression character features. */
				sepchar = string_dup( sepptr );
				sepchar[1] = '\0';

				Allocate(msg, MAXFEATNAME*sizeof(char), "msg: gather_initial_freqs");

				if ( allcaps )
				{
					featname = add_chars( sepchar, "all capitals " );
				}
				else if ( true_capital )
				{
					featname = add_chars( sepchar, "true capital " );
				}
				else if ( mixed_capital )
				{
					featname = add_chars( sepchar, "mixed capital " );
				}
				else if ( features->initials_exist && initial_capital )
				{
					featname = add_chars( sepchar, "initial capital " );
				}
				else
				{
					featname = add_chars( sepchar, "plain " );
				}

				if ( has_numbers && (sepcharnum == 1) )
				{
					sprintf( msg, "%s (contains numbers)", featname );
				}
				else if ( has_numbers && (sepcharnum > 1) )
				{
					sprintf( msg, "%s (>1) (contains numbers)", featname );
				}
				else if ( !has_numbers && (sepcharnum == 1) )
				{
					sprintf( msg, "%s", featname );
				}
				else if ( !has_numbers && (sepcharnum > 1) )
				{
					sprintf( msg, "%s (>1)", featname );
				}

				log_feature( NO_SKIP_CLOSED_TAGS, TRUE, msg, d->tag );

				free( featname );
				free( sepchar );
				free( msg );
			}
			else if ( !needs_compression && !has_numbers )
			{
				BOOL  char_found = FALSE;
				char  *specgroup = SpecialChars;
				int   j;
				char *specchar;

				/* Special character features. */
				for ( j = 0; (specgroup[j] != '\0') && !char_found; j++ )
				{
					if ( contains_this_char( (vanilla_text+testpos), specgroup[j] ) )
					{
						specchar = string_dup( &(specgroup[j]) );
						specchar[1] = '\0';

						char_found = TRUE;
					}
				}

				if ( char_found )
				{
					if ( contains_allcaps( (vanilla_text+testpos) ) )
					{
						featname = add_chars( specchar, "contains (allcaps) " );
					}
					else if ( true_capital || mixed_capital )
					{
						featname = add_chars( specchar, "contains (capital) " );
					}
					else
					{
						featname = add_chars( specchar, "contains " );
					}

					log_feature( NO_SKIP_CLOSED_TAGS, TRUE, featname, d->tag );

					free( featname );
					free( specchar );
				}
			}
		}

		/* Suffixes */
		if ( strlen((char *)(text+testpos)) >= MinAffWord )
		{
			char *local_text = text;

			if ( use_separator_suffix )
			{
				add_affix( separator_suffix_capital, NO_CAPITAL, NO_CAPITAL, VARIABLE_SUFFIX, 
							&(features->variable_separator_sufflist), 0, 
							c_new_suffix_word, NO_TESTPOS, d->unktag );

				add_affix( separator_suffix_capital, NO_CAPITAL, NO_CAPITAL, SUFFIX, 
							&(features->separator_sufflist), features->maxsuffix, 
							c_new_suffix_word, NO_TESTPOS, d->unktag );
			}
			else if ( !has_numbers && !has_separator )
			{
				if ( !true_capital )
				{
					local_text = down_text;
				}

				add_affix( true_capital, NO_CAPITAL, mixed_capital, VARIABLE_SUFFIX, 
							&(features->variable_sufflist), 0, local_text, testpos, d->unktag );

				add_affix( true_capital, NO_CAPITAL, mixed_capital, SUFFIX, &(features->sufflist), 
							features->maxsuffix, local_text, testpos, d->unktag );
			}
		}

		/* Cuts */
		if ( !has_numbers && ((int)strlen((char *)(text+testpos)) >= MinCutLen) )
		{
			char *local_text = text;

			if ( !true_capital )
			{
				local_text = down_text;
			}

			make_cuts( true_capital, NO_CAPITAL, mixed_capital, d, dict, local_text, testpos, 
						features->indexlist, &(features->cut_list), trans, dbp, asc);

			/* Perform special cuts. */
			make_special_cuts( true_capital, pseudo_capital, mixed_capital, d, dict, local_text, 
						testpos, &(features->special_cut_list), trans, dbp, asc );
		}

		/* Separator Cuts */
		if ( has_separator && ((features->sepdict).size > 0) && (end_text != NULL) )
		{
			end_text++;

			if ( end_text != NULL )
			{
				if ( is_allalpha( end_text ) && (int)strlen((char *)end_text) >= MinCutLen )
				{
					char *local_text;

					if ( true_capital )
					{
						local_text = string_dup( end_text );
					}
					else
					{
						local_text = downcase( end_text );
					}

					make_cuts( true_capital, NO_CAPITAL, NO_CAPITAL, d, &(features->sepdict), 
								local_text, NO_TESTPOS, features->separator_indexlist, 
								&(features->sep_cut_list), trans, dbp, asc );

					/* Perform separator special cuts. */
					make_special_cuts( true_capital, NO_CAPITAL, NO_CAPITAL, d, dict, local_text, 
								NO_TESTPOS, &(features->sep_special_cut_list), trans, dbp, asc );

					free( local_text );
				}
			}
		}

		if ( c_new_suffix_word != NULL )
		{
			free( c_new_suffix_word );
		}

		if ( c_new_prefix_word != NULL )
		{
			free( c_new_prefix_word );
		}

		if ( needs_compression )
		{
			free( comptext );
		}

		free( down_vanilla_text );
		free( down_text );

	} /* End of working through the dictionary */

	/* Sort the dictionaries */
	sort_dict( &(features->featdict) );
	sort_dict( &(features->capdict) );
	sort_dict( &(features->sepdict) );

	/* Make an index of the separator dictionary, if it has any entries. */
	if ( (features->sepdict).size > 0 )
	{
		DictWord  *k = (features->sepdict).key;

		/* Work through the separator dictionary */
		for ( i = 0 ; i < (features->sepdict).size ; i++, k++ )
		{
			DictWord d = *k;

			if ( is_allalpha( d->text ) )
			{
				/* Add word to the separator index. */
				add_index( NO_DOWNCASE, d, NO_TESTPOS, MinTestLen, &(features->separator_indexlist) );
			}
		}

		sort_indexlist( &(features->separator_indexlist) );

	}

	/* Sort the affix lists */
	sort_afflist( &(features->sufflist) );
	sort_afflist( &(features->variable_sufflist) );
	sort_afflist( &(features->separator_sufflist) );
	sort_afflist( &(features->variable_separator_sufflist) );

	/* Sort special cut lists */
	sort_cutlist( &(features->special_cut_list) );
	sort_cutlist( &(features->sep_special_cut_list) );

	/* Other Cuts */
	/* Perform replacement and container cuts. */
	make_other_cuts( features->indexlist, &(features->cut_list), &(features->container_cut_list), 
				&(features->replacement_cut_list) );

	if ( (features->sepdict).size > 0 )
	{
		make_other_cuts( features->separator_indexlist, &(features->sep_cut_list), 
					&(features->sep_container_cut_list), 
					&(features->sep_replacement_cut_list) );
	}

	/* Sort the cut lists */
	sort_cutlist( &(features->cut_list) );
	sort_cutlist( &(features->container_cut_list) );
	sort_cutlist( &(features->replacement_cut_list) );

	sort_cutlist( &(features->sep_cut_list) );
	sort_cutlist( &(features->sep_container_cut_list) );
	sort_cutlist( &(features->sep_replacement_cut_list) );
}

void process_lines(FILE *rsf, int nol)
{int  i;
	char line[1000];

	for ( i = 0; i < nol; i++)
	{fgets(line,sizeof(line),rsf); 
	} 
}

char* process_heading(char line[10000],char heading[100])
{int f,i;

	for ( f = 1, i = 0; !(line[f] == ' ' && line[f-1] == ']'); f++, i++)
	{sscanf(&line[f], "%c", &heading[i]);
	} 
	heading[i-1] = '\0';

	return(&heading[0]);
}

void process_line(DictWord t2,char line[10000])
{char     next;
	int      i,pos;         
	char     freqstr[10],tagstr[10];
	Tag      tag;
	Score    freq;

	i = 0;
	pos = 0;

	pos = pos + strlen(t2->text) + 3;
	sscanf(&line[pos], "%c", &next);
	for ( i = 0; next != '\n'; i++)
	{sscanf(&line[pos], "%s", tagstr);
		pos = pos + strlen(tagstr) + 1;
		sscanf(&line[pos], "%s", freqstr);
		sscanf(&line[pos], "%lf", &freq);
		pos = pos + strlen(freqstr) + 1;
		tag = map_tag(tagstr);
		add_tags(t2, &tag, &freq, 1, TRUE);
		sscanf(&line[pos], "%c", &next);
	} 
	t2->ntag = i;
}

void process_dictionary(FILE *rsf, Dict dictionary, int *s)
{char     next_char;
	int      i,pos;
	char     heading[100] = "init";
	char     line[1000];

	i = pos = 0;

	fgets(line,sizeof(line),rsf); 
	sscanf(&line[pos], "%c", &next_char);
	for ( i = 0; next_char != '\n'; i++)
	{DictWord dw1,dw2; 

		dw1 = (DictWordSt *)malloc(sizeof(DictWordSt));
		dw1->tag = NULL;
		dw1->text = process_heading(line,heading);;

		process_line(dw1,line);

		dw2 = find_word(&dictionary,dw1->text,TRUE);
		dw2->unktag = dw1->tag;
		dw2->tag = dw1->tag;
		dw2->ntag = dw1->ntag;

		fgets(line,sizeof(line),rsf); 
		sscanf(&line[pos], "%c", &next_char);
	}
	*s = i;
	sort_dict(&dictionary);
}

/* This function expects to be called with line of LineLength length
   (not checked by compiler). This was changed to be hash defined and
   assertions put in: 26/01/02 */

void process_words(char line[LineLength],IndexWord *start_iw,IndexWord *end_iw)
{char      next_char;
	int       i,j,pos;         
	Tag       tag;
	IndexWord current_iw;
	char      scorestr[10],tagstr[10];
	double    score;
	pos = 0;

	sscanf(&line[pos], "%c", &next_char);
	current_iw = *start_iw = *end_iw = (IndexWordSt *)malloc(sizeof(IndexWordSt));
	for ( i = 0; next_char != '\n'; i++)
	{current_iw->word = (DictWordSt *)malloc(sizeof(DictWordSt));

		/* Original current_iw->word->text was malloc(50) */

		current_iw->word->text = (char*)malloc(MAXWORD);
		sscanf(&line[pos], "%s",current_iw->word->text);
		current_iw->length = strlen(current_iw->word->text);

		pos = pos + strlen(current_iw->word->text) + 1;
		assert(pos < LineLength);

		sscanf(&line[pos], "%c", &next_char);

		current_iw->word->unktag = (TagScoreSt *)malloc(sizeof(TagScoreSt));
		current_iw->word->tag = NULL;
		for ( j = 0; next_char != '*'; j++)
		{(void) sscanf(&line[pos],"%s",tagstr);
			pos = pos + strlen(tagstr) + 1;
			(void) sscanf(&line[pos],"%s",scorestr);
			(void) sscanf(&line[pos],"%lf",&score);

			tag = map_tag(tagstr);

			add_tags(current_iw->word, &tag, &score, 1, TRUE);

			pos = pos + strlen(scorestr) + 1;
			assert(pos < LineLength);

			sscanf(&line[pos], "%c", &next_char);
			current_iw->word->unktag = current_iw->word->tag;
		}
		current_iw->word->ntag = j;
		current_iw->next = (IndexWordSt *)malloc(sizeof(IndexWordSt));
		*end_iw = current_iw; 
		current_iw = current_iw->next;

		pos = pos + 2;
		assert(pos < LineLength);

		sscanf(&line[pos], "%c", &next_char);
	}
	(*end_iw)->next = NULL;
}

void process_indices(FILE *rsf, IndexList indl, int *s)
{Index *k;
	char  line[LineLength];
	char  next_char;
	int   wordnum,i,pos;

	wordnum = i = pos = 0;
	k = (Index *)malloc(sizeof(Index));

	fgets(line,sizeof(line),rsf); 
	sscanf(&line[pos], "%c", &next_char);
	for ( i = 0; next_char != '\n'; i++)
	{Index i1,i2;

		i1 = (IndexSt *)malloc(sizeof(IndexSt));
		i1->prefix = (char *)malloc(50);

		sscanf(&line[pos], "%s %d\n",i1->prefix,&(i1->wordnum));

		fgets(line,sizeof(line),rsf); 
		process_words(line,&(i1->wordlist_start),&(i1->wordlist_end));

		i2 = find_index(&indl,i1->prefix);

		i2->prefix = i1->prefix;
		i2->wordnum = i1->wordnum;
		i2->wordlist_start = i1->wordlist_start;
		i2->wordlist_end = i1->wordlist_end;

		fgets(line,sizeof(line),rsf); 
		sscanf(&line[pos], "%c", &next_char);
	}
	*s = i;
	sort_indexlist(&indl);
}

TagScore process_source_tag(DictWord t2,char uline[1000], 
			int *startpos)
{char     next_char;
	int      i,pos;         
	char     freqstr[10],tagstr[10];
	Tag      tag;
	Score    freq;

	i = 0;
	pos = *startpos;
	t2->tag = NULL;

	sscanf(&uline[pos], "%c", &next_char);
	for ( i = 0; next_char != '*'; i++)
	{sscanf(&uline[pos], "%s", tagstr);
		pos = pos + strlen(tagstr) + 1;
		sscanf(&uline[pos], "%s", freqstr);
		sscanf(&uline[pos], "%lf", &freq);
		pos = pos + strlen(freqstr) + 1;
		tag = map_tag(tagstr);
		add_tags(t2, &tag, &freq, 1, TRUE);
		sscanf(&uline[pos], " %c", &next_char);
	} 
	*startpos = pos + 2;
	return t2->tag; 
}

TagScore process_transform_tags(DictWord t2,char uline[1000], 
			int *startpos)
{char     next_char;
	int      i,pos;         
	char     freqstr[10],tagstr[10];
	Tag      tag;
	Score    freq;

	i = 0;
	pos = *startpos;
	t2->tag = NULL;

	sscanf(&uline[pos], "%c", &next_char);
	for ( i = 0; next_char != '\n'; i++)
	{sscanf(&uline[pos], "%s", tagstr);
		pos = pos + strlen(tagstr) + 1;
		sscanf(&uline[pos], "%s", freqstr);
		sscanf(&uline[pos], "%lf", &freq);
		pos = pos + strlen(freqstr) + 1;
		tag = map_tag(tagstr);
		add_tags(t2, &tag, &freq, 1, TRUE);
		sscanf(&uline[pos], "%c", &next_char);
	} 
	return t2->tag; 
}

void process_affixes(FILE *rsf, AffList affl, int *s)
{char     line[1000];
	char     next_char;
	int      i,j,pos;
	TagTrans start,current,trailer;
	DictWord dw; 
	char temp_affix[1000];

	pos = 0;
	dw = (DictWordSt *)malloc(sizeof(DictWordSt));

	fgets(line,sizeof(line),rsf); 
	sscanf(&line[pos], "%c", &next_char);
	for ( i = 0; next_char != '\n'; i++)
	{TagAff   ta1,ta2;

		sscanf(&line[pos], "%s ", temp_affix);

		ta1 = (AffTagSt *)malloc(sizeof(AffTagSt));
		ta1->affix = (char *)malloc(strlen(temp_affix) + 1);

		strcpy(ta1->affix, temp_affix);

		sscanf(&line[pos], "%lf\n",&(ta1->total_score));

		ta2 = find_affix(&affl,ta1->affix);
		ta2->affix = ta1->affix;

		start = current = (TransFormSt *)malloc(sizeof(TransFormSt));
		fgets(line,sizeof(line),rsf); 
		sscanf(&line[pos], "%c", &next_char);
		for ( j = 0; next_char == ' '; j++)
		{pos = pos + 3;
			current->source_tag = process_source_tag(dw,line,&pos);
			current->transform_tags = process_transform_tags(dw,line,&pos);
			current->next = (TransFormSt *)malloc(sizeof(TransFormSt));
			trailer = current;
			current = current->next;
			fgets(line,sizeof(line),rsf); 
			pos = 0;
			sscanf(&line[pos], "%c", &next_char);
		}
		trailer->next = NULL;
		ta2->vanilla_tagscore_list = start;
	}
	*s = i;
	sort_afflist(&affl);
}

void process_cuts(FILE *rsf, CutList cutl, int *s)
{char     line[1000];
	char     next_char;
	int      i,j,pos;
	TagTrans start,current,trailer;
	DictWord dw; 
	char temp_cut[1000];
	/* Have a temporary cut that should be longer than the original. */

	pos = 0;
	dw = (DictWordSt *)malloc(sizeof(DictWordSt));

	fgets(line,sizeof(line),rsf); 
	sscanf(&line[pos], "%c", &next_char);
	for ( i = 0; next_char != '\n'; i++)
	{TagCut   tc1,tc2;

		sscanf(&line[pos], "%s ", temp_cut);

		tc1 = (CutTagSt *)malloc(sizeof(CutTagSt));
		tc1->cut = (char *)malloc(strlen(temp_cut) + 1);

		strcpy(tc1->cut, temp_cut);
		sscanf(&line[pos], "%lf\n", &(tc1->special_total_score));

		tc2 = find_cut(&cutl,tc1->cut);
		tc2->cut = tc1->cut;

		start = current = (TransFormSt *)malloc(sizeof(TransFormSt));
		fgets(line,sizeof(line),rsf); 
		sscanf(&line[pos], "%c", &next_char);
		for ( j = 0; next_char == ' '; j++)
		{pos = pos + 3;
			current->source_tag = process_source_tag(dw,line,&pos);
			current->transform_tags = process_transform_tags(dw,line,&pos);
			current->next = (TransFormSt *)malloc(sizeof(TransFormSt));
			trailer = current;
			current = current->next;
			fgets(line,sizeof(line),rsf); 
			pos = 0;
			sscanf(&line[pos], "%c", &next_char);
		}
		trailer->next = NULL;
		tc2->transform_list = start;
		tc2->next = NULL;
	}
	*s = i;
	sort_cutlist(&cutl);
}

void process_max_cap(FILE *rsf)
{TagScore ptr,trailer;
	char     next_char; 
	int      i,pos,score;
	char	  tagstr[10],line[100]; 

	score = i = pos = 0;
	ptr = features->max_capital = (TagScoreSt *)malloc(sizeof(TagScoreSt));

	fgets(line,sizeof(line),rsf); 
	sscanf(&line[pos], "%c", &next_char);
	for ( i = 0; next_char != '\n'; i++)
	{sscanf(&line[pos], "%s %d", tagstr, &score);
		ptr->tag = map_tag(tagstr);
		ptr->score = score;

		ptr->next = (TagScoreSt *)malloc(sizeof(TagScoreSt));
		trailer = ptr;
		ptr = ptr->next;

		fgets(line,sizeof(line),rsf); 
		sscanf(&line[pos], "%c", &next_char);
	}
	trailer->next = NULL;
}

void initialize_features4reading(int *i1, int *i2, int *i3, int *i4,
			int *i5, int *i6, int *i7, int *i8, int *i9,
			int *i10, int *i11, int *i12, int *i12a, int *i13, int *i14,
			int *i15, int *i16, int *i17, int *i18, int *i19,
			int *i20 )
{   features->badwordfile_open = FALSE;
	features->all_wordnum = features->cap_wordnum = features->aff_wordnum = features->cut_wordnum 
		= features->container_cut_wordnum = features->separator_wordnum = 
		features->separator_aff_wordnum = features->separator_cut_wordnum = 
		features->separator_container_cut_wordnum = 0;

	InitDict((features->sepdict));
	create_dict(&(features->sepdict), *i1);
	clear_dict(&(features->sepdict));
	InitDict((features->featdict));
	create_dict(&(features->featdict), *i2);
	clear_dict(&(features->featdict));
	InitDict((features->capdict));
	create_dict( &(features->capdict), *i3);
	clear_dict(&(features->capdict));
	InitDict((features->unkstatdict));
	create_dict(&(features->unkstatdict), *i4);
	clear_dict(&(features->unkstatdict));

	InitList( features->indexlist );
	create_indexlist( &(features->indexlist), *i5 );
	clear_indexlist( &(features->indexlist) );
	InitList( features->enclosure_indexlist );
	create_indexlist( &(features->enclosure_indexlist), *i6 );
	clear_indexlist( &(features->enclosure_indexlist) );
	InitList( features->partialcap_indexlist );
	create_indexlist( &(features->partialcap_indexlist), *i7 );
	clear_indexlist( &(features->partialcap_indexlist) );
	InitList( features->separator_indexlist );
	create_indexlist( &(features->separator_indexlist), *i8);
	clear_indexlist( &(features->separator_indexlist) );

	InitList( features->cut_list );
	create_cutlist( &(features->cut_list), *i9);
	clear_cutlist( &(features->cut_list) );
	InitList( features->container_cut_list );
	create_cutlist( &(features->container_cut_list), *i10);
	clear_cutlist( &(features->container_cut_list) );
	InitList( features->replacement_cut_list );
	create_cutlist( &(features->replacement_cut_list), *i11);
	clear_cutlist( &(features->replacement_cut_list) );
	InitList( features->special_cut_list );
	create_cutlist( &(features->special_cut_list), *i12);
	clear_cutlist( &(features->special_cut_list) );
	InitList( features->smart_cut_list );
	create_cutlist( &(features->smart_cut_list), *i12a);
	clear_cutlist( &(features->smart_cut_list) );

	InitList( features->sep_cut_list );
	create_cutlist( &(features->sep_cut_list), *i13);
	clear_cutlist( &(features->sep_cut_list) );
	InitList( features->sep_container_cut_list );
	create_cutlist( &(features->sep_container_cut_list), *i14);
	clear_cutlist( &(features->sep_container_cut_list) );
	InitList( features->sep_replacement_cut_list );
	create_cutlist( &(features->sep_replacement_cut_list), *i15);
	clear_cutlist( &(features->sep_replacement_cut_list) );
	InitList( features->sep_special_cut_list );
	create_cutlist( &(features->sep_special_cut_list), *i16);
	clear_cutlist( &(features->sep_special_cut_list) );

	InitList( features->sufflist );
	create_afflist( &(features->sufflist), *i17);
	clear_afflist( &(features->sufflist) );
	InitList( features->variable_sufflist );
	create_afflist( &(features->variable_sufflist), *i18);
	clear_afflist( &(features->variable_sufflist) );
	InitList( features->separator_sufflist );
	create_afflist( &(features->separator_sufflist), *i19);
	clear_afflist( &(features->separator_sufflist) );
	InitList( features->variable_separator_sufflist );
	create_afflist( &(features->variable_separator_sufflist), *i20);
	clear_afflist( &(features->variable_separator_sufflist) );
}

void process_init_information(FILE *rsf, int *i1, int *i2, int *i3, int *i4,
			int *i5, int *i6, int *i7, int *i8, int *i9,
			int *i10, int *i11, int *i12, int *i12a, int *i13, int *i14,
			int *i15, int *i16, int *i17, int *i18, int *i19,
			int *i20)
{char	  line[2000]; 

	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%d %d %d %d",i1, i2, i3, i4);
	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%d %d %d %d",i5, i6, i7, i8); 
	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%d %d %d %d %d",i9, i10, i11, i12, i12a);
	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%d %d %d %d",i13, i14, i15, i16);
	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%d %d %d %d",i17, i18, i19, i20);
}

void process_feature_information(FILE *rsf, int *s1, int *s2, int *s3, int *s4,
			int *s5, int *s6, int *s7, int *s8, int *s9,
			int *s10, int *s11, int *s12, int *s13, int *s14,
			int *s15, int *s16, int *s17, int *s18, Score *s19, 
			Score *s20)
{char	  line[2000]; 

	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%d %d %d %d",s1, s2, s3, s4);
	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%d %d %d %d",s5, s6, s7, s8);
	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%d %d %d %d %d",s9, s10, s11, s12, s13);
	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%d %d %d %d %d",s14, s15, s16, s17, s18);
	fgets(line,sizeof(line),rsf); 
	sscanf(&line[0], "%lf %lf",s19, s20);
	fgets(line,sizeof(line),rsf); 
}

void process_gamma(FILE *rsf)
{char line[2000]; 
	int  i,pos; 
	char gamma_str[100];

	fgets(line,sizeof(line),rsf); 
	for (i = pos = 0; i < 400 ; i++) 
	{sscanf(&line[pos], "%s",gamma_str);
		sscanf(&line[pos], "%lf",&(features->gamma[i]));
		pos = pos + strlen(gamma_str) + 1;
	}
}

void process_features(FILE *rsf)
{int size, i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i12a,i13,i14,
	i15,i16,i17,i18,i19,i20;
	int s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,
		s15,s16,s17,s18;
	Score s19,s20;

	size = 0; 

	process_init_information(rsf,&i1,&i2,&i3,&i4,&i5,&i6,&i7,&i8,
				&i9,&i10,&i11,&i12,&i12a,&i13,&i14,&i15,&i16,&i17,
				&i18,&i19,&i20);
	process_lines(rsf,2);

	/* initialise unkdict and unigramdict - John Carroll 28-06-02 */
	unknown_word_handling_initialization();

	initialize_features4reading(&i1,&i2,&i3,&i4,&i5,&i6,&i7,&i8,&i9,
				&i10,&i11,&i12,&i12a,&i13,&i14,&i15,&i16,&i17,&i18,&i19,
				&i20);

	process_feature_information(rsf,&s1,&s2,&s3,&s4,&s5,&s6,&s7,&s8,
				&s9,&s10,&s11,&s12,&s13,&s14,&s15,&s16,&s17,
				&s18,&s19,&s20);

	features->initials_exist = s1; 
	features->badwordfile_open = s2; 

	features->all_wordnum = s3; 
	features->cap_wordnum = s4; 
	features->aff_wordnum = s5; 
	features->cut_wordnum = s6; 
	features->container_cut_wordnum = s7; 
	features->replacement_cut_wordnum = s8; 
	features->separator_wordnum = s9; 
	features->separator_aff_wordnum = s10; 
	features->separator_cut_wordnum = s11; 
	features->separator_container_cut_wordnum = s12; 
	features->separator_replacement_cut_wordnum = s13; 
	features->maxprefix = s14; 
	features->maxsuffix = s15; 
	features->maxprefcut = s16; 
	features->maxsuffcut = s17; 
	features->maxunkwords = s18; 

	features->unigramtotal = s19; 
	features->unigram_open_total = s20; 

	features->type_info[0] = '\0';

	process_lines(rsf,2);
	process_gamma(rsf);

	process_lines(rsf,2);
	process_max_cap(rsf); 
	process_lines(rsf,2);
	process_dictionary(rsf,features->sepdict,&size);
	(features->sepdict).size = size; 
	process_lines(rsf,2);
	process_dictionary(rsf,features->featdict,&size);
	(features->featdict).size = size; 
	process_lines(rsf,2);
	process_dictionary(rsf,features->capdict,&size);
	(features->capdict).size = size; 
	process_lines(rsf,2);
	process_dictionary(rsf,features->unkdict,&size);
	(features->unkdict).size = size; 
	process_lines(rsf,2);
	process_dictionary(rsf,features->unkstatdict,&size);
	(features->unkstatdict).size = size; 
	process_lines(rsf,2);
	process_dictionary(rsf,features->unigramdict,&size);
	(features->unigramdict).size = size; 
	process_lines(rsf,2);

	process_indices(rsf,features->indexlist,&size);
	(features->indexlist).size = size; 
	process_lines(rsf,2);
	process_indices(rsf,features->enclosure_indexlist,&size);
	(features->enclosure_indexlist).size = size;
	process_lines(rsf,2);
	process_indices(rsf,features->partialcap_indexlist,&size);
	(features->partialcap_indexlist).size = size;
	process_lines(rsf,2);
	process_indices(rsf,features->separator_indexlist,&size);
	(features->separator_indexlist).size = size; 
	process_lines(rsf,2);

	process_cuts(rsf, features->cut_list,&size); 
	(features->cut_list).size = size; 
	process_lines(rsf,2);
	process_cuts(rsf, features->container_cut_list,&size);
	(features->container_cut_list).size = size; 
	process_lines(rsf,2);
	process_cuts(rsf, features->replacement_cut_list,&size);
	(features->replacement_cut_list).size = size; 
	process_lines(rsf,2);
	process_cuts(rsf, features->special_cut_list,&size);
	(features->special_cut_list).size = size; 
	process_lines(rsf,2);
	process_cuts(rsf, features->smart_cut_list,&size);
	(features->smart_cut_list).size = size; 
	process_lines(rsf,2);

	process_cuts(rsf, features->sep_cut_list,&size);
	(features->sep_cut_list).size = size; 
	process_lines(rsf,2);
	process_cuts(rsf, features->sep_container_cut_list,&size);
	(features->sep_container_cut_list).size = size; 
	process_lines(rsf,2);
	process_cuts(rsf, features->sep_replacement_cut_list,&size);
	(features->sep_replacement_cut_list).size = size; 
	process_lines(rsf,2);
	process_cuts(rsf, features->sep_special_cut_list,&size);
	(features->sep_special_cut_list).size = size; 
	process_lines(rsf,2);

	process_affixes(rsf,features->sufflist,&size);
	(features->sufflist).size = size; 
	process_lines(rsf,2);
	process_affixes(rsf,features->variable_sufflist,&size);
	(features->variable_sufflist).size = size; 
	process_lines(rsf,2);
	process_affixes(rsf,features->separator_sufflist,&size);
	(features->separator_sufflist).size = size; 
	process_lines(rsf,2);
	process_affixes(rsf,features->variable_separator_sufflist,&size);
	(features->variable_separator_sufflist).size = size; 
}

/* If option bdbm is not specified and unknown word statistics are read */
/* in selected features of lexicon are analyzed for use in generating tag */
/* probabilities for unknown words. */
void analyze_features(Dict *dict, char *ofeaturesname, char *obadwordname, Trans *trans, 
			DB *dbp, Dict *asc, char *runkstatname)
{FILE *runkstatfile;

	if (Option(runkstat)) 
	{if (Option(verbose)) printf("Read unknown word statistics\n");
		runkstatfile = open_file(runkstatname, "r");
		/* Read in unknown word statistics  */
		process_features(runkstatfile); 
		(void) fclose(runkstatfile);
	}
	else
	{if (!Option(bdbm))
		{if (Option(verbose)) printf("Compute unknown word statistics\n");
			initialize_features();
			if (ofeaturesname[0] != 0)
			  features->ofeaturesfile = open_file(ofeaturesname, "w");
			if (obadwordname[0] != 0)
			{features->obadwordfile = open_file(obadwordname, "w");
				features->badwordfile_open = TRUE;
			}
			gather_initial_freqs(dict, trans, dbp, asc);
			initialize_other_features();
			gather_other_freqs(dict, trans, dbp, asc);
			sort_dict(&(features->unkdict));
		}
		else
		  if (Option(verbose)) printf("\nNo unkown word statistics used\n\n");
	}
	if (Option(unknown_morph))
	{if (ofeaturesname[0] != 0)
		{if (!Option(runkstat)) 
			{if (Option(verbose) && Option(bdbm))
				{printf("\nNo c_newly computed unknown word statistics to write\n\n"); 
				}
				else
				{write_features(ofeaturesname);
				}
			}
			else 
			{if Option(verbose) printf("\nNo c_newly computed unknown word statistics to write\n\n");
			}
		}
	}    
}

void show_local_time()
{
	time_t tim;
	struct tm *at;
	char now[80];
	time(&tim);
	at=localtime(&tim);
	strftime(now,79,"%Y-%m-%d %H:%M:%S",at);
	printf("%s\n",now);
}


