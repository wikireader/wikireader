import os

def opj(path):
    """Convert paths to the platform-specific separator"""
    st = apply(os.path.join, tuple(path.split('/')))
    # HACK: on Linux, a leading / gets lost...
    if path.startswith('/'):
        st = '/' + st
    return st

def getProgramFolder():
    moduleFile = __file__
    moduleDir = os.path.split(os.path.abspath(moduleFile))[0]
    programFolder = os.path.abspath(moduleDir)
    return programFolder

def resource_path(relative):
    return os.path.join(
        os.environ.get(
            "_MEIPASS2",
            os.path.abspath(".")
        ),
        relative
    )