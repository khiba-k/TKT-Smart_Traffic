import os
from flask import Blueprint, request, jsonify
from pymongo import MongoClient
from datetime import datetime
from datetime import datetime, timedelta, timezone
from dotenv import load_dotenv

load_dotenv()

# Get the database URL from environment variables
DATABASE_URL = os.getenv("DATABASE_URL")

client = MongoClient(DATABASE_URL)
db = client['quicksmartsolutions']
camera_collection = db['cameras']

# Create a Blueprint for sensor routes
camera_bp = Blueprint('camera_routes', __name__)

def add_timestamp(camera_id, timestamp):
    """
    Add a timestamp to the camera collection for a specific camera.
    Keep only the latest 10 timestamps.
    """
    # Format timestamp in ISO 8601 format
    formatted_timestamp = timestamp.strftime('%Y-%m-%dT%H:%M:%SZ')

    # Update the camera data and ensure only the latest 10 timestamps are kept
    camera_collection.update_one(
        {"camera_id": camera_id},
        {
            "$push": {
                "timestamps": {
                    "$each": [formatted_timestamp],
                    "$slice": -10  # Keep only the latest 10 timestamps
                }
            }
        },
        upsert=True  # Create the document if it doesn't exist
    )

def check_timestamp_match(camera_id, sensor_timestamp):
    """
    Check if the sensor timestamp matches any of the stored timestamps
    in the camera collection within a 10-second tolerance.
    """
    camera_data = camera_collection.find_one({"camera_id": camera_id})

    if not camera_data:
        return False

    timestamps = camera_data.get("timestamps", [])
    tolerance = timedelta(seconds=10)

    # Ensure sensor_timestamp is timezone-aware in UTC
    if sensor_timestamp.tzinfo is None:
        sensor_timestamp = sensor_timestamp.replace(tzinfo=timezone.utc)

    for ts in timestamps:
        # Parse camera_timestamp as UTC
        camera_timestamp = datetime.strptime(ts, '%Y-%m-%dT%H:%M:%SZ').replace(tzinfo=timezone.utc)
        
        if abs(sensor_timestamp - camera_timestamp) <= tolerance:
            return True

    return False


@camera_bp.route('/camera1', methods=['POST'])
def camera1_data():
    data = request.json
    vehicle = data.get('vehicle_detected')

    if not vehicle:
        return jsonify({"error": "Vehicle detection status required"}), 400

    # Add timestamp for camera 1
    timestamp = datetime.now(timezone.utc)
    add_timestamp("camera1", timestamp)
    
    return jsonify({"message": "Timestamp added for camera 1"}), 200

@camera_bp.route('/camera2', methods=['POST'])
def camera2_data():
    data = request.json
    vehicle = data.get('boolean')
    
    if not vehicle:
        return jsonify({"error": "Timestamp required"}), 400

    # Add timestamp for camera 2
    timestamp = datetime.now(timezone.utc)
    add_timestamp("camera2", timestamp)
    return jsonify({"message": "Timestamp added for camera 1"}), 200