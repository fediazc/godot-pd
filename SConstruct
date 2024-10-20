#!/usr/bin/env python
import os
from glob import glob

libpd_lib_dir = ARGUMENTS.pop("libpd_lib_dir", "libpd/build/libs")
pd_multi = ARGUMENTS.pop("pd_multi", False)

env = SConscript("godot-cpp/SConstruct")

main_target_dir = "demo/addons/bin"
pthread_dll_dir = "3rdparty/pthreads-win32/dll/" + "x64" if env["arch"] == "x86_64" else "x86"

if pd_multi:
    env.Append(CPPDEFINES="PDINSTANCE")
    env.Append(LIBS=["libpd-multi"])
else:
    env.Append(LIBS=["libpd"])

env.Append(LIBPATH=[libpd_lib_dir])
env.Append(CPPPATH=["src/"])

env.Append(CPPPATH=[
    "libpd/cpp",
    "libpd/libpd_wrapper",
    "libpd/pure-data/src"
])

sources = Glob("src/*.cpp")

if env["platform"] == "macos":
    env.Append(LINKFLAGS=['-rpath', '@loader_path'])
    library = env.SharedLibrary(
        target=os.path.join(main_target_dir, "libgodotpd.{}.{}.framework/libgodotpd.{}.{}".format(env["platform"], env["target"], env["platform"], env["target"])),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        target=os.path.join(main_target_dir, "libgodotpd{}{}".format(env["suffix"], env["SHLIBSUFFIX"])),
        source=sources,
    )

Default(library)
