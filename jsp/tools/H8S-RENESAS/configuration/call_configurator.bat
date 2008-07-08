echo off
rem システム・コンフィギュレーションとベクタテーブルの自動生成

rem HEWでは、相対パス指定で外部ツールを呼び出せないため、
rem バッチファイルを経由して、コンフィギュレータを呼び出している
rem
rem システム・コンフィギュレーション・ファイルがプリプロセスされた
rem 結果のファイル名が、引数%1としてHEWからこのバッチファイルに渡される。

rem 以下の部分は移植の際に変更する必要がある
rem システム依存部のディレクトリ名
rem cfg.exe -s %1 -c -obj -cpu h8 -system xxxxxxxx

rem ベクタテーブルのサイズ
rem genvector.pl -s xxx

rem HEWでは、同じファイル名でパスのみ異なるソースファイルをコンフィ
rem ギュレーション(Debug/Release)によって、ビルド対象を使い分けること
rem ができない。
rem これを回避するため、vector.srcをdebug_vector.srcとrelease_vector.
rem srcにコピーし、一度、両方をプロジェクトに登録し、コンフィギュレー
rem ション毎にいらないファイルをビルド対象から除外する。
rem kernel_cfg.cについても同様

rem Perlの呼び出しに失敗する場合はパスを通すこと。

echo on

..\..\..\..\cfg\cfg.exe -s %1 -c -obj -cpu h8s -system minet_h8s_2339f
type kernel_cfg.c > debug_kernel_cfg.c
type kernel_cfg.c > release_kernel_cfg.c
del kernel_cfg.c

perl ..\..\..\..\utils\h8\grep_def_inh.pl %1 > tmp_script.pl
type ..\..\..\..\config\h8s-renesas\vector_header.src > vector.src
perl ..\..\..\..\utils\h8-renesas\genvector.pl -s 92 >> vector.src
perl ..\..\..\..\utils\h8s-renesas\gen_intentry.pl >> vector.src
del tmp_script.pl
type vector.src > debug_vector.src
type vector.src > release_vector.src
del vector.src

