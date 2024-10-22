import os
from flask import Blueprint, request, jsonify
from pymongo import MongoClient
from datetime import datetime
from dotenv import load_dotenv

load_dotenv()

# Get the database URL from environment variables
DATABASE_URL = os.getenv("DATABASE_URL")

client = MongoClient(DATABASE_URL)
db = client['quicksmartsolutions']
sensors_collection = db['sensors']

# Create a Blueprint for sensor routes
sensor_bp = Blueprint('sensor_routes', __name__)

def update_sensor_data(sensor_id, speed):
    updated_data = {
        "sensor_id": sensor_id,
        "latest_reading": {
            "speed": speed,
            "timestamp": datetime.utcnow()  # Current UTC time
        }
    }

    sensors_collection.update_one(
        {"sensor_id": sensor_id},
        {"$set": updated_data},
        upsert=True
    )

@sensor_bp.route('/add_ultrasonic1', methods=['POST'])
def add_ultrasonic1_data():
    data = request.json
    speed = data.get('speed', 0)  # Default to 0 if no speed is provided
    update_sensor_data("ultra_sonic1", speed)
    return jsonify({"message": "Data updated for ultra_sonic1"}), 200

@sensor_bp.route('/add_ultrasonic2', methods=['POST'])
def add_ultrasonic2_data():
    data = request.json
    speed = data.get('speed', 0)  # Default to 0 if no speed is provided
    update_sensor_data("ultra_sonic2", speed)
    return jsonify({"message": "Data updated for ultra_sonic2"}), 200

@sensor_bp.route('/get_speed/<sensor_id>', methods=['GET'])
def get_speed(sensor_id):
    # Find the sensor data based on sensor_id
    sensor = sensors_collection.find_one({"sensor_id": sensor_id})
    if not sensor:
        return jsonify({"error": "Sensor not found"}), 404
    
    return jsonify({
        "sensor_id": sensor["sensor_id"],
        "latest_reading": sensor["latest_reading"]
    })