echo off
rem TCB構造体のオフセット算出

rem HEWでは、相対パス指定で外部ツールを呼び出せないため、
rem バッチファイルを経由して、コンフィギュレータを呼び出している
rem
rem jsp/config/h8s-renessas/makeoffset.cがコンパイルされた
rem 結果のファイル名が、引数%1としてHEWからこのバッチファイルに渡される。
rem
rem Perlの呼び出しに失敗する場合はパスを通すこと。

echo on

perl ..\..\..\..\utils\h8-renesas\genoffset %1 > offset.inc
