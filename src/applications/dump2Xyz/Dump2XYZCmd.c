/*
  File autogenerated by gengetopt version 2.11
  generated with the following command:
  /home/maul/gezelter/tim/bin/gengetopt -F Dump2XYZCmd 

  The developers of gengetopt consider the fixed text that goes in all
  gengetopt output files to be in the public domain:
  we make no copyright claims on it.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "getopt.h"

#include "Dump2XYZCmd.h"

void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n", CMDLINE_PARSER_PACKAGE, CMDLINE_PARSER_VERSION);
}

void
cmdline_parser_print_help (void)
{
  cmdline_parser_print_version ();
  printf("\n"
  "Usage: %s [OPTIONS]...\n", CMDLINE_PARSER_PACKAGE);
  printf("\n");
  printf("  -h, --help                         Print help and exit\n");
  printf("  -V, --version                      Print version and exit\n");
  printf("  -i, --input=filename               input dump file\n");
  printf("  -o, --output=filename              output file name\n");
  printf("  -n, --frame=INT                    print every n frame  (default=`1')\n");
  printf("  -d, --dipole                       print the dipole moments  (default=off)\n");
  printf("  -w, --water                        skip the the waters  (default=off)\n");
  printf("  -m, --periodicBox                  map to the periodic box  (default=off)\n");
  printf("  -z, --zconstraint                  replace the atom types of zconstraint \n                                       molecules  (default=off)\n");
  printf("  -r, --rigidbody                    add a pseudo COM atom to rigidbody  \n                                       (default=off)\n");
  printf("  -t, --watertype                    replace the atom type of water model  \n                                       (default=on)\n");
  printf("  -g, --ignore=atomtype              ignore the atom types\n");
  printf("  -s, --selection=selection script  selection syntax:\n"
         "                                    select DMPC -- select DMPC molecule\n"
         "                                    select DMPC.* -- select all atoms and all rigidbodies belong to DMPC molecule(same as above)\n"
         "                                    select not DMPC -- select all atoms and all rigidbodies do not belong to DMPC molecule\n"
         "                                    select C* and 4 to 7 -- select stuntdoubles which match C* and which index is between [4, 7)\n"
         "                                    select 5 -- select stuntdobule whose global index is 5\n"
         "                                    select within (5.0, HDP or PO4) -- select stuntdoubles which is within 5 A to HDP or PO4\n"
         "                                    select DMPC.3 -- select stuntdouble which internal index is 3\n"
         "                                    select DMPC.DMPC_RB_*.* --select atoms belong to rigid body inside DMPC molecule \n");
  printf("      --originsele=selection script  select origin\n");
  printf("      --refsele=selection script     select reference\n");
  printf("      --repeatX=INT                  The number of images to repeat in the x \n                                       direction  (default=`0')\n");
  printf("      --repeatY=INT                  The number of images to repeat in the y \n                                       direction  (default=`0')\n");
  printf("      --repeatZ=INT                  The number of images to repeat in the z \n                                       direction  (default=`0')\n");
}


static char *gengetopt_strdup (const char *s);

/* gengetopt_strdup() */
/* strdup.c replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

int
cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info)
{
  int c;	/* Character of the parsed option.  */
  int i;        /* Counter */
  struct ignore_list
  {
    char * ignore_arg;
    struct ignore_list * next;
  };
  struct ignore_list * ignore_list = NULL,* ignore_new = NULL;
  
  int missing_required_options = 0;

  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->input_given = 0 ;
  args_info->output_given = 0 ;
  args_info->frame_given = 0 ;
  args_info->dipole_given = 0 ;
  args_info->water_given = 0 ;
  args_info->periodicBox_given = 0 ;
  args_info->zconstraint_given = 0 ;
  args_info->rigidbody_given = 0 ;
  args_info->watertype_given = 0 ;
  args_info->ignore_given = 0 ;
  args_info->selection_given = 0 ;
  args_info->originsele_given = 0 ;
  args_info->refsele_given = 0 ;
  args_info->repeatX_given = 0 ;
  args_info->repeatY_given = 0 ;
  args_info->repeatZ_given = 0 ;
