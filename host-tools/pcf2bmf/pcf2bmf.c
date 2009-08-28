#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <getopt.h>
#include "pcf.h"

pcffont_t pcfFont;
char sOutFilename[256];
int  nFontCount;

static void help(void)
{
	printf("Usage: pcf2bmf [options] ...\n"
		"  -h --help\t\t\tPrint this help message\n"
		"  -f --file\t\t\tpcf name to process \n"
		"  -o --output\t\t\toutput filename\n"
		"  -c --count\t\t\tgenerate font count\n"
		);
}

static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "file", 1, 0, 'f' },
	{ "output", 1, 0, 'o' },
	{ "count", 1, 0, 'c' },
	{ NULL, 0, NULL, 0 }
};

int main(int argc, char *argv[])
{
	ucs4_t c;
	int bStop;
	pcf_bm_t *bm;
	pcf_charmet_t cm;
	pcf_SCcharmet_t sm;
	char sFile[128],sFilename[256];

	setlocale( LC_ALL, "" );
        if(argc < 3)
        {
           help();
           exit(2);
        }
        int oc;
        nFontCount = 65535;
        while((oc=getopt(argc,argv,"hf:o:c:"))!=-1)
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
		case 'o':
			strncpy(sOutFilename, optarg, sizeof(sOutFilename) - 1);
			sOutFilename[sizeof(sOutFilename) - 1] = '\0';
			break;
		case 'c':
                        nFontCount = atoi(optarg);
			break;
		default:
			help();
			exit(2);
		} 
        }

        
	pcfFont.file = sFilename;
	if (load_pcf(&pcfFont) >= 0)
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
