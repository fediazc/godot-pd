extends Node2D

var pd_playback: AudioStreamPlaybackPD

@onready var freq_label = $CanvasLayer/Control/VBoxContainer/FreqLabel

func _ready():
	AudioStreamPlayerPD.play()
	pd_playback = AudioStreamPlayerPD.get_stream_playback()
	pd_playback.add_patch("test-send.pd", "./pd")

func _process(_delta):
	var rect_size = get_viewport().get_visible_rect().size
	var mouse_pos = get_global_mouse_position()
	var freq = clampf((1 - mouse_pos.y / rect_size.y), 0, 1) * 880
	
	freq_label.text = "Frequency: %.0f Hz" % freq
	
	if Input.is_action_just_pressed("play_tone"):
		pd_playback.send_bang("bang")
		
	pd_playback.send_float("freq", freq)
