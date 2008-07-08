/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RIOCH COMPANY,LTD. JAPAN
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: tlcs900vec.cpp,v 1.4 2007/04/17 05:08:07 honda Exp $
 */
// tlcs900vec.cpp : コンソール アプリケーション用のエントリ ポイントの定義
//
//------------------------------------------------------------------------------
// 変更履歴
//version yyyy/mm/dd 概要
//---------- -------------------------------------------------------------------
// 01.00   2004/01/02 初版
// 01.01   2004/08/06 FIX VECTOR が固定値で有ったのをパラメタにて変更可能とした
//                    EXCINIB の配列が０の場合の問題修正。                      
//                    M30262F8FGの生成時、割り込みベクタが1行多い不具合修正。
//                    M30620FCAFPのJSP1.3用ベクタ生成コードを条件コンパイル化。
//----------------------------------------------------------------------------- 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	MAX_INCLUDE		8
#define	MAX_INT			64
#define	MAX_EXC			9
#define	MAX_VEC			MAX_INT
#define	BUF_SIZE		512
#define	STR_SIZE		128

#define	INT_STATE		0
#define	EXC_STATE		1
#define	NORMAL_STATE	2

struct VEC {
	int		no;
	char	no_name[STR_SIZE];
	char	no_label[STR_SIZE];
};

static char const default_input_file[] = "kernel_cfg.i";
static char const default_output_file[] = "tlcs900vec.inc";
static char const default_unused_int[] = "unused_interrupt";

//start出力設定
static char const start_extern[] ="\n\textern large _start";
static char const start_vector[] = "\tdl\t_start\t; 01  0xffff00";
static char const lf[] = "\n";

static char input_file[STR_SIZE];
static char output_file[STR_SIZE];
static char include_file[MAX_INCLUDE][STR_SIZE];
static int  num_include = 0;
static int  num_vec[2] = {0, 0};
static int  max_int    = 47; 
static int  num_nodec = 0;
static int  vec_state = NORMAL_STATE;
static struct VEC vec_table[2][MAX_VEC];
static char unused_vec[2][STR_SIZE];

static void set_TLSS_TLCS900(FILE* pfo, int m);
static void set_global(FILE* pfo, char const * t);
static void set_global_function(FILE* pfo, char const * t);
static void set_vector(FILE* pfo, int kind, int no);
static bool test_string(char** s, char const * t);
static bool skip_space(char** s);
static bool skip_char(char** s, char const c);

