#!/bin/env python3

""" MIT License

Copyright (c) 2018 leimiaos

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

"""

from __future__ import print_function

VERSION = "0.1"

AddOption("--libarchive-path",
          dest="libarchive_path",
          type="string",
          nargs=1,
          action="store",
          metavar="DIR")

AddOption("--prefix",
          dest="prefix",
          type="string",
          action="store",
          default="/usr")

env = Environment(CPPPATH=["#include"], LIBARCHIVE_PATH=GetOption("libarchive_path"), SHLIBVERSION=VERSION, PREFIX=GetOption("prefix"))

env.MergeFlags({'CPPPATH': ['$LIBARCHIVE_PATH/include'], 'LIBPATH': ['$LIBARCHIVE_PATH/lib']})

if not GetOption("libarchive_path"):
    try:
        env.ParseConfig("pkg-config libarchive --cflags --libs")
    except OSError:
        pass

conf = Configure(env)

if not conf.CheckHeader("archive.h"):
    print("libarchive is required")
    Exit(1)

if not conf.CheckLib("archive"):
    print("libarchive is required")
    Exit(1)


env = conf.Finish()
Export("VERSION")
Export("env")

SConscript("src/SConscript", variant_dir="build", duplicate=0)
SConscript("example/SConscript")

env.Alias("all", "build")
env.Alias("install", "$PREFIX")
