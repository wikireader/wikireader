echo off
rem カーネルソースファイルのコピー

rem 関数単位でリンクするため、カーネルのソースコードに#define文を追加して
rem コピーする。

rem jsp\tools\H8S-RENESAS\libkernel\sourceから見た相対パスで指定する。

perl ../../../../utils/h8-renesas/copy_kernel_source.pl ../../../../kernel .
if errorlevel 1 exit 1
