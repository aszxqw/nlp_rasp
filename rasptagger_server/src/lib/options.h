#ifndef OPTIONS_H
#define OPTIONS_H
#ifdef __cplusplus
extern "C" {
#endif
#include "mainl.h"
void set_up_options(int argc, char *argv[],
			int *iterations, int *initialise, int *dict_size,
			char *dict, char *tran, char *odict, char *otran,
			char *out, char *map, char *skip, char *reduce,
			char *fsm, char *grammar, char *infer, char *ukw,
			char *ofeatures, char *obadword, char *bdbm, char *runkstat,
			char *wunkstat );

BOOL check_names(char *dict, char *tran, char *kind);
#ifdef __cplusplus
}
#endif
#endif
