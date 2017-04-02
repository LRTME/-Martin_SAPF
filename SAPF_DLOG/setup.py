
from cx_Freeze import setup, Executable


setup(  
        name = "DLOG_viewer",
        version = "0.1",
        description = "DLOG_viewer",
        executables = [Executable("DLOG_viewer.py", base = "Win32GUI")]
        )
        