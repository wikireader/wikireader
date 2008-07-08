/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2005 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 */

/*
 *  EVAデバッガ用機種判別情報
 *  (このsectionは0x7f00-0x7fffにリンクされる)
 */

#define CHIP_NAME "LC59_32K"
#define RAM_SIZE 0x7E00
#define ROM_SIZE 0x78000
#define VRAM_SIZE 0x0000
#define VRAM_ROW 0
#define VRAM_COLUMN 0
#define CGROM_SIZE 0x0000
#define BCD(x)  (((x)/10)%10)*0x10+((x)%10)
#define BCD4(x) BCD((x)/100),BCD(x)
#define BCD6(x) BCD((x)/10000),BCD((x)/100),BCD(x)

        .section .chip_info,"a"
        .global _chip_info
_chip_info:
        .space  0xc0,0x00
        .ascii  "CHIPINFORMATION"               /* C0-CF : fixed string */
1:      .space  (0xd0-1b),0x00
        .ascii  CHIP_NAME                       /* D0-DF : chip name */
2:      .space  (0xe0-2b),0x00
        .byte   0x10, 0x00                      /* E0-E1 : format version */
        .space  6, 0x00
        .byte   BCD4(ROM_SIZE/1024)             /* E8-F5 : Memory sizes */
        .byte   BCD6(RAM_SIZE)
        .byte   BCD6(VRAM_SIZE)
        .byte   BCD4(VRAM_ROW)
        .byte   BCD4(VRAM_COLUMN)
        .byte   BCD4(CGROM_SIZE/1024)
        .space  3, 0x00
        .byte   0x01                            /* F9 : package type */
        .space  6, 0x00

        .end
