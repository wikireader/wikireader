/*
 * binary search 
 * (C) Copyright 2008 OpenMoko, Inc.
 * Author: xiangfu liu <xiangfu@openmoko.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define SHA1CHARS 100		/* sha1 char count */
#define MAXCHARS 300		/* the max chars of the title */
#define LINECHARS 400
#define RESULTCOUNT 10

FILE *fp;
char *g_titles[] = {
	". .\0",
	"1\0",
	"2 am club\0",
	"3 car garage\0",
	"4 aquilae\0",
	"5 alive\0",
	"6 april\0",
	"7 as\0",
	"8 ball\0",
	"9 brigade\0",
	"0 k\0",
	"a 44\0",
	"bch formula\0",
	"bchb\0",
	"bche\0",
	"bchl\0",
	"bchs\0",
	"bchydro\0",
	"d arae\0",
	"e boats\0",
	"egf\0",
	"egfa\0",
	"egfe\0",
	"egff\0",
	"egfh\0",
	"egfl7\0",
	"egfp\0",
	"egfr\0",
	"egg project\0",
	"eggd\0",
	"eggp\0",
	"eggr 239\0",
	"eggr 265\0",
	"eggr 572\0",
	"f w olin\0",
	"f w olin foundation\0",
	"f w pomeroy\0",
	"f w s craig\0",
	"george a. loud\0",
	"octavian paler\0",
	"ou\0",
	"ou andromedae\0",
	"ou center for public management\0",
	"ou chant\0",
	"ou sooners\0",
	"ou band\0",
	"ou812\0",
	"ou812 tour\0",
	"oua\0",
	"ouabc\0",
	"ouac\0",
	"ouat\0",
	"ouatic\0",
	"ouatitw\0",
	"yzosse\0",
	"z\0",
	"z\0",
	"zyzzyxdonta\0",
	"zyzzyxdonta alata\0",
	"zyzzyzus\0",
};

char **g_hash = {
	"7264a4c17f2d9d1dbc4bd02731b61722d8d40ef0\0",
	"4d3a29a587a59da7a4a234f7261e4fc4d38e5105\0",
	"4e491fd4a13e63623f10283a53d2e0ed5d51fb13\0",
	"d71e704651948a90d52d539f0a838b578d3dbba2\0",
	"0171634ddad02e36d5c890680fd7acb6b329ac9\0",
	"de98b5313f9d094da906992b6d5ad679da81f24c\0",
	"7c3ea60927fd5acd902dae6f6d5bfe6474d09de2\0",
	"becefce21068a05fea53388c790d87d324f324a8\0",
	"350410fc4fc2160b61281d3f96fde1e4a188a88e\0",
	"0c8cc026358e83d75fcbc8019f5d064111f0d2e3\0",
	"c777478eb4ece5f67cecf7a888124972d5e3cdba\0",
	"53e7c979ec49aca49ed1a1fbfda0684a62f5aeeb\0",
	"450694bfb9737727e9c6598f02bc1fc9460487ca\0",
	"85fa500984444f20b001a026324e330fddcd5897\0",
	"36a3ea503692796f265f23abc8075d90bda82ffd\0",
	"705af60dc0c553ec50b39b64fb23ca531a5fd6f7\0",
	"a854c2c9a83445912cee3e5eb9b9f2c615fd339b\0",
	"3a9b5bd5f8cb468d2fbed9ee1e610d3788ef58b7\0",
	"96283dbce2bdf882203ba9f0c13b1fe6a77e9eb9\0",
	"9cfb00219884b33d79b91d9447821a3ada4a1dc2\0",
	"26fdb0e23dd78b58d79163ec6e8eafb2bb736b2a\0",
	"0ff5a1aee1ba4698392c51079b7cbbb1ccfb798f\0",
	"7a86636e79ef3d61f9daadd456a624c14517dae4\0",
	"761514dd65d2928e8d1c1d591af7b086af074b02\0",
	"cf5858df721a2f0f0d0def0c9481d37cca205207\0",
	"1e93b79ccde9d0ec1e5a3bd9922a2393927abd46\0",
	"28fd1712f1e642a457393a040268fce78ce7426c\0",
	"527a0b3aa1a0deff029d6f865dd464c522d45979\0",
	"2fb835ac4c2fdc5562ad21896765a5488d005d99\0",
	"7dd1ff7bc1c1170c4854b411efc09f9d00e81311\0",
	"6b3c05161fdb7f5ff64f62ce03c5744b7046dbdf\0",
	"60831f8c46d5523947f5d9a015aafc93759a04a3\0",
	"c253e070274c025207a6953cc174c5b28dba176b\0",
	"86010f050e829bc41591367e3d43b1ac41957150\0",
	"31e96a5eec9f68fb00d6e54bb97b3dd3f2b22bf2\0",
	"babd1285a08bf7a6cb68a3296f912a4ee58b06db\0",
	"fd75d76e816874d352977f2c4a6ccb03aad6dc03\0",
	"7392528ab81f2d56903611d0aaca819f321f0d2f\0",
	"f83ed9d451781fcd822d05cdd6d87570ff3ab062\0",
	"024f4d24c5619af7f67adf59d059292ca1c3bd3d\0",
	"4d9ccb5331dea3aab31487e513c2fe11c46055da\0",
	"611f88d11d9813b6178cf104660e94dfcb35b66d\0",
	"93707eb763a0898d582b9b36c8e5006264564e7e\0",
	"991068f641dacbbe5a725e328f9e4d576d658d07\0",
	"6409aac47ea292abf71e5a727fc3481cd044246d\0",
	"f2bc73e68378d6d6e6454bdc6b7707db0065ae72\0",
	"32fb25755f458012a18e3521074d456e5531f4da\0",
	"558a075602d24b4e33f6a091581f4e75163d095a\0",
	"2ff8b996a7a9d982340395263221a5146745aba6\0",
	"18038cc12153786cc2779c2eacb9ddd8f12dada2\0",
	"a8bec86e8534edc88e60ce2737ae5979948437fa\0",
	"e49c2a8becf2a8c7548adb804f4ceba96949eae4\0",
	"e3f9b81f96300c541184f8c3b0cf2d7b4fd52749\0",
	"b4612bbeffdc2371d8d6e9a8bd7d1d1300fbf9ce\0",
	"d6b6d6f317d247c660e42e86d7a4f75ed7d4b021\0",
	"8056f82374131414e7fbb021d2b40062605c28b7\0",
	"3b6d1327b30b9e693fce24c81a12ac97e010cd06\0",
	"95555a00198f058ec0f9379e4d02985120177ea4\0",
	"95555a00198f058ec0f9379e4d02985120177ea4\0",
	"e189d1dbab15374976c2e87d5d97ef5a5dd86084\0",
};

