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

func begin_playback():
	AudioStreamPlayerPD.play()
	pd_playback = AudioStreamPlayerPD.get_stream_playback()
	pd_playback.subscribe("godot")
	pd_playback.receive_bang.connect(_on_receive_bang)
	pd_playback.receive_float.connect(_on_receive_float)
	pd_playback.receive_symbol.connect(_on_receive_symbol)
	pd_playback.add_patch("test-receive.pd", "./pd")
	pd_playback.add_patch("test-send.pd", "./pd")

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
