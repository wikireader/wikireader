/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Holger Hans Peter Freyther <zecke@openmoko.org>
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

#include "search.h"
#include "msg.h"
#include "wl-keyboard.h"
#include <stdlib.h>
#include <string.h>
#include <guilib.h>
#include <glyph.h>
#include <lcd.h>
#include <file-io.h>
#include <input.h>
#include <malloc-simple.h>

#include "Alloc.h"
#include "Bra.h"
#include "LzmaDec.h"
#include "wikilib.h"
#include "lcd_buf_draw.h"
#include "bigram.h"
#include "history.h"
#include "wikilib.h"
#include "search_hash.h"
#include "wiki_info.h"

#if !defined(INCLUDED_FROM_KERNEL)
#include "time.h"
#else
#include <tick.h>
#endif

#define DBG_SEARCH 0

#define DELAYED_SEARCH_TIME 0.3
unsigned long time_search_string_changed = 0;
bool search_string_changed = false;
bool search_string_changed_remove = false;
int more_search_results = 0;
extern ARTICLE_LINK articleLink[MAX_ARTICLE_LINKS];
extern int article_link_count;
extern long saved_idx_article;
int search_interrupted = 0;

typedef struct _search_results {
	char title[NUMBER_OF_FIRST_PAGE_RESULTS][MAX_TITLE_ACTUAL];
	char title_search[NUMBER_OF_FIRST_PAGE_RESULTS][MAX_TITLE_SEARCH];
	uint32_t idx_article[NUMBER_OF_FIRST_PAGE_RESULTS];  // index (wiki.idx) for loading the article
	uint32_t offset_list[NUMBER_OF_FIRST_PAGE_RESULTS];  // offset (wiki.fnd) of each search title in list
	uint32_t offset_next;		// offset (wiki.fnd) of the next title after the list
	uint32_t count;
	uint32_t result_populated;
	int32_t cur_selected;		// -1 when no selection.
} SEARCH_RESULTS;
static SEARCH_RESULTS *result_list = NULL;

typedef struct _search_info {
	int32_t inited;
	int32_t fd_pfx;
	int32_t fd_idx;
	int32_t fd_dat[MAX_DAT_FILES];
	uint32_t max_article_idx;
	uint32_t prefix_index_table[SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT];
	uint32_t b_prefix_index_block_loaded[SEARCH_CHR_COUNT];
	char buf[NUMBER_OF_FIRST_PAGE_RESULTS * sizeof(TITLE_SEARCH)];	// buf correspond to result_list
	uint32_t buf_len;
	uint32_t offset_current;	// offset (wiki.fnd) of the content of buffer
} SEARCH_INFO;
static SEARCH_INFO *search_info = NULL;

#define LONGEST_HIRAGANA_ENGLISH_CHARS 4
struct _english_hiragana_mapping {
	char *english;
	char *hiragana;
	char *unified_english;
} english_hiragana_mapping[] = {
	{"-",   "ー",	NULL	},
	{"a",	"あ",	 NULL    },
	{"ba",	"ば",	 NULL   },
	{"bba",    "っば"    , NULL },
	{"bbe",    "っべ"    , NULL },
	{"bbi",    "っび"    , NULL },
	{"bbo",    "っぼ"    , NULL },
	{"bbu",	"っぶ",	NULL	},
	{"bbya","っびゃ"     , NULL },
	{"bbye","っびぇ"     , NULL },
	{"bbyi","っびぃ"     , NULL },
	{"bbyo","っびょ"     , NULL },
	{"bbyu","っびゅ",	NULL	},
	{"be",	"べ",	 NULL   },
	{"bi",	"び",	 NULL   },
	{"bo",	"ぼ",	 NULL   },
	{"bu",	"ぶ",	 NULL   },
	{"bya",	"びゃ",	NULL  },
	{"bye",	"びぇ",	NULL  },
	{"byi",	"びぃ",	NULL  },
	{"byo",	"びょ",	NULL  },
	{"byu",	"びゅ",	NULL  },
	{"ccha","っちゃ"     , NULL },
	{"cche","っちぇ"     , NULL },
	{"cchi",   "っち"    , NULL },
	{"ccho","っちょ"     , NULL },
	{"cchu","っちゅ",	NULL	},
	{"ccyi","っちぃ"     , NULL },
	{"cha",	"ちゃ",	"cya"  },
	{"che",	"ちぇ",	"cye"  },
	{"chi",	"ち",	NULL  },
	{"cho",	"ちょ", "cyo"  },
	{"chu",	"ちゅ", "cyu"  },
	{"cya",	"ちゃ", "cya"  },
	{"cye",	"ちぇ", "cye"  },
	{"cyi",	"ちぃ", "cyi"  },
	{"cyo",	"ちょ", "cyo"  },
	{"cyu",	"ちゅ", "cyu"  },
	{"da",	"だ",	 NULL   },
	{"dda",    "っだ"    , NULL },
	{"dde",    "っで"    , NULL },
	{"ddha","っでぁ"     , NULL },
	{"ddhe","っでぇ"     , NULL },
	{"ddhi","っでぃ"     , NULL },
	{"ddho","っでょ"     , NULL },
	{"ddhu","っでゅ",	NULL	},
	{"ddi",    "っヂ"    , NULL },
	{"ddo",    "っど"    , NULL },
	{"ddu",	"っづ",	NULL	},
	{"de",	"で",	 NULL   },
	{"di",	"ぢ",	 NULL  },
	{"do",	"ど",	 NULL  },
	{"du",	"づ",	 NULL  },
	{"e",	"え",	 NULL  },
	{"fa",	"ふぁ",	NULL   },
	{"fe",	"ふぇ",	NULL   },
	{"ffa", "っふぁ"     , NULL },
	{"ffe", "っふぇ"     , NULL },
	{"ffi", "っふぃ"     , NULL },
	{"ffo", "っふぉ"     , NULL },
	{"ffu",	"っふ",	NULL	},
	{"ffyu","っふゅ",	NULL	},
	{"fi",	"ふぃ",	NULL   },
	{"fo",	"ふぉ",	NULL   },
	{"fu",	"ふ",	 NULL  },
	{"fya",	"ふゃ",	NULL  },
	{"fye",	"ふぇ", "fe"  },
	{"fyi",	"ふぃ", "fi"  },
	{"fyo",	"ふょ", NULL  },
	{"fyu",	"ふゅ", NULL  },
	{"ga",	"が",	 NULL  },
	{"ge",	"げ",	 NULL  },
	{"gga",    "っが"    , NULL },
	{"gge",    "っげ"    , NULL },
	{"ggha","っぐぁ"     , NULL },
	{"gghe","っぐぇ"     , NULL },
	{"gghi","っぐぃ"     , NULL },
	{"ggho","っぐぉ"     , NULL },
	{"gghu","っぐぅ",	NULL	},
	{"ggi",    "っぎ"    , NULL },
	{"ggo",    "っご"    , NULL },
	{"ggu",	"っぐ",	NULL	},
	{"ggya","っぎゃ"     , NULL },
	{"ggye","っぎぇ"     , NULL },
	{"ggyi","っぎぃ"     , NULL },
	{"ggyo","っぎょ"     , NULL },
	{"ggyu","っぎゅ",	NULL	},
	{"gi",	"ぎ",	 NULL  },
	{"go",	"ご",	 NULL  },
	{"gu",	"ぐ",	 NULL  },
	{"gya",	"ぎゃ",	NULL  },
	{"gye",	"ぎぇ",	NULL  },
	{"gyi",	"ぎぃ",	NULL  },
	{"gyo",	"ぎょ",	NULL  },
	{"gyu",	"ぎゅ",	NULL  },
	{"ha",	"は",	 NULL  },
	{"he",	"へ",	 NULL  },
	{"hha",    "っは"    , NULL },
	{"hhe",    "っへ"    , NULL },
	{"hhi",    "っひ"    , NULL },
	{"hho",    "っほ"    , NULL },
	{"hhya","っひゃ"     , NULL },
	{"hhye","っひぇ"     , NULL },
	{"hhyi","っひぃ"     , NULL },
	{"hhyo","っひょ"     , NULL },
	{"hhyu","っひゅ",	NULL	},
	{"hi",	"ひ",	 NULL  },
	{"ho",	"ほ",	 NULL  },
	{"hu",	"ふ",	 NULL  },
	{"hya",	"ひゃ",	NULL  },
	{"hye",	"ひぇ",	NULL  },
	{"hyi",	"ひぃ",	NULL  },
	{"hyo",	"ひょ",	NULL  },
	{"hyu",	"ひゅ",	NULL  },
	{"i",	"い",	 NULL  },
	{"ja",	"じゃ",	NULL   },
	{"je",	"じぇ",	NULL   },
	{"ji",	"じ",	 NULL  },
	{"jja", "っじゃ"     , NULL },
	{"jji",    "っじ"    , NULL },
	{"jjo", "っじょ"     , NULL },
	{"jju",	"っじゅ",	NULL	},
	{"jjye","っじぇ"     , NULL },
	{"jjyi","っじぃ"     , NULL },
	{"jo",	"じょ",	NULL   },
	{"ju",	"じゅ",	NULL   },
	{"jya",	"じゃ", "ja"  },
	{"jye",	"じぇ", "je"  },
	{"jyi",	"じぃ", NULL  },
	{"jyo",	"じょ", "jo"  },
	{"jyu",	"じゅ", "ju"  },
	{"ka",	"か",	 NULL },
	{"ke",	"け",	 NULL },
	{"ki",	"き",	 NULL },
	{"kka",    "っか"    , NULL },
	{"kke",    "っけ"    , NULL },
	{"kkha","っくぁ"     , NULL },
	{"kkhe","っくぇ"     , NULL },
	{"kkhi","っくぃ"     , NULL },
	{"kkho","っくぉ"     , NULL },
	{"kkhu","っくぅ",	NULL	},
	{"kki",    "っき"    , NULL },
	{"kko",    "っこ"    , NULL },
	{"kku",	"っく",	NULL	},
	{"kkya","っきぁ"     , NULL },
	{"kkye","っきぇ"     , NULL },
	{"kkyi","っきぃ"     , NULL },
	{"kkyo","っきょ"     , NULL },
	{"kkyu","っきゅ",	NULL	},
	{"ko",	"こ",	 NULL },
	{"ku",	"く",	 NULL },
	{"kya",	"きゃ",	NULL  },
	{"kye",	"きぇ",	NULL  },
	{"kyi",	"きぃ",	NULL  },
	{"kyo",	"きょ",	NULL  },
	{"kyu",	"きゅ",	NULL  },
	{"la",	"ら",	 "ra"   },
	{"le",	"れ",	 "re"   },
	{"li",	"り",	 "ri"   },
	{"lo",	"ろ",	 "ro"   },
	{"lu",	"る",	 "ru"   },
	{"lya",	"りゃ", "rya"  },
	{"lye",	"りぇ", "rye"  },
	{"lyi",	"りぃ", "ryi"  },
	{"lyo",	"りょ", "ryo"  },
	{"lyu",	"りゅ", "ryu"  },
	{"ma",	"ま",	 NULL },
	{"me",	"め",	 NULL },
	{"mi",	"み",	 NULL },
	{"mma",    "っま"    , NULL },
	{"mme",    "っめ"    , NULL },
	{"mmi",    "っみ"    , NULL },
	{"mmo",    "っも"    , NULL },
	{"mmu",	"っむ",	NULL	},
	{"mmya","っみゃ"     , NULL },
	{"mmye","っみぇ"     , NULL },
	{"mmyi","っみぃ"     , NULL },
	{"mmyo","っみょ"     , NULL },
	{"mmyu","っみゅ",	NULL	},
	{"mo",	"も",	 NULL },
	{"mu",	"む",	 NULL },
	{"mya",	"みゃ",	NULL  },
	{"mye",	"みぇ",	NULL  },
	{"myi",	"みぃ",	NULL  },
	{"myo",	"みょ",	NULL  },
	{"myu",	"みゅ",	NULL  },
	{"na",	"な",	 NULL },
	{"ne",	"ね",	 NULL },
	{"ni",	"に",	 NULL },
	{"nn",	"ん",	 NULL },
	{"no",	"の",	 NULL },
	{"nu",	"ぬ",	 NULL },
	{"nya",	"にゃ",	NULL  },
	{"nye",	"にぇ",	NULL  },
	{"nyi",	"にぃ",	NULL  },
	{"nyo",	"にょ",	NULL  },
	{"nyu",	"にゅ",	NULL  },
	{"o",	"お",	 NULL },
	{"pa",	"ぱ",	 NULL },
	{"pe",	"ぺ",	 NULL },
	{"pi",	"ぴ",	 NULL },
	{"po",	"ぽ",	 NULL },
	{"ppa",    "っぱ"    , NULL },
	{"ppe",    "っぺ"    , NULL },
	{"ppi",    "っぴ"    , NULL },
	{"ppo",    "っぽ"    , NULL },
	{"ppu",	"っぷ",	NULL	},
	{"ppya","っぴゃ"     , NULL },
	{"ppye","っぴぇ"     , NULL },
	{"ppyi","っぴぃ"     , NULL },
	{"ppyo","っぴょ"     , NULL },
	{"ppyu","っぴゅ",	NULL	},
	{"pu",	"ぷ",	 NULL },
	{"pya",	"ぴゃ",	NULL  },
	{"pye",	"ぴぇ",	NULL  },
	{"pyi",	"ぴぃ",	NULL  },
	{"pyo",	"ぴょ",	NULL  },
	{"pyu",	"ぴゅ",	NULL  },
	{"ra",	"ら",	 NULL },
	{"re",	"れ",	 NULL },
	{"ri",	"り",	 NULL },
	{"ro",	"ろ",	 NULL },
	{"rra",    "っら"    , NULL },
	{"rre",    "っれ"    , NULL },
	{"rri",    "っり"    , NULL },
	{"rro",    "っろ"    , NULL },
	{"rru",	"っる",	NULL	},
	{"rrya","っりゃ"     , NULL },
	{"rrye","っりぇ"     , NULL },
	{"rryi","っりぃ"     , NULL },
	{"rryo","っりょ"     , NULL },
	{"rryu","っりゅ",	NULL	},
	{"ru",	"る",	 NULL },
	{"rya",	"りゃ",	NULL  },
	{"rye",	"りぇ",	NULL  },
	{"ryi",	"りぃ",	NULL  },
	{"ryo",	"りょ",	NULL  },
	{"ryu",	"りゅ",	NULL  },
	{"sa",	"さ",	 NULL },
	{"se",	"せ",	 NULL },
	{"sha",	"しゃ", "sya"  },
	{"she",	"しぇ", "sye"  },
	{"shi",	"し",	 NULL  },
	{"sho",	"しょ", "syo"  },
	{"shu",	"しゅ", "syu"  },
	{"si",	"し",	 "shi"   },
	{"so",	"そ",	 NULL   },
	{"ssa",    "っさ"    , NULL },
	{"sse",    "っせ"    , NULL },
	{"ssha","っしゃ"     , NULL },
	{"sshe","っしぇ"     , NULL },
	{"sshi",   "っし"    , NULL },
	{"ssho","っしょ"     , NULL },
	{"sshu","っしゅ",	NULL	},
	{"sso",    "っそ"    , NULL },
	{"ssu",	"っす",	NULL	},
	{"ssyi","っしぃ"     , NULL },
	{"su",	"す",	NULL   },
	{"sya",	"しゃ",	NULL	},
	{"sye",	"しぇ",	NULL	},
	{"syi",	"しぃ",	NULL	},
	{"syo",	"しょ",	NULL	},
	{"syu",	"しゅ",	NULL	},
	{"ta",	"た",	NULL  },
	{"te",	"て",	NULL  },
	{"ti",	"ち",	NULL  },
	{"to",	"と",	NULL  },
	{"tsu",	"つ",	NULL  },
	{"tta",    "った"    , NULL },
	{"tte",    "って"    , NULL },
	{"ttha","ってゃ"     , NULL },
	{"tthe","ってぇ"     , NULL },
	{"tthi","ってぃ"     , NULL },
	{"ttho","ってょ"     , NULL },
	{"tthu","ってゅ",	NULL	},
	{"tto",    "っと"    , NULL },
	{"tts",   "っつ"     , NULL },
	{"ttsu","っつ",	NULL	},
	{"tu",	"つ",	"tsu"   },
	{"tya",	"ちゃ",	"cya"	},
	{"tye",	"ちぇ",	"cye"	},
	{"tyi",	"ちぃ",	"cyi"	},
	{"tyo",	"ちょ",	"cyo"	},
	{"tyu",	"ちゅ",	"cyu"	},
	{"u",	"う",	NULL   },
	{"vva", "っヴぁ"     , NULL },
	{"vve", "っヴぇ"     , NULL },
	{"vvi", "っヴぃ"     , NULL },
	{"vvo", "っヴぉ"     , NULL },
	{"vvu",	"っヴ",	NULL	},
	{"wa",	"わ",	NULL   },
	{"we",	"うぇ",	"whe"	},
	{"wha",	"うぁ",	NULL	},
	{"whe",	"うぇ",	NULL	},
	{"whi",	"うぃ",	NULL	},
	{"who",	"うぉ",	NULL	},
	{"whu",	"う",	"u"  },
	{"wi",	"うぃ",	"whi"	},
	{"wo",	"を",	NULL   },
	{"wu",	"う",	NULL   },
	{"wwa",    "っわ"    , NULL },
	{"wwha","っうぁ"     , NULL },
	{"wwhe","っうぇ"     , NULL },
	{"wwho","っうぉ"     , NULL },
	{"wwi", "っうぃ"     , NULL },
	{"ya",	"や",	NULL   },
	{"ye",	"いぇ",	NULL	},
	{"yi",	"い",	NULL   },
	{"yo",	"よ",	NULL   },
	{"yu",	"ゆ",	NULL   },
	{"yya",    "っや"    , NULL },
	{"yyo",    "っよ"    , NULL },
	{"yyu",	"っゆ",	NULL	},
	{"za",	"ざ",	NULL   },
	{"ze",	"ぜ",	NULL   },
	{"zi",	"じ",	NULL   },
	{"zo",	"ぞ",	NULL   },
	{"zu",	"ず",	NULL   },
	{"zza",    "っざ"    , NULL },
	{"zze",    "っぜ"    , NULL },
	{"zzo",    "っぞ"    , NULL },
	{"zzu",	"っず",	NULL	},
};

