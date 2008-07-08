/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *
 *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
 *  によって公表されている GNU General Public License の Version 2 に記
 *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: tinet_cpu_config.h,v 1.2 2007/01/05 02:10:17 honda Exp $
 */

#ifndef _TINET_CPU_CONFIG_H_
#define _TINET_CPU_CONFIG_H_

/*
 *  TCP/IP に関する定義
 */

/* TCP に関する定義 */

/*
 *  MAX_TCP_RCV_SEG: 受信セグメントサイズの最大値
 *
 *    注意: 変更すべきではない。
 */

#define MAX_TCP_RCV_SEG		(IF_MTU - (IP_HDR_SIZE + TCP_HDR_SIZE))

/*
 *  MAX_TCP_SND_SEG: 送信セグメントサイズの最大値
 *
 *    相手から MSS オプションでセグメントサイズを指定されても、
 *    この値で、セグメントサイズを制限できる。
 */

#ifdef SUPPORT_PPP

#define MAX_TCP_SND_SEG		(256 - ((IF_IP_TCP_HDR_SIZE + 3) >> 2 << 2))
/*#define MAX_TCP_SND_SEG		(IF_MTU - (IP_HDR_SIZE + TCP_HDR_SIZE))*/
/*#define MAX_TCP_SND_SEG		TCP_MSS*/

#endif	/* of #ifdef SUPPORT_PPP */

#ifdef SUPPORT_ETHER

/*#define MAX_TCP_SND_SEG		(1024 - ((IF_IP_TCP_HDR_SIZE + 3) >> 2 << 2))*/
#define MAX_TCP_SND_SEG		(IF_MTU - (IP_HDR_SIZE + TCP_HDR_SIZE))
/*#define MAX_TCP_SND_SEG		TCP_MSS*/

#endif	/* of #ifdef SUPPORT_ETHER */

#ifdef SUPPORT_LOOP

#define MAX_TCP_SND_SEG		(IF_MTU - (IP_HDR_SIZE + TCP_HDR_SIZE))

#endif	/* of #ifdef SUPPORT_LOOP */

/*
 *  DEF_TCP_SND_SEG: 送信セグメントサイズの規定値
 *  DEF_TCP_RCV_SEG: 受信セグメントサイズの規定値
 */

#ifdef SUPPORT_PPP

#define DEF_TCP_SND_SEG		MAX_TCP_SND_SEG

#define DEF_TCP_RCV_SEG		(256 - ((IF_IP_TCP_HDR_SIZE + 3) >> 2 << 2))
/*#define DEF_TCP_RCV_SEG		MAX_TCP_RCV_SEG*/

#endif	/* of #ifdef SUPPORT_PPP */

#ifdef SUPPORT_ETHER

#define DEF_TCP_SND_SEG		MAX_TCP_SND_SEG

/*#define DEF_TCP_RCV_SEG		(1024 - ((IF_IP_TCP_HDR_SIZE + 3) >> 2 << 2))*/
#define DEF_TCP_RCV_SEG		(IF_MTU - (IP_HDR_SIZE + TCP_HDR_SIZE))
/*#define DEF_TCP_RCV_SEG		TCP_MSS*/

#endif	/* of #ifdef SUPPORT_ETHER */

#ifdef SUPPORT_LOOP

#define DEF_TCP_SND_SEG		MAX_TCP_SND_SEG
#define DEF_TCP_RCV_SEG		(IF_MTU - (IP_HDR_SIZE + TCP_HDR_SIZE))

#endif	/* of #ifdef SUPPORT_LOOP */

/* 
 *  セグメントの順番を入れ替えるときに新たにネットワークバッファを割当てて、
 *  データをコピーするサイズのしきい値
 */
#define MAX_TCP_REALLOC_SIZE	1024	

#define TMO_TCP_GET_NET_BUF	1000	/* [ms]、net_buf 獲得タイムアウト		*/
#define TMO_TCP_OUTPUT		1000	/* [ms]、TCP 出力タイムアウト			*/

