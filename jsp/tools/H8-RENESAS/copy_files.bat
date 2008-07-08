echo off
rem 注意　HEWでビルドする前に、このバッチファイルを予め実行しておく。

rem 依存部と非依存部のソースファイルを
rem jsp\tools\H8-RENESAS\libkernel\kernel_sourceにコピー

cd libkernel\kernel_source
xcopy /Q /I /D /S ..\..\..\..\config\h8-renesas h8-renesas
xcopy /Q /I /D ..\..\..\..\kernel kernel
xcopy /Q /I /D ..\..\..\..\library library
xcopy /Q /I /D ..\..\..\..\systask systask

rem カーネルソースファイルのコピー
rem 関数単位でリンクするため、カーネルのソースコードに#define文を追加して
rem コピーする。

mkdir kernel_api
perl ../../../../utils/h8-renesas/copy_kernel_source.pl ../../../../kernel kernel_api
cd ..\..
