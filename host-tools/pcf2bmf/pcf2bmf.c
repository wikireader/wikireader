#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <getopt.h>
#include "pcf.h"

char sOutFilename[256];
int  nFontCount;
int nAddGap = 0;

static void help(void)
{
	printf("Usage: pcf2bmf [options] ...\n"
		"  -h --help\t\t\tPrint this help message\n"
		"  -f --file\t\t\tpcf name to process \n"
		"  -o --output\t\t\toutput filename\n"
		"  -c --count\t\t\tgenerate font count\n"
		"  -g --gap\t\t\tadd 1 pixel to the right of each character\n"
		"  -m --merge\t\tbase bmf file name for the pcf file to be merged into\n"
		);
}

static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "file", 1, 0, 'f' },
	{ "output", 1, 0, 'o' },
	{ "count", 1, 0, 'c' },
	{ "gap", 0, 0, 'g' },
	{ "merge", 1, 0, 'm' },
	{ NULL, 0, NULL, 0 }
};

int main(int argc, char *argv[])
{
	ucs4_t c;
	char sPcfFilename[256];
	char sBmfFilename[256];
	pcffont_t pcfFont;

	setlocale( LC_ALL, "" );
	if(argc < 3)
	{
	   help();
	   exit(2);
	}
	int oc;
	nFontCount = 65535;
	sPcfFilename[0] = '\0';
	sBmfFilename[0] = '\0';
	while((oc=getopt_long(argc,argv,"hf:m:n:o:c:gpq", opts, NULL))!=-1)
	{
		switch (oc) {
		case 'h':
			help();
			exit(0);
			break;
		case 'f':
			strncpy(sPcfFilename, optarg, sizeof(sPcfFilename) - 1);
			sPcfFilename[sizeof(sPcfFilename) - 1] = '\0';
			break;
		case 'm':
			strncpy(sBmfFilename, optarg, sizeof(sBmfFilename) - 1);
			sBmfFilename[sizeof(sBmfFilename) - 1] = '\0';
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
		default:
			help();
			exit(2);
		}
	}

	if (sPcfFilename[0])
	{
		pcfFont.file = sPcfFilename;
		if (load_pcf(&pcfFont, sBmfFilename) >= 0)
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