#define TCP_CFG_OPT_MSS		/* コネクション開設時に、セグメントサイズオプションをつけて送信する。*/
#define TCP_CFG_DELAY_ACK	/* ACK を遅らせるときはコメントを外す。			*/
#define TCP_CFG_ALWAYS_KEEP	/* 常にキープアライブする時はコメントを外す。		*/

/*
 *  出力タスク
 */

#define	TCP_OUT_TASK_STACK_SIZE	1024	/* TCP 出力タスクのスタックサイズ		*/
#define TCP_OUT_TASK_PRIORITY	5	/* TCP 出力タスクの優先度			*/

/* UDP に関する定義 */

					/* UDP 受信キューサイズ				*/
#define NUM_DTQ_UDP_RCVQ	(NUM_MPF_NET_BUF_IF_PDU-1)	/* 非コールバック用	*/
#define NUM_DTQ_UDP_CB_RCVQ	(NUM_DTQ_UDP_RCVQ+1)		/* コールバック用	*/

#define TMO_UDP_OUTPUT	10000		/* [ms]、UDP 出力タイムアウト			*/

#define UDP_CFG_IN_CHECKSUM	/* UDP の入力チェックサムを行う場合はコメントを外す。	*/
#define UDP_CFG_OUT_CHECKSUM	/* UDP の出力チェックサムを行う場合はコメントを外す。	*/

/*
 *  ノンブロッキングコールを行う場合に組み込むタスクとデータキュー
 */

#define NUM_DTQ_UDP_OUTPUT	1	/* UDP 出力データキューサイズ			*/
#define	UDP_OUT_TASK_STACK_SIZE	1024	/* UDP 出力タスクのスタックサイズ			*/
#define UDP_OUT_TASK_PRIORITY	5	/* UDP 出力タスクの優先度				*/

/* ICMPv4/v6 に関する定義 */

#define ICMP_REPLY_ERROR		/* ICMP エラーメッセージを送信する場合はコメントを外す。*/

#define TMO_ICMP_OUTPUT		1000	/* [ms]、ICMP 出力バッファ獲得タイムアウト	*/

/* IPv4/IPv6 共通定義 */

#define TMO_IN_REDIRECT	(10*60*1000)	/* [ms]、向け直しルーティングエントリタイムアウト	*/

/* IPv4 に関する定義 */

/*#define IP4_CFG_FRAGMENT		 データグラムの分割・再構成行う場合はコメントを外す。*/
#define NUM_IP4_FRAG_QUEUE	2	/* データグラム再構成キューサイズ			*/
#define TMO_IP4_FRAG_GET_NET_BUF	1000	/* [ms]、net_buf 獲得タイムアウト		*/

/* IPv6 に関する定義 */

#ifdef SUPPORT_ETHER

#define IP6_CFG_AUTO_LINKLOCAL		/* リンクローカルアドレスの自動設定を行う場合はコメントを外す。*/

#endif	/* of #ifdef SUPPORT_ETHER */

#define NUM_IP6_DAD_COUNT	1	/* 重複アドレス検出で送信する近隣要請の回数、		*/
					/*  0 を指定すると、重複アドレス検出を行わない。	*/
#define NUM_ND6_CACHE_ENTRY	10	/* 近隣キャッシュのエントリ数			*/

#ifdef SUPPORT_ETHER

/*
 *  ディフォルトルータリストのエントリ数。
 *  0 を指定するとルータ通知を受信しない。
 */
#define NUM_ND6_DEF_RTR_ENTRY	2

/*
 *  起動時のルータ要請出力回数。
 *  0 を指定するとルータ要請を出力しない。
 */
#define NUM_ND6_RTR_SOL_RETRY	3	

#endif	/* of #ifdef SUPPORT_ETHER */

#define TMO_ND6_NS_OUTPUT	1000	/* [ms]、近隣要請出力タイムアウト		*/
#define TMO_ND6_NA_OUTPUT	1000	/* [ms]、近隣通知出力タイムアウト		*/
#define TMO_ND6_RS_OUTPUT	1000	/* [ms]、ルータ要請出力タイムアウト		*/
#define TMO_ND6_RTR_SOL_DELAY	1000	/* [ms]、ルータ要請出力遅延			*/
#define TMO_ND6_RTR_SOL_INTERVAL	\
				3000	/* [ms]、ルータ要請出力間隔			*/

/*
 *  データリンク層 (ネットワークインタフェース) に関する定義
 */

/*
 *  PPP に関する定義
 */

#define NUM_DTQ_PPP_OUTPUT	2	/* PPP 出力データキューサイズ、2 以上	*/

#define	PPP_INPUT_STACK_SIZE	1024	/* PPP タスクのスタックサイズ		*/
#define	PPP_OUTPUT_STACK_SIZE	1024	/* PPP 出力タスクのスタックサイズ		*/

#define PPP_INPUT_PRIORITY	5	/* PPP タスクの優先度			*/
#define PPP_OUTPUT_PRIORITY	5	/* PPP タスクの優先度			*/

#define TMO_PPP_GET_NET_BUF	1000	/* [ms]、受信用 net_buf 獲得タイムアウト	*/

/*
 *  PPP、モデムに関する定義
 */

#define MODEM_CFG_DIAL		"ATD"	/* ダイアルコマンド文字列			*/
#define MODEM_CFG_RETRY_CNT	3	/* ダイアルリトライ回数			*/
#define MODEM_CFG_RETRY_WAIT	10000	/* ダイアルリトライまでの待ち時間 [ms]	*/

/*
 *  PPP、HDLC に関する定義
 */

#define DEF_LOCAL_ACCM		0x000a0000	/* 自分の ACCM、XON と XOFF のみ変換	*/
#define DEF_REMOTE_ACCM		0xffffffff	/* 相手の ACCM、初期値は全て変換		*/

/*
 *  PPP、LCP に関する定義
 */

#define LCP_CFG_MRU		0x0001	/* MRU					*/
#define LCP_CFG_ACCM		0x0002	/* ACCM					*/
#define LCP_CFG_MAGIC		0x0004	/* マジック番号				*/
#define LCP_CFG_PCOMP		0x0008	/* プロトコル部圧縮機能			*/
#define LCP_CFG_ACCOMP		0x0010	/* アドレス・制御部圧縮			*/
#define LCP_CFG_PAP		0x0020	/* PAP					*/
/*#define LCP_CFG_CHAP		0x0040	   CHAP は実装予定			*/

#ifdef LCP_CFG_MAGIC

#define LCP_ECHO_INTERVAL	(20*NET_TIMER_HZ)	/* インターバル時間		*/
#define LCP_ECHO_FAILS		9			/* 失敗閾値			*/

#endif	/* of #ifdef LCP_CFG_MAGIC */

/*
 *  PPP、PAP に関する定義
 */

#define DEF_PAP_TIMEOUT		(3*NET_TIMER_HZ)
#define DEF_PAP_REQTIME		(30*NET_TIMER_HZ)	/* タイムアウト処理を行うときはコメントを外す。*/
#define MAX_PAP_REXMT		10			/* 認証要求の最大再送回数	*/

/*
 *  ARP に関する定義
 */

#define NUM_ARP_ENTRY		10			/* ARP キャッシュエントリ数	*/
#define ARP_CACHE_KEEP		(20*60*NET_TIMER_HZ)	/* ARP キャッシュのタイムアウト	*/
#define TMO_ARP_GET_NET_BUF	1000	/* [ms]、アドレス解決要求用 net_buf 獲得タイムアウト*/
#define TMO_ARP_OUTPUT		1000	/* [ms]、アドレス解決要求出力タイムアウト	*/

/*
 *  DHCP に関する定義
 *
 *    ・注意: TINET は、DHCP を実装していない。応用プログラムで、
 *            DHCP メッセージを受信するための定義である。
 *            また、現在は IPv4 のみ有効である。
 */

/*#define DHCP_CFG					 DHCP を処理する場合はコメントを外す。*/

/*
 *  Ethernet に関する定義
 */

#define NUM_DTQ_ETHER_OUTPUT	2	/* Ethernet 出力データキューサイズ	*/

