from cv2 import (VideoCapture, imshow, waitKey, destroyAllWindows, CAP_PROP_FRAME_WIDTH, CAP_PROP_FRAME_HEIGHT, CAP_PROP_FPS)

# Create video capture object
capture = VideoCapture('http://192.168.1.55/video')

# Check that a camera connection has been established
if not capture.isOpened():
    print("Error opening video file")

else:
    # Get video properties and print them
    frame_width = capture.get(CAP_PROP_FRAME_WIDTH)
    frame_height = capture.get(CAP_PROP_FRAME_HEIGHT)
    fps = capture.get(CAP_PROP_FPS)

    print("Image frame width: ", int(frame_width))
    print("Image frame height: ", int(frame_height))
    print("Frame rate: ", int(fps))

while capture.isOpened():

    # Read an image frame
    ret, frame = capture.read()

    # If an image frame has been grabbed, display it
    if ret:
        imshow('Displaying image frames from video file', frame)

    # If the Esc key is pressed, terminate the while loop
    if waitKey(25) == 27:
        break

# Release the video capture and close the display window
capture.release()
destroyAllWindows()