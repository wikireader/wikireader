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

rem システム・コンフィギュレーション
..\..\..\..\cfg\cfg.exe -s %1 -c -obj -cpu h8 -system hsb8f3048bf25
if errorlevel 1 exit 1
type kernel_cfg.c > debug_kernel_cfg.c
type kernel_cfg.c > release_kernel_cfg.c
del kernel_cfg.c

rem ベクタテーブルの自動生成
perl ..\..\..\..\utils\h8\grep_def_inh.pl %1 > tmp_script.pl
if errorlevel 1 exit 1
type ..\..\..\..\config\h8-renesas\vector_header.src > vector.src
perl ..\..\..\..\utils\h8-renesas\genvector.pl -s 61 >> vector.src
if errorlevel 1 exit 1
perl ..\..\..\..\utils\h8-renesas\gen_intentry.pl >> vector.src
if errorlevel 1 exit 1
del tmp_script.pl
type vector.src > debug_vector.src
type vector.src > release_vector.src
del vector.src