#define clear_args() { \
  args_info->input_arg = NULL; \
  args_info->output_arg = NULL; \
  args_info->frame_arg = 1 ;\
  args_info->dipole_flag = 0;\
  args_info->water_flag = 0;\
  args_info->periodicBox_flag = 0;\
  args_info->zconstraint_flag = 0;\
  args_info->rigidbody_flag = 0;\
  args_info->watertype_flag = 1;\
  args_info->ignore_arg = NULL; \
  args_info->selection_arg = NULL; \
  args_info->originsele_arg = NULL; \
  args_info->refsele_arg = NULL; \
  args_info->repeatX_arg = 0 ;\
  args_info->repeatY_arg = 0 ;\
  args_info->repeatZ_arg = 0 ;\
}

  clear_args();

  optarg = 0;
  optind = 1;
  opterr = 1;
  optopt = '?';

  while (1)
    {
      int option_index = 0;
      char *stop_char;

      static struct option long_options[] = {
        { "help",	0, NULL, 'h' },
        { "version",	0, NULL, 'V' },
        { "input",	1, NULL, 'i' },
        { "output",	1, NULL, 'o' },
        { "frame",	1, NULL, 'n' },
        { "dipole",	0, NULL, 'd' },
        { "water",	0, NULL, 'w' },
        { "periodicBox",	0, NULL, 'm' },
        { "zconstraint",	0, NULL, 'z' },
        { "rigidbody",	0, NULL, 'r' },
        { "watertype",	0, NULL, 't' },
        { "ignore",	1, NULL, 'g' },
        { "selection",	1, NULL, 's' },
        { "originsele",	1, NULL, 0 },
        { "refsele",	1, NULL, 0 },
        { "repeatX",	1, NULL, 0 },
        { "repeatY",	1, NULL, 0 },
        { "repeatZ",	1, NULL, 0 },
        { NULL,	0, NULL, 0 }
      };

      stop_char = 0;
      c = getopt_long (argc, argv, "hVi:o:n:dwmzrtg:s:", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':	/* Print help and exit.  */
          clear_args ();
          cmdline_parser_print_help ();
          exit (EXIT_SUCCESS);

        case 'V':	/* Print version and exit.  */
          clear_args ();
          cmdline_parser_print_version ();
          exit (EXIT_SUCCESS);

        case 'i':	/* input dump file.  */
          if (args_info->input_given)
            {
              fprintf (stderr, "%s: `--input' (`-i') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->input_given = 1;
          args_info->input_arg = gengetopt_strdup (optarg);
          break;

        case 'o':	/* output file name.  */
          if (args_info->output_given)
            {
              fprintf (stderr, "%s: `--output' (`-o') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->output_given = 1;
          args_info->output_arg = gengetopt_strdup (optarg);
          break;

        case 'n':	/* print every n frame.  */
          if (args_info->frame_given)
            {
              fprintf (stderr, "%s: `--frame' (`-n') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->frame_given = 1;
          args_info->frame_arg = strtol (optarg,&stop_char,0);
          break;

        case 'd':	/* print the dipole moments.  */
          if (args_info->dipole_given)
            {
              fprintf (stderr, "%s: `--dipole' (`-d') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->dipole_given = 1;
          args_info->dipole_flag = !(args_info->dipole_flag);
          break;

        case 'w':	/* skip the the waters.  */
          if (args_info->water_given)
            {
              fprintf (stderr, "%s: `--water' (`-w') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->water_given = 1;
          args_info->water_flag = !(args_info->water_flag);
          break;

        case 'm':	/* map to the periodic box.  */
          if (args_info->periodicBox_given)
            {
              fprintf (stderr, "%s: `--periodicBox' (`-m') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->periodicBox_given = 1;
          args_info->periodicBox_flag = !(args_info->periodicBox_flag);
          break;

        case 'z':	/* replace the atom types of zconstraint molecules.  */
          if (args_info->zconstraint_given)
            {
              fprintf (stderr, "%s: `--zconstraint' (`-z') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->zconstraint_given = 1;
          args_info->zconstraint_flag = !(args_info->zconstraint_flag);
          break;

        case 'r':	/* add a pseudo COM atom to rigidbody.  */
          if (args_info->rigidbody_given)
            {
              fprintf (stderr, "%s: `--rigidbody' (`-r') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->rigidbody_given = 1;
          args_info->rigidbody_flag = !(args_info->rigidbody_flag);
          break;

        case 't':	/* replace the atom type of water model.  */
          if (args_info->watertype_given)
            {
              fprintf (stderr, "%s: `--watertype' (`-t') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->watertype_given = 1;
          args_info->watertype_flag = !(args_info->watertype_flag);
          break;

        case 'g':	/* ignore the atom types.  */
          args_info->ignore_given++;
          ignore_new = (struct ignore_list *) malloc (sizeof (struct ignore_list));
          ignore_new->next = ignore_list;
          ignore_list = ignore_new;
          ignore_new->ignore_arg = gengetopt_strdup (optarg);
          break;

        case 's':	/* general selection syntax(now only support select atoms within a specified range).  */
          if (args_info->selection_given)
            {
              fprintf (stderr, "%s: `--selection' (`-s') option given more than once\n", CMDLINE_PARSER_PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->selection_given = 1;
          args_info->selection_arg = gengetopt_strdup (optarg);
          break;


        case 0:	/* Long option with no short option */
          /* select origin.  */
          if (strcmp (long_options[option_index].name, "originsele") == 0)
          {
            if (args_info->originsele_given)
              {
                fprintf (stderr, "%s: `--originsele' option given more than once\n", CMDLINE_PARSER_PACKAGE);
                clear_args ();
                exit (EXIT_FAILURE);
              }
            args_info->originsele_given = 1;
            args_info->originsele_arg = gengetopt_strdup (optarg);
            break;
          }
          
          /* select reference.  */
          else if (strcmp (long_options[option_index].name, "refsele") == 0)
          {
            if (args_info->refsele_given)
              {
                fprintf (stderr, "%s: `--refsele' option given more than once\n", CMDLINE_PARSER_PACKAGE);
                clear_args ();
                exit (EXIT_FAILURE);
              }
            args_info->refsele_given = 1;
            args_info->refsele_arg = gengetopt_strdup (optarg);
            break;
          }
          
          /* The number of images to repeat in the x direction.  */
          else if (strcmp (long_options[option_index].name, "repeatX") == 0)
          {
            if (args_info->repeatX_given)
              {
                fprintf (stderr, "%s: `--repeatX' option given more than once\n", CMDLINE_PARSER_PACKAGE);
                clear_args ();
                exit (EXIT_FAILURE);
              }
            args_info->repeatX_given = 1;
            args_info->repeatX_arg = strtol (optarg,&stop_char,0);
            break;
          }
          
          /* The number of images to repeat in the y direction.  */
          else if (strcmp (long_options[option_index].name, "repeatY") == 0)
          {
            if (args_info->repeatY_given)
              {
                fprintf (stderr, "%s: `--repeatY' option given more than once\n", CMDLINE_PARSER_PACKAGE);
                clear_args ();
                exit (EXIT_FAILURE);
              }
            args_info->repeatY_given = 1;
            args_info->repeatY_arg = strtol (optarg,&stop_char,0);
            break;
          }
          
          /* The number of images to repeat in the z direction.  */
          else if (strcmp (long_options[option_index].name, "repeatZ") == 0)
          {
            if (args_info->repeatZ_given)
              {
                fprintf (stderr, "%s: `--repeatZ' option given more than once\n", CMDLINE_PARSER_PACKAGE);
                clear_args ();
                exit (EXIT_FAILURE);
              }
            args_info->repeatZ_given = 1;
            args_info->repeatZ_arg = strtol (optarg,&stop_char,0);
            break;
          }
          

        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          exit (EXIT_FAILURE);

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c\n", CMDLINE_PARSER_PACKAGE, c);
          abort ();
        } /* switch */
    } /* while */


  if (! args_info->input_given)
    {
      fprintf (stderr, "%s: '--input' ('-i') option required\n", CMDLINE_PARSER_PACKAGE);
      missing_required_options = 1;
    }
  if ( missing_required_options )
    exit (EXIT_FAILURE);

  if (args_info->ignore_given)
    {
      args_info->ignore_arg = (char * *) malloc (args_info->ignore_given * sizeof (char *));
      for (i = 0; i < args_info->ignore_given; i++)
        {
          args_info->ignore_arg [i] = ignore_list->ignore_arg;
          ignore_list = ignore_list->next;
        }
    }
  
  return 0;
}
