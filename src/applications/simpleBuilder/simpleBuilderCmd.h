/* simpleBuilderCmd.h */

/* File autogenerated by gengetopt version 2.11  */

#ifndef SIMPLEBUILDERCMD_H
#define SIMPLEBUILDERCMD_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
#define CMDLINE_PARSER_PACKAGE "simpleBuilder"
#endif

#ifndef CMDLINE_PARSER_VERSION
#define CMDLINE_PARSER_VERSION "1.0"
#endif

struct gengetopt_args_info
{
  char * output_arg;	/* Output file name.  */
  char * latticetype_arg;	/* Lattice type string. Valid types are fcc,hcp,bcc and hcp-water. (default='fcc').  */
  double density_arg;	/* density (g/cm^3).  */
  int nx_arg;	/* number of unit cells in x.  */
  int ny_arg;	/* number of unit cells in y.  */
  int nz_arg;	/* number of unit cells in z.  */

  int help_given ;	/* Whether help was given.  */
  int version_given ;	/* Whether version was given.  */
  int output_given ;	/* Whether output was given.  */
  int latticetype_given ;	/* Whether latticetype was given.  */
  int density_given ;	/* Whether density was given.  */
  int nx_given ;	/* Whether nx was given.  */
  int ny_given ;	/* Whether ny was given.  */
  int nz_given ;	/* Whether nz was given.  */

  char **inputs ; /* unamed options */
  unsigned inputs_num ; /* unamed options number */
} ;

int cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info);

void cmdline_parser_print_help(void);
void cmdline_parser_print_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* SIMPLEBUILDERCMD_H */