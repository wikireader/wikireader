%{
#include <stdio.h>
#include "ib_internal.h"
#undef EXTERN
#include "ibP.h"
#include <string.h>
#include <stdlib.h>
#include "ibConfYacc.h"
#include "ibConfLex.h"

#define YYERROR_VERBOSE

YY_DECL;

#define YYPARSE_PARAM parse_arg
#define YYLEX_PARAM priv(YYPARSE_PARAM)->yyscanner

typedef struct
{
	yyscan_t yyscanner;
	ibConf_t *configs;
	unsigned int configs_length;
	unsigned int config_index;
	ibBoard_t *boards;
	unsigned int boards_length;
	int board_index;
}gpib_yyparse_private_t;

static inline gpib_yyparse_private_t* priv( gpib_yyparse_private_t *parse_arg )
{
	return parse_arg;
}

static inline ibConf_t* current_config( gpib_yyparse_private_t *parse_arg )
{
	return &parse_arg->configs[ parse_arg->config_index ];
}

static inline ibBoard_t* current_board( gpib_yyparse_private_t *parse_arg )
{
	if( parse_arg->board_index < 0 ) return NULL;
	return &parse_arg->boards[ parse_arg->board_index ];
}

void init_gpib_yyparse_private( gpib_yyparse_private_t *priv )
{
	priv->yyscanner = 0;
	priv->configs = NULL;
	priv->configs_length = 0;
	priv->config_index = 0;
	priv->boards = NULL;
	priv->boards_length = 0;
	priv->board_index = -1;
}

int parse_gpib_conf( const char *filename, ibConf_t *configs, unsigned int configs_length,
	ibBoard_t *boards, unsigned int boards_length )
{
	FILE *infile;
	int retval = 0;
	int i;
	gpib_yyparse_private_t priv;

	if( ( infile = fopen( filename, "r" ) ) == NULL )
	{
		fprintf(stderr, "failed to open configuration file\n");
		setIberr( EDVR );
		setIbcnt( errno );
		return -1;
	}

	init_gpib_yyparse_private( &priv );
	priv.configs = configs;
	priv.configs_length = configs_length;
	priv.boards = boards;
	priv.boards_length = boards_length;
	for( i = 0; i < priv.configs_length; i++ )
	{
		init_ibconf( &priv.configs[ i ] );
	}
	for( i = 0; i < priv.boards_length; i++ )
	{
		init_ibboard( &priv.boards[ i ] );
	}
	gpib_yylex_init(&priv.yyscanner);
	gpib_yyrestart(infile, priv.yyscanner);
	if(gpib_yyparse(&priv))
	{
		fprintf(stderr, "libgpib: failed to parse configuration file\n");
//XXX setIberr()
		retval = -1 ;
	}
	gpib_yylex_destroy(priv.yyscanner);
	fclose(infile);

	if( retval == 0 )
	{
		for(i = 0; i < priv.configs_length && priv.configs[ i ].defaults.board >= 0; i++)
		{
			priv.configs[ i ].settings = priv.configs[ i ].defaults;
		}
	}

	return retval;
}

static void gpib_conf_warn_missing_equals()
{
	fprintf(stderr, "WARNING: omitting \"=\" before a boolean value in gpib config file is deprecated.\n");
}

%}

%pure_parser

%union
{
int  ival;
char *sval;
char bval;
char cval;
}

%token T_INTERFACE T_DEVICE T_NAME T_MINOR T_BASE T_IRQ T_DMA
%token T_PAD T_SAD T_TIMO T_EOSBYTE T_BOARD_TYPE T_PCI_BUS T_PCI_SLOT
%token T_REOS T_BIN T_INIT_S T_DCL T_XEOS T_EOT
%token T_MASTER T_LLO T_EXCL T_INIT_F T_AUTOPOLL

%token T_NUMBER T_STRING T_BOOL T_TIVAL
%type <ival> T_NUMBER
%type <ival> T_TIVAL
%type <sval> T_STRING
%type <bval> T_BOOL

