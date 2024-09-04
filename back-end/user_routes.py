# user_routes.py
from flask import Blueprint, request, jsonify
from pymongo import MongoClient

# Connect to MongoDB
client = MongoClient('mongodb://localhost:27017/')
db = client['smart-traffic']
user_profiles = db['user_profiles']

# Create a Blueprint for user routes
user_bp = Blueprint('user_routes', __name__)


@user_bp.route('/clerk-webhook', methods=['POST'])
def clerk_webhook():
    data = request.json
    print("Received webhook data:", data)
    user_id = data.get('data', {}).get('id')
    
    if not user_id:
        return jsonify({'error': 'User ID not found in the webhook payload'}), 400

    # Check if user exists, if not create a new one
    user = user_profiles.find_one({'user_id': user_id})
    if not user:
        user_profiles.insert_one({
            'user_id': user_id,
            'recent_searches': [],
            'saved_locations': []
        })

    return jsonify({'message': 'User ID processed successfully'}), 200

@user_bp.route('/add_search', methods=['POST'])
def add_search():
    data = request.json
    user_id = data.get('user_id')
    search_query = data.get('search_query')

    # Ensure user exists
    user = user_profiles.find_one({'user_id': user_id})
    if not user:
        return jsonify({'error': 'User not found'}), 404

    # Ensure search_query is in the correct format
    if not isinstance(search_query, list) or not all(isinstance(i, str) for i in search_query) or len(search_query) != 2:
        return jsonify({'error': 'Invalid search query format'}), 400

    # Add search query to recent searches
    user_profiles.update_one(
        {'user_id': user_id},
        {'$push': {'recent_searches': search_query}}
    )
    return jsonify({'message': 'Search added successfully'}), 201

@user_bp.route('/add_location', methods=['POST'])
def add_location():
    data = request.json
    user_id = data.get('user_id')
    location = data.get('location')

    # Ensure user exists
    user = user_profiles.find_one({'user_id': user_id})
    if not user:
        return jsonify({'error': 'User not found'}), 404

    # Add location to saved locations
    user_profiles.update_one(
        {'user_id': user_id},
        {'$push': {'saved_locations': location}}
    )
    return jsonify({'message': 'Location added successfully'}), 201

@user_bp.route('/get_user_data/<user_id>', methods=['GET'])
def get_user_data(user_id):
    user = user_profiles.find_one({'user_id': user_id})
    if not user:
        return jsonify({'error': 'User not found'}), 404

    return jsonify({
        'user_id': user['user_id'],
        'recent_searches': user['recent_searches'],
        'saved_locations': user['saved_locations']
    })

@user_bp.route('/delete_location', methods=['POST'])
def delete_location():
    data = request.json
    user_id = data.get('user_id')
    location = data.get('location')

    # Ensure user exists
    user = user_profiles.find_one({'user_id': user_id})
    if not user:
        return jsonify({'error': 'User not found'}), 404

    # Remove location from saved locations
    user_profiles.update_one(
        {'user_id': user_id},
        {'$pull': {'saved_locations': location}}
    )
    return jsonify({'message': 'Location removed successfully'}), 200

@user_bp.route('/delete_recent/<user_id>', methods=['POST'])
def delete_recent(user_id):
    data = request.get_json()
    location_pair = data.get('location_pair')

    # Find the user
    user = user_profiles.find_one({'user_id': user_id})
    if not user:
        return jsonify({'error': 'User not found'}), 404

    # Remove the location pair from the recent searches
    if location_pair in user['recent_searches']:
        user['recent_searches'].remove(location_pair)
        user_profiles.update_one({'user_id': user_id}, {'$set': {'recent_searches': user['recent_searches']}})
        return jsonify({'message': 'Recent search deleted successfully'}), 200
    else:
        return jsonify({'error': 'Location pair not found in recent searches'}), 404
