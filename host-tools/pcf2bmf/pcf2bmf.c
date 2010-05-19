#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <getopt.h>
#include "pcf.h"

pcffont_t pcfFont, pcfFontMerged;
char sOutFilename[256];
int  nFontCount;
int nAddGap = 0;
int nAddGapMerged = 0;

static void help(void)
{
	printf("Usage: pcf2bmf [options] ...\n"
		"  -h --help\t\t\tPrint this help message\n"
		"  -f --file\t\t\tpcf name to process \n"
		"  -o --output\t\t\toutput filename\n"
		"  -c --count\t\t\tgenerate font count\n"
		"  -g --gap\t\t\tadd 1 pixel to the right of each character\n"
		"  -m --merge\t\tpcf name to be merged\n"
		"  -p --mergegap\t\tadd 1 pixel to the right of each character of the merged pcf\n"
		);
}

static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "file", 1, 0, 'f' },
	{ "output", 1, 0, 'o' },
	{ "count", 1, 0, 'c' },
	{ "gap", 0, 0, 'g' },
	{ "merge", 1, 0, 'm' },
	{ "mergegap", 0, 0, 'p' },
	{ NULL, 0, NULL, 0 }
};

int main(int argc, char *argv[])
{
	ucs4_t c;
	char sFilename[256];
	char sFilenameMerged[256];

	setlocale( LC_ALL, "" );
	if(argc < 3)
	{
	   help();
	   exit(2);
	}
	int oc;
	nFontCount = 65535;
	sFilename[0] = '\0';
	sFilenameMerged[0] = '\0';
	while((oc=getopt_long(argc,argv,"hf:m:o:c:gp", opts, NULL))!=-1)
	{
		switch (oc) {
		case 'h':
			help();
			exit(0);
			break;
		case 'f':
			strncpy(sFilename, optarg, sizeof(sFilename) - 1);
			sFilename[sizeof(sFilename) - 1] = '\0';
			break;
		case 'm':
			strncpy(sFilenameMerged, optarg, sizeof(sFilenameMerged) - 1);
			sFilename[sizeof(sFilenameMerged) - 1] = '\0';
			break;
		case 'o':
			strncpy(sOutFilename, optarg, sizeof(sOutFilename) - 1);
			sOutFilename[sizeof(sOutFilename) - 1] = '\0';
			break;
		case 'c':
			nFontCount = atoi(optarg);
			break;
		case 'g':
			nAddGap = 1;
			break;
		case 'p':
			nAddGapMerged = 1;
			break;
		default:
			help();
			exit(2);
		}
	}

	if (sFilename[0] && sFilenameMerged[0])
	{
		pcfFont.file = sFilename;
		pcfFontMerged.file = sFilenameMerged;
		if (load_pcf(&pcfFont, &pcfFontMerged) >= 0)
		{
			fprintf(stdout, "%d %d %d\n", pcfFont.Fmetrics.ascent, pcfFont.Fmetrics.descent,
					pcfFont.Fmetrics.linespace);

			return 0;
		}
		else
		{
			fprintf(stdout,"Fail to load font file.  Press ENTER to exit.\n");
			c = getwchar();
			return -1;
		}
	}
	else if (sFilename[0])
	{
		pcfFont.file = sFilename;
		if (load_pcf(&pcfFont, NULL) >= 0)
		{
			fprintf(stdout, "%d %d %d\n", pcfFont.Fmetrics.ascent, pcfFont.Fmetrics.descent,
					pcfFont.Fmetrics.linespace);

			return 0;
		}
		else
		{
			fprintf(stdout,"Fail to load font file.  Press ENTER to exit.\n");
			c = getwchar();
			return -1;
		}
	}
	else
	{
		fprintf(stdout,"No PCF file specified.  Press ENTER to exit.\n");
		c = getwchar();
		return -1;
	}
}
