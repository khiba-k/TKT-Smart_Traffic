import torch
import requests
import cv2
from flask import Flask, jsonify
import threading
import traceback
import sys

# Force CPU usage
torch.cuda.is_available = lambda : False

# Initialize Flask app
app = Flask(__name__)

try:
    # Load the pre-trained YOLOv5 model
    model = torch.hub.load('ultralytics/yolov5', 'custom', path='yolov5s.pt', force_reload=True)
except Exception as e:
    print(f"Error loading model: {e}")
    print(traceback.format_exc())
    sys.exit(1)

# Replace the URL with your ESP32-CAM stream URL
url = 'http://192.168.1.28/video'

try:
    # Initialize video capture with the URL and set timeouts
    cap = cv2.VideoCapture(url)
    cap.set(cv2.CAP_PROP_BUFFERSIZE, 2)  # Set buffer size to 2 frames
    cap.set(cv2.CAP_PROP_FPS, 30)        # Set FPS to 30
except Exception as e:
    print(f"Error initializing video capture: {e}")
    print(traceback.format_exc())
    sys.exit(1)

# Check if the video stream is opened successfully
if not cap.isOpened():
    print("Error: Could not open video stream.")
    sys.exit(1)

# Initialize a flag to check if a car, truck, or van has been detected
vehicle_detected = False

# Function to check for cars, trucks, or vans in the video stream
def check_for_vehicles():
    global vehicle_detected
    while cap.isOpened():
        try:
            ret, frame = cap.read()
            if not ret:
                print("Error: Could not read frame.")
                break

            # Convert the frame to RGB (YOLOv5 uses RGB images)
            frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

            # Perform inference on the frame
            results = model(frame_rgb)

            # Get detection results for the current frame
            detections = results.pandas().xyxy[0]  # Get results in pandas DataFrame format

            # Check if there are any detections of cars (ID 2), trucks (ID 7), or vans (ID 14)
            if any(name in ['car', 'truck', 'van', 'train'] for name in detections['name'].values):
                vehicle_detected = True
            else:
                vehicle_detected = False

            url = "http://127.0.0.1:5000/camera/camera1"  # Replace with your actual URL
            
            if vehicle_detected:
                data = {"vehicle_detected": vehicle_detected}
                try:
                    response = requests.post(url, json=data)
                    response.raise_for_status()  # Raise an HTTPError for bad responses
                    print("Status Code:", response.status_code)
                    print("Response:", response.json())
                except requests.exceptions.HTTPError as http_err:
                    print(f"HTTP error occurred: {http_err}")
                    print(f"Response Text: {response.text}")  # Print server response
                except requests.exceptions.RequestException as req_err:
                    print(f"Request error occurred: {req_err}")

            # Draw the results on the frame
            results.render()

            # Convert the frame back to BGR (OpenCV uses BGR images)
            frame_bgr = cv2.cvtColor(results.ims[0], cv2.COLOR_RGB2BGR)

            # Display the resulting frame
            cv2.imshow('YOLOv5 Object Detection', frame_bgr)

            # Break the loop if 'q' is pressed
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        except Exception as e:
            print(f"Error in check_for_vehicles: {e}")
            print(traceback.format_exc())
            break

    cap.release()
    cv2.destroyAllWindows()

try:
    vehicle_detection_thread = threading.Thread(target=check_for_vehicles, daemon=True)
    vehicle_detection_thread.start()
    vehicle_detection_thread.join()  # Wait for the thread to finish
except Exception as e:
    print(f"Error in main thread: {e}")
    print(traceback.format_exc())

print("Program completed.")