struct _hiragana_english_mapping {
	char *english;
	char *hiragana;
	char *unified_english;
} hiragana_english_mapping[] = {
	{"a",	"あ"    },
	{"i",	"い"    },
	{"yi",	"い"   },
	{"ye",	"いぇ"	},
	{"u",	"う"    },
	{"wha",	"うぁ"	},
	{"whi",	"うぃ"	},
	{"whe",	"うぇ"	},
	{"who",	"うぉ"	},
	{"e",	"え"    },
	{"o",	"お"    },
	{"ka",	"か"   },
	{"ga",	"が"   },
	{"ki",	"き"   },
	{"kyi",	"きぃ"  },
	{"kye",	"きぇ"  },
	{"kya",	"きゃ"  },
	{"kyu",	"きゅ"  },
	{"kyo",	"きょ"  },
	{"gi",	"ぎ"   },
	{"gyi",	"ぎぃ"  },
	{"gye",	"ぎぇ"  },
	{"gya",	"ぎゃ"  },
	{"gyu",	"ぎゅ"  },
	{"gyo",	"ぎょ"  },
	{"ku",	"く"   },
	{"gu",	"ぐ"   },
	{"ke",	"け"   },
	{"ge",	"げ"   },
	{"ko",	"こ"   },
	{"go",	"ご"   },
	{"sa",	"さ"   },
	{"za",	"ざ"   },
	{"shi",	"し"  },
	{"syi",	"しぃ"	},
	{"sye",	"しぇ"	},
	{"sya",	"しゃ"	},
	{"syu",	"しゅ"	},
	{"syo",	"しょ"	},
	{"ji",	"じ"   },
	{"jyi",	"じぃ"  },
	{"je",	"じぇ"   },
	{"ja",	"じゃ"   },
	{"ju",	"じゅ"   },
	{"jo",	"じょ"   },
	{"su",	"す"   },
	{"zu",	"ず"   },
	{"se",	"せ"   },
	{"ze",	"ぜ"   },
	{"so",	"そ"   },
	{"zo",	"ぞ"   },
	{"ta",	"た"   },
	{"da",	"だ"   },
	{"chi",	"ち"  },
	{"cyi",	"ちぃ"  },
	{"cye",	"ちぇ"  },
	{"che",	"ちぇ"  },
	{"cya",	"ちゃ"  },
	{"cyu",	"ちゅ"  },
	{"cyo",	"ちょ"  },
	{"di",	"ぢ"   },
	{"tsu",	"っ" },
	{"wwha","っうぁ"  },
	{"wwi",	"っうぃ"  },
	{"wwhe","っうぇ"  },
	{"wwho","っうぉ"  },
	{"kka",	"っか" },
	{"gga",	"っが" },
	{"kki",	"っき" },
	{"kkya","っきぁ"  },
	{"kkyi","っきぃ"  },
	{"kkye","っきぇ"  },
	{"kkyu","っきゅ"	},
	{"kkyo","っきょ"  },
	{"ggi",	"っぎ" },
	{"ggyi","っぎぃ"  },
	{"ggye","っぎぇ"  },
	{"ggya","っぎゃ"  },
	{"ggyu","っぎゅ"	},
	{"ggyo","っぎょ"  },
	{"kku",	"っく"	},
	{"kkha","っくぁ"  },
	{"kkhi","っくぃ"  },
	{"kkhu","っくぅ"	},
	{"kkhe","っくぇ"  },
	{"kkho","っくぉ"  },
	{"ggu",	"っぐ"	},
	{"ggha","っぐぁ"  },
	{"gghi","っぐぃ"  },
	{"gghu","っぐぅ"	},
	{"gghe","っぐぇ"  },
	{"ggho","っぐぉ"  },
	{"kke",	"っけ" },
	{"gge",	"っげ" },
	{"kko",	"っこ" },
	{"ggo",	"っご" },
	{"ssa",	"っさ" },
	{"zza",	"っざ" },
	{"sshi","っし" },
	{"ssyi","っしぃ"  },
	{"sshe","っしぇ"  },
	{"ssha","っしゃ"  },
	{"sshu","っしゅ"	},
	{"ssho","っしょ"  },
	{"jji",	"っじ" },
	{"jjyi","っじぃ"  },
	{"jjye","っじぇ"  },
	{"jja",	"っじゃ"  },
	{"jju",	"っじゅ"	},
	{"jjo",	"っじょ"  },
	{"ssu",	"っす"	},
	{"zzu",	"っず"	},
	{"sse",	"っせ" },
	{"zze",	"っぜ" },
	{"sso",	"っそ" },
	{"zzo",	"っぞ" },
	{"tta",	"った" },
	{"dda",	"っだ" },
	{"cchi","っち" },
	{"ccyi","っちぃ"  },
	{"cche","っちぇ"  },
	{"ccha","っちゃ"  },
	{"cchu","っちゅ"	},
	{"ccho","っちょ"  },
	{"ttsu","っつ"	},
	{"tts",	"っつ"  },
	{"ddu",	"っづ"	},
	{"tte",	"って" },
	{"tthi","ってぃ"  },
	{"tthe","ってぇ"  },
	{"ttha","ってゃ"  },
	{"tthu","ってゅ"	},
	{"ttho","ってょ"  },
	{"dde",	"っで" },
	{"ddha","っでぁ"  },
	{"ddhi","っでぃ"  },
	{"ddhe","っでぇ"  },
	{"ddhu","っでゅ"	},
	{"ddho","っでょ"  },
	{"tto",	"っと" },
	{"ddo",	"っど" },
	{"hha",	"っは" },
	{"bba",	"っば" },
	{"ppa",	"っぱ" },
	{"hhi",	"っひ" },
	{"hhyi","っひぃ"  },
	{"hhye","っひぇ"  },
	{"hhya","っひゃ"  },
	{"hhyu","っひゅ"	},
	{"hhyo","っひょ"  },
	{"bbi",	"っび" },
	{"bbyi","っびぃ"  },
	{"bbye","っびぇ"  },
	{"bbya","っびゃ"  },
	{"bbyu","っびゅ"	},
	{"bbyo","っびょ"  },
	{"ppi",	"っぴ" },
	{"ppyi","っぴぃ"  },
	{"ppye","っぴぇ"  },
	{"ppya","っぴゃ"  },
	{"ppyu","っぴゅ"	},
	{"ppyo","っぴょ"  },
	{"ffu",	"っふ"	},
	{"ffu",	"っふ"  },
	{"ffa",	"っふぁ"  },
	{"ffi",	"っふぃ"  },
	{"ffe",	"っふぇ"  },
	{"ffo",	"っふぉ"  },
	{"ffyu","っふゅ"	},
	{"bbu",	"っぶ"	},
	{"ppu",	"っぷ"	},
	{"hhe",	"っへ" },
	{"bbe",	"っべ" },
	{"ppe",	"っぺ" },
	{"hho",	"っほ" },
	{"bbo",	"っぼ" },
	{"ppo",	"っぽ" },
	{"mma",	"っま" },
	{"mmi",	"っみ" },
	{"mmyi","っみぃ"  },
	{"mmye","っみぇ"  },
	{"mmya","っみゃ"  },
	{"mmyu","っみゅ"	},
	{"mmyo","っみょ"  },
	{"mmu",	"っむ"	},
	{"mme",	"っめ" },
	{"mmo",	"っも" },
	{"yya",	"っや" },
	{"yyu",	"っゆ"	},
	{"yyo",	"っよ" },
	{"rra",	"っら" },
	{"rri",	"っり" },
	{"rryi","っりぃ"  },
	{"rrye","っりぇ"  },
	{"rrya","っりゃ"  },
	{"rryu","っりゅ"	},
	{"rryo","っりょ"  },
	{"rru",	"っる"	},
	{"rru",	"っる"  },
	{"rre",	"っれ" },
	{"rro",	"っろ" },
	{"wwa",	"っわ" },
	{"ddi",	"っヂ" },
	{"vvu",	"っヴ"	},
	{"vva",	"っヴぁ"  },
	{"vvi",	"っヴぃ"  },
	{"vve",	"っヴぇ"  },
	{"vvo",	"っヴぉ"  },
	{"tsu",	"つ"  },
	{"du",	"づ"   },
	{"te",	"て"   },
	{"de",	"で"   },
	{"to",	"と"   },
	{"do",	"ど"   },
	{"na",	"な"   },
	{"ni",	"に"   },
	{"nyi",	"にぃ"  },
	{"nye",	"にぇ"  },
	{"nya",	"にゃ"  },
	{"nyu",	"にゅ"  },
	{"nyo",	"にょ"  },
	{"nu",	"ぬ"   },
	{"ne",	"ね"   },
	{"no",	"の"   },
	{"ha",	"は"   },
	{"ba",	"ば"   },
	{"pa",	"ぱ"   },
	{"hi",	"ひ"   },
	{"hyi",	"ひぃ"  },
	{"hye",	"ひぇ"  },
	{"hya",	"ひゃ"  },
	{"hyu",	"ひゅ"  },
	{"hyo",	"ひょ"  },
	{"bi",	"び"   },
	{"byi",	"びぃ"  },
	{"bye",	"びぇ"  },
	{"bya",	"びゃ"  },
	{"byu",	"びゅ"  },
	{"byo",	"びょ"  },
	{"pi",	"ぴ"   },
	{"pyi",	"ぴぃ"  },
	{"pye",	"ぴぇ"  },
	{"pya",	"ぴゃ"  },
	{"pyu",	"ぴゅ"  },
	{"pyo",	"ぴょ"  },
	{"fu",	"ふ"   },
	{"fa",	"ふぁ"   },
	{"fi",	"ふぃ"   },
	{"fe",	"ふぇ"   },
	{"fo",	"ふぉ"   },
	{"fyu",	"ふゅ"  },
	{"fyo",	"ふょ"  },
	{"bu",	"ぶ"   },
	{"pu",	"ぷ"   },
	{"he",	"へ"   },
	{"be",	"べ"   },
	{"pe",	"ぺ"   },
	{"ho",	"ほ"   },
	{"bo",	"ぼ"   },
	{"po",	"ぽ"   },
	{"ma",	"ま"   },
	{"mi",	"み"   },
	{"myi",	"みぃ"  },
	{"mye",	"みぇ"  },
	{"mya",	"みゃ"  },
	{"myu",	"みゅ"  },
	{"myo",	"みょ"  },
	{"mu",	"む"   },
	{"me",	"め"   },
	{"mo",	"も"   },
	{"ya",	"や"   },
	{"yu",	"ゆ"   },
	{"yo",	"よ"   },
	{"ra",	"ら"   },
	{"ri",	"り"   },
	{"ryi",	"りぃ"  },
	{"rye",	"りぇ"  },
	{"rya",	"りゃ"  },
	{"ryu",	"りゅ"  },
	{"ryo",	"りょ"  },
	{"ru",	"る"   },
	{"re",	"れ"   },
	{"ro",	"ろ"   },
	{"wa",	"わ"   },
	{"wo",	"を"   },
	{"nn",	"ん"   },
	{"-",   "ー"	},
};

