# godot-pd

GDExtension that allows you to interact with and play [Pure Data](https://puredata.info/) patches in Godot.

Latest tested Godot version: **4.3 stable**.

## Installation

See [compiling from source](#compiling-from-source).

## Overview

To begin using godot-pd, first create an `AudioStreamPlayer` node, and set an `AudioStreamPD` as its `stream` property in the inspector. Call `play()` on the `AudioStreamPlayer` node, then call `get_stream_playback()`. The `AudioStreamPlaybackPD` object returned by `get_stream_playback()` is your main handle for interacting with Pure Data.

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

If you need to get around these limitations for your project, you can [compile godot-pd](#compiling-from-source) with multi-instance support enabled, but keep in mind that this has not been tested.

## Compiling from source

- Clone the repository and initialize submodules:

```
git clone --recurse-submodules https://github.com/fediazc/godot-pd.git
```

- Install the tools necessary for compiling Godot. You **will NOT** need to compile Godot, but they are the same tools you need to compile this extension. See [Godot docs: Building from source](https://docs.godotengine.org/en/stable/contributing/development/compiling/index.html#toc-devel-compiling) for details.
- Build libpd. Please see the [libpd repo](https://github.com/libpd/libpd) for details. By default, the `libpd/build/libs` folder will be searched for the build files. You can specify a different location by passing `libpd_lib_dir` as an argument to SCons:

```
scons libpd_lib_dir=path/to/libpd/libs
```

> [!NOTE]
> If you're [building libpd for Windows using CMake](https://github.com/libpd/libpd?tab=readme-ov-file#building-with-cmake), a prebuilt copy of the required pthread library is located in this repo at `3rdpary/pthreads-win32`. It's assumed you are using these files (or at least the same pthread library) for your libpd build. If you're using a different library, you can specify the location of the .dll file by passing a path to SCons with the `pthread_dll_path` option, so that it's copied over to the final build folder. You will also need to manually add this file to the dependencies section of the `gdpd.gdextension` file so it's exported along with your project. Please see [Godot docs: the .gdextension file](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_file.html#dependencies-section) for more details.

- Finally, run SCons from the project root:

```
scons platform=<platform>
```

The completed build can be found in `demo/addons/`. To use with your own Godot project, copy the `addons/` into your project folder.

### Compiling with multiple-instance support

Please keep in mind that libpd multiple-instance support has not been tested with this extension.

To compile with multiple-instance support, follow the same steps as above, but make sure you build libpd with the proper options enabled. Please see the [libpd repo](https://github.com/libpd/libpd) for details. Then make sure to pass `pd_multi=true` as an option to SCons:

```
scons pd_multi=true
```