#define	ETHER_INPUT_STACK_SIZE	1024	/* Ethernet 入力タスクのスタックサイズ	*/
#define	ETHER_OUTPUT_STACK_SIZE	1024	/* Ethernet 出力タスクのスタックサイズ	*/

#define ETHER_INPUT_PRIORITY	5	/* Ethernet 入力タスクの優先度		*/
#define ETHER_OUTPUT_PRIORITY	5	/* Ethernet 出力タスクの優先度		*/

/*#define ETHER_CFG_ACCEPT_ALL		 マルチキャスト、エラーフレームも受信するときはコメントを外す。	*/
/*#define ETHER_CFG_UNEXP_WARNING	 非サポートフレームの警告を表示するときはコメントを外す。		*/
/*#define ETHER_CFG_802_WARNING		 IEEE 802.3 フレームの警告を表示するときはコメントを外す。		*/
/*#define ETHER_CFG_MCAST_WARNING	 マルチキャストの警告を表示するときはコメントを外す。		*/

/*
 *  ループバックインタフェースに関する定義
 */

#define LOMTU			1500	/* ループバックの MTU			*/

#define NUM_DTQ_LOOP_INPUT	0	/* ループバック入力データキューサイズ	*/
#define NUM_DTQ_LOOP_OUTPUT	0	/* ループバック出力データキューサイズ	*/

#define	LOOP_INPUT_STACK_SIZE	1024	/* ループバック入力タスクのスタックサイズ	*/
#define	LOOP_OUTPUT_STACK_SIZE	1024	/* ループバック出力タスクのスタックサイズ	*/

#define LOOP_INPUT_PRIORITY	5	/* ループバック入力タスクの優先度		*/
#define LOOP_OUTPUT_PRIORITY	5	/* ループバック出力タスクの優先度		*/

/*
 *  ネットワークに関する定義
 */

/* ネットワークタイマタスク */

#define	NET_TIMER_STACK_SIZE	1024	/* ネットワークタイマタスクのスタックサイズ */
#define NET_TIMER_PRIORITY	5	/* ネットワークタイマタスクの優先度 */

#define NUM_NET_CALLOUT		10	/* タイムアウト呼出し数、10 以上	*/

/* net_buf 数 */

#ifdef SUPPORT_PPP

/*
 *  PPP では、受信用の net_buf は PDU (1502) サイズを常に
 *  割り当てなければならないので PDU サイズを多めに確保する。
 *  128 サイズの net_buf は、TCP セグメント用に多めに確保する。
 */

#define NUM_MPF_NET_BUF_128	2	/* net_buf 数、128、2 以上	*/
#define NUM_MPF_NET_BUF_256	4	/* net_buf 数、256		*/
#define NUM_MPF_NET_BUF_512	1	/* net_buf 数、512		*/
#define NUM_MPF_NET_BUF_1024	1	/* net_buf 数、1024		*/
#define NUM_MPF_NET_BUF_IF_PDU	1	/* net_buf 数、PDU		*/

#if defined(SUPPORT_INET4) && defined(IP4_CFG_FRAGMENT)

#ifndef NUM_MPF_NET_BUF_4096
#define NUM_MPF_NET_BUF_4096	2	/* net_buf 数、4096		*/
#endif	/* of #ifndef NUM_MPF_NET_BUF_4096 */

#endif	/* of #if defined(SUPPORT_INET4) && defined(IP4_CFG_FRAGMENT) */

#endif	/* of #ifdef SUPPORT_PPP */

#ifdef SUPPORT_ETHER

/*
 *  イーサネットでは、受信用の net_buf は、NIC のバッファにある
 *  フレーム長を見て割り当てればよい。
 *  TCP の最大セグメントサイズは 1024 オクテットの net_buf に調整する場合は、
 *  このサイズの net_buf は多めに確保する。
 *  128 サイズの net_buf も、TCP の制御セグメント用に多めに確保する。
 */

#ifndef NUM_MPF_NET_BUF_128
#define NUM_MPF_NET_BUF_128	4	/* net_buf 数、128、2 以上	*/
#endif	/* of #ifndef NUM_MPF_NET_BUF_128 */