char  g_result[RESULTCOUNT][LINECHARS];

int split(char *source, char *word, char *sha1, char split_char)
{
	if(*source == 0){
		*word = 0;
		*sha1 = 0;
		return 0;
	}
	char *p = strrchr(source, split_char);
	int i=0;
	int split_char_pos = p - source;
	for(i = 0; i < split_char_pos; i++){
                *(word++) = *(source++);
	}
	word -= 2;
	*word='\0';
	source++;		/* eat the blank */
        while(*source != '\n' && *source != EOF)
                *(sha1++) = *(source++);
	*sha1='\0';

        return 0;
}

int scomp(const void *p, const void *q )
{
	return strcmp( (char*) p, (char*) q );
}

int display_array(char array[][LINECHARS], int n)
{
	printf("---------------------\n");
	int i = 0, k = 0;
	while (i < n - 2 && array[i][0] != NULL) {
		while ( array[i][k] != '\0')
			printf("%c", array[i][k++]);
		i++;
		k = 0;
	}
	return 0;
}

int binary_search (FILE *fp, char *key)
{
	char line[LINECHARS], title[MAXCHARS], hash[SHA1CHARS];
	long left = 0;
	long right = 0;
	long middle;
	int comp;

	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &right);
	printf("binary search end:%d\n", right);

	rewind(fp);

	while (left <= right) { 
		middle = (left + right) / 2;
		printf("middle:%ld\nleft:%ld\nright:%ld\n", middle, left, right);
		fsetpos(fp, &middle);
		fgets(line, LINECHARS, fp);
		printf("binary search line 1:%s\n", line);
		fgets(line, LINECHARS, fp);
		printf("binary search line 2:%s\n", line);
		split(line, title, hash, '-');
		printf("binary search title:%s\n", title);
		comp = scomp(key, title);
		if (comp == 0) {
			strcpy(g_result[0], line);
			int i = 1;
			for (i = 1; i < RESULTCOUNT; i++) {
				fgets(line, LINECHARS, fp);
				strcpy(g_result[i], line);
			}
			return middle;
		}
		if (comp > 0)
			left = middle + 1;
		else 
			right = middle - 1;
	}
	return -1;
}

char ** lookup(char *key)
{
	clock_t i=0, globalstarttime=0;

	char line[LINECHARS], title[MAXCHARS], hash[SHA1CHARS];
	char line_malloc[LINECHARS];
	int k = 0;

	globalstarttime = clock();
	while (!feof(fp)){
		if (fgets(line, LINECHARS, fp) != NULL){
			split(line, title, hash, '-');
			printf("%s", line);
			printf("%s---%s\n", title, hash);
			int comp = strcmp(key, title);
			if (comp == 0) {
				break;
			}
		}
	}
	i = clock() - globalstarttime;
	printf("Total clicks\t%ld\nTotal secs\t%4.3f\n",
	       i, (double) i / CLOCKS_PER_SEC);

	strcpy(g_result[0], line);
	for (k = 1; k< RESULTCOUNT; k++) {
		if (!feof(fp)){
			if (fgets(line_malloc, LINECHARS, fp) != NULL){
				strcpy(g_result[k], line_malloc);
			} else {
				break;
			}
		} else {
			break;
		}
	}

	return g_result;
}

int main(int argc, char *argv[])
{
	int k = 0;
	char ** result;
	char *fname;

	if (argc != 2)/* no args */
		printf("one arg: file name \n");
	else /* at least one arg: file name */
		fname = argv[1];
	
	setbuf(stdout, 0);
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "  Can't open file\n");
		exit(1);
	}

	/*result = lookup(g_titles[2]); */

	clock_t i=0, globalstarttime=0;

	globalstarttime = clock();
	if (binary_search(fp, "z\0"/*g_titles[5]*/) < 0) {
		printf("not found!\n");
	}
	i = clock() - globalstarttime;
	printf("Total clicks\t%ld\nTotal secs\t%4.3f\n",
	       i, (double) i / CLOCKS_PER_SEC);

	display_array(g_result, RESULTCOUNT);

	return 0;
}