%%

	input: /* empty */
		| device input
		| interface input
		| error
			{
				fprintf(stderr, "input error on line %i of %s\n", gpib_yyget_lineno(priv(parse_arg)->yyscanner), DEFAULT_CONFIG_FILE);
				YYABORT;
			}
		;

	interface: T_INTERFACE '{' minor parameter '}'
			{
				current_config( parse_arg )->is_interface = 1;
				if( ++( priv(parse_arg)->config_index ) >= priv(parse_arg)->configs_length )
				{
					fprintf(stderr, "too many devices in config file\n");
					YYERROR;
				}
			}
		;

	minor : T_MINOR '=' T_NUMBER {
				priv(parse_arg)->board_index = $3;
				current_config(parse_arg)->defaults.board = $3;
				if(priv(parse_arg)->board_index < priv(parse_arg)->boards_length )
					snprintf(current_board(parse_arg)->device, sizeof(current_board( parse_arg )->device), "/dev/gpib%i", priv(parse_arg)->board_index);
				else
					YYERROR;
			}
		;

	parameter: /* empty */
		| statement parameter
		| error
			{
				fprintf(stderr, "parameter error on line %i of %s\n", @1.first_line, DEFAULT_CONFIG_FILE);
				YYABORT;
			}
		;

	statement: T_PAD '=' T_NUMBER      { current_config( parse_arg )->defaults.pad = $3;}
		| T_SAD '=' T_NUMBER      { current_config( parse_arg )->defaults.sad = $3 - sad_offset;}
		| T_EOSBYTE '=' T_NUMBER  { current_config( parse_arg )->defaults.eos = $3;}
		| T_REOS T_BOOL           { gpib_conf_warn_missing_equals(); current_config( parse_arg )->defaults.eos_flags |= $2 * REOS;}
		| T_BIN  T_BOOL           { gpib_conf_warn_missing_equals(); current_config( parse_arg )->defaults.eos_flags |= $2 * BIN;}
		| T_REOS '=' T_BOOL           { current_config( parse_arg )->defaults.eos_flags |= $3 * REOS;}
		| T_XEOS '=' T_BOOL           { current_config( parse_arg )->defaults.eos_flags |= $3 * XEOS;}
		| T_BIN '=' T_BOOL           { current_config( parse_arg )->defaults.eos_flags |= $3 * BIN;}
		| T_EOT '=' T_BOOL           { current_config( parse_arg )->defaults.send_eoi = $3;}
		| T_TIMO '=' T_TIVAL      { current_config( parse_arg )->defaults.usec_timeout = $3; }
		| T_TIMO '=' T_NUMBER      { current_config( parse_arg )->defaults.usec_timeout = timeout_to_usec( $3 ); }
		| T_BASE '=' T_NUMBER     { current_board( parse_arg )->base = $3; }
		| T_IRQ  '=' T_NUMBER     { current_board( parse_arg )->irq = $3; }
		| T_DMA  '=' T_NUMBER     { current_board( parse_arg )->dma = $3; }
		| T_PCI_BUS  '=' T_NUMBER     { current_board( parse_arg )->pci_bus = $3; }
		| T_PCI_SLOT  '=' T_NUMBER     { current_board( parse_arg )->pci_slot = $3; }
		| T_MASTER T_BOOL	{ gpib_conf_warn_missing_equals(); current_board( parse_arg )->is_system_controller = $2; }
		| T_MASTER '=' T_BOOL	{ current_board( parse_arg )->is_system_controller = $3; }
		| T_BOARD_TYPE '=' T_STRING
			{
				strncpy(current_board( parse_arg )->board_type, $3,
					sizeof(current_board( parse_arg )->board_type));
			}
		| T_NAME '=' T_STRING
			{
				strncpy(current_config( parse_arg )->name, $3,
					sizeof(current_config( parse_arg )->name));
			}
		;

	device: T_DEVICE '{' option '}'
			{
				current_config( parse_arg )->is_interface = 0;
				if( ++( priv(parse_arg)->config_index ) >= priv(parse_arg)->configs_length )
				{
					fprintf(stderr, "too many devices in config file\n");
					YYERROR;
				}
			}
		;

	option: /* empty */
		| assign option
		| error
 			{
 				fprintf(stderr, "option error on line %i of config file\n", @1.first_line );
				YYABORT;
			}
		;

	assign:
		T_PAD '=' T_NUMBER { current_config( parse_arg )->defaults.pad = $3; }
		| T_SAD '=' T_NUMBER { current_config( parse_arg )->defaults.sad = $3 - sad_offset; }
		| T_INIT_S '=' T_STRING { strncpy(current_config( parse_arg )->init_string,$3,60); }
		| T_EOSBYTE '=' T_NUMBER  { current_config( parse_arg )->defaults.eos = $3; }
		| T_REOS T_BOOL           { gpib_conf_warn_missing_equals(); current_config( parse_arg )->defaults.eos_flags |= $2 * REOS;}
		| T_REOS '=' T_BOOL           { current_config( parse_arg )->defaults.eos_flags |= $3 * REOS;}
		| T_XEOS '=' T_BOOL           { current_config( parse_arg )->defaults.eos_flags |= $3 * XEOS;}
		| T_BIN T_BOOL           { gpib_conf_warn_missing_equals(); current_config( parse_arg )->defaults.eos_flags |= $2 * BIN; }
		| T_BIN '=' T_BOOL           { current_config( parse_arg )->defaults.eos_flags |= $3 * BIN; }
		| T_EOT '=' T_BOOL           { current_config( parse_arg )->defaults.send_eoi = $3;}
		| T_AUTOPOLL              { current_config( parse_arg )->flags |= CN_AUTOPOLL; }
		| T_INIT_F '=' flags
		| T_NAME '=' T_STRING	{ strncpy(current_config( parse_arg )->name,$3, sizeof(current_config( parse_arg )->name));}
		| T_MINOR '=' T_NUMBER	{ current_config( parse_arg )->defaults.board = $3;}
		| T_TIMO '=' T_TIVAL      { current_config( parse_arg )->defaults.usec_timeout = $3; }
		| T_TIMO '=' T_NUMBER      { current_config( parse_arg )->defaults.usec_timeout = timeout_to_usec( $3 ); }
		;

	flags: /* empty */
		| ',' flags
		| oneflag flags
		;

	oneflag: T_LLO       { current_config( parse_arg )->flags |= CN_SLLO; }
		| T_DCL       { current_config( parse_arg )->flags |= CN_SDCL; }
		| T_EXCL      { current_config( parse_arg )->flags |= CN_EXCLUSIVE; }
		;

%%



void yyerror(char *s)
{
	fprintf(stderr, "%s\n", s);
}



