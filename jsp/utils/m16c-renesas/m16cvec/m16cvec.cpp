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
 *  @(#) $Id: m16cvec.cpp,v 1.3 2004/09/03 17:33:49 honda Exp $
 */
// m16cvec.cpp : コンソール アプリケーション用のエントリ ポイントの定義
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

#include "stdafx.h"
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

static char const version[] = "01.01";
static char const default_input_file[] = "kernel_cfg.i";
static char const default_output_file[] = "m16cvec.inc";
static char const default_unused_int[] = "unused_interrupt";
static char const in_portid[] = "serial_in_portid";
static char const out_portid[] = "serial_out_portid";
static char const in_handler[] = "serial_in_handler_xyzx";
static char const out_handler[] = "serial_out_handler_xyzx";
static char const comment[] = ";************************************************";
static char const int_vec[] = ";* interrupt vectors                            *";
static char const exc_vec[] = ";* exception vectors                            *";
static char const func01[] = "\t.section bss_NE,DATA,ALIGN";
static char const func02[] = "\t.blkw\t1";
static char const func03[] = "\t.section program, code, align";
static char const lf[] = "\n";
static char const default_fvector[] = "0fffdch";

static int  board_mode = 0;
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

static void set_M30620FCAFP(FILE* pfo, int no, int m);
static void set_M30262F8FG(FILE* pfo, int no, int m);
static void set_global(FILE* pfo, char const * t);
static void set_comment(FILE* pfo, char const * t);
static void set_org(FILE* pfo, char const * t);
static void set_vector(FILE* pfo, int kind, int no);
static void set_vector2(FILE* pfo, int kind, int no);
static bool test_string(char** s, char const * t);
static bool skip_space(char** s);
static bool skip_char(char** s, char const c);
static char fvector[STR_SIZE];

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
	strcpy(fvector,default_fvector);

	for(i = 1 ; i < argc ; i++){
		s = argv[i];
		if(*s++ == '-'){
			c = *s++;
			skip_space(&s);
			switch(c){
			case 'I':		// インクルードファイルの設定
			case 'i':
				if(num_include < MAX_INCLUDE){
					strcpy(include_file[num_include], s);
					num_include++;
				}
				break;
			case 'F':		// フィックスベクタの定義
			case 'f':
				strcpy(fvector, s);
				break;
			case 'O':		// アウトプットファイルの設定
			case 'o':
				strcpy(output_file, s);
				break;
			case 'R':		// インプットファイルの設定
			case 'r':
				strcpy(input_file, s);
				break;
			case 'M':		// ボードモード
			case 'm':
				board_mode = atoi(s);
				break;
			default:
				printf("m16cvec -R<input_file> -O<output_file> -I<include_file> -M<mode> -F<fixvector>\n");
				break;
			}
		}
	}
	printf("version      = %s\n", version);
	printf("input file   = %s\n", input_file);
	printf("output file  = %s\n", output_file);
	printf("Fix Vector   = %s\n", fvector);
	printf("board mode   = %d : ", board_mode);
	if(board_mode == 1)
		printf("M30262F8FG(OAKS16 MINI)\n");
	else
		printf("M30620FCAFP(OAKS16)\n");
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
	for(;;){
		if((fgets(buf, BUF_SIZE, pfi)) == NULL)
			break;
		s = buf;
		switch(vec_state){
		case INT_STATE:
		case EXC_STATE:
			if(test_string(&s, ";"))
				vec_state = NORMAL_STATE;
			else if(test_string(&s, "{")){
				v = &vec_table[vec_state][num_vec[vec_state]];
				p = &v->no_name[0];
				if(skip_space(&s))
					continue;
				dec = true;
				while(*s != ','){
					if(*s == 0)
						continue;
					if(*s == '(' || *s == ')' || *s <= ' '){
						s++;
						continue;
					}
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
				test_string(&s, "INT_ENTRY(");
				test_string(&s, "EXC_ENTRY(");
				test_string(&s, "CFG_INT_ENTRY(");
				test_string(&s, "CFG_EXC_ENTRY(");
				test_string(&s, "(FP)");
				if(skip_space(&s))
					continue;
				while(*s != ')' && *s != ',' && *s > ' '){
					*p++ = *s++;
				}
				*p++ = 0;
				num_vec[vec_state]++;
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
		printf("int %d:%d,%s,%s\n", j, v->no, v->no_name, v->no_label);
	}
	for(j = 0 ; j < num_vec[EXC_STATE] ; j++){
		v = &vec_table[EXC_STATE][j];
		if((v->no >= MAX_EXC && v->no < 32) || v->no >= MAX_INT)
			strcpy(unused_vec[EXC_STATE], v->no_label);
		printf("exc %d:%d,%s,%s\n", j, v->no, v->no_name, v->no_label);
	}
	if(num_nodec > 0)
		printf("%dのエクセプション番号を特定できません！\n", num_nodec);
	else{
		fputs(lf, pfo);
		for(i = 0 ; i < 2 ; i++){
			for(j = 0 ; j < num_vec[i] ; j++)
				set_global(pfo, vec_table[i][j].no_label);
		}
		if(!strcmp(default_unused_int, unused_vec[0]))
			set_global(pfo, unused_vec[0]);
		else if(!strcmp(default_unused_int, unused_vec[1]))
			set_global(pfo, unused_vec[1]);

		switch(board_mode){
		case 1:				// M30262F8FG(OAKS16 MINI)
			set_M30262F8FG(pfo, board_mode, max_int);
			break;
		default:			// M30620FCAFP(OAKS16)
			set_M30620FCAFP(pfo, board_mode, max_int);
			break;
		}

		set_comment(pfo, exc_vec);
		fputs("\t.section\tfvector", pfo);
		fputs(lf, pfo);
		set_org(pfo, fvector);

		for(i = 0 ; i < (MAX_EXC-1) ; i++)
			set_vector(pfo, EXC_STATE, i);
		fputs("\t.lword\t_hardware_start\t\t\t; RESET", pfo);
		fputs(lf, pfo);
	}
	fclose(pfi);
	fclose(pfo);
	return 0;
}

/*
 * set_M30620FCAFP(OAKS16)用のベクトルを設定する
 */
static void set_M30620FCAFP(FILE* pfo, int no, int m)
{
#ifdef OAKS16_JSP13
	struct VEC *v;
	int x;
#endif /* OAKS16_JSP13 */
	int i;

#ifdef OAKS16_JSP13
	if(no == 2){
		set_global(pfo, in_portid);
		set_global(pfo, out_portid);
		fputs(lf, pfo);
		fputs(func01, pfo);
		fputs(lf, pfo);
		fputs("_", pfo);
		fputs(in_portid, pfo);
		fputs(":", pfo);
		fputs(lf, pfo);
		fputs(func02, pfo);
		fputs(lf, pfo);
		fputs("_", pfo);
		fputs(out_portid, pfo);
		fputs(":", pfo);
		fputs(lf, pfo);
		fputs(func02, pfo);
		fputs(lf, pfo);
		fputs(lf, pfo);
		v = &vec_table[0][0];
		for(i = x = 0 ; i < num_vec[0] ; i++, v++){
			if(v->no >= 17 && v->no < 21){
				x = 1;
				break;
			}
		}
		if(x){
			fputs(func03, pfo);
			fputs(lf, pfo);
			v = &vec_table[0][0];
			for(i = x = 0 ; i < num_vec[0] ; i++, v++){
				if(v->no == 17 || v->no == 19){
					fputs(out_handler, pfo);
					fputs("1:", pfo);
					fputs(lf, pfo);
					fputs("\tmov.w\t#1, _", pfo);
					fputs(out_portid, pfo);
					fputs(lf, pfo);
					fputs("\tjmp\t_", pfo);
					fputs(v->no_label, pfo);
					fputs(lf, pfo);
					fputs(out_handler, pfo);
					fputs("2:", pfo);
					fputs(lf, pfo);
					fputs("\tmov.w\t#2, _", pfo);
					fputs(out_portid, pfo);
					fputs(lf, pfo);
					fputs("\tjmp\t_", pfo);
					fputs(v->no_label, pfo);
					fputs(lf, pfo);
					break;
				}
			}
			v = &vec_table[0][0];
			for(i = x = 0 ; i < num_vec[0] ; i++, v++){
				if(v->no == 18 || v->no == 20){
					fputs(in_handler, pfo);
					fputs("1:", pfo);
					fputs(lf, pfo);
					fputs("\tmov.w\t#1, _", pfo);
					fputs(in_portid, pfo);
					fputs(lf, pfo);
					fputs("\tjmp\t_", pfo);
					fputs(v->no_label, pfo);
					fputs(lf, pfo);
					fputs(in_handler, pfo);
					fputs("2:", pfo);
					fputs(lf, pfo);
					fputs("\tmov.w\t#2, _", pfo);
					fputs(in_portid, pfo);
					fputs(lf, pfo);
					fputs("\tjmp\t_", pfo);
					fputs(v->no_label, pfo);
					fputs(lf, pfo);
					break;
				}
			}
		}
	}
#endif /* OAKS16_JSP13 */

	set_comment(pfo, int_vec);
	fputs("\t.section\tvvector", pfo);
	fputs(lf, pfo);
	set_org(pfo, "VECTOR_ADR");
	for(i = 0 ; i < MAX_INT ; i++){
		if(no == 2){
			if(i >= 17 && i < 21)
				set_vector2(pfo, INT_STATE, i);
			else
				set_vector(pfo, INT_STATE, i);
		}
		else
			set_vector(pfo, INT_STATE, i);
	}
}

/*
 * M30262F8FG(OAKS16 MINI)のベクトルを設定する
 */
static void set_M30262F8FG(FILE* pfo, int no, int m)
{
	int i;

	set_comment(pfo, int_vec);
	fputs("\t.section\tvector", pfo);
	fputs(lf, pfo);
	set_org(pfo, "VECTOR_ADR");
	fputs(";", pfo);
	fputs(lf, pfo);
	set_vector(pfo, INT_STATE, 0);
	set_org(pfo, "(VECTOR_ADR+44)");
	fputs(lf, pfo);
	for(i = 11 ; i < 15 ; i++)
		set_vector(pfo, INT_STATE, i);
	set_org(pfo, "(VECTOR_ADR+68)");
	fputs(lf, pfo);
	if(m < MAX_INT)
		m = MAX_INT;
	for(i = 17 ; i < m ; i++)
		set_vector(pfo, INT_STATE, i);
}

/*
 * グローバル宣言を設定する
 */
static void set_global(FILE* pfo, char const * t)
{
	fputs("\t.glb\t_", pfo);
	fputs(t, pfo);
	fputs(lf, pfo);
}

/*
 * コメントを設定する
 */
static void set_comment(FILE* pfo, char const * t)
{
	fputs(";", pfo);
	fputs(lf, pfo);
	fputs(comment, pfo);
	fputs(lf, pfo);
	fputs(int_vec, pfo);
	fputs(lf, pfo);
	fputs(comment, pfo);
	fputs(lf, pfo);
	fputs(";", pfo);
	fputs(lf, pfo);
}

/*
 * オリジンを設定する
 */
static void set_org(FILE* pfo, char const * t)
{
	fputs("\t.org\t", pfo);
	fputs(t, pfo);
	fputs(lf, pfo);
}

/*
 * ベクトルを設定する
 */
static void set_vector(FILE* pfo, int kind, int no)
{
	struct VEC* v = &vec_table[kind][0];
	char s[6];
	int  i;

	fputs("\t.lword\t_", pfo);
	s[0] = '\t';
	s[1] = ';';
	s[2] = ' ';
	s[3] = (no/10)+'0';
	s[4] = (no%10)+'0';
	s[5] = 0;
	for(i = 0 ; i < num_vec[kind] ; i++, v++){
		if(v->no == no){
			fputs(v->no_label, pfo);
			fputs(s, pfo);
			fputs(lf, pfo);
			return;
		}
	}
	fputs(unused_vec[kind], pfo);
	fputs(s, pfo);
	fputs(lf, pfo);
}

/*
 * ベクトルを設定する(特別版)
 */
static void set_vector2(FILE* pfo, int kind, int no)
{
	struct VEC* v = &vec_table[kind][0];
	char s[8];
	int  i;

	s[1] = '\t';
	s[2] = ';';
	s[3] = ' ';
	s[4] = (no/10)+'0';
	s[5] = (no%10)+'0';
	s[6] = 0;
	for(i = 0 ; i < num_vec[kind] ; i++, v++){
		switch(no){
		case 17:
			s[0] = '1';
			if(v->no == 17 || v->no == 19){
				fputs("\t.lword\t", pfo);
				fputs(out_handler, pfo);
				fputs(s, pfo);
				fputs(lf, pfo);
				return;
			}
			break;
		case 18:
			s[0] = '1';
			if(v->no == 18 || v->no == 20){
				fputs("\t.lword\t", pfo);
				fputs(in_handler, pfo);
				fputs(s, pfo);
				fputs(lf, pfo);
				return;
			}
			break;
		case 19:
			s[0] = '2';
			if(v->no == 17 || v->no == 19){
				fputs("\t.lword\t", pfo);
				fputs(out_handler, pfo);
				fputs(s, pfo);
				fputs(lf, pfo);
				return;
			}
			break;
		case 20:
			s[0] = '2';
			if(v->no == 18 || v->no == 20){
				fputs("\t.lword\t", pfo);
				fputs(in_handler, pfo);
				fputs(s, pfo);
				fputs(lf, pfo);
				return;
			}
			break;
		default:
			break;
		}
	}
	fputs("\t.lword\t_", pfo);
	fputs(unused_vec[kind], pfo);
	fputs(s, pfo);
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
	while(*p <= ' '){
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