//#define SIZE_PREFIX_INDEX_TABLE SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(long)
//static struct search_state state;
//static struct search_state last_first_hit;

static char search_string[MAX_TITLE_SEARCH];
static int search_string_pos[MAX_TITLE_SEARCH];
static char search_string_hiragana[MAX_TITLE_SEARCH * 3];
static int search_str_len = 0;
static int search_str_converted_len = 0;
static int search_str_hiragana_len = 0;

//static char s_find_first = 1;

static void *SzAlloc(void *p, size_t size) { p = p; return malloc_simple(size, MEM_TAG_INDEX_M1); }
static void SzFree(void *p, void *address) { p = p; free_simple(address, MEM_TAG_INDEX_M1); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

//const char* search_fetch_result()
//{
//	DP(DBG_SEARCH, ("O search_fetch_result() called\n"));
//	if (search_str_len == 0)
//		return NULL;
//#ifdef WOM_ON
//	const wom_article_index_t* idx;
//	static char result_buf[MAXSTR]; // we know that the returned pointer is copied immediately
//	if (s_find_first) {
//		s_find_first = 0;
//		idx = wom_find_article(g_womh, search_string, search_str_len);
//	} else
//		idx = wom_get_next_article(g_womh);
//	if (!idx) return 0;
//	sprintf(result_buf, "%.*s%.6x", idx->uri_len, idx->abbreviated_uri, (unsigned int) idx->offset_into_articles);
//	DP(DBG_SEARCH, ("O search_fetch_result() '%s'\n", result_buf));
//	return result_buf;
//#else // !WOM_ON
//	char* result = search_fast(&global_search, search_string, &state);
//	if (s_find_first) {
//		s_find_first = 0;
//		store_state(&global_search, &state, &last_first_hit);
//	}
//	return result;
//#endif
//}

long result_list_offset_next(void)
{
	return result_list->offset_next;
}

long result_list_next_result(long offset_next, long *idxArticle, char *sTitleActual)
{
	TITLE_SEARCH titleSearch;
	char sTitleSearch[MAX_TITLE_SEARCH];

	copy_fnd_to_buf(offset_next, (void *)&titleSearch, sizeof(TITLE_SEARCH));
	retrieve_titles_from_fnd(offset_next, sTitleSearch, sTitleActual);
	if (!search_string_cmp(sTitleSearch, search_string, search_str_len)) // match!
	{
		*idxArticle = titleSearch.idxArticle;
		offset_next += sizeof(titleSearch.idxArticle) + strlen(titleSearch.sTitleSearch) + 2; // now point to the actual title
		offset_next += strlen(&titleSearch.sTitleSearch[strlen(titleSearch.sTitleSearch) + 1]) + 1; // skip the actual title to the next titleSearch
		return offset_next;
	}
	else
		return 0;
}

void get_article_title_from_idx(long idx, char *title)
{
	ARTICLE_PTR article_ptr;
	TITLE_SEARCH title_search;
	int wiki_id;
	int wiki_idx;
	int nTmpeCurrentWiki = nCurrentWiki;
	char sTitleSearch[MAX_TITLE_SEARCH];

	title[0] = '\0';
	wiki_id = idx >> 24;
	idx &= 0x00FFFFFF;
	if (wiki_id > 0)
	{
		wiki_idx = get_wiki_idx_from_id(wiki_id);
		if (wiki_idx < 0) // wiki not loaded
			return;
		nCurrentWiki = wiki_idx;
	}

	wl_seek(search_info[nCurrentWiki].fd_idx, (idx - 1) * sizeof(ARTICLE_PTR) + 4);
	wl_read(search_info[nCurrentWiki].fd_idx, (void *)&article_ptr, sizeof(article_ptr));
	if (article_ptr.offset_fnd)
	{
		copy_fnd_to_buf(article_ptr.offset_fnd, (char *)&title_search, sizeof(title_search));
		retrieve_titles_from_fnd(article_ptr.offset_fnd, sTitleSearch, title);
		title[MAX_TITLE_ACTUAL - 1] = '\0';
	}
	nCurrentWiki = nTmpeCurrentWiki;
}

void load_prefix_index(int nWikiIdx)
{
	int i;

	if (!search_info[nWikiIdx].inited)
	{
		search_info[nWikiIdx].fd_pfx = wl_open(get_wiki_file_path(nWikiIdx, "wiki.pfx"), WL_O_RDONLY);
		search_info[nWikiIdx].fd_idx = wl_open(get_wiki_file_path(nWikiIdx, "wiki.idx"), WL_O_RDONLY);
		for (i=0; i < MAX_DAT_FILES; i++)
			search_info[nWikiIdx].fd_dat[i] = -1;
		search_info[nWikiIdx].offset_current = -1;
		if (search_info[nWikiIdx].fd_pfx >= 0 && search_info[nWikiIdx].fd_idx >= 0)
		{
			wl_read(search_info[nWikiIdx].fd_idx, (void *)&search_info[nWikiIdx].max_article_idx, sizeof(search_info[nWikiIdx].max_article_idx));
			memset((char *)search_info[nWikiIdx].b_prefix_index_block_loaded, 0, sizeof(search_info[nWikiIdx].b_prefix_index_block_loaded));
			search_info[nWikiIdx].inited = 1;
		}
		else
			fatal_error("index file open error");
	}
}

void search_init()
{
	int i;
	int nWikiCount = get_wiki_count();

	if (!result_list)
	{
		result_list = (SEARCH_RESULTS *)malloc_simple(sizeof(SEARCH_RESULTS), MEM_TAG_INDEX_M1);
		if (!result_list)
			fatal_error("search_init malloc error");
	}
	if (!search_info)
	{
		search_info = (SEARCH_INFO *)malloc_simple(sizeof(SEARCH_INFO) * nWikiCount, MEM_TAG_INDEX_M1);
		if (!search_info)
			fatal_error("search_init malloc error");
		else
		{
			for (i = 0; i < nWikiCount; i++)
			{
				search_info[i].inited = 0;
			}
		}
	}

	load_prefix_index(nCurrentWiki);
	result_list->count = 0;
	init_search_hash();
}

void memrcpy(char *dest, char *src, int len) // memory copy starting from the last byte
{
	if (len >= 0)
	{
		dest += len - 1;
		src += len - 1;

		while (len--)
		{
			*dest = *src;
			dest--;
			src--;
		}
	}
}

char article_error[100] = "";
char article_error2[100] = "";
static void print_article_error()
{
	char msg[100];

	sprintf(msg, "The article, %lx, failed to load.", saved_idx_article);
	guilib_fb_lock();
	guilib_clear();
	render_string(SEARCH_LIST_FONT_IDX, -1, 84, msg, strlen(msg), 0);
	render_string(SEARCH_LIST_FONT_IDX, -1, 104, "Please restart your WikiReader and", 34, 0);
	render_string(SEARCH_LIST_FONT_IDX, -1, 124, "try again.", 10, 0);
//render_string(SEARCH_LIST_FONT_IDX, -1, 124, article_error, strlen(article_error), 0);
//render_string(SEARCH_LIST_FONT_IDX, -1, 144, article_error2, strlen(article_error2), 0);
	guilib_fb_unlock();
}

// check if null terminator exists
int is_proper_string(char *s, int len)
{
	while (len >= 0)
	{
		if (!*s)
			return 1;
		s++;
		len--;
	}
	return 0;
}

char *strnstr(char *s1, char *s2, int len)
{
	int bFound = 0;
	int s2_len = strlen(s2);

	while (!bFound && len >= s2_len)
	{
		if (!memcmp(s1, s2, s2_len))
			bFound = 1;
		else
		{
			s1++;
			len--;
		}
	}

	if (bFound)
		return s1;
	else
		return NULL;
}

// locate the patterh of the beginning of a TITLE_SEARCH structure
// the continuous 8 bytes should look like the following:
// 0xXX00 (null terminated last two bytes of the last TITLE_SEARCH.sTitleActual where XX is not 00) +
// 0xXXXXXX00 (TITLE_SEARCH.idxArticle where XXXXXX is not 000000) +
// 0x00 (TITLE_SEARCH.cZero) +
// 0xXX (Non-zero of TITLE_SEARCH.sTitleSearch)
bool is_title_search_pattern(char *pBuf)
{
	if (pBuf[0] && !pBuf[1] && (pBuf[2] || pBuf[3] || pBuf[4]) && !pBuf[5] && !pBuf[6] && pBuf[7])
		return true;
	else
		return false;
}

TITLE_SEARCH *locate_proper_title_search(char *buf_middle, int len)
{
	char *pBuf = buf_middle - 2; 	// including the possible null terminated last two bytes of the last TITLE_SEARCH
	// for finding the proper TITLE_SEARCH pattern
	int i = 0;
	bool bFound = false;

	while (!bFound && i < len + 2 - 8) // the pattern consists of 8 bytes
	{
		if (is_title_search_pattern(&pBuf[i]))
			bFound = true;
		else
			i++;
	}

	if (bFound)
		return (TITLE_SEARCH *)(pBuf + i + 2);
	else
		return NULL;
}

uint32_t get_article_idx_from_offset_range(char *sInputTitleActual, long offset_fnd_start, long offset_fnd_end)
{
	int len;
	int rc;
	uint32_t article_idx = 0;
	char sTitleSearch[MAX_TITLE_SEARCH];
	char sTitleActual[MAX_TITLE_ACTUAL];
	char buf_middle[sizeof(TITLE_SEARCH) * 2];
	long offset_middle;
	static TITLE_SEARCH *pTitleSearch;
	int offsetNextTitleSearch = 0;
	bool bFound = false;

	while (!bFound && offset_fnd_start >= 0)
	{
		if (search_info[nCurrentWiki].offset_current != offset_fnd_start)
		{
			search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
			search_info[nCurrentWiki].offset_current = offset_fnd_start;
			if (search_interrupted)
			{
				goto out;
			}
			if (search_info[nCurrentWiki].buf_len < sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
			{
				goto out;
			}
		}

		pTitleSearch = (TITLE_SEARCH *)&search_info[nCurrentWiki].buf[offsetNextTitleSearch];
		if (offsetNextTitleSearch < search_info[nCurrentWiki].buf_len &&
		    is_proper_string(pTitleSearch->sTitleSearch, search_info[nCurrentWiki].buf_len - offsetNextTitleSearch -
				     sizeof(pTitleSearch->idxArticle) - sizeof(pTitleSearch->cZero)))
		{
			retrieve_titles_from_fnd(offset_fnd_start + offsetNextTitleSearch,
				sTitleSearch, sTitleActual);
			rc = search_string_cmp(sTitleSearch, search_string, search_str_len);
//#ifndef INCLUDED_FROM_KERNEL
//msg(MSG_INFO, "offset (%x + %x, %x) ", offset_fnd_start, offsetNextTitleSearch, offset_fnd_end);
//msg(MSG_INFO, "rc %d, [%s], [%s], [", rc, sTitleSearch, sTitleActual);
//int i;
//for (i=0;i<search_str_len;i++)
//msg(MSG_INFO, "%c", search_string[i]);
//msg(MSG_INFO, "]\n");
//#endif
			if (!rc) // match!
			{
				if (!strcmp(sTitleActual, sInputTitleActual))
				{
					article_idx = pTitleSearch->idxArticle;
					bFound = true;
				}
				else
				{
					offset_fnd_end = -1; // start sequential search
					offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) +
						strlen(&pTitleSearch->sTitleSearch[strlen(pTitleSearch->sTitleSearch) + 1]) + 3;
					offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
					offsetNextTitleSearch = 0;
				}
			}
			else if (rc < 0)
			{
				if (offset_fnd_end <= 0)
				{
					offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) +
						strlen(&pTitleSearch->sTitleSearch[strlen(pTitleSearch->sTitleSearch) + 1]) + 3;
					offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
				}
				else // binary search
				{
					offset_middle = offset_fnd_start + (offset_fnd_end - offset_fnd_start) / 2; // position to the middle of the range
					if (offset_middle <= offset_fnd_start)
						offset_fnd_end = -1;
					else
					{
						len = copy_fnd_to_buf(offset_middle, buf_middle, sizeof(buf_middle));
						if (search_interrupted)
						{
							goto out;
						}
						if (len >= sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
						{
							pTitleSearch = locate_proper_title_search(buf_middle, len);
							if (pTitleSearch)
							{
								char local_title_search[MAX_TITLE_SEARCH];
								char sTitleActual[MAX_TITLE_ACTUAL];

								offset_middle += (char *)pTitleSearch - buf_middle;
								retrieve_titles_from_fnd(offset_middle, local_title_search, sTitleActual);
								rc = search_string_cmp(local_title_search, search_string, search_str_len);
								if (rc >= 0)	// the first mactch will be in front or at offset_middle
								{
									if (offset_middle == offset_fnd_end) // search from offset_fnd_start directly
										offset_fnd_end = -1;
									else
										offset_fnd_end = offset_middle;
								}
								else		// the first mactch will be after offset_middle
								{
									offset_fnd_start = offset_middle;
									search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
									search_info[nCurrentWiki].offset_current = offset_fnd_start;
									if (search_interrupted)
									{
										goto out;
									}
									if (search_info[nCurrentWiki].buf_len <= 0)
									{
										goto out;
									}
									offsetNextTitleSearch = 0;
								}
							}
						}
						else
							offset_fnd_end = -1;
					}
				}
				offsetNextTitleSearch = 0;
			}
			else
			{
				bFound = true; // return 0
				goto out;
			}
		}
		else
		{
			offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
			search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
			search_info[nCurrentWiki].offset_current = offset_fnd_start;
			if (search_interrupted)
			{
				goto out;
			}
			if (search_info[nCurrentWiki].buf_len <= 0)
			{
				goto out;
			}
			offsetNextTitleSearch = 0;
		}
	}

out:
	return article_idx;
}

int fetch_search_result(long input_offset_fnd_start, long input_offset_fnd_end, int bInit)
{
	int len;
	int rc;
	char buf_middle[sizeof(TITLE_SEARCH) * 2];
	long offset_middle;
	static TITLE_SEARCH *pTitleSearch;
	static int offsetNextTitleSearch = 0;
	static long offset_fnd_start = -1;
	static long offset_fnd_end = -1;

	if (bInit)
	{
		result_list->result_populated = 0;
		offset_fnd_start = input_offset_fnd_start;
		offset_fnd_end = input_offset_fnd_end;
		result_list->count = 0;
		offsetNextTitleSearch = 0;
	}
	if (result_list->result_populated || offset_fnd_start < 0)
		return 0;

	if (search_info[nCurrentWiki].offset_current != offset_fnd_start)
	{
		search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
		search_info[nCurrentWiki].offset_current = offset_fnd_start;
		if (search_interrupted)
		{
			search_interrupted = 7;
			goto interrupted;
		}
		if (search_info[nCurrentWiki].buf_len < sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
		{
			result_list->result_populated = 1;
			goto out;
		}
	}

	if (!result_list->result_populated)
	{
		pTitleSearch = (TITLE_SEARCH *)&search_info[nCurrentWiki].buf[offsetNextTitleSearch];
		if (offsetNextTitleSearch < search_info[nCurrentWiki].buf_len &&
		    is_proper_string(pTitleSearch->sTitleSearch, search_info[nCurrentWiki].buf_len - offsetNextTitleSearch -
				     sizeof(pTitleSearch->idxArticle) - sizeof(pTitleSearch->cZero)))
		{
		retrieve_titles_from_fnd(offset_fnd_start + offsetNextTitleSearch,
					 result_list->title_search[result_list->count], result_list->title[result_list->count]);
			rc = search_string_cmp(result_list->title_search[result_list->count], search_string, search_str_len);
//#ifndef INCLUDED_FROM_KERNEL
//msg(MSG_INFO, "bInit %d, input (%x, %x) ", bInit, input_offset_fnd_start, input_offset_fnd_end);
//msg(MSG_INFO, "result count %d, offset (%x + %x, %x) ", result_list->count, offset_fnd_start, offsetNextTitleSearch, offset_fnd_end);
//msg(MSG_INFO, "rc %d, [%s], [%s], [", rc, result_list->title_search[result_list->count], result_list->title[result_list->count]);
//int i;
//for (i=0;i<search_str_len;i++)
//msg(MSG_INFO, "%c", search_string[i]);
//msg(MSG_INFO, "]\n");
//#endif
			if (!rc) // match!
			{
				if (!result_list->count)
				{
					offset_fnd_start =  offset_fnd_start + offsetNextTitleSearch;
					search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
					search_info[nCurrentWiki].offset_current = offset_fnd_start;
					if (search_interrupted)
					{
						search_interrupted = 8;
						goto interrupted;
					}
					if (search_info[nCurrentWiki].buf_len < sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
					{
						result_list->result_populated = 1;
						goto out;
					}
					offsetNextTitleSearch = 0;
					pTitleSearch = (TITLE_SEARCH *)&search_info[nCurrentWiki].buf[offsetNextTitleSearch];
				}
				// use memcpy to avoid "Unaligned data access"
				memcpy((void *)&result_list->idx_article[result_list->count],
				       (void *)&pTitleSearch->idxArticle,
				       sizeof(result_list->idx_article[result_list->count]));
				result_list->offset_list[result_list->count] = offset_fnd_start + offsetNextTitleSearch;
				offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) +
					strlen(&pTitleSearch->sTitleSearch[strlen(pTitleSearch->sTitleSearch) + 1]) + 3;
				result_list->offset_next = offset_fnd_start + offsetNextTitleSearch;
				result_list->count++;
				if (result_list->count >= NUMBER_OF_FIRST_PAGE_RESULTS)
				{
					result_list->result_populated = 1;
					goto out;
				}
			}
			else if (rc < 0)
			{
				if (offset_fnd_end <= 0)
				{
					offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) +
						strlen(&pTitleSearch->sTitleSearch[strlen(pTitleSearch->sTitleSearch) + 1]) + 3;
					offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
				}
				else // binary search
				{
					offset_middle = offset_fnd_start + (offset_fnd_end - offset_fnd_start) / 2; // position to the middle of the range
					if (offset_middle <= offset_fnd_start)
						offset_fnd_end = -1;
					else
					{
						len = copy_fnd_to_buf(offset_middle, buf_middle, sizeof(buf_middle));
						if (search_interrupted)
						{
							search_interrupted = 9;
							goto interrupted;
						}
						if (len >= sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
						{
							pTitleSearch = locate_proper_title_search(buf_middle, len);
							if (pTitleSearch)
							{
								char local_title_search[MAX_TITLE_SEARCH];
								char sTitleActual[MAX_TITLE_ACTUAL];

								offset_middle += (char *)pTitleSearch - buf_middle;
//#ifndef INCLUDED_FROM_KERNEL
//msg(MSG_INFO, "offset_middle %x\n", offset_middle);
//#endif
								retrieve_titles_from_fnd(offset_middle, local_title_search, sTitleActual);
								rc = search_string_cmp(local_title_search, search_string, search_str_len);
//#ifndef INCLUDED_FROM_KERNEL
//msg(MSG_INFO, "rc %d, [%s], [%s], [", rc, local_title_search, sTitleActual);
//int i;
//for (i=0;i<search_str_len;i++)
//msg(MSG_INFO, "%c", search_string[i]);
//msg(MSG_INFO, "]\n");
//#endif
								if (rc >= 0)	// the first mactch will be in front or at offset_middle
								{
									if (offset_middle == offset_fnd_end) // search from offset_fnd_start directly
										offset_fnd_end = -1;
									else
										offset_fnd_end = offset_middle;
								}
								else		// the first mactch will be after offset_middle
								{
									offset_fnd_start = offset_middle;
									search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
									search_info[nCurrentWiki].offset_current = offset_fnd_start;
									if (search_interrupted)
									{
										search_interrupted = 10;
										goto interrupted;
									}
									if (search_info[nCurrentWiki].buf_len <= 0)
									{
										result_list->result_populated = 1;
										goto out;
									}
									offsetNextTitleSearch = 0;
								}
							}
						}
						else
							offset_fnd_end = -1;
					}
				}
				offsetNextTitleSearch = 0;
			}
			else
			{
				result_list->result_populated = 1;
				goto out;
			}
		}
		else
		{
			offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
			search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
			search_info[nCurrentWiki].offset_current = offset_fnd_start;
			if (search_interrupted)
			{
				search_interrupted = 11;
				goto interrupted;
			}
			if (search_info[nCurrentWiki].buf_len <= 0)
			{
				result_list->result_populated = 1;
				goto out;
			}
			offsetNextTitleSearch = 0;
		}
	}
out:
	if (result_list->result_populated)
	{
		if (!bInit) // just completed search result
			search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_NORMAL);
		return 0;
	}
	else
	{
		return 1;
	}

interrupted:
	return 1;
}

long get_prefix_index_table(int idx_prefix_index_table)
{
	int idxBlock = idx_prefix_index_table / (SEARCH_CHR_COUNT * SEARCH_CHR_COUNT);

	load_prefix_index(nCurrentWiki);
	if (!search_info[nCurrentWiki].b_prefix_index_block_loaded[idxBlock])
	{
#ifdef INCLUDED_FROM_KERNEL
		if (wl_input_event_pending())
			search_interrupted = 1;
#endif
		wl_seek(search_info[nCurrentWiki].fd_pfx,
			idxBlock * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(uint32_t));
		wl_read(search_info[nCurrentWiki].fd_pfx,
			&(search_info[nCurrentWiki].prefix_index_table[idxBlock * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT]),
			SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(uint32_t));
		search_info[nCurrentWiki].b_prefix_index_block_loaded[idxBlock]++;
	}
	return search_info[nCurrentWiki].prefix_index_table[idx_prefix_index_table];
}

long get_search_result_start()
{
	long offset_search_result_start = -1;
	int idx_prefix_index_table;
	char c1, c2, c3;
	int found = 0;
/* Disable hashing
	int i;
	int lenCompared;
	int lenCopied;
*/
	long offset;
/* Disable hashing
	static int lenHashedSearchString = 0;
	static char sHashedSearchString[MAX_SEARCH_STRING_HASHED_LEN];
	static long offsetHasedSearchString[MAX_SEARCH_STRING_HASHED_LEN];
*/

/* Disable hashing
	if (search_str_len > 3)
	{
		// check the length of the hashed search string can be reused
		if (search_str_len > lenHashedSearchString)
			lenCompared = lenHashedSearchString;
		else
			lenCompared = search_str_len;
		lenHashedSearchString = 0;
		for (i = 0; i < lenCompared; i++)
		{
			if (sHashedSearchString[i] != search_string[i])
				lenHashedSearchString = i;
		}

		// Check if hashed
		if (lenHashedSearchString > 3)
		{
			if (search_str_len > lenHashedSearchString)
			{
				if (search_str_len > MAX_SEARCH_STRING_HASHED_LEN)
					lenCopied = MAX_SEARCH_STRING_HASHED_LEN - lenHashedSearchString;
				else
					lenCopied = search_str_len - lenHashedSearchString;
				memcpy(&sHashedSearchString[lenHashedSearchString], &search_string[lenHashedSearchString], lenCopied);
				// check the extended part first
				for (i = 3; i < lenHashedSearchString + lenCopied; i++)
				{
					if (i >= lenHashedSearchString)
						offsetHasedSearchString[i] = get_search_hash_offset_fnd(sHashedSearchString, i + 1);
					if (search_interrupted)
					{
						search_interrupted = 12;
						goto interrupted;
					}
					if (offsetHasedSearchString[i] &&
					    (i >= MAX_SEARCH_STRING_ALL_HASHED_LEN || i == search_str_len - 1))
					{
						found = 1;
						offset_search_result_start = offsetHasedSearchString[i]; // use the longest hashed search string
					}
				}
				lenHashedSearchString += lenCopied;
			}

			if (!found) // not hashed at the extended part
			{
				for (i = 3; i < search_str_len && i < lenHashedSearchString; i++)
				{
					if (offsetHasedSearchString[i] &&
					    (i >= MAX_SEARCH_STRING_ALL_HASHED_LEN || i == search_str_len - 1))
					{
						found = 1;
						offset_search_result_start = offsetHasedSearchString[i]; // use the longest hashed search string
					}
					else
						break;
				}
			}
		}
		else
		{
			if (search_str_len > MAX_SEARCH_STRING_HASHED_LEN)
				lenHashedSearchString = MAX_SEARCH_STRING_HASHED_LEN;
			else
				lenHashedSearchString = search_str_len;
			memcpy(sHashedSearchString, search_string, lenHashedSearchString);
			for (i = 3; i < lenHashedSearchString; i++)
			{
				offsetHasedSearchString[i] = get_search_hash_offset_fnd(sHashedSearchString, i + 1);
				if (search_interrupted)
				{
					search_interrupted = 13;
					goto interrupted;
				}
				if (offsetHasedSearchString[i] &&
				    (i >= MAX_SEARCH_STRING_ALL_HASHED_LEN || i == search_str_len - 1))
				{
					found = 1;
					offset_search_result_start = offsetHasedSearchString[i]; // use the longest hashed search string
				}
			}
		}
	}
*/

//	if (!found && (3 >= search_str_len || search_str_len > MAX_SEARCH_STRING_ALL_HASHED_LEN))
	if (!found)
	{
		switch(search_str_len)
		{
		case 1:
			c1 = search_string[0];
			c2 = '\0';
			c3 = '\0';
			break;
		case 2:
			c1 = search_string[0];
			c2 = search_string[1];
			c3 = '\0';
			break;
		default:
			c1 = search_string[0];
			c2 = search_string[1];
			c3 = search_string[2];
			break;
		}
		idx_prefix_index_table = bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT +
			bigram_char_idx(c2) * SEARCH_CHR_COUNT + bigram_char_idx(c3);
		if ((offset = get_prefix_index_table(idx_prefix_index_table)))
		{
			found = 1;
			offset_search_result_start = offset;
		}
		if (search_interrupted)
		{
			search_interrupted = 14;
			goto interrupted;
		}
	}

interrupted:
	return offset_search_result_start;
}

int next_search_string(char *local_search_string, int len_local_search_string)
{
	char *pSupportedChars = SUPPORTED_SEARCH_CHARS;
	int idxChar;

	if (len_local_search_string > MAX_SEARCH_STRING_HASHED_LEN)
		len_local_search_string = MAX_SEARCH_STRING_HASHED_LEN;
	if (len_local_search_string > 0)
	{
		idxChar = 0;
		while (idxChar < strlen(pSupportedChars) && pSupportedChars[idxChar] != local_search_string[len_local_search_string - 1])
			idxChar++;
		if (idxChar >= strlen(pSupportedChars) - 1)
		{
			len_local_search_string--;
			len_local_search_string = next_search_string(local_search_string, len_local_search_string);
		}
		else
			local_search_string[len_local_search_string - 1] = pSupportedChars[idxChar + 1];
	}
	return len_local_search_string;
}

long get_search_result_end()
{
	long offset_search_result_end = -1;
	char local_search_string[MAX_TITLE_SEARCH];
	int len_local_search_string;
	int last_len_local_search_string;
	int found = 0;
	int idx_prefix_index_table;
	char c1, c2, c3;
	long offset;

	if (search_str_len  > MAX_SEARCH_STRING_ALL_HASHED_LEN)
		len_local_search_string = MAX_SEARCH_STRING_ALL_HASHED_LEN;
	else
		len_local_search_string = search_str_len;
	memcpy(local_search_string, search_string, len_local_search_string);

/* disable hashing
	while (!found && len_local_search_string > 3)
	{
		last_len_local_search_string = len_local_search_string;
		len_local_search_string = next_search_string(local_search_string, len_local_search_string);
		if (!memcmp(search_string, local_search_string, len_local_search_string))
			found = 1; // returns -1 directly
		else
		{
			offset_search_result_end = get_search_hash_offset_fnd(local_search_string, len_local_search_string);
			if (search_interrupted)
			{
				search_interrupted = 15;
				goto interrupted;
			}
			if (offset_search_result_end > 0)
			{
				found = 1;
			}
		}
		len_local_search_string = last_len_local_search_string - 1;
	}
*/

	if (!found)
	{
		if (len_local_search_string > 3)
			len_local_search_string = 3;

		while (!found && len_local_search_string > 0)
		{
			last_len_local_search_string = len_local_search_string;
			len_local_search_string = next_search_string(local_search_string, len_local_search_string);
			if (!memcmp(search_string, local_search_string, len_local_search_string))
				found = 1; // returns -1 directly
			else
			{
				switch(len_local_search_string)
				{
				case 1:
					c1 = local_search_string[0];
					c2 = '\0';
					c3 = '\0';
					break;
				case 2:
					c1 = local_search_string[0];
					c2 = local_search_string[1];
					c3 = '\0';
					break;
				default:
					c1 = local_search_string[0];
					c2 = local_search_string[1];
					c3 = local_search_string[2];
					break;
				}
				idx_prefix_index_table = bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT +
					bigram_char_idx(c2) * SEARCH_CHR_COUNT + bigram_char_idx(c3);
				if ((offset = get_prefix_index_table(idx_prefix_index_table)))
				{
					found = 1;
					offset_search_result_end = offset;
				}
				if (search_interrupted)
				{
					search_interrupted = 14;
					goto interrupted;
				}
//				if (search_info[nCurrentWiki].prefix_index_table[idx_prefix_index_table])
//				{
//					found = 1;
//					offset_search_result_end = search_info[nCurrentWiki].prefix_index_table[idx_prefix_index_table];
//				}
			}
			len_local_search_string = last_len_local_search_string - 1;
		}
	}

interrupted:
	return offset_search_result_end;
}

uint32_t get_article_idx_by_title(char *titleSearch, char *titleActual)
{
	uint32_t article_idx = 0;
	long offset_search_result_start = -1;
	long offset_search_result_end = -1;
	int i = 0;

	search_str_len = 0;
	search_str_hiragana_len = 0;
	while (titleSearch[i] && titleSearch[i] != CHAR_LANGUAGE_LINK_TITLE_DELIMITER && search_str_len < MAX_TITLE_SEARCH - 1)
	{
		if (is_supported_search_char(titleSearch[i]))
		{
			if ('A' <= titleSearch[i] && titleSearch[i] <= 'Z')
				search_string[search_str_len++] = 'a' + (titleSearch[i] - 'A');
			else
				search_string[search_str_len++] = titleSearch[i];
		}
		i++;
	}

	if (search_str_len > 0)
	{
		search_string[search_str_len] = '\0';
		offset_search_result_start = get_search_result_start();
		if (!search_interrupted && offset_search_result_start > 0)
		{
			offset_search_result_end = get_search_result_end();
			if (!search_interrupted)
				article_idx = get_article_idx_from_offset_range(titleActual, offset_search_result_start, offset_search_result_end);
		}
		search_str_len = 0;
		search_str_hiragana_len = 0;
	}

	return article_idx;
}

int search_populate_result()
{
	int found = 0;
	long offset_search_result_start = -1;
	long offset_search_result_end = -1;

	search_string_changed = false;
	result_list->count = 0;
	result_list->result_populated = 0;
	result_list->cur_selected = -1;
	if (search_str_len > 0)
	{
		offset_search_result_start = get_search_result_start();
		if (search_interrupted)
		{
			search_interrupted = 16;
			goto interrupted;
		}
		if (offset_search_result_start > 0)
		{
			found = 1;
			offset_search_result_end = get_search_result_end();
			if (search_interrupted)
			{
				search_interrupted = 17;
				goto interrupted;
			}
			fetch_search_result(offset_search_result_start, offset_search_result_end, 1);
			if (search_interrupted)
			{
				search_interrupted = 18;
				goto interrupted;
			}
		}
		else
		{
			result_list->result_populated = 1;
		}
	}
	return found;

interrupted:
	search_string_changed = true;
	return found;
}

void capitalize(char *in_str, char *out_str, int len)
{
	//char cPrev = ' ';
	int i = 0;

	while (i < len)
	{
		//if (cPrev == ' ' && 'a' <= in_str[i] && in_str[i] <= 'z')
		if (i == 0 && 'a' <= in_str[i] && in_str[i] <= 'z')
			out_str[i] = in_str[i] - 32;
		else
			out_str[i] = in_str[i];
		//cPrev = in_str[i];
		i++;
	}
	out_str[i] = '\0';
}

char *get_hiragana(char *in_str, int len, int *used_len, char **pEnglish)
{
	int i;
	int bFound = 0;
	int iStart = 0;
	int iEnd = sizeof(english_hiragana_mapping) / sizeof(struct _english_hiragana_mapping);
	int iMiddle;
	char *pReturn = NULL;

	while (!bFound && iStart < iEnd)
	{
		if (*in_str == english_hiragana_mapping[iStart].english[0])
			bFound = 1;
		else if (*in_str > english_hiragana_mapping[iStart].english[0])
		{
			iMiddle = (iStart + iEnd) / 2;
			if (iMiddle != iStart)
			{
				if (*in_str == english_hiragana_mapping[iMiddle].english[0])
				{
					iStart = iMiddle;
					bFound = 1;
				}
				else if (*in_str > english_hiragana_mapping[iMiddle].english[0])
					iStart = iMiddle;
				else
					iEnd = iMiddle;
			}
			else if (iStart != iEnd)
			{
				iStart = iEnd;
				if (*in_str == english_hiragana_mapping[iEnd].english[0])
					bFound = 1;
			}
		}
	}

	if (bFound) // find the first hiragana_mapping entry with the same starting character as in_str
	{
		while (iStart > 0 && *in_str == english_hiragana_mapping[iStart - 1].english[0])
			iStart--;

		for (i = iStart; i < sizeof(english_hiragana_mapping) / sizeof(struct _english_hiragana_mapping) && *in_str == english_hiragana_mapping[i].english[0]; i++)
		{
			if (len >= strlen(english_hiragana_mapping[i].english) && !strncmp(in_str, english_hiragana_mapping[i].english, strlen(english_hiragana_mapping[i].english)))
			{
				*used_len = strlen(english_hiragana_mapping[i].english);
				if (english_hiragana_mapping[i].unified_english)
					*pEnglish = english_hiragana_mapping[i].unified_english;
				else
					*pEnglish = english_hiragana_mapping[i].english;
				pReturn = english_hiragana_mapping[i].hiragana;
			}
		}
	}

	return pReturn;
}

char *get_english(char *in_str, int len, int *used_len)
{
	int i;
	int bFound = 0;
	int iStart = 0;
	int iEnd = sizeof(hiragana_english_mapping) / sizeof(struct _hiragana_english_mapping);
	int iMiddle;
	char *pReturn = NULL;

	while (!bFound && iStart < iEnd)
	{
		if (*in_str == hiragana_english_mapping[iStart].hiragana[0])
			bFound = 1;
		else if (*in_str > hiragana_english_mapping[iStart].hiragana[0])
		{
			iMiddle = (iStart + iEnd) / 2;
			if (iMiddle != iStart)
			{
				if (*in_str == hiragana_english_mapping[iMiddle].hiragana[0])
				{
					iStart = iMiddle;
					bFound = 1;
				}
				else if (*in_str > hiragana_english_mapping[iMiddle].hiragana[0])
					iStart = iMiddle;
				else
					iEnd = iMiddle;
			}
			else if (iStart != iEnd)
			{
				iStart = iEnd;
				if (*in_str == hiragana_english_mapping[iEnd].hiragana[0])
					bFound = 1;
			}
		}
	}

	if (bFound) // find the first hiragana_mapping entry with the same starting character as in_str
	{
		while (iStart > 0 && *in_str == hiragana_english_mapping[iStart - 1].hiragana[0])
			iStart--;

		for (i = iStart; i < sizeof(hiragana_english_mapping) / sizeof(struct _hiragana_english_mapping) && *in_str == hiragana_english_mapping[i].hiragana[0]; i++)
		{
			if (len >= strlen(hiragana_english_mapping[i].hiragana) && !strncmp(in_str, hiragana_english_mapping[i].hiragana, strlen(hiragana_english_mapping[i].hiragana)))
			{
				*used_len = strlen(hiragana_english_mapping[i].hiragana);
				pReturn = hiragana_english_mapping[i].english;
			}
		}
	}

	return pReturn;
}

void search_reload(int flag)
{
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
		NUMBER_OF_FIRST_PAGE_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int y_pos,start_x_search=0;
	int end_y_pos;
	static int last_start_x_search=0;
	char *title;
	char temp_search_string[MAX_TITLE_SEARCH * 3];
	static int bNoResultLastTime = 0;
	int keyboard_mode = keyboard_get_mode();
	char *pMsg;

	more_search_results = 0;
	guilib_fb_lock();
	if (keyboard_mode == KEYBOARD_NONE)
	{
		if (result_list->result_populated || flag == SEARCH_RELOAD_KEEP_REFRESH)
		{
			if (flag == SEARCH_RELOAD_KEEP_RESULT)
				guilib_clear_area(0, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT, 239, LCD_HEIGHT_LINES - 1);
			else
				guilib_clear();
		}
		else
			guilib_clear_area(start_x_search, 0, LCD_BUF_WIDTH_PIXELS, 30);
	}
	else
	{
		if (flag != SEARCH_RELOAD_NO_POPULATE)
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
		if (flag == SEARCH_RELOAD_KEEP_REFRESH)
			guilib_clear_area(0, 0, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
		if(search_string_changed_remove)
		{
			if(!search_str_len)
				start_x_search = 0;
			else
				start_x_search = search_string_pos[search_str_len];
			search_string_changed_remove = false;
			if (start_x_search < LCD_BUF_WIDTH_PIXELS)
				guilib_clear_area(start_x_search, 0, LCD_BUF_WIDTH_PIXELS, 30);
			//else
			//{
			//	guilib_clear_area(0, 0, LCD_BUF_WIDTH_PIXELS, 30);
			//}
		}


//		if (result_list->result_populated)
//			guilib_clear_area(0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
	}

	if (!search_str_len)
	{
		search_string_changed = 0;
		bNoResultLastTime = 0;
		result_list->count = 0;
		result_list->result_populated = 1;
		result_list->cur_selected = -1;
		pMsg = get_nls_text("type_a_word");
		render_string_and_clear(SUBTITLE_FONT_IDX, -1, 55, pMsg, strlen(pMsg), 0,
					0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
		keyboard_paint();
		goto out;
	}

	if (wiki_is_japanese())
		capitalize(search_string_hiragana, temp_search_string, search_str_hiragana_len);
	else
		capitalize(search_string, temp_search_string, search_str_len);
	if (last_start_x_search >= LCD_BUF_WIDTH_PIXELS)
		guilib_clear_area(0, 0, LCD_BUF_WIDTH_PIXELS, 30);
	start_x_search = render_string_right(SEARCH_HEADING_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, temp_search_string, strlen(temp_search_string), 0);
	if (last_start_x_search < LCD_BUF_WIDTH_PIXELS && start_x_search >= LCD_BUF_WIDTH_PIXELS)
	{
		guilib_clear_area(0, 0, LCD_BUF_WIDTH_PIXELS, 30);
		start_x_search = render_string_right(SEARCH_HEADING_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, temp_search_string, strlen(temp_search_string), 0);
	}
	last_start_x_search = start_x_search;
	search_string_pos[search_str_len]=start_x_search;
	y_pos = RESULT_START;


	if (result_list->result_populated && flag != SEARCH_RELOAD_NO_POPULATE)
	{
		unsigned int i;
		unsigned int count = result_list->count < screen_display_count ?
			result_list->count : screen_display_count;
		if (!result_list->count) {
			if (!bNoResultLastTime)
			{
				guilib_clear_area(0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
				pMsg = get_nls_text("no_results");
				render_string(SEARCH_LIST_FONT_IDX, -1, 55, pMsg, strlen(pMsg), 0);
				bNoResultLastTime = 1;
			}
			goto out;
		}
		bNoResultLastTime = 0;

		article_link_count = 0;
		for (i = 0; i < screen_display_count; i++)
		{
			end_y_pos = y_pos + RESULT_HEIGHT - 1;
			if (screen_display_count < NUMBER_OF_FIRST_PAGE_RESULTS && end_y_pos > LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1)
				end_y_pos = LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1;
			guilib_clear_area(0, y_pos, 239, end_y_pos);
			if (i < count)
			{
				if (keyboard_mode == KEYBOARD_NONE)
				{
					articleLink[article_link_count].start_xy = (unsigned  long)((y_pos - 2) << 8); // consider the difference between render_string and draw_string
					articleLink[article_link_count].end_xy = (unsigned  long)((LCD_BUF_WIDTH_PIXELS) | ((end_y_pos - 2) << 8));
					articleLink[article_link_count++].article_id = result_list->idx_article[i];
				}
				title = result_list->title[i];
				render_string(SEARCH_LIST_FONT_IDX, LCD_LEFT_MARGIN, y_pos, title, strlen(title), 0);
			}
			y_pos += RESULT_HEIGHT;
			if((y_pos+RESULT_HEIGHT)>guilib_framebuffer_height())
				break;
		}
		if (keyboard_mode == KEYBOARD_NONE)
		{
			if (result_list->count == NUMBER_OF_FIRST_PAGE_RESULTS)
			{
				more_search_results = 1;
			}
		}
	}
out:
	guilib_fb_unlock();
}

void search_to_be_reloaded(int to_be_reloaded_flag, int reload_flag)
{
	static bool to_be_reloaded = false;
	static int saved_reload_flag;

	switch (to_be_reloaded_flag)
	{
	case SEARCH_TO_BE_RELOADED_CLEAR:
		to_be_reloaded = false;
		break;
	case SEARCH_TO_BE_RELOADED_SET:
		if (reload_flag == SEARCH_RELOAD_NORMAL && keyboard_key_inverted())
		{
			to_be_reloaded = true;
			saved_reload_flag = reload_flag;
		}
		else
		{
			search_reload(reload_flag);
			to_be_reloaded = false;
		}
		break;
	case SEARCH_TO_BE_RELOADED_CHECK:
		if (to_be_reloaded && !keyboard_key_inverted())
		{
			search_reload(saved_reload_flag);
			to_be_reloaded = false;
		}
		break;
	default:
		break;
	}
}

void search_result_display()
{
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
		NUMBER_OF_FIRST_PAGE_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int y_pos=0;
	char *title;
	char *pMsg;

	guilib_fb_lock();
	guilib_clear_area(0, RESULT_START, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);

	if (!search_str_len)
	{
		result_list->count = 0;
		result_list->cur_selected = -1;
		pMsg = get_nls_text("type_a_word");
		render_string_and_clear(SUBTITLE_FONT_IDX, -1, 55, pMsg, strlen(pMsg), 0,
					0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
		keyboard_paint();
		goto out;
	}


	y_pos = RESULT_START;

	if (result_list->result_populated && !result_list->count) {
		pMsg = get_nls_text("no_results");
		render_string(SEARCH_LIST_FONT_IDX, -1, 55, pMsg, strlen(pMsg), 0);
		goto out;
	}

	if (result_list->count) {
		unsigned int i;
		unsigned int count = result_list->count < screen_display_count ?
			result_list->count : screen_display_count;

		for (i = 0; i < count; i++) {
			title = result_list->title[i];
			render_string(SEARCH_LIST_FONT_IDX, LCD_LEFT_MARGIN, y_pos, title, strlen(title), 0);
			y_pos += RESULT_HEIGHT;
			if((y_pos+RESULT_HEIGHT)>guilib_framebuffer_height())
				break;
		}
		if (result_list->cur_selected >= screen_display_count)
			result_list->cur_selected = screen_display_count - 1;
		invert_selection(result_list->cur_selected, -1, RESULT_START, RESULT_HEIGHT);
	}
out:
	guilib_fb_unlock();
}

void search_hiragana_add_char()
{
	int i;
	char *pHiragana = NULL;
	int used_len;
	char *pEnglish = NULL;

	if (search_str_len == 1)
		search_str_converted_len = 0;

	if (keyboard_get_mode() == KEYBOARD_CHAR ||
		(search_str_len > 0 && search_string[search_str_len - 1] == '-' && (search_string_hiragana[search_str_hiragana_len - 1] & 0x80)))
	{
		i = search_str_converted_len;
		if (search_str_len - i > LONGEST_HIRAGANA_ENGLISH_CHARS)
			i = search_str_len - LONGEST_HIRAGANA_ENGLISH_CHARS;
		while (i < search_str_len && (search_string[i] == ' ' ||
			!(pHiragana = get_hiragana(&search_string[i], search_str_len - i, &used_len, &pEnglish))))
			i++;
		if (pHiragana)
		{
			memcpy(&search_string_hiragana[search_str_hiragana_len - (search_str_len - i - 1)], pHiragana, strlen(pHiragana) + 1);
			search_str_hiragana_len += - (search_str_len - i - 1) + strlen(pHiragana);
			memcpy(&search_string[i], pEnglish, strlen(pEnglish) + 1);
			search_str_len = i + strlen(pEnglish);
			search_str_converted_len = search_str_len;
		}
		else
		{
			search_string_hiragana[search_str_hiragana_len++] = search_string[search_str_len - 1];
		}
	}
	else
	{
		search_string_hiragana[search_str_hiragana_len++] = search_string[search_str_len - 1];
		search_str_converted_len = search_str_len;
	}

}

int search_add_char(char c, unsigned long ev_time)
{
	(void)ev_time;
	if((c == 0x20 && search_str_len>0 && search_string[search_str_len-1] == 0x20) ||
	   (search_str_len >= MAX_TITLE_SEARCH - 2) ||
	   (!search_str_len && c == 0x20))
		return -1;

	if (!search_str_len) // clear type_a_word message
	{
		guilib_fb_lock();
		guilib_clear_area(0, 55, 239, 80);
		guilib_fb_unlock();
	}
	if ('A' <= c && c <= 'Z')
		c += 32;

	search_string[search_str_len++] = c;
	search_string[search_str_len] = '\0';
	if (wiki_is_japanese())
	{
		search_hiragana_add_char();
	}
	time_search_string_changed = get_time_ticks();
	search_string_changed = true;
	return 0;
}

int check_search_string_change(void)
{
	if (!search_string_changed)
		return 0;

	if (time_diff(get_time_ticks(), time_search_string_changed) > seconds_to_ticks(DELAYED_SEARCH_TIME))
	{
		search_interrupted = 0;
		search_populate_result();
		if (search_interrupted)
			goto interrupted;
		result_list->cur_selected = -1;
		if (result_list->result_populated)
			search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_NORMAL);
	}
	return 1;

interrupted:
	return 1;
}

void search_fetch()
{
	search_populate_result();
	result_list->cur_selected = -1;
	search_string_changed = false;
}

int hiragana_to_english(char *sEnglish, char *sHiragana, int lenHiragana)
{
	int lenEnglish = 0;
	int i = 0;
	char *pEnglish;
	int used_len;

	while (i < lenHiragana)
	{
		pEnglish = get_english(&sHiragana[i], lenHiragana - i, &used_len);
		if (pEnglish)
		{
			memcpy(&sEnglish[lenEnglish], pEnglish, strlen(pEnglish));
			lenEnglish += strlen(pEnglish);
			i += used_len;
		}
		else
		{
			sEnglish[lenEnglish++] = sHiragana[i++];
		}
	}
	sEnglish[lenEnglish] = '\0';
	return lenEnglish;
}
/*
 * return value - 0: remove ok, -1: no key to remove
 */
int search_remove_char(int bPopulate, unsigned long ev_time)
{
	(void)ev_time;
	if (search_str_len == 0)
		return -1;

	if (wiki_is_japanese())
	{
		if (!(search_string_hiragana[search_str_hiragana_len - 1] & 0x80))
		{
			search_string[--search_str_len] = '\0';
			search_string_hiragana[--search_str_hiragana_len] = '\0';
			if (search_str_converted_len > search_str_len)
				search_str_converted_len = search_str_len;
		}
		else
		{
			while (search_str_hiragana_len > 0 &&
					((search_string_hiragana[search_str_hiragana_len - 1] & 0x80) &&
					!(search_string_hiragana[search_str_hiragana_len - 1] & 0x40)))
				search_str_hiragana_len--;
			search_str_hiragana_len--;
			search_str_len = hiragana_to_english(search_string, search_string_hiragana, search_str_hiragana_len);
			search_str_converted_len = search_str_len;
		}
	}
	else
		search_string[--search_str_len] = '\0';

	if (bPopulate || !search_str_len)
	{
		search_populate_result();
		search_string_changed_remove = true;
		result_list->cur_selected = -1;
	}
	else
	{
		search_string_changed = true;
		search_string_changed_remove = true;
		time_search_string_changed = get_time_ticks();
	}
	return 0;
}

TITLE_SEARCH *locate_previous_title(char *buf, int len)
{
	char *p;
	int bFound = 0;

	if (len > 2)
	{
		p = buf + len - 2;
		len--;
		while (!bFound && len > sizeof(uint32_t))
		{
			if (!*p) {
				bFound = 1;
				p -= sizeof(uint32_t);
			}
			else
			{
				p--;
				len--;
			}
		}
		if (bFound)
			return (TITLE_SEARCH *)p;
	}
	return NULL;
}

int search_current_selection(void)
{
//	const char* title;

//	if (result_list->cur_selected >= result_list->count - result_list->first_item) {
//		DP(DBG_SEARCH, ("O search_current_title() NO TITLE\n"));
//		return NULL;
//	}
//	title = result_list_title(result_list->cur_selected+result_list->first_item);
//	DP(DBG_SEARCH, ("O search_current_title() '%s'\n", title));
	return result_list->cur_selected;
}

unsigned int search_result_count()
{
	return result_list->count;
}

int clear_search_string()
{
	if (search_str_len == 0)
		return -1;

	result_list->count = 0;
	strcpy(search_string,"");
	search_str_len = 0;
	search_str_hiragana_len = 0;
	return 0;
}

int get_search_string_len()
{
	return search_str_len;
}

int search_result_selected()
{
	return result_list->cur_selected;
}

extern unsigned char *file_buffer;
extern int restricted_article;
extern int current_article_wiki_id;

char *compressed_buf = NULL;
int retrieve_article(long idx_article_with_wiki_id)
{
	ARTICLE_PTR article_ptr;
	long idx_article;
	int nWikiIdx;

	if (!compressed_buf)
		compressed_buf = (char *)malloc_simple(MAX_COMPRESSED_ARTICLE, MEM_TAG_INDEX_M1);

	current_article_wiki_id = (unsigned long)idx_article_with_wiki_id >> 24;
	if (current_article_wiki_id == 0)
		nWikiIdx = nCurrentWiki;
	else
	{
		nWikiIdx = get_wiki_idx_from_id(current_article_wiki_id );
		if (nWikiIdx >= 0)
			load_prefix_index(nWikiIdx);
		else
		{
			print_article_error();
			return -1;
		}
	}

	idx_article = idx_article_with_wiki_id & 0x00FFFFFF;

	if (nWikiIdx >= 0 && compressed_buf && 0 < idx_article && idx_article <= search_info[nWikiIdx].max_article_idx) {
		wl_seek(search_info[nWikiIdx].fd_idx, sizeof(uint32_t) + (idx_article - 1) * sizeof(article_ptr));
		wl_read(search_info[nWikiIdx].fd_idx, &article_ptr, sizeof(article_ptr));

		int dat_file_id = article_ptr.file_id;

		if (search_info[nWikiIdx].fd_dat[dat_file_id] < 0)
		{
			char file_name[13];
			sprintf(file_name, "wiki%d.dat", dat_file_id);
			search_info[nWikiIdx].fd_dat[dat_file_id] = wl_open(get_wiki_file_path(nWikiIdx, file_name),
									    WL_O_RDONLY);
		}
		if (search_info[nWikiIdx].fd_dat[dat_file_id] >= 0)
		{
			CONCAT_ARTICLE_INFO concat_article_infos[MAX_ARTICLES_PER_COMPRESSION];
			uint8_t nArticlesConcatnated;
			uint32_t dat_article_len;
			SizeT required_len = 0;
			uint32_t offset = 0;
			int i;
			int idx_concat_article = -1;

			wl_seek(search_info[nWikiIdx].fd_dat[dat_file_id], article_ptr.offset_dat & 0x7FFFFFFF);

			wl_read(search_info[nWikiIdx].fd_dat[dat_file_id], &nArticlesConcatnated,
				sizeof(nArticlesConcatnated));

			wl_read(search_info[nWikiIdx].fd_dat[dat_file_id], &concat_article_infos,
				nArticlesConcatnated * sizeof(CONCAT_ARTICLE_INFO));
			for (i = 0; i < nArticlesConcatnated; i++)
			{
				if (concat_article_infos[i].article_id == idx_article)
				{
					idx_concat_article = i;
					offset = concat_article_infos[i].offset_article & ~0x80000000;
					required_len = offset + concat_article_infos[i].article_len;
					break;
				}
			}

			wl_read(search_info[nWikiIdx].fd_dat[dat_file_id], &dat_article_len, sizeof(dat_article_len));

			wl_read(search_info[nWikiIdx].fd_dat[dat_file_id], compressed_buf, dat_article_len);

			dat_article_len -= LZMA_PROPS_SIZE;

			ELzmaStatus status;
			//SizeT file_buffer_len = FILE_BUFFER_SIZE;
			SizeT compressed_buffer_len = dat_article_len;
			int rc = (int)LzmaDecode(file_buffer,
						 &required_len,
						 (const Byte *)compressed_buf + LZMA_PROPS_SIZE,
						 &compressed_buffer_len,
						 (const Byte *)compressed_buf, LZMA_PROPS_SIZE,
						 LZMA_FINISH_ANY,
						 &status, &g_Alloc);

			if (rc == SZ_OK || rc == SZ_ERROR_INPUT_EOF) // can generate SZ_ERROR_INPUT_EOF but result is OK
			{
				if (idx_concat_article >= 0)
				{
					if (concat_article_infos[idx_concat_article].offset_article & 0x80000000) {
						restricted_article = 1;
					} else {
						restricted_article = 0;
					}
					memcpy(file_buffer, &file_buffer[offset], concat_article_infos[idx_concat_article].article_len);
					file_buffer[concat_article_infos[idx_concat_article].article_len] = '\0';
					return 0;
				}
			}
		}

	}
	print_article_error();
	return -1;
}

void search_set_selection(int new_selection)
{

	result_list->cur_selected = new_selection;
}
void search_open_article(int new_selection)
{
	int list_idx;

	list_idx = new_selection;
	if (list_idx >= NUMBER_OF_FIRST_PAGE_RESULTS)
		list_idx -= NUMBER_OF_FIRST_PAGE_RESULTS;
	display_link_article(result_list->idx_article[list_idx]);
}

long find_closest_idx(long idx, char *title)
{
	ARTICLE_PTR article_ptr;
	TITLE_SEARCH title_search;
	static int count = 0;
	char sTitleSearch[MAX_TITLE_SEARCH];

	title[0] = '\0';

	if (idx > search_info[nCurrentWiki].max_article_idx)
		idx -= search_info[nCurrentWiki].max_article_idx;
	wl_seek(search_info[nCurrentWiki].fd_idx, (idx - 1) * sizeof(ARTICLE_PTR) + sizeof(uint32_t));
	wl_read(search_info[nCurrentWiki].fd_idx, &article_ptr, sizeof(article_ptr));

	if (!article_ptr.offset_fnd)
	{
		if (count < 10)
		{
			count++;
			return find_closest_idx(idx + 1, title);
		}
		else
		{
			count = 0;
			return 0;
		}
	}
	else
	{
		count = 0;
		copy_fnd_to_buf(article_ptr.offset_fnd, (char *)&title_search, sizeof(title_search));
		retrieve_titles_from_fnd(article_ptr.offset_fnd, sTitleSearch, title);
		title[MAX_TITLE_ACTUAL - 1] = '\0';
		return idx;
	}
}

#if !defined(INCLUDED_FROM_KERNEL)
#include <time.h>
#include <sys/time.h>
#endif

extern int last_display_mode;
void random_article(void)
{
	long idx_article;
	char title[MAX_TITLE_ACTUAL];
	unsigned long clock_ticks;

#if defined(INCLUDED_FROM_KERNEL)
	clock_ticks = Tick_get();
#else
	//clock_ticks = clock();  // this gets CPU time used, it not suitable for random
	struct timeval t;         // get elapsed time in us but do not care about overflow
	gettimeofday(&t, NULL);   // this more-or-less simulates what happens on WR hardware
	clock_ticks = t.tv_sec * 1000000 + t.tv_usec;
#endif
	idx_article = clock_ticks % search_info[nCurrentWiki].max_article_idx + 1;
	idx_article = find_closest_idx(idx_article, title);

	if (idx_article)
	{
		last_display_mode = DISPLAY_MODE_INDEX; // for history_save not to log the last article index
		display_link_article(idx_article);
#ifdef INCLUDED_FROM_KERNEL
		wl_input_reset_random_key(); // only respond to the first random key
#endif
	}
}
