# godot-pd

GDExtension that allows you to interact with and run [Pure Data](https://puredata.info/) patches in Godot.

Currently uses Pd vanilla version [**0.54-1**](https://puredata.info/downloads/pure-data/releases/0.54-1).

Latest tested Godot version: **4.3 stable**.

## Installation

Download the latest release from the [release page](https://github.com/fediazc/godot-pd/releases), then copy the `addons/` folder to your Godot project's root folder.

You can also [build from source](#building-from-source).

## Overview

To begin using the extension, assign an `AudioStreamPD` resource to an `AudioStreamPlayer`'s `stream` property in the inspector. For more details, please see the in-editor documentation for both `AudioStreamPlaybackPD` and `AudioStreamPD`.

Here's some sample code:

```GDScript
@onready var player: AudioStreamPlayer = $AudioStreamPlayer

var pd: AudioStreamPlaybackPD

func _ready():
    # setup
    player.play()
    pd = player.get_stream_playback()
    
    # playing patches
    pd.open_patch("example.pd")

    # sending messages
    pd.send_bang("start-loop")
    pd.send_float("set-pitch", 440)

    # receiving messages
    pd.subscribe("my-tag")

    pd.receive_bang.connect(_on_receive_bang)
    pd.receive_float.connect(_on_receive_float)

func _on_receive_bang(dest: String):
    # do something...

func _on_receive_float(dest: String, num: float):
    # do something with num...

```

## Issues and Limitations

- Sometimes, trying to open an **invalid or missing** patch file with `AudioStreamPlaybackPD.open_patch()` will cause a crash. This is _probably_ related to [libpd issue #372](https://github.com/libpd/libpd/issues/372), although I have not been able to reproduce this consistently.
- It's not possible to access patch files directly from `res://` with this extension. You will need to access them using regular file system paths. As a result, when exporting you will need to manually move patch files into your export folder as it will not be possible to access them from the exported PCK.

## Building from source

**Important**: It's recommended that you build libpd (see step 3) with multi-instance support enabled. Otherwise, this extension is not guaranteed to work, and using multiple `AudioStreamPlayer` nodes may not behave as expected.

1. Clone the repository and initialize submodules:

```
git clone --recurse-submodules https://github.com/fediazc/godot-pd.git
```

2. Install the tools necessary for compiling Godot. You will **not** need to compile Godot, but they are the same tools you need to compile this extension. See [Godot docs: Building from source](https://docs.godotengine.org/en/stable/contributing/development/compiling/index.html#toc-devel-compiling) for details.

3. Build libpd. Please see the [libpd repo](https://github.com/libpd/libpd) for details (it's recommended that you build with multi-instance support enabled). By default, the build files will be searched for in the `libpd/build/libs` folder. You can specify a different location by passing `libpd_lib_dir` as an argument to SCons in the next step:

```
scons libpd_lib_dir=path/to/libpd/libs
```

**Note**: If you are building without multi-instance support, you will need to pass `pd_multi=no` as an argument to SCons in the next step.

4. To compile the extension, run SCons from the project root:

```
scons platform=<platform>
```

5. After compilation, copy the libpd dynamic library files (named libpd or libpd-multi, with .dll, .so, or .dylib extensions, depending on your platform and build settings) to `demo/addons/godot-pd/bin/<platform>/<target>-<arch>/`.
    - **If you are building for macOS**, this folder will be `demo/addons/godot-pd/bin/macos/libgodotpd.macos.<target>.framework/` instead.
    - **If you are building for Windows**, you will also need to copy the pthread library used to build libpd. If you compiled libpd with MinGW, the library is likely libwinpthread-1.dll; if you used Visual Studio, it is likely pthreadVC2.dll.

6. Finally, ensure the paths in `demo/addons/godot-pd/godot-pd.gdextension` are correct. This file assumes you followed the previous steps and used MinGW to compile for Windows. For more details about .gdextension files, please see the [Godot docs](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_file.html).

To use the extension in your project, copy the `addons/` folder from `demo/` directory into the root of your project folder.
