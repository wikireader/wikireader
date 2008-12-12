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
#ifndef _SEARCH_H_
#define _SEARCH_H_

#define SPLIT_CHAR	'-'
#define LINE_END		'\n'
#define SERAIL_ENTER	'\r'

#define SHA1CHARS	300		/* sha1 char count */
#define TITLECHARS	260		/* the max chars of the title, the longest title is 250 chars*/
#define LINECHARS		560
#define RESULTCOUNT	5

char g_algorithm = 'L';
char g_result[RESULTCOUNT][TITLECHARS];

int g_titles_count = 58;
char *g_titles[]= {
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
	"0 0\0",
	"a 44\0",
	"bch formula\0",
	"bchb\0",
	"bche\0",
	"bchl\0",
	"bchs\0",
	"bchydro\0",
	"d arae\0",
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
	"zyzzyxdonta\0",
	"zyzzyxdonta alata\0",
	"zyzzyzus\0"
};

char *g_hash[] = {
	"7264a4c17f2d9d1dbc4bd02731b61722d8d40ef0",
	"4d3a29a587a59da7a4a234f7261e4fc4d38e5105",
	"4e491fd4a13e63623f10283a53d2e0ed5d51fb13",
	"d71e704651948a90d52d539f0a838b578d3dbba2",
	"0171634ddad02e36d5c890680fd7acb6b329ac9",
	"de98b5313f9d094da906992b6d5ad679da81f24c",
	"7c3ea60927fd5acd902dae6f6d5bfe6474d09de2",
	"becefce21068a05fea53388c790d87d324f324a8",
	"350410fc4fc2160b61281d3f96fde1e4a188a88e",
	"0c8cc026358e83d75fcbc8019f5d064111f0d2e3",
	"c777478eb4ece5f67cecf7a888124972d5e3cdba",
	"53e7c979ec49aca49ed1a1fbfda0684a62f5aeeb",
	"450694bfb9737727e9c6598f02bc1fc9460487ca",
	"85fa500984444f20b001a026324e330fddcd5897",
	"36a3ea503692796f265f23abc8075d90bda82ffd",
	"705af60dc0c553ec50b39b64fb23ca531a5fd6f7",
	"a854c2c9a83445912cee3e5eb9b9f2c615fd339b",
	"3a9b5bd5f8cb468d2fbed9ee1e610d3788ef58b7",
	"96283dbce2bdf882203ba9f0c13b1fe6a77e9eb9",
	"26fdb0e23dd78b58d79163ec6e8eafb2bb736b2a",
	"0ff5a1aee1ba4698392c51079b7cbbb1ccfb798f",
	"7a86636e79ef3d61f9daadd456a624c14517dae4",
	"761514dd65d2928e8d1c1d591af7b086af074b02",
	"cf5858df721a2f0f0d0def0c9481d37cca205207",
	"1e93b79ccde9d0ec1e5a3bd9922a2393927abd46",
	"28fd1712f1e642a457393a040268fce78ce7426c",
	"527a0b3aa1a0deff029d6f865dd464c522d45979",
	"2fb835ac4c2fdc5562ad21896765a5488d005d99",
	"7dd1ff7bc1c1170c4854b411efc09f9d00e81311",
	"6b3c05161fdb7f5ff64f62ce03c5744b7046dbdf",
	"60831f8c46d5523947f5d9a015aafc93759a04a3",
	"c253e070274c025207a6953cc174c5b28dba176b",
	"86010f050e829bc41591367e3d43b1ac41957150",
	"31e96a5eec9f68fb00d6e54bb97b3dd3f2b22bf2",
	"babd1285a08bf7a6cb68a3296f912a4ee58b06db",
	"fd75d76e816874d352977f2c4a6ccb03aad6dc03",
	"7392528ab81f2d56903611d0aaca819f321f0d2f",
	"f83ed9d451781fcd822d05cdd6d87570ff3ab062",
	"024f4d24c5619af7f67adf59d059292ca1c3bd3d",
	"4d9ccb5331dea3aab31487e513c2fe11c46055da",
	"611f88d11d9813b6178cf104660e94dfcb35b66d",
	"93707eb763a0898d582b9b36c8e5006264564e7e",
	"991068f641dacbbe5a725e328f9e4d576d658d07",
	"6409aac47ea292abf71e5a727fc3481cd044246d",
	"f2bc73e68378d6d6e6454bdc6b7707db0065ae72",
	"32fb25755f458012a18e3521074d456e5531f4da",
	"558a075602d24b4e33f6a091581f4e75163d095a",
	"2ff8b996a7a9d982340395263221a5146745aba6",
	"18038cc12153786cc2779c2eacb9ddd8f12dada2",
	"a8bec86e8534edc88e60ce2737ae5979948437fa",
	"e49c2a8becf2a8c7548adb804f4ceba96949eae4",
	"e3f9b81f96300c541184f8c3b0cf2d7b4fd52749",
	"b4612bbeffdc2371d8d6e9a8bd7d1d1300fbf9ce",
	"d6b6d6f317d247c660e42e86d7a4f75ed7d4b021",
	"8056f82374131414e7fbb021d2b40062605c28b7",
	"95555a00198f058ec0f9379e4d02985120177ea4",
	"95555a00198f058ec0f9379e4d02985120177ea4",
	"e189d1dbab15374976c2e87d5d97ef5a5dd86084",
};

int g_offset[] = {
	0,
	104665,
	127253,
	3991876,
	5960398,
	6391087,
	6719029,
	6997238,
	7219991,
	7444822,
	7627346,
	7825439,
	28721374,
	44396238,
	65963716,
	78251755,
	88736449,
	98976296,
	110596688,
	122037060,
	129117230,
	139544876,
	147481004,
	165913448,
	185176916,
	195652565,
	201265385,
	217542249,
	218483878,
	230457532,
	255508709,
	274340806,
	279533507,
	284222680,
	292717664,
	293216307,
	295092029,
	296486592
};

char g_offset_char[] = {
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'a',
	'b',
	'c',
	'd',
	'e',
	'f',
	'g',
	'h',
	'i',
	'j',
	'k',
	'l',
	'm',
	'n',
	'o',
	'p',
	'q',
	'r',
	's',
	't',
	'u',
	'v',
	'w',
	'x',
	'y',
	'z'
};

int search_test();

#endif /* _SEARCH_H_ */