#ifndef NUM_MPF_NET_BUF_256
#define NUM_MPF_NET_BUF_256	2	/* net_buf 数、256		*/
#endif	/* of #ifndef NUM_MPF_NET_BUF_256 */

#ifndef NUM_MPF_NET_BUF_512
#define NUM_MPF_NET_BUF_512	2	/* net_buf 数、512		*/
#endif	/* of #ifndef NUM_MPF_NET_BUF_512 */

#ifndef NUM_MPF_NET_BUF_1024
#define NUM_MPF_NET_BUF_1024	2	/* net_buf 数、1024		*/
#endif	/* of #ifndef NUM_MPF_NET_BUF_1024 */

#ifndef NUM_MPF_NET_BUF_IF_PDU
#define NUM_MPF_NET_BUF_IF_PDU	4	/* net_buf 数、PDU		*/
#endif	/* of #ifndef NUM_MPF_NET_BUF_IF_PDU */

#if defined(SUPPORT_INET4) && defined(IP4_CFG_FRAGMENT)

#ifndef NUM_MPF_NET_BUF_4096
#define NUM_MPF_NET_BUF_4096	2	/* net_buf 数、4096		*/
#endif	/* of #ifndef NUM_MPF_NET_BUF_4096 */

#endif	/* of #if defined(SUPPORT_INET4) && defined(IP4_CFG_FRAGMENT) */

#endif	/* of #ifdef SUPPORT_ETHER */

#ifdef SUPPORT_LOOP

#define NUM_MPF_NET_BUF_128	2	/* net_buf 数、128、2 以上	*/
#define NUM_MPF_NET_BUF_256	2	/* net_buf 数、256		*/
#define NUM_MPF_NET_BUF_512	2	/* net_buf 数、512		*/
#define NUM_MPF_NET_BUF_1024	2	/* net_buf 数、1024		*/
#define NUM_MPF_NET_BUF_IF_PDU	2	/* net_buf 数、PDU		*/

#endif	/* of #ifdef SUPPORT_PPP */

/*
 *  ネットワーク統計情報の計測
 *
 *  ネットワーク統計情報の計測を行う場合は、tinet/include/net/net.h
 *  で定義されているプロトコル識別フラグを指定する。
 */

#if 1

#ifdef SUPPORT_INET4

#define NET_COUNT_ENABLE	(0			\
				| PROTO_FLG_PPP_HDLC	\
				| PROTO_FLG_PPP_PAP	\
				| PROTO_FLG_PPP_LCP	\
				| PROTO_FLG_PPP_IPCP	\
				| PROTO_FLG_PPP		\
				| PROTO_FLG_LOOP	\
				| PROTO_FLG_ETHER_NIC	\
				| PROTO_FLG_ETHER	\
				| PROTO_FLG_ARP		\
				| PROTO_FLG_IP4		\
				| PROTO_FLG_ICMP4	\
				| PROTO_FLG_UDP		\
				| PROTO_FLG_TCP		\
				| PROTO_FLG_NET_BUF	\
				)

#endif	/* of #ifdef SUPPORT_INET4 */

#ifdef SUPPORT_INET6

#define NET_COUNT_ENABLE	(0			\
				| PROTO_FLG_PPP_HDLC	\
				| PROTO_FLG_PPP_PAP	\
				| PROTO_FLG_PPP_LCP	\
				| PROTO_FLG_PPP_IPCP	\
				| PROTO_FLG_PPP		\
				| PROTO_FLG_LOOP	\
				| PROTO_FLG_ETHER_NIC	\
				| PROTO_FLG_ETHER	\
				| PROTO_FLG_IP6		\
				| PROTO_FLG_ICMP6	\
				| PROTO_FLG_ND6		\
				| PROTO_FLG_UDP		\
				| PROTO_FLG_TCP		\
				| PROTO_FLG_NET_BUF	\
				)

#endif	/* of #ifdef SUPPORT_INET6 */

#else	/* of #if 0 */

#define NET_COUNT_ENABLE	(0			\
				)

#endif	/* of #if 0 */

#endif /* _TINET_CPU_CONFIG_H_ */
