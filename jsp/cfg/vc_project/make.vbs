' 
'   TOPPERS/JSP Kernel
'       Toyohashi Open Platform for Embedded Real-Time Systems/
'       Just Standard Profile Kernel
'  
'   Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
'                               Toyohashi Univ. of Technology, JAPAN
'  
'   上記著作権者は，以下の (1)～(4) の条件か，Free Software Foundation 
'   によって公表されている GNU General Public License の Version 2 に記
'   述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
'   を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
'   利用と呼ぶ）することを無償で許諾する．
'   (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
'       権表示，この利用条件および下記の無保証規定が，そのままの形でソー
'       スコード中に含まれていること．
'   (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
'       用できる形で再配布する場合には，再配布に伴うドキュメント（利用
'       者マニュアルなど）に，上記の著作権表示，この利用条件および下記
'       の無保証規定を掲載すること．
'   (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
'       用できない形で再配布する場合には，次のいずれかの条件を満たすこ
'       と．
'     (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
'         作権表示，この利用条件および下記の無保証規定を掲載すること．
'     (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
'         報告すること．
'   (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
'       害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
'  
'   本ソフトウェアは，無保証で提供されているものである．上記著作権者お
'   よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
'   含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
'   接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
'  
'   @(' ) $Id: make.vbs,v 1.11 2003/12/26 03:49:46 honda Exp $
'  

dim model
dim msdev

if msgbox("TOPPERS/JSP カーネルコンフィギュレータ および 構成チェッカを生成します．処理を開始してもよろしいですか?", vbYesno) = vbyes then

	if msgbox("リリースビルドしますか? (「いいえ」を選ぶとデバッグ可能な実行ファイルを生成します)",vbyesno) = vbyes then
		model = " - Win32 Release"
	else
		model = " - Win32 Debug"
	end if

		'Visual C++ の起動
	set msdev = CreateObject("MSDEV.APPLICATION")
	msdev.visible = true

	msdev.documents.open left(Wscript.ScriptFullName, len(Wscript.scriptfullname) - len(wscript.scriptname)) + "configurator.dsw"

		'チェッカの生成
	set msdev.activeproject = msdev.projects("checker")
	set msdev.activeconfiguration = msdev.activeproject.configurations("checker" + model)

	if msgbox("チェッカはWindows用ですか?" & vbcr & "(「いいえ」を選ぶとcygwin/MinGW用のチェッカを生成します)", vbyesno) = vbno then
		msdev.activeconfiguration.removetoolsettings "cl.exe","/D " & chr(34) & "FILECONTAINER_WINDOWS" & chr(34)
		msdev.activeconfiguration.addtoolsettings "cl.exe","/D " & chr(34) & "FILECONTAINER_BINUTILS" & chr(34)
	else
		msdev.activeconfiguration.removetoolsettings "cl.exe","/D " & chr(34) & "FILECONTAINER_BINUTILS" & chr(34)
		msdev.activeconfiguration.addtoolsettings "cl.exe","/D " & chr(34) & "FILECONTAINER_WINDOWS" & chr(34)
	end if

	msdev.rebuildall

		'コンフィギュレータの生成
	set msdev.activeproject = msdev.projects("configurator")
	set msdev.activeconfiguration = msdev.activeproject.configurations("configurator" + model)
	msdev.build

msdev.quit
set msdev = nothing

msgbox "全ての処理が終了しました"
end if
