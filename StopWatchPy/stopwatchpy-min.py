#!/usr/bin/env python

import pygame
import os, sys
#import serial
#import stopwatch
import time
from pygame.locals import *

if not pygame.font: print 'Warning, fonts disabled'

fsock = open('error.log', 'w')
sys.stderr = fsock
sys.stdout = fsock

#serialPort = '/dev/ttyACM0'
#serialPort = '/dev/ttyS8'
#serialBaud = 9600

startText = "Press start button to begin"
fontSizeSmall = 150
#fontSizeLarge = 950
fontSizeLarge = 550
fontColorYellow = (255, 255, 0)
fontColorGreen = (0, 255, 0)

timer = False

def init_pygame():

	pygame.init()
	pygame.display.set_caption("Indy Car")
	pygame.mouse.set_visible(False)

	return get_screen()

def get_screen():
	size = (pygame.display.Info().current_w, pygame.display.Info().current_h)
#	return pygame.display.set_mode(size, pygame.FULLSCREEN)
	return pygame.display.set_mode((1600,1024))

def format_time(seconds):
	m, s = divmod(seconds, 60)
	ms = int(round(seconds * 100))
	ms = str(ms)[-2:]

	return "%02d:%02d:%02s" % (m, s, ms)

def start_timer():

	start = format_time(timer.elapsed) 

	text = fontLarge.render(start, 1, fontColorYellow)
	textpos = text.get_rect(centerx=background.get_width() / 2, centery=background.get_height() / 2)
	screen.blit(text, textpos)
	pygame.display.flip()

if __name__ == '__main__':
	running = True

	# Initialize Serial
#	ser = serial.Serial(serialPort, serialBaud, timeout=0)

	# Initialize Screen
	screen = init_pygame()

	# erase screen
	screen.fill(pygame.Color("black"))

	background = pygame.Surface(screen.get_size())
	background = background.convert()
	background.fill((0, 0, 0))

	fontSmall = pygame.font.Font(None, fontSizeSmall)
	fontLarge = pygame.font.Font(None, fontSizeLarge)

	text = fontSmall.render(startText, 1, fontColorYellow)
	textpos = text.get_rect(centerx=background.get_width() / 2, centery=background.get_height() / 2)

	screen.blit(background, (0, 0))
	screen.blit(text, textpos)
	pygame.display.flip()

	
	while running:
		screen.fill(pygame.Color("black"))
		serialInput = readline()
		serialInput = serialInput.strip()

		print serialInput

		if serialInput == "start":
			timer = stopwatch.Timer()
			start_timer()

		elif serialInput == "stop":	
			if timer:
				timer.stop()
				start = format_time(timer.elapsed)

				text = fontLarge.render(start, 1, fontColorGreen)
				textpos = text.get_rect(centerx=background.get_width() / 2, centery=background.get_height() / 2)

				screen.fill(pygame.Color("black"))
				screen.blit(background, (0, 0))
				screen.blit(text, textpos)

				pygame.display.flip()

				time.sleep(5)

				timer = False


			text = fontSmall.render(startText, 1, fontColorYellow)
			textpos = text.get_rect(centerx=background.get_width() / 2, centery=background.get_height() / 2)

			screen.fill(pygame.Color("black"))
			screen.blit(background, (0, 0))
			screen.blit(text, textpos)

			pygame.display.flip()

		else:
			if timer: 
				start_timer()

		for event in pygame.event.get():
			if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
				running = False

	pygame.quit()