int main(int argc, char* argv[])
{
	FILE* pfi;
	FILE* pfo;
	FILE* pfw;
	char  buf[BUF_SIZE];
	struct VEC *v;
	int   i, j, k;
	bool  cnv, dec;
	char* s;
	char* p;
	char* q;
	char  c;

	strcpy(input_file, default_input_file);
	strcpy(output_file, default_output_file);
	strcpy(unused_vec[0], default_unused_int);
	strcpy(unused_vec[1], default_unused_int);

	//オプション指定
	for(i = 1 ; i < argc ; i++){
		s = argv[i];
		if(*s++ == '-'){
			c = *s++;
			skip_space(&s);
			switch(c){
			case 'O':		// アウトプットファイルの設定
			case 'o':
				strcpy(output_file, s);
				break;
			case 'R':		// インプットファイルの設定
			case 'r':
				strcpy(input_file, s);
				break;
			default:
				printf("m16cvec -R<input_file> -O<output_file> -I<include_file> -M<mode> -F<fixvector>\n");
				break;
			}
		}
	}
	printf("input file   = %s\n", input_file);
	printf("output file  = %s\n", output_file);
	if((pfi = fopen(input_file, "r")) == NULL){
		fprintf(stderr, "can't open input file !");
		exit(1);
	}
	if((pfo = fopen(output_file, "w")) == NULL){
		fclose(pfi);
		fprintf(stderr, "can't open output file !");
		exit(1);
	}
	for(i = 0 ; i < num_include ; i++){
		printf("include file = %s\n", include_file[i]);
	}
	//ファイル入力
	for(;;){
		if((fgets(buf, BUF_SIZE, pfi)) == NULL)
			break;
		s = buf;
		switch(vec_state){
		case INT_STATE:
		case EXC_STATE:
			//入力文の文末まできた時の処理
			if(test_string(&s, ";"))
				vec_state = NORMAL_STATE;
			//文字列抽出開始
			else if(test_string(&s, "{")){
				v = &vec_table[vec_state][num_vec[vec_state]];
				p = &v->no_name[0];
				if(skip_space(&s))
					continue;
				dec = true;

				while(*s != ','){
					if(*s == 0)
						continue;
					//(,),spaceの場合は次のポインタを進める
					if(*s == '(' || *s == ')'|| *s <= ' '){
						s++;
						continue;
					}
					//sが[0-9]でない場合
					if(*s < '0' || *s > '9')
						dec = false;
					*p++ = *s++;
				}
				s++;
				*p++ = 0;
				if(dec)
					v->no = atoi(v->no_name);
				else{
					v->no = -1;
					num_nodec++;
				}
				p = &v->no_label[0];

				if(skip_char(&s, ','))
					continue;

				test_string(&s, "(FP)");

				if(skip_space(&s))
					continue;
				while(*s != ')' && *s != ',' && *s > ' ' && *s != '}'){
					*p++ = *s++;
				}
				*p++ = 0;
				num_vec[vec_state]++;

				printf("%s\n",v->no_label);
			}
			else
				continue;
			break;
		default:
			if(!test_string(&s, "const"))
				continue;
			if(test_string(&s, "INHINIB"))
				vec_state = INT_STATE;
			else if(test_string(&s, "EXCINIB"))
				vec_state = EXC_STATE;
			break;
		}
	}
	do{
		cnv = false;
		for(i = 0 ; i < num_include ; i++){
			if((pfw = fopen(include_file[i], "r")) != NULL){
				for(;;){
					if((fgets(buf, BUF_SIZE, pfw)) == NULL)
						break;
					s = buf;
					if(!test_string(&s, "#define"))
						continue;
					if(skip_space(&s))
						continue;
					for(k = 0 ; k < 2 ; k++){
						for(j = 0 ; j < num_vec[k] ; j++){
							v = &vec_table[k][j];
							if(v->no < 0){
								p = s;
								dec = true;
								if(test_string(&p, v->no_name)){
									q = &v->no_name[0];
									if(!skip_space(&p)){
										while(*p > ' '){
											if(*p == '(' || *p == ')'){
												p++;
												continue;
											}
											if(*p < '0' || *p > '9')
												dec = false;
											*q++ = *p++;
										}
									}
									*q++ = 0;
								}
								else
									dec = false;
								if(dec){
									v->no = atoi(v->no_name);
									num_nodec--;
								}
							}
						}
					}
				}
				fclose(pfw);
			}
			else
				printf("open error %s !!\n", include_file[i]);
		}
	}while(cnv);

	for(i = 0 ; i < num_vec[EXC_STATE] ; i++){
		v = &vec_table[EXC_STATE][i];
		if(v->no >= 32 && v->no < MAX_INT){
			vec_table[INT_STATE][num_vec[INT_STATE]] = vec_table[EXC_STATE][i];
			num_vec[INT_STATE]++;
			for(j = i ; j < (num_vec[EXC_STATE]-1) ; j++)
				vec_table[EXC_STATE][j] = vec_table[EXC_STATE][j+1];
			num_vec[EXC_STATE]--;
		}
	}
	for(j = 0 ; j < num_vec[INT_STATE] ; j++){
		v = &vec_table[INT_STATE][j];
		if(v->no >= MAX_INT)
			strcpy(unused_vec[INT_STATE], v->no_label);
		else if(max_int < v->no)
			max_int = v->no;
	}
	for(j = 0 ; j < num_vec[EXC_STATE] ; j++){
		v = &vec_table[EXC_STATE][j];
		if((v->no >= MAX_EXC && v->no < 32) || v->no >= MAX_INT)
			strcpy(unused_vec[EXC_STATE], v->no_label);
	}
	//extern startを出力
	fputs(start_extern, pfo);

	if(num_nodec > 0)
		printf("%dのエクセプション番号を特定できません！\n", num_nodec);
	else{
		fputs(lf, pfo);
		for(i = 0 ; i < 2 ; i++){
			for(j = 0 ; j < num_vec[i] ; j++)
				set_global_function(pfo, vec_table[i][j].no_label);
		}
		if(!strcmp(default_unused_int, unused_vec[0]))
			set_global(pfo, unused_vec[0]);
		else if(!strcmp(default_unused_int, unused_vec[1]))
			set_global(pfo, unused_vec[1]);
			set_TLSS_TLCS900(pfo, max_int);

	}
	fclose(pfi);
	fclose(pfo);
	return 0;
}

