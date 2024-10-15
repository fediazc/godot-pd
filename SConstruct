#!/usr/bin/env python
import os

opts = Variables()
opts.Add(BoolVariable("pd_multi", "compile libpd with multi-instance support", False))
env = Environment(variables=opts)
env = SConscript("godot-cpp/SConstruct", 'env')

if env["pd_multi"]:
    env.Append(CPPDEFINES='PDINSTANCE')
    env.Append(LIBS=["libpd-multi"])
else:
    env.Append(LIBS=["libpd"])

env.Append(LIBPATH=["libs/"])
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
