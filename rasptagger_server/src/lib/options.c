#include "options.h"

/*--------------------------------------------------------------------------
  set_up_options

  Set all the option flags, file names and numerical parameters.
  ---------------------------------------------------------------------------*/

void set_up_options(int argc, char *argv[],
			int *iterations, int *initialise, int *dict_size,
			char *dict, char *tran, char *odict, char *otran,
			char *out, char *map, char *skip, char *reduce,
			char *fsm, char *grammar, char *infer, char *ukw,
			char *ofeatures, char *obadword, char *bdbm, char *runkstat,
			char *wunkstat )
{
	char opt;
	int  arg = 2, i = 0;
	char root[MAXFN];
	BOOL error = FALSE;
	BOOL unkopt = FALSE;

	/* The following string defines the known option letters. ':' indicates
	   that an argument is needed. */
#ifdef BT
	char *opt_string = "aA:bB:c:C:d:D:fFi:I:lm:M:nNo:O:pr:R:St:T:uVwx:XzZ";
#else
	char *opt_string =
		"aA:bB:c:C:d:D:e:EfFgGh:H:i:I:j:J:k:K:lL:m:M:nNo:O:pPq:Qr:R:s:St:T:uUv:VwWx:XYzZ";
#endif

	*iterations = 1;
	*initialise = 0;
	*dict_size  = 0;
	SetOutOpt(out_word);
	SetInOpt(tagged);
	dict[0] = tran[0] = odict[0] = otran[0] = out[0] = map[0] =
		fsm[0] = grammar[0] = infer[0] = skip[0] = reduce[0] = 
		ofeatures[0] = obadword[0] = bdbm[0]= runkstat[0] = wunkstat[0] = 0;

	/* Analyse the options */
	while ((opt = get_option(argc, argv, opt_string, &arg, &i)) != 0)
	{
		switch (opt)
		{
			case 'a': SetOption(anchored); break;
			case 'A':
					  if (!get_opt_int(argv, dict_size, &arg, &i, opt))
						*dict_size = 0;
					  break;
			case 'b': SetOption(num_stabilise); break;
			case 'B':
					  SetOption(reestimate);
					  if (!get_opt_int(argv, iterations, &arg, &i, opt))
						*iterations = 1;
					  break;
			case 'c':
					  {
						  Score threshold;

						  if (get_opt_double(argv, &threshold, &arg, &i, opt))
						  {
							  SetOption(use_threshold);
							  set_output_threshold(threshold);
						  }
						  break;
					  }
			case 'C':
					  if (!get_opt_int(argv, &options.in, &arg, &i, opt))
						options.in = tagged;
					  break;
			case 'd':
					  if (check_names(dict, NULL, "Input"))
						get_opt_string(argv, dict, MAXFN, &arg, &i, opt);
					  break;
			case 'D':
					  if (check_names(odict, NULL, "Output"))
						get_opt_string(argv, odict, MAXFN, &arg, &i, opt);
					  break;
			case 'e':
					  /* Have to get string so arg and i are straight */
					  get_opt_string(argv, fsm, MAXFN, &arg, &i, opt);
#ifdef Use_FSM
					  SetOption(use_fsm);
#else
					  fprintf(stderr, "Option 'e' ignored (no FSMs)\n");
#endif
					  break;
			case 'E':
#ifdef Use_FSM
					  SetOption(fsm_trace);
#else
					  fprintf(stderr, "Option 'E' ignored (no FSMs)\n");
#endif
					  break;
			case 'f': SetOption(most_freq); break;
			case 'F': SetOption(fb_tagging); break;
			case 'g': SetOption(good_turing); break;
			case 'G': SetOption(good_turing_lex); break;
			case 'h':
					  /* don't set unknown_morph option since only used in presence of some other unk option */
					  if ( features == NULL )
					  {
						  /* Set up and initialize the features structure */
						  Allocate(features, sizeof(FeatureSt),
									  "feature structure - unk words");
					  }
					  if (!get_opt_int(argv, &(features->maxunkwords), &arg, &i, 
									  opt))
						features->maxunkwords = MAXUNKWORDS;
					  break;
			case 'H':
					  SetOption(unknown_morph);
					  if ( features == NULL )
					  {
						  /* Set up and initialize the features structure */
						  Allocate(features, sizeof(FeatureSt), 
									  "feature structure - unk words");
					  }
					  if (!get_opt_int(argv, &(features->maxsuffix), &arg, &i, 
									  opt))
					  {features->maxsuffix = MinSuffixLen;
					  }
					  unkopt = TRUE;
					  break;
			case 'i':
					  get_opt_string(argv, infer, MAXFN, &arg, &i, opt);
					  break;
			case 'I':
					  if (!get_opt_int(argv, initialise, &arg, &i, opt))
						*initialise = 0;
					  break;
			case 'j': SetOption(runkstat); 
					  SetOption(unknown_morph);
					  if ( features == NULL )
					  {
						  /* Set up and initialize the features structure */
						  Allocate(features, sizeof(FeatureSt), "feature structure - unk words");
					  }
					  get_opt_string(argv, runkstat, MAXFN, &arg, &i, opt);
					  break;
			case 'J': SetOption(wunkstat);  
					  if (check_names(ofeatures, NULL, "Output"))
						get_opt_string(argv, ofeatures, MAXFN, &arg, &i, opt);
					  break;
			case 'k':
					  SetOption(unknown_morph);
					  if ( features == NULL )
					  {
						  /* Set up and initialize the features structure */
						  Allocate(features, sizeof(FeatureSt), 
									  "feature structure - unk words");
					  }
					  if (!get_opt_int(argv, &(features->maxprefcut), &arg, &i, opt))
					  {features->maxprefcut = MinPrefixLen;
					  }
					  unkopt = TRUE;
					  break;
			case 'K':
					  SetOption(unknown_morph);
					  if ( features == NULL )
					  {
						  /* Set up and initialize the features structure */
						  Allocate(features, sizeof(FeatureSt), "feature structure - unk words");
					  }
					  if (!get_opt_int(argv, &(features->maxsuffcut), &arg, &i, opt))
					  {features->maxsuffcut = MinSuffixLen;
					  }
					  unkopt = TRUE;
					  break;
			case 'l': SetOption(training); break;
			case 'L':
					  {
						  Score threshold;
						  if (get_opt_double(argv, &threshold, &arg, &i, opt))
						  {
							  SetOption(reest_threshold);
							  set_re_est_threshold(threshold);
						  }
						  break;
					  }
			case 'm':
					  if (map[0] != 0)
						fprintf(stderr,
									"Map file name specified more than once\n");
					  else
						get_opt_string(argv, map, MAXFN, &arg, &i, opt);
					  break;
			case 'M':
					  if (reduce[0] != 0)
						fprintf(stderr,
									"Reduced tag set file specified more than once\n");
					  else
					  {
						  get_opt_string(argv, reduce, MAXFN, &arg, &i, opt);
						  SetOption(reduced_tags);
					  }
					  break;
			case 'n': SetOption(any_digit); break;
			case 'N': SetOption(parsed_number); break;
			case 'o':
					  if (out[0] != 0)
						fprintf(stderr,
									"Output file name specified more than once\n");
					  else
						get_opt_string(argv, out, MAXFN, &arg, &i, opt);
					  break;
			case 'O':
					  if (!get_opt_int(argv, &options.out, &arg, &i, opt))
						options.out = out_word;
					  if (!no_output) SetOutOpt(out_word);
					  break;
			case 'p': SetOption(product); break;
			case 'P':
#ifdef Phrasal
					  SetOption(anchor_bracket);
#else
					  fprintf(stderr, "Option 'P' ignored (not phrasal)\n");
#endif
					  break;
			case 'q':
					  get_opt_string(argv, grammar, MAXFN, &arg, &i, opt);
#ifdef Use_Parser
					  SetOption(use_parser);
#else
					  fprintf(stderr, "Option 'q' ignored (no parser)\n");
#endif
					  break;
			case 'Q':
#ifdef Use_Parser
					  SetOption(parser_trace);
#else
					  fprintf(stderr, "Option 'Q' ignored (no parser)\n");
#endif
					  break;
			case 'r':
					  if (check_names(dict, tran, "Input") &&
								  get_opt_string(argv, root, MAXFN, &arg, &i, opt))
						make_names(root, dict, tran, MAXFN);
					  break;
			case 'R':
					  if (check_names(odict, otran, "Output") &&
								  get_opt_string(argv, root, MAXFN, &arg, &i, opt))
						make_names(root, odict, otran, MAXFN);
					  break;
			case 's':
					  if (check_names(obadword, NULL, "Output"))
						get_opt_string(argv, obadword, MAXFN, &arg, &i, opt);
					  break;
			case 'S': SetOption(report_stats); break;
			case 't':
					  if (check_names(NULL, tran, "Input"))
						get_opt_string(argv, tran, MAXFN, &arg, &i, opt);
					  break;
			case 'T':
					  if (check_names(NULL, otran, "Output"))
						get_opt_string(argv, otran, MAXFN, &arg, &i, opt);
					  break;
			case 'u': SetOption(report_unknown); break;
			case 'U':
					  if (get_opt_string(argv, ukw, MAXFN, &arg, &i, opt))
					  {
						  SetOption(unknown_rules);
						  if ( features == NULL )
						  {
							  /* Set up and initialize the features structure */
							  Allocate(features, sizeof(FeatureSt), "feature structure - unk words");
						  }
					  }
					  break;
			case 'v': SetOption(bdbm); 
					  get_opt_string(argv, bdbm, MAXFN, &arg, &i, opt);
					  break;
			case 'V': SetOption(Viterbi); break;
			case 'w': SetOption(use_wordlist); break;
			case 'W': SetOption(word_fuzzy_lookup); break;
			case 'x':
					  if (check_names(NULL, skip, "Skip list"))
					  {
						  get_opt_string(argv, skip, MAXFN, &arg, &i, opt);
						  SetOption(skip_list);
					  }
					  break;
			case 'X': SetOption(special); break;
			case 'Y': SetOption(unkdebug); break;
			case 'z': SetOption(verbose); break;
			case 'Z': SetOption(debug); break;
		}
	}

	/* Set up default for mapping */
	if (map[0] == 0) strcpy(map, "tags.map");

	/* Fiddle iterations if training */
	if (Option(training))
	{
		if (*iterations < 1)	*iterations = 1;
		if (Option(Viterbi))    *iterations = 2;
		if (Option(fb_tagging) && !Option(reestimate))	*iterations = 2;
		if (*dict_size <= 0)	*dict_size = DICTLEN;
	}

	/* If there were no other tagging options, set f-b */
	if (!Option(most_freq) && !Option(Viterbi))
	  SetOption(fb_tagging);

	/* Verification of options */
	/* Check numbers */
	if (Option(any_digit) && Option(parsed_number))
	{
		fprintf(stderr, "Both number options specified\n");
		error = TRUE;
	}

	/* Check input files */
	if (Option(training) && Option(most_freq))
	{
		fprintf(stderr,
					"Training must be a separate run from 'most frequent' tagging\n");
		error = TRUE;
	}

	if (Option(training) && Option(unknown_rules))
	{
		fprintf(stderr,
					"Training must be a separate run from tagging with unknown word rules\n");
		error = TRUE;
	}

	if (!Option(training) && (Option(good_turing) || Option(good_turing_lex)))
	{
		fprintf(stderr,	"Good-Turing adjustment only applies when training\n");
		ClearOption( good_turing );
		ClearOption( good_turing_lex );
		/* Just a warning! */
	}

	if (dict[0] == 0 && (!Option(training) || Option(use_wordlist)))
	{
		fprintf(stderr, "Must specify input dictionary (unless training)\n");
		error = TRUE;
	}

	if (tran[0] == 0 && !Option(training) &&
				!(*initialise & Init_t_1 & Init_t_tagmax 
					& (Init_d_ntag | Init_d_tagmax | Init_d_s)))
	{
		fprintf(stderr,
					"Must specify either transitions file or initialisation option\n");
		error = TRUE;
	}
	if (Option(training) && !Option(use_wordlist) &&
				(dict[0] != 0 || tran[0] != 0))
	{
		fprintf(stderr,
					"Dictionary/transitions input file ignored for training run\n");
		dict[0] = tran[0] = 0;
	}
	if (Option(use_wordlist) && dict[0] == 0)
	{
		fprintf(stderr, "Dictionary must be specified for wordlist\n");
		error = TRUE;
	}

	if (dict[0] == 0 && infer[0] != 0)
	{
		fprintf(stderr,
					"Tag inference may only be specified with an input dictionary\n");
		error = TRUE;
	}

	/* Check corpus */
	if (Option(training) && !Option(use_wordlist) && InOpt(untagged_input))
	{
		fprintf(stderr, "Training requires a tagged corpus\n");
		error = TRUE;
	}

	/* Check output files and options */
	if (Option(Viterbi) && (OutOpt(all_tags) || OutOpt(out_scores)))
	{
		fprintf(stderr, "Viterbi run: output options ignored\n");
		ClearOutOpt(all_tags | out_scores);
	}
	if (InOpt(untagged_input))
	{
		ClearOutOpt(err_only);
		ClearOption(report_stats);
		ClearOutOpt(prob_dist);
#ifdef Analyse
		ClearOutOpt(analyse);
#endif
	}
	if (Option(training) && (*iterations == 1) &&
				odict[0] == 0 && otran[0] == 0)
	{
		fprintf(stderr, "Warning: training run with no output files\n");
	}

	/* Check tagging options */
	if (Option(Viterbi) && (*iterations != (Option(training) ? 2 : 1)))
	{
		fprintf(stderr, "Iterations parameter ignored for Viterbi run\n");
		*iterations = 1;
	}
	if (*iterations < 1)
	{
		fprintf(stderr, "Number of iterations must be 1 or more\n");
		error = TRUE;
	}
	if (Option(training) && (*iterations == 1) && !no_output)
	{
		fprintf(stderr,
					"Warning: 'No output' set since training and 1 iteration\n");
		options.out = no_out_opt;
		ClearOption(report_stats);
		ClearOutOpt(prob_dist);
	}
	if (Option(most_freq) && (Option(Viterbi) || Option(reestimate)))
	{
		fprintf(stderr, "'Most frequent option set: ignoring others\n");
		ClearOption(Viterbi);
		ClearOption(reestimate);
	}

	if (Option(reest_threshold) && !Option(reestimate))
	{
		fprintf(stderr, "Re-estimation threshold ignored\n");
		ClearOption(reest_threshold);
	}

	if (OutOpt(prob_dist) &&
				((Option(training) && *iterations == 1) || Option(most_freq) ||
				 Option(Viterbi)))
	{
		fprintf(stderr, "Probability distribution option ignored\n");
		ClearOutOpt(prob_dist);
	}    

	if (Option(use_threshold) &&
				((Option(training) && *iterations == 1) || Option(most_freq) ||
				 Option(Viterbi)))
	{
		fprintf(stderr, "Thresholding distribution option ignored\n");
		ClearOption(use_threshold);
	}    

	/* Check initialisation options */
	if (Option(product) & !Option(training))
	{
		fprintf(stderr,
					"Product option ignored except when training\n");
		ClearOption(product);
	}

	/* ********************************************************************************* */
	/* The following restrictions on option combinations are related to the use of a bdbm  */
	/* dictionary and the reading in of unknown word statistics   */

	/* When unknown word statistics are read in the values for these options are read in as well  */
	if (Option(runkstat) && unkopt)
	{fprintf(stderr,
				"Reading unknown word statistics so H, K and k options ignored\n");
	}

	/* It is not possible to update and output a bdbm dictionary */
	if (odict[0] != 0 && Option(bdbm))
	{fprintf(stderr,
				"No dictionary output possible in case of bdbm lexical lookup\n");
	error = TRUE;
	}

	/* It is not possible to update and output a transition file when */
	/*    using a bdbm dictionary */
	if (otran[0] != 0 && Option(bdbm))
	{fprintf(stderr,
				"No transition output possible in case of bdbm lexical lookup\n");
	error = TRUE;
	}

	/* When the unknown word handling component is not activated, it is */
	/*    not possible to output unknown word statistics.  */
	if (!Option(unknown_morph) && Option(wunkstat))
	{fprintf(stderr,
				"Unknown word handling is turned off therefore no statistics to write\n");
	error = TRUE;
	}

	/* It is not possible to combine training/reestimation and bdbm dictionary lookup  */
	if ((Option(training)||(*iterations > 1)) && Option(bdbm))
	{fprintf(stderr,
				"Combination of training/reestimation and bdbm lexical lookup is not possible.\n");
	error = TRUE;
	}

	/* We want to exclude all uses of the bdbm dictionary other than simple tagging  */
	if ((options.in != 1) && Option(bdbm))
	{fprintf(stderr,
				"Using a tagged corpus in case of bdbm lexical lookup is not possible.\n");
	error = TRUE;
	}

	/* ********************************************************************************* */

	/* Check we made it through unscathed */
	if (error) get_out();
}
/*------------------------------------------------------------------------
  check_names

  check dict and tran names have not already been specified.
  -------------------------------------------------------------------------*/

BOOL check_names(char *dict, char *tran, char *kind)
{
	BOOL ok = TRUE;

	if (dict != NULL && dict[0] != 0)
	{
		fprintf(stderr, "%s dictionary specified more than once\n", kind);
		ok = FALSE;
	}

	if (tran != NULL && tran[0] != 0)
	{
		fprintf(stderr, "%s transitions specified more than once\n", kind);
		ok = FALSE;
	}

	return ok;
}

