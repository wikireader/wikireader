VERSION 5.00
Object = "{194B359F-594E-4EE4-A804-84723D1CF1C4}#1.0#0"; "ATLDevice.dll"
Begin VB.Form Form1 
   BorderStyle     =   3  '固定ダイアログ
   Caption         =   "Form1"
   ClientHeight    =   1140
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   3045
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1140
   ScaleWidth      =   3045
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows の既定値
   Begin ATLDEVICELibCtl.DeviceControl DeviceControl1 
      Height          =   375
      Left            =   2400
      OleObjectBlob   =   "Form1.frx":0000
      TabIndex        =   1
      Top             =   0
      Visible         =   0   'False
      Width           =   495
   End
   Begin VB.Label Label1 
      Alignment       =   2  '中央揃え
      Caption         =   "Label1"
      Height          =   255
      Index           =   4
      Left            =   1920
      TabIndex        =   5
      Top             =   360
      Width           =   975
   End
   Begin VB.Label Label1 
      Alignment       =   2  '中央揃え
      Caption         =   "Label1"
      Height          =   255
      Index           =   3
      Left            =   1560
      TabIndex        =   4
      Top             =   720
      Width           =   975
   End
   Begin VB.Label Label1 
      Alignment       =   2  '中央揃え
      Caption         =   "Label1"
      Height          =   255
      Index           =   2
      Left            =   480
      TabIndex        =   3
      Top             =   720
      Width           =   975
   End
   Begin VB.Label Label1 
      Alignment       =   2  '中央揃え
      Caption         =   "Label1"
      Height          =   255
      Index           =   1
      Left            =   0
      TabIndex        =   2
      Top             =   360
      Width           =   975
   End
   Begin VB.Label Label1 
      Alignment       =   2  '中央揃え
      Caption         =   "Label1"
      Height          =   255
      Index           =   0
      Left            =   960
      TabIndex        =   0
      Top             =   0
      Width           =   975
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'
'  TOPPERS/JSP Kernel
'      Toyohashi Open Platform for Embedded Real-Time Systems/
'      Just Standard Profile Kernel
' 
'  Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
'                              Toyohashi Univ. of Technology, JAPAN
' 
'  上記著作権者は，以下の (1)～(4) の条件か，Free Software Foundation 
'  によって公表されている GNU General Public License の Version 2 に記
'  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
'  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
'  利用と呼ぶ）することを無償で許諾する．
'  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
'      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
'      スコード中に含まれていること．
'  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
'      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
'      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
'      の無保証規定を掲載すること．
'  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
'      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
'      と．
'    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
'        作権表示，この利用条件および下記の無保証規定を掲載すること．
'    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
'        報告すること．
'  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
'      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
' 
'  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
'  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
'  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
'  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
'
'  @(#) $Id: form1.frm,v 1.5 2004/09/09 03:40:51 honda Exp $
'

Private Sub SetLabelString(index As Integer, letter As String, color As ColorConstants)
    Label1(index) = letter
    Label1(index).ForeColor = color
End Sub

Private Sub DeviceControl1_OnKernelExit()
    Dim i As Integer
    
    For i = 0 To 4
        SetLabelString i, "終了", vbBlue
    Next i
    
End Sub

Private Sub DeviceControl1_OnKernelStart()
    Dim i As Integer
    
    For i = 0 To 4
        SetLabelString i, "取得待ち", vbBlack
    Next i
End Sub

Private Sub DeviceControl1_OnWrite(ByVal address As Long, ByVal sz As Long)
    Dim i As Integer
    Dim work As Long
    work = DeviceControl1.Value
    i = work And &HF
        
    Select Case (work And &HF0)
    Case &H10
        SetLabelString i, "取得待ち", vbBlack
    Case &H20
        SetLabelString i, "食事", vbRed
    Case &H40
        SetLabelString i, "休憩", vbBlack
    End Select

End Sub

Private Sub Form_Load()
    Dim i As Integer
    
    For i = 0 To 4
        SetLabelString i, "終了", vbBlue
    Next i
    
    DeviceControl1.Connect
    DeviceControl1.Map 100, 4
End Sub

Private Sub Form_Unload(Cancel As Integer)
    DeviceControl1.Close
End Sub
