import React, { useEffect, useState } from 'react';
import { GoogleMap, LoadScript, DirectionsService, DirectionsRenderer, DistanceMatrixService } from '@react-google-maps/api';
import "../App.css";
import { useAuth } from '@clerk/clerk-react';
import axios from "axios";
import fetchRecents from "./Tabs";

const MyGoogleMap = ({ currentLocation, destination, setLocationName, setDistances }) => {
  const mapStyles = {
    height: '500px',
    width: '100%',
  };

  const [directions, setDirections] = useState(null);
  const [center, setCenter] = useState({ lat: -29.311667, lng: 27.481389 });

  useEffect(() => {
    if (currentLocation && destination) {
      calculateDirections();
    }
  }, [currentLocation, destination]);

  const calculateDirections = () => {
    const directionsService = new window.google.maps.DirectionsService();
    directionsService.route(
      {
        origin: currentLocation,
        destination: destination,
        travelMode: 'DRIVING',
        provideRouteAlternatives: true, // Request multiple routes
      },
      (result, status) => {
        if (status === 'OK') {
          setDirections(result);
          setLocationName(`${currentLocation} to ${destination}`);
          calculateDistances(result.routes);
          saveSearchToDatabase(currentLocation, destination);
        } else {
          console.error('Directions request failed due to ' + status);
          setLocationName('Route Not Found');
        }
      }
    );
  };

  const calculateDistances = (routes) => {
    const service = new window.google.maps.DistanceMatrixService();
    const origins = [currentLocation];
    const destinations = routes.map(route => route.legs[0].end_location);

    service.getDistanceMatrix(
      {
        origins,
        destinations,
        travelMode: 'DRIVING',
      },
      (response, status) => {
        if (status === 'OK') {
          const distances = response.rows[0].elements.map(element => element.distance.text);
          setDistances(distances);
        } else {
          console.error('Distance Matrix request failed due to ' + status);
        }
      }
    );
  };

  // Save recent searches
  const { userId } = useAuth();
  const saveSearchToDatabase = async (origin, destination) => {
    try {
      await axios.post('http://127.0.0.1:5000/users/add_search', {
        user_id: userId,  // Replace with actual user ID
        search_query: [origin, destination]
      });
      fetchRecents();
    } catch (error) {
      console.error("Error saving search:", error.response ? error.response.data : error.message);
    }
  };
  const googleMapsApiKey = import.meta.env.VITE_GOOGLE_MAPS_API_KEY;
  return (
    <div>
      <LoadScript googleMapsApiKey={googleMapsApiKey}>
        <GoogleMap
          mapContainerStyle={mapStyles}
          zoom={12}
          center={center} // Default center if no route is displayed
        >
          {directions && directions.routes.map((route, index) => (
            <DirectionsRenderer
              key={index}
              directions={directions}
              routeIndex={index}
              options={{
                polylineOptions: {
                  strokeColor: index === 0 ? 'blue' : 'gray',
                },
              }}
            />
          ))}
        </GoogleMap>
      </LoadScript>
    </div>
  );
};

export default MyGoogleMap;
