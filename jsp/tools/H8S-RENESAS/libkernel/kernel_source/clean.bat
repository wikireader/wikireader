echo off
rem 中間ファイルを削除するバッチファイル
rem
rem　　このスクリプト自体はビルドには使用されない。
rem　　このファイルをアーカイブに含めることにより、
rem　　kernel_sourceディレクトリのアーカイブ漏れを防ぐ。

del *.c
