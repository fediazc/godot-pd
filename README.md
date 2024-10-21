# godot-pd

GDExtension that allows you to interact with and play [Pure Data](https://puredata.info/) patches in Godot.

Latest tested Godot version: **4.3 stable**.

## Installation

See [building from source](#building-from-source).

## Overview

To begin using godot-pd, first create an `AudioStreamPlayer` node, and assign an `AudioStreamPD` as its `stream` property in the inspector. Call `play()` on the `AudioStreamPlayer` node, and then call `get_stream_playback()`. The `AudioStreamPlaybackPD` object returned by `get_stream_playback()` is your main handle for interacting with Pure Data.

For more details, please see the documentation for both `AudioStreamPlaybackPD` and `AudioStreamPD`. Also see [issues and limitations](#issues-and-limitations).

```GDScript
@onready var player: AudioStreamPlayer = $AudioStreamPlayer

var pd: AudioStreamPlaybackPD

func _ready():
    # setup
    player.play()
    pd = player.get_stream_playback()
    
    # playing patches
    pd.add_patch("example.pd", "./")

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

## Issues and limitations

The [releases](https://github.com/fediazc/godot-pd/releases) for godot-pd are (will be) compiled **without** multiple-instance support for libpd. Every `AudioStreamPD` resource will use **the same instance of Pure Data**. If, for example, you stop playback from one `AudioStreamPlayer` node, **all audio from Pure Data will be stopped** even if other `AudioStreamPlayer` nodes are still running.

To avoid these types of issues, it's recommended that you only use a single `AudioStreamPlayer` to interact with Pure Data, through an [autoload](https://docs.godotengine.org/en/stable/tutorials/scripting/singletons_autoload.html).

If you need to get around these limitations for your project, you can [compile godot-pd](#building-from-source) with multiple-instance support enabled, but keep in mind that this has not been tested.

## Building from source

1. Clone the repository and initialize submodules:

```
git clone --recurse-submodules https://github.com/fediazc/godot-pd.git
```

2. Install the tools necessary for compiling Godot. You **will NOT** need to compile Godot, but they are the same tools you need to compile this extension. See [Godot docs: Building from source](https://docs.godotengine.org/en/stable/contributing/development/compiling/index.html#toc-devel-compiling) for details.
3. Build libpd. Please see the [libpd repo](https://github.com/libpd/libpd) for details. By default, the build files will be searched for in the `libpd/build/libs` folder. You can specify a different location by passing `libpd_lib_dir` as an argument to SCons:

```
scons libpd_lib_dir=path/to/libpd/libs
```

4. Run SCons from the project root:

```
scons platform=<platform>
```

5. Once the build finishes, you will need to copy the libpd dynamic library files (libpd.dll, libpd.so, or libpd.dylib, depending on your platform) to `demo/addons/godot-pd/bin/` 
    - **If you are building for macOS**, this folder will be `demo/addons/godot-pd/bin/libgodotpd.macos.<target>.framework/` instead, where `<target>` will be either `template_release` or `template_debug`.
    - **If you are building for Windows**, you will also need to copy the pthread library used to build libpd. If you compiled libpd with MinGW, the library is likely libwinpthread-1.dll; if you used Visual Studio, it is likely pthreadVC2.dll.

6. Finally, ensure the paths in `demo/addons/godot-pd/godot-pd.gdextension` are correct. This file assumes you followed the previous steps and used MinGW to compile for Windows. For more details about .gdextension files, please see the [Godot docs](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_file.html).

To use the extension in your project, copy the `addons/` folder from `demo/` directory into the root of your project folder.

### Building with multiple instance support

Please keep in mind that libpd multiple-instance support has not been tested with this extension.

To compile with multiple-instance support, follow the same steps as above, but make sure you build libpd with the proper options enabled. Please see the [libpd repo](https://github.com/libpd/libpd) for details. Then make sure to pass `pd_multi=true` as an option to SCons:

```
scons pd_multi=true
```