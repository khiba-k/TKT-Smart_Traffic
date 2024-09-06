// MainPage.jsx
import React, { useState, useEffect } from 'react';
import "bootstrap/dist/css/bootstrap.min.css";
import '../App.css';
import Header from './Header';
import SearchBar from './SearchBar';
import Content from './Content';
import axios from 'axios';
import { useAuth } from '@clerk/clerk-react';


const MainPage = () => {
  const [currentLocation, setCurrentLocation] = useState('');
  const [destination, setDestination] = useState('');
  const [locationName, setLocationName] = useState('');
  const [distances, setDistances] = useState([]);
  const [trafficSpeeds, setTrafficSpeeds] = useState({ speed1: 0, speed2: 0 });
  const [activeTab, setActiveTab] = useState("recents");
  const [isTabColumnVisible, setIsTabColumnVisible] = useState(false);
  const [isBlackAndWhiteTheme, setIsBlackAndWhiteTheme] = useState(false);
  const [currentTime, setCurrentTime] = useState('');
  const [eta, setEta] = useState({ ETA1: '', ETA2: '' });
  const [duration, setDuration] = useState({ duration1: 0, duration2: 0 });


  const { isLoaded, userId } = useAuth();

  useEffect(() => {
    if (window.innerWidth >= 992) {
      setIsTabColumnVisible(true);
    }
  }, []);

  useEffect(() => {
    const fetchTrafficSpeeds = async () => {
      try {
        const [res1, res2] = await Promise.all([
          axios.get('http://127.0.0.1:5000/sensors/get_speed/ultra_sonic1'),
          axios.get('http://127.0.0.1:5000/sensors/get_speed/ultra_sonic2'),
        ]);

        const speed1 = parseFloat(res1.data.latest_reading.speed);
        const speed2 = parseFloat(res2.data.latest_reading.speed);

        setTrafficSpeeds({ speed1, speed2 });
      } catch (error) {
        console.error('Error fetching traffic speeds:', error);
      }
    };

    // const fetchDistances = async () => {
    //   try {
    //     // Replace with your API call to get distances
    //     const response = await axios.get('http://127.0.0.1:5000/get_distances');
    //     setDistances(response.data);
    //   } catch (error) {
    //     console.error('Error fetching distances:', error.response ? error.response.data : error.message);
    //   }
    // };

    fetchTrafficSpeeds();
    // fetchDistances();
    const intervalId = setInterval(fetchTrafficSpeeds, 10000);
    return () => clearInterval(intervalId);
  }, []);

  useEffect(() => {
    const calculateETA = () => {
      const now = new Date();
  
      if (trafficSpeeds.speed1 && distances[0]) {
        const distance1 = parseFloat(distances[0]);
        const speed1 = trafficSpeeds.speed1;
        const time1 = (distance1 / speed1) * 60; // Time in minutes
        const eta1Date = new Date(Date.now() + time1 * 60000);
        const eta1 = eta1Date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
  
        const duration1 = Math.max(Math.round((eta1Date - now) / 60000), 0);
  
        setEta(prev => ({ ...prev, ETA1: eta1 }));
        setDuration(prev => ({ ...prev, duration1 }));
      }
  
      if (trafficSpeeds.speed2 && distances[1]) {
        const distance2 = parseFloat(distances[1]);
        const speed2 = trafficSpeeds.speed2;
        const time2 = (distance2 / speed2) * 60; // Time in minutes
        const eta2Date = new Date(Date.now() + time2 * 60000);
        const eta2 = eta2Date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
  
        const duration2 = Math.max(Math.round((eta2Date - now) / 60000), 0);
  
        setEta(prev => ({ ...prev, ETA2: eta2 }));
        setDuration(prev => ({ ...prev, duration2 }));
      }
    };
  
    calculateETA();
  }, [trafficSpeeds, distances]);

  const handleSaveLocation = async () => {
    try {
      if (!isLoaded || !userId) {
        console.error('User not authenticated or data not loaded');
        return;
      }

      const response = await axios.post('http://127.0.0.1:5000/users/add_location', {
        user_id: userId, // Use the actual user ID
        location: destination,
      });

      console.log(response.data.message);
    } catch (error) {
      console.error('Error saving location:', error.response ? error.response.data : error.message);
    }
  };

  const toggleTabColumn = () => {
    setIsTabColumnVisible(prev => !prev);
  };

  const handleThemeChange = () => {
    setIsBlackAndWhiteTheme(prev => !prev);
  };

  return (
    <div id="main" className={`bg ${isBlackAndWhiteTheme ? "black-and-white-theme" : ""}`}>
      <Header />
      <SearchBar
        currentLocation={currentLocation}
        setCurrentLocation={setCurrentLocation}
        destination={destination}
        setDestination={setDestination}
        locationName={locationName}
        handleSaveLocation={handleSaveLocation}
        toggleTabColumn={toggleTabColumn}
        isTabColumnVisible={isTabColumnVisible}
      />
      <Content
        currentLocation={currentLocation}
        destination={destination}
        setLocationName={setLocationName}
        setDistances={setDistances}
        distances={distances}
        trafficSpeeds={trafficSpeeds}
        ETA1={eta.ETA1}
        ETA2={eta.ETA2}
        duration1={duration.duration1}
        duration2={duration.duration2}
        isTabColumnVisible={isTabColumnVisible}
        toggleTabColumn={toggleTabColumn}
        activeTab={activeTab}
        setActiveTab={setActiveTab}
        handleThemeChange={handleThemeChange}
      />
    </div>
  );
};

export default MainPage;
