"""
Raspberry Pi Face Recognition
This code is inspired from the work of Tony DiCola for the Adafruit
learning system
"""
import cv2

import config
import face
import hardware


if __name__ == '__main__':
	# Load training data into model
	print 'Loading training data...'
	model = cv2.createEigenFaceRecognizer()
	model.load(config.TRAINING_FILE)
	print 'Training data loaded!'
	# Initialize camer and box.
	camera = config.get_camera()
	box = hardware.Box()
	print 'Running box...'
	print 'Press button to lock (if unlocked), or unlock if the correct face is detected.'
	print 'Press Ctrl-C to quit.'
	while True:
		# Check if capture should be made.
		# TODO: Check if button is pressed.
		if box.is_button_up():
			if not box.is_locked:
				print 'Box is now locked.'
			else:
				print 'Button pressed, looking for face...'
				# Check for the positive face and unlock if found.
				image = camera.read()
				# Convert image to grayscale.
				image = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
				# Get coordinates of single face in captured image.
				result = face.detect_single(image)
				if result is None:
					print 'Could not detect single face!  Check the image in capture.pgm' \
						  ' to see what was captured and try again with only one face visible.'
					continue
				x, y, w, h = result
				# Crop and resize image to face.
				crop = face.resize(face.crop(image, x, y, w, h))
				# Test face against model.
				label, confidence = model.predict(crop)
				print 'Predicted {0} face with confidence {1} (lower is more confident).'.format(
					'POSITIVE' if label == config.POSITIVE_LABEL else 'NEGATIVE',
					confidence)
				if label == config.POSITIVE_LABEL and confidence < config.POSITIVE_THRESHOLD:
					print 'Recognized face!'
				else:
					print 'Did not recognize face!'
