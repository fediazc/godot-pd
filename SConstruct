#!/usr/bin/env python
import os

env = SConscript("godot-cpp/SConstruct")

env.Append(LIBPATH=["libs/"])
env.Append(LIBS=["libpd"])
env.Append(CPPPATH=["src/"])

if "msvc_include" in ARGUMENTS:
    env.Append(CPPPATH=[ARGUMENTS["msvc_include"]])

include = []
for dirpath, dirnames, filenames in os.walk("libpd"):
    for dname in dirnames:
        include.append(os.path.join(dirpath, dname))
env.Append(CPPPATH=include)

sources = Glob("src/*.cpp")

library = env.SharedLibrary(
    "demo/addons/bin/godotpd{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
    source=sources,
)

Default(library)
