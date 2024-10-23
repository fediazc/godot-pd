#!/usr/bin/env python
import os
from glob import glob

libpd_lib_dir = ARGUMENTS.pop("libpd_lib_dir", "libpd/build/libs")
pd_multi = ARGUMENTS.pop("pd_multi", False)

env = SConscript("godot-cpp/SConstruct")

main_target_dir = "demo/addons/godot-pd/bin"

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

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

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
