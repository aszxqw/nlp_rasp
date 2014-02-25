#include "Tagger.h"


const char * const Tagger::IN_FNAME = "sample_input.txt";
const char * const Tagger::SENTENCE_OUT_FNAME = "sentence.out";
const char * const Tagger::TOKEN_OUT_FNAME = "token.out";
const char * const Tagger::TAGGER_OUT_FNAME = "tagger.out";

int Tagger::TAGGER_ARGC = 16;
char * Tagger::TAGGER_ARGS[16]{
	"", "-", "B1", "b", "C1", "N", "W", 
		"t", "../auxiliary_files/slb.trn",
		"d", "../auxiliary_files/seclarge.lex",
		"j", "../auxiliary_files/unkstats-seclarge",
		"m", "../auxiliary_files/tags.map",
		"O36"
};

void Tagger::Init()
{
	Init(TAGGER_ARGC, TAGGER_ARGS);
}

void Tagger::Init(int argc, char * argv[])
{

	setlocale(LC_CTYPE, "iso_8858_1");
#ifdef SpecialMalloc
	/* Force fast allocation */
	set_small_allocation(100);
#endif

	/* Clear data structures */
	InitDict(dict);
	InitDict(skip_dict);
	InitTrans(trans);
	InitTrans(c_newtrans);
	odictfile = otranfile = NULL;

	/* Verify command line */
	if (argc <= 2)
	  error_exit("Usage: label corpus options\n");

	/* Form options */
	InitOptions;

	set_up_options(argc, argv, &iterations, &initialise, &dict_size,
				dictname, tranname, odictname, otranname, outname, mapname,
				skipname, reducename, fsmname, grammarname, infername, ukwname,
				ofeaturesname, obadwordname, bdbmname, runkstatname, wunkstatname);

	any_output = !no_output || Option(report_stats) || OutOpt(prob_dist);


	/* Open BDBM dictionary */
	if (Option(bdbm)){
		/* Berkeley DB: first of all need to create the dbp data structure*/
		if((ret = db_create(&dbp, NULL, 0)) != 0) {
			fprintf(stderr, "db_create: %s\n", db_strerror(ret));
			exit (1);
		}
		/* Berkeley DB: Then you open it, readonly  */
		if((ret = dbp->open(dbp,bdbmname, NULL, DB_BTREE, DB_RDONLY, 0777)) != 0) {
			dbp->err(dbp, ret, "%s", bdbmname);
			exit(1);
		} 
	}

	/* Read mappings */
	if (Option(verbose)) printf("Read mappings\n");
	read_mapping(mapname);

	/* Read tag reduction mappings */
	if (Option(reduced_tags))
	{
		if (Option(verbose)) printf("Read reduced tag set\n");
		read_reduce_mapping(reducename);
	}

#ifdef Use_Parser
	/* Read parse rules */
	if (Option(use_parser))
	{
		if (Option(verbose)) printf("Read parse rules\n");
		parser_read_named(grammarname);
	}
#endif
#ifdef Use_FSM
	/* Read FSM definitions */
	if (Option(use_fsm))
	{
		if (Option(verbose)) printf("Read FSMs\n");
		fsm_read_named(fsmname);
	}
#endif

	/* Read skip list */
	if (Option(skip_list))
	{
		if (Option(verbose)) printf("Read skip list\n");
		read_named_dict(skipname, &skip_dict, -1);
	}

	/* Read unknown word rules */
	if (Option(unknown_rules))
	{
		if (Option(verbose)) printf("Read unknown word rules\n");
		read_unknown_rules(ukwname);
	}

	/* Set up dictionary [note]:it costs a few seconds*/
	if (dictname[0] == 0)
	{
		create_dict(&dict, dict_size);
		clear_dict(&dict);
	}
	else
	{
		if (Option(verbose)) printf("Read dictionary\n");
		read_named_dict(dictname, &dict, -1);
		if (infername[0] != 0)
		{
			if (Option(verbose)) printf("Read inference rules\n");
			infer_tags((char *)infername, &dict);

		}
	}

	/* Set up transitions [note] it costs a few seconds*/
	if (tranname[0] == 0)
	{
		create_trans(&trans, tags_all);
		clear_trans_all(&trans);
	}
	else
	{
		if (Option(verbose)) printf("Read transitions\n");
		read_named_ascii_trans(tranname, &trans);

		/* Analyze selected features of lexicon to generate tag probabilities for unknown words. */
		if ( Option(unknown_morph) || Option(unknown_rules))
		{
			/* Initialize feature values */

			Allocate(features->featuretags, sizeof(FeatureTagSt), "features->featuretags: main");
			features->featuretags->next_open_slot = 0;

			features->gamma = trans.gamma;

			if ( features->maxsuffix == 0 )
			  features->maxsuffix = MinSuffixLen;
			if ( features->maxunkwords == 0 )
			  features->maxunkwords = MAXUNKWORDS;
			if ( features->maxprefcut == 0 )
			  features->maxprefcut = MinPrefixLen;
			if ( features->maxsuffcut == 0 )
			  features->maxsuffcut = MinSuffixLen;

			unknown_word_handling_initialization();
			gather_unigram_freqs( &dict );
		}

		if ( Option(unknown_morph) )
		{
			analyze_features( &dict, ofeaturesname, obadwordname, &trans, dbp, &dict, runkstatname );
		}
	}

	set_special_words(&dict, features );

	/* Create space for re-estimation or training */
	if (Option(reestimate) || Option(training))
	{
		c_newtrans.gamma = trans.gamma; /* Share arrays */
		create_trans(&c_newtrans, tags_all);
	}

	if (odictname[0] != 0)
	  odictfile = open_file(odictname, "w");
	if (otranname[0] != 0)
	  otranfile = open_file(otranname, "w");

	/* Set up anchor word */
	set_anchor(&dict);

	adjust_dict(&dict, trans.gamma, FALSE);
	adjust_trans(&trans, NULL);
}

