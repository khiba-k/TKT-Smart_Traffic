import torch
import cv2

# Load the pre-trained YOLOv5 model
model = torch.hub.load('ultralytics/yolov5', 'yolov5s')

# Replace the URL with your ESP32-CAM stream URL

url = 'http://192.168.1.55/video'

# Initialize video capture with the URL and set timeouts
cap = cv2.VideoCapture(url)
cap.set(cv2.CAP_PROP_BUFFERSIZE, 2)  # Set buffer size to 2 frames
cap.set(cv2.CAP_PROP_FPS, 30)        # Set FPS to 30

# Check if the video stream is opened successfully
if not cap.isOpened():
    print("Error: Could not open video stream.")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Error: Could not read frame.")
        break

    # Convert the frame to RGB (YOLOv5 uses RGB images)
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Perform inference on the frame
    results = model(frame_rgb)

    # Draw the results on the frame
    results.render()

    # Convert the frame back to BGR (OpenCV uses BGR images)
    frame_bgr = cv2.cvtColor(results.ims[0], cv2.COLOR_RGB2BGR)

    # Display the resulting frame
    cv2.imshow('YOLOv5 Object Detection', frame_bgr)

    # Break the loop if 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
