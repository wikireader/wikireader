# WikiReader
## Package Note

### Windows

[Pyinstaller - pack applcation to exe](http://www.pyinstaller.org/)

```
python Makespec.py -F -w --icon=icon.ico "Wikireader Update.py"
python Build.py .\Wikireader Update\Wikireader Update.spec
```

### Mac

**py2app - Create standalone Mac OS X applications with Python**

```
pip install py2app
./build.sh
```

run **build.sh** to simple package the app.

### Ubuntu Linux
sudo apt-get install gtk2-engines-pixbuf 
sudo apt-get install python-wxgtk2.8
sudo apt-get install python-wxtools


## Mac : Code Sign for ML

disable
sudo spctl --master-disable

[Sign for Gatekeeper](http://web.archiveorange.com/archive/v/cW70Ll4ViLCVNwKOE3gl)

Download Mac Developer then install to keychain

```
codesign -f -s "Developer ID Application" dist/WikiReader.app/
# check
spctl -a -v dist/WikiReader.app
```

#### Refernce

http://developer.apple.com/library/mac/#documentation/ToolsLanguages/Conceptual/OSXWorkflowGuide/DistributingApplicationsOutside/DistributingApplicationsOutside.html

https://developer.apple.com/library/mac/#documentation/security/Conceptual/CodeSigningGuide/Procedures/Procedures.html

http://www.red-sweater.com/blog/2390/developer-id-gotcha
