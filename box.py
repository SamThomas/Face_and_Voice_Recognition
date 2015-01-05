"""
Raspberry Pi Face Recognition
This code is inspired from the work of Tony DiCola for the Adafruit
learning system
"""
import cv2
import config
import face
import os
import sys
import signal


def main(argv):
    pid = int(sys.argv[1])
    print 'PID is: ', pid

    # Load training data into model
    print 'Loading training data...'
    model = cv2.createEigenFaceRecognizer()
    print 'Model created'
    model.load(config.TRAINING_FILE)
    print 'Training data loaded!'
    # Initialize camera and box.
    camera = config.get_camera()

    print 'Press Ctrl-C to quit.'
    goodpicture = False;
    while goodpicture == False:
        print 'Looking for face...'
        print 'Check for the positive face and unlock if found.'
        image = camera.read()

        print 'Convert image to grayscale.'
        image = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)

        print 'Get coordinates of single face in captured image.'
        result = face.detect_single(image)

        if result is None:
            print 'Could not detect single face!  Check the image in capture.pgm to see what was captured and try again with only one face visible.'
            #continue
        else: goodpicture = True;
    x, y, w, h = result
    print 'Crop and resize image to face.'
    crop = face.resize(face.crop(image, x, y, w, h))
    print 'Test face against model.'
    label, confidence = model.predict(crop)
    print 'Predicted {0} face with confidence {1} (lower is more confident).'.format(
        'POSITIVE' if label == config.POSITIVE_LABEL else 'NEGATIVE',
        confidence)

    print 'Starting to print in file'
    fo = open("foo.txt", "wr")

    if label == config.POSITIVE_LABEL and confidence < config.POSITIVE_THRESHOLD:
        print 'Recognized face!'
        fo.write("recognized")
    else:
        print 'Did not recognize face!'
        fo.write("echec")

    fo.close()
    os.kill(pid, signal.SIGUSR2)

if __name__ == '__main__':
    main(sys.argv[1])
