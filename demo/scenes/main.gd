extends Node

var playback: AudioStreamPlayback

func _ready():
	$AudioStreamPlayer.play()
