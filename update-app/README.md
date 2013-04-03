# Wikireader Update

### Package list

* Python 2.7.3
* wxPython (2.9.3.1 on OSX, 2.8.12.1 on Windows)
* py2app 0.6.4 (for OSX)
* wmi
* [py2win](http://sourceforge.net/projects/pywin32/files/)
* simplejson

### Check packages version

```
$ python --version
Python 2.7.3

$ python
>>> import wx
>>> import wx.__version__
'2.9.3.1'
```


### Windows

**Use Pyinstaller to generate exe file on Windows.**

[Pyinstaller - pack applcation to exe](http://www.pyinstaller.org/)


**Recreate Spec file**
```
python Makespec.py -F -w --icon=icon.ico "WikiReader Update.py"
python Build.py .\WikiReader Update\WikiReader Update.spec
```

**Generate executable file on Windows**
```
build.bat
```

**Important**: Add 7za path to spec.
```
a.datas += [('7za.exe', 'tools\\7za.exe', 'DATA')]
```

### OSX

**py2app - Create standalone Mac OS X applications with Python**

```
pip install py2app
./buildMacApp.sh
```

run **buildMacApp.sh** to simple deploy the app.


## OSX : Code Sign for Mountain Lion

**Enable/Disable Gatekeeper**

```
sudo spctl --master-disable
```

Download Mac Developer code sign then install to keychain

```
codesign -f -s "Developer ID Application" dist/WikiReader.app/
# check
spctl -a -v dist/WikiReader.app
```

[Sign for Gatekeeper](http://web.archiveorange.com/archive/v/cW70Ll4ViLCVNwKOE3gl)

#### Reference

[Mac Developer - Distributing Outside the Mac App Store](
http://developer.apple.com/library/mac/#documentation/ToolsLanguages/Conceptual/OSXWorkflowGuide/DistributingApplicationsOutside/DistributingApplicationsOutside.html)

[Mac Developer - Code Signing Tasks](https://developer.apple.com/library/mac/#documentation/security/Conceptual/CodeSigningGuide/Procedures/Procedures.html)

[Developer ID Gotcha](http://www.red-sweater.com/blog/2390/developer-id-gotcha)
