# -*- mode: python -*-
a = Analysis(['Wikireader Update.py'],
             hiddenimports=[],
             hookspath=None)
a.datas += [('7za.exe', 'tools\\7za.exe', 'DATA')]
pyz = PYZ(a.pure)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
          name=os.path.join('dist', 'Wikireader Update.exe'),
          debug=False,
          strip=None,
          upx=True,
          console=False , icon='images\\icon_512x512.ico')
app = BUNDLE(exe,
             name=os.path.join('dist', 'Wikireader Update.exe.app'))