static void set_TLSS_TLCS900(FILE* pfo, int m)
{
	int iVec_num;

	fputs("\nf_vecter section code large align=4", pfo);
	fputs(lf, pfo);

	for(iVec_num = 0 ; iVec_num < MAX_INT ; iVec_num++){
		if(iVec_num == 0){
			fputs(start_vector, pfo);
			fputs(lf, pfo);}
		else
			//この部分で設定
			set_vector(pfo, INT_STATE, iVec_num+1);
	}
}

/*
 * extern 宣言をする(名前の前にアンダーバーが不要なもの)
 */
static void set_global(FILE* pfo, char const * t)
{
	fputs("\textern large ", pfo);	
	fputs(t, pfo);
	fputs(lf, pfo);
}
/*
 * extern 宣言をする(名前の前にアンダーバーが必要なもの)
 */
static void set_global_function(FILE* pfo, char const * t)
{
	fputs("\textern large _", pfo);	
	fputs(t, pfo);
	fputs(lf, pfo);
}

/*
 * ベクトルを設定する
 */
static void set_vector(FILE* pfo, int kind, int no)
{
	struct VEC* v = &vec_table[kind][0];
	int  vec_cnt;
	int  address;
	int  address_high;
	int  address_low;
	char comment[15];
	char comment_high[2];
	char comment_low[2];


	address = (no - 1) * 4;
	address_high = address/16;
	address_low = address%16;

	sprintf(comment_high, "%x", address_high);
	sprintf(comment_low, "%x", address_low);

	fputs("\tdl\t", pfo);

	comment[0] = '\t';
	comment[1] = ';';
	comment[2] = ' ';
	comment[3] = (no/10)+'0';
	comment[4] = (no%10)+'0';
	comment[5] = ' ';
	comment[6] = '0';
	comment[7] = 'x';
	comment[8] = 'f';
	comment[9] = 'f';
	comment[10] = 'f';
	comment[11] = 'f';
	comment[12] = comment_high[0];
	comment[13] = comment_low[0];
	comment[14] = 0;

	for(vec_cnt = 0 ; vec_cnt < num_vec[kind] ; vec_cnt++, v++){
		if(v->no == no){
			fputs("_", pfo);
			fputs(v->no_label, pfo);
			fputs(comment, pfo);
			fputs(lf, pfo);
			return;
		}
	}
	//unused interrupt
	fputs(unused_vec[kind], pfo);
	fputs(comment, pfo);
	fputs(lf, pfo);
}


/*
 * 文字列を比較して同一ならばTURE
 */
static bool test_string(char** s, char const *t)
{
	char*  p;
	char   c;

	p = *s;
	while(*p <= ' '){
		if(*p == 0)
			return false;
		p++;
	}
	while((c = *p++) != 0){
		if(c != *t++)
			break;
		if(*t == 0){
			*s = p;
			return true;
		}
	}
	return false;
}

/*
 * 指定のキャラクタまでスキップ
 */
static bool skip_char(char** s, char const c)
{
	char* p;

	p = *s;
	while(*p != c){
		if(*p == 0)
			break;
		p++;
	}
	if(*p){
		p++;
		*s = p;
		return false;
	}
	else{
		*s = p;
		return true;
	}
}

/*
 * スペースまたはタブをスキップする
 */
static bool skip_space(char** s)
{
	char* p;

	p = *s;
	while(*p <= ' ' ){
		if(*p == 0)
			break;
		p++;
	}
	*s = p;
	if(*p)
		return false;
	else
		return true;
}