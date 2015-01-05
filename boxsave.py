"""
Raspberry Pi Face Recognition
This code is inspired from the work of Tony DiCola for the Adafruit
learning system
"""
import cv2
import config
import face

if __name__ == '__main__':
    # Load training data into model
    print 'Loading training data...'
    model = cv2.createEigenFaceRecognizer()
    model.load(config.TRAINING_FILE)
    print 'Training data loaded!'
    # Initialize camera and box.
    camera = config.get_camera()

    print 'Press Ctrl-C to quit.'
    while True:
        print 'Looking for face...'
        print 'Check for the positive face and unlock if found.'
        image = camera.read()

        print 'Convert image to grayscale.'
        image = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)

        print 'Get coordinates of single face in captured image.'
        result = face.detect_single(image)

        if result is None:
            print 'Could not detect single face!  Check the image in capture.pgm to see what was captured and try again with only one face visible.'
            continue
        x, y, w, h = result
        print 'Crop and resize image to face.'
        crop = face.resize(face.crop(image, x, y, w, h))
        print 'Test face against model.'
        label, confidence = model.predict(crop)
        print 'Predicted {0} face with confidence {1} (lower is more confident).'.format(
            'POSITIVE' if label == config.POSITIVE_LABEL else 'NEGATIVE',
            confidence)
        if label == config.POSITIVE_LABEL and confidence < config.POSITIVE_THRESHOLD:
            print 'Recognized face!'
        else:
            print 'Did not recognize face!'