string Tagger::RunFromString(const string & text)
{
	string res;
	fstream fs;
	fs.open(g_BUFFER_IN_FILENAME, ios::out);
	if(fs)
	{
		fs<<text<<endl;
		fs.close();
		RunFromFile(g_BUFFER_IN_FILENAME, g_BUFFER_OUT_FILENAME);
		res = loadFile2Str(g_BUFFER_OUT_FILENAME);
	}
	return res;
}

void Tagger::RunFromFile(const char * const in_fname, const char * out_fname)
{
	_TokenFromFile(in_fname, TOKEN_OUT_FNAME);
	_TaggingFromFile(TOKEN_OUT_FNAME, out_fname);
}


void Tagger::_TokenFromFile(const char * const in_fname, const char * const out_fname)
{
	string cmd;
	cmd += "./token.sh ";
	cmd += in_fname;
	cmd += " ";
	cmd += out_fname;
	int res = system(cmd.c_str());
	if(res)
	{
		cerr<<"system cmd : ["<<cmd<<"] failed! return "<<res<<"."<<endl;
		exit(1);
	}
}

void Tagger::_TaggingFromFile(const char * const in_fname, const char * const out_fname)
{
	FILE * infile , * outfile;
	if(NULL == (infile = fopen(in_fname, "r")))
	{
		cerr<<"fopen "<<in_fname<<" failed!"<<endl;
		exit(1);
	}
	if(NULL == (outfile = fopen(out_fname, "w")))
	{
		cerr<<"fopen "<<out_fname<<" failed!"<<endl;
		exit(1);
	}
	_Tagging(infile, outfile);
	fclose(infile);
	fclose(outfile);
}

void Tagger::_Tagging(FILE * infile, FILE * outfile)
{
	init_statistics();
	clear_trans_all(&c_newtrans);
	reset_corpus(infile);
	tag_corpus(infile, outfile, &dict, &skip_dict, &trans, 
				&c_newtrans, dbp, &dict, tranname);
}

void Tagger::Clear()
{
	/* Write c_new arrays */
	if (odictfile != NULL)
	{
		if (Option(verbose))
		  printf("Writing dictionary (%d entries)\n", dict.size);
		write_dict(odictfile, &dict, 
					Option(training) || Option(reestimate));
		fclose(odictfile);
	}
	if (otranfile != NULL)
	{
		if (Option(verbose))
		  printf("Writing transitions\n");
		if (Option(training) || Option(reestimate))
		  write_ascii_trans(otranfile, &c_newtrans);
		else
		  write_ascii_trans(otranfile, &trans);
		fclose(otranfile);
	}

	if (Option(unknown_morph))
	{
		if (obadwordname[0] != 0)
		  fclose(features->obadwordfile);
	}    


	free_dict( &dict );

	if ( Option(bdbm))
	  /* Berkeley DB: close it */
	  dbp->close(dbp,(u_int32_t)0);
}

