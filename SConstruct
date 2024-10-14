#!/usr/bin/env python
import os

env = SConscript("godot-cpp/SConstruct")

env.Append(LIBPATH=["libs/"])
env.Append(LIBS=["libpd"])
env.Append(CPPPATH=["src/"])

env.Append(CPPPATH=[
    "libpd\\cpp",
    "libpd\\libpd_wrapper",
    "libpd\\pure-data\\src"
])

sources = Glob("src/*.cpp")

library = env.SharedLibrary(
    "demo/addons/bin/godotpd{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
    source=sources,
)

Default(library)
