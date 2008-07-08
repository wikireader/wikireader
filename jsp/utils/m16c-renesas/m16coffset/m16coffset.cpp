/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RIOCH COMPANY,LTD. JAPAN
 *  Copyright (C) 2007 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
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
 *  @(#) $Id: m16coffset.cpp,v 1.2 2007/03/23 07:12:10 honda Exp $
 */
// m16coffset.cpp : コンソール アプリケーション用のエントリ ポイントの定義
//

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define	BUF_SIZE		512
#define	STR_SIZE		128

#define	NORMAL_STATE	0
#define	CHECK_STATE		1

struct OFF {
	int		bit;
	int		offset;
	int 	check_type; /* OFF=0 or BIT=1 */
	char	label[STR_SIZE];
	char	mask[STR_SIZE];
};

static char const version[] = "02.00";
static char const default_input_file[] = "makeoffset.a30";
static char const default_output_file[] = "offset.inc";

static char input_file[STR_SIZE];
static char output_file[STR_SIZE];
static int  num_offset = 0;
static int  offset_state = NORMAL_STATE;
static struct OFF off_table[20];

static bool test_string(char** s, char const * t);
static bool skip_space(char** s);

int main(int argc, char* argv[])
{
	FILE* pfo;
	FILE* pfi;
	struct OFF *v = NULL;
	char* s;
	char* p;
	char  buf[BUF_SIZE];
	char  c;
	int i;
	int val;
	int ofs = 0;
	char ct[4]; /* "BIT" or "OFF" */

	strcpy(input_file, default_input_file);
	strcpy(output_file, default_output_file);

	for(i = 1 ; i < argc ; i++){
		s = argv[i];
		if(*s++ == '-'){
			c = *s;
			while(*s != 0){
				if(*s == ':')
					break;
				s++;
			}
			if(*s++ == ':'){
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
					printf("m16coffset -R:input_file -O:output_file\n");
					break;
				}
			}
		}
	}
	printf("version      = %s\n", version);
	printf("input file   = %s\n", input_file);
	printf("output file  = %s\n", output_file);

	if((pfi = fopen(input_file, "r")) == NULL){
		fprintf(stderr, "can't open input file!");
		exit(1);
	}
	if((pfo = fopen(output_file, "w")) == NULL){
		fclose(pfi);
		fprintf(stderr, "can't open output file!");
		exit(1);
	}

	/* 一行入力 → 解析 */
	for(;;){
		if((fgets(buf, BUF_SIZE, pfi)) == NULL)	break;
		s = buf;

		switch(offset_state){
		  case CHECK_STATE:
			if(skip_space(&s)) /* 行頭の空白を除く */
				continue;

			/* オフセット */
			if(test_string(&s, ".word")){
				ofs = 2;
			}
			else if(test_string(&s, ".lword")){
				ofs = 4;
			}
			else if(test_string(&s, ".byte")){
				ofs = 1;
			}
			else if(test_string(&s, ".addr")){
				ofs = 3;
			}
			else {
				continue;
			}

			if(skip_space(&s))
				continue;

			sscanf(s, "%x", &val);
			if(val){
				offset_state = NORMAL_STATE;
				if(v->check_type) {
					sprintf(v->mask, "%xH", val);
					for(v->bit=0; val>1; v->bit++) {
						val /= 2;
					}
				}
				continue;
			}
			v->offset += ofs;
			break;
		  default:
			if(!test_string(&s, "_____BEGIN_")) {
				continue;
			}
			v = &off_table[num_offset++];

			for(i = 0, p = ct; i < 4 && isalpha(*s) && *s != '_'; i++){
				*p++ = *s++;
			}
			*p++ = 0, *s++;
			if(strncmp(ct, "BIT", sizeof("BIT")) == 0) {
				v->check_type = 1;
			}

			p = v->label;
			while(*s > ':')
				*p++ = *s++;
			*p++ = 0;
			offset_state = CHECK_STATE;
			break;
		}
	}

	/* 出力 */
	for(i = 0 ; i < num_offset ; i++){
		v = &off_table[i];
		fprintf(pfo, "%s\t.equ\t%d\n", v->label, v->offset);
		printf("%s\t.equ\t%d\n", v->label, v->offset);
		if(v->check_type) {
			fprintf(pfo, "%s_bit\t.equ\t%d\n", v->label, v->bit);
			fprintf(pfo, "%s_mask\t.equ\t%s\n", v->label, v->mask);
			printf("%s_bit\t.equ\t%d\n", v->label, v->bit);
			printf("%s_mask\t.equ\t%s\n", v->label, v->mask);
		}
	}
	fclose(pfi);
	fclose(pfo);

	return 0;
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
