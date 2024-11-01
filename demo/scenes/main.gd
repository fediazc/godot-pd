extends Node2D

var pd_playback: AudioStreamPlaybackPD
var playing = false

@onready var freq_label = $CanvasLayer/Control/VBoxContainer/FreqLabel

func _ready():
	begin_playback()

func _process(_delta):
	var rect_size = get_viewport().get_visible_rect().size
	var mouse_pos = get_global_mouse_position()
	var freq = clampf(1 - mouse_pos.y / rect_size.y, 0, 1) * 880
	
	freq_label.text = "Frequency: %.0f Hz" % freq
	
	if Input.is_action_just_pressed("play_tone") and playing:
		pd_playback.send_bang("bang")
	
	if Input.is_action_just_pressed("toggle_play"):
		if playing:
			stop_playback()
		else:
			begin_playback()
	
	if playing:
		pd_playback.send_float("freq", freq)
	pass

func begin_playback():
	AudioStreamPlayerPD.play()
	pd_playback = AudioStreamPlayerPD.get_stream_playback()
	var id = pd_playback.open_patch("./pd/test-send.pd")
	print("test-send.pd $0 = ", id)
	pd_playback.open_patch("pd/test-array.pd")
	var sz = pd_playback.get_array_size("array")
	var arr = pd_playback.read_array("array")
	print("%d element array before write: %s" % [sz, arr])
	pd_playback.write_array("array", [1, 2, 3, 4, 5])
	arr = pd_playback.read_array("array")
	print("array after write: %s" % [arr])
	pd_playback.resize_array("array", 2)
	arr = pd_playback.read_array("array")
	print("array after resize: %s" % [arr])
	pd_playback.close_patch("./pd/test-array.pd")
	
	pd_playback.subscribe("godot")
	pd_playback.receive_bang.connect(_on_receive_bang)
	pd_playback.receive_float.connect(_on_receive_float)
	pd_playback.receive_symbol.connect(_on_receive_symbol)
	pd_playback.receive_list.connect(_on_receive_list)
	pd_playback.receive_note_on.connect(_on_receive_note_on)
	pd_playback.open_patch("./pd/test-receive.pd")
	pd_playback.open_patch("pd/test-midi.pd")
	
	pd_playback.send_list("list", ["test", 1, 2, 3])
	
	playing = true

func stop_playback():
	AudioStreamPlayerPD.stop()
	pd_playback = null
	playing = false
	
func _on_receive_bang(dest: String):
	print("pd sent a bang to %s @ %ds" % [dest, Time.get_ticks_msec() / 1000.0])

func _on_receive_float(dest: String, num: float):
	print("pd sent a float (%.2f) to %s" % [num, dest])

func _on_receive_symbol(dest: String, symbol: String):
	print("pd sent a symbol ('%s') to %s" % [symbol, dest])

func _on_receive_list(dest: String, list: Array):
	print("pd sent a list (%s) to %s" % [list, dest])
	
func _on_receive_note_on(channel: int, pitch: int, velocity: int):
	print("pd sent a midi note ([ch: %d, pitch: %d, vel: %d])" % [channel, pitch, velocity])
