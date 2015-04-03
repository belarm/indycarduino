#!/usr/bin/env python

import pygame
import os, sys
import serial
import stopwatch
from pygame.locals import *

if not pygame.font: print 'Warning, fonts disabled'

fsock = open('error.log', 'w')
sys.stderr = fsock
sys.stdout = fsock

serialPort = '/dev/ttyACM0'
serialBaud = 9600

fontSize = 950
fontColor = (255, 255, 0)

timer = False

def init_pygame():

	pygame.init()
	pygame.display.set_caption("Indy Car")
	pygame.mouse.set_visible(False)

	return get_screen()

def get_screen():
	size = (pygame.display.Info().current_w, pygame.display.Info().current_h)
	return pygame.display.set_mode(size, pygame.FULLSCREEN)

def format_time(seconds):
	m, s = divmod(seconds, 60)

	return "%02d:%02d" % (m, s)

def start_timer():

	start = format_time(timer.elapsed) 

	print start

	text = font.render(start, 1, fontColor)
	textpos = text.get_rect(centerx=background.get_width() / 2, centery=background.get_height() / 2)
	screen.blit(text, textpos)
	pygame.display.flip()

if __name__ == '__main__':
	running = True

	# Initialize Serial
	ser = serial.Serial(serialPort, serialBaud)

	# Initialize Screen
	screen = init_pygame()

	# erase screen
	screen.fill(pygame.Color("black"))

	background = pygame.Surface(screen.get_size())
	background = background.convert()
	background.fill((0, 0, 0))

	font = pygame.font.Font(None, fontSize)
	text = font.render("Start", 1, fontColor)
	textpos = text.get_rect(centerx=background.get_width() / 2, centery=background.get_height() / 2)

	screen.blit(background, (0, 0))
	screen.blit(text, textpos)
	pygame.display.flip()

	
	while running:
		screen.fill(pygame.Color("black"))
		serialInput = ser.readline()
		serialInput = serialInput.strip()

		if serialInput == "start":
			timer = stopwatch.Timer()
			start_timer()

		elif serialInput == "stop":	
			text = font.render("Start", 1, fontColor)
			textpos = text.get_rect(centerx=background.get_width() / 2, centery=background.get_height() / 2)
			screen.blit(text, textpos)

			pygame.display.flip()

			if timer:
				timer.stop()
				timer = False
		else:
			if timer: 
				start_timer()

		for event in pygame.event.get():
			if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
				running = False

	pygame.quit()

