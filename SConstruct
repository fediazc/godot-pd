#!/usr/bin/env python
import os
from glob import glob

libpd_lib_dir = ARGUMENTS.pop("libpd_lib_dir", "libpd/build/libs")
pthread_dll_path = ARGUMENTS.pop("pthread_dll_path", "")
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

library = env.SharedLibrary(
    target=os.path.join(main_target_dir, "godotpd{}{}".format(env["suffix"], env["SHLIBSUFFIX"])),
    source=sources,
)

Default(library)

shared_libs = []
if env["platform"] == "windows":
    shared_libs += glob(os.path.join(pthread_dll_dir, "*.dll"))
    libpd_dll = "libpd" + ("-multi" if pd_multi else "") + ".dll"
    shared_libs.append(os.path.join(libpd_lib_dir, libpd_dll))

    if pthread_dll_path:
        shared_libs.append(pthread_dll_path)
else:
    libpd_so = "libpd" + ("-multi" if pd_multi else "") + ".so"
    shared_libs.append(os.path.join(libpd_lib_dir, libpd_so))

for path in shared_libs:
    copy_target = env.Command(
        target=os.path.join(main_target_dir, os.path.basename(path)),
        source=path,
        action=Copy("$TARGET", "$SOURCE")
    )

    Default(copy_target)
