import React, { useState } from "react";
import axios from "axios";
import { useAuth } from '@clerk/clerk-react';
import IconRecentsTab from "./IconRecentsTab";
import IconCustomize from "./IconCustomize";
import IconSavedTab from "./IconSavedTab";
import DeleteData from "./DeleteData";
import IconTheme from "./IconTheme";

const Tabs = ({ activeTab, setActiveTab, handleThemeChange }) => {
  const { userId } = useAuth();
  const [savedLocations, setSavedLocations] = useState([]);
  const [recentSearches, setRecentSearches] = useState([]);

  // Saved Locations
  const fetchMarkedLocations = async () => {
    try {
      const user_id = userId;
      const response = await axios.get(`http://localhost:5000/users/get_user_data/${user_id}`);
      if (response.status === 200) {
        const { saved_locations } = response.data;
        setSavedLocations(saved_locations);
        console.log("Saved Locations:", saved_locations);
      } else {
        console.error("Failed to fetch user data");
      }
    } catch (error) {
      console.error("Error fetching marked locations:", error.response ? error.response.data : error.message);
    }
  };

  const handleDelete = async (location) => {
    try {
      await axios.post('http://localhost:5000/users/delete_location', {
        user_id: userId,
        location: location
      });
      // Remove the deleted location from the state
      setSavedLocations(savedLocations.filter(loc => loc !== location));
    } catch (error) {
      console.error("Error deleting location:", error.response ? error.response.data : error.message);
    }
  };

  // Recent Searches
  const fetchRecents = async () => {
    try {
      const response = await axios.get(`http://localhost:5000/users/get_user_data/${userId}`);
      if (response.status === 200) {
        const { recent_searches } = response.data;
        // Limit to the last 4 recent searches
        const limitedRecents = recent_searches.slice(-4);
        setRecentSearches(limitedRecents);
        console.log("Recent Searches:", limitedRecents);
      } else {
        console.error("Failed to fetch recent searches");
      }
    } catch (error) {
      console.error("Error fetching recent searches:", error.response ? error.response.data : error.message);
    }
  };

  const handleDeleteRecent = async (search) => {
    try {
      // Make an API request to delete the recent search
      await axios.post(`http://localhost:5000/users/delete_recent/${userId}`, {
        location_pair: search, // Pass the correct recent search to the backend
      });

      // Update the state by filtering out the deleted recent search
      setRecentSearches(recentSearches.filter((pair) => pair !== search));
    } catch (error) {
      console.error("Error deleting recent search:", error.response ? error.response.data : error.message);
    }
  };


  const handleTabClick = (tab) => {
    setActiveTab(tab);
    if (tab === "marked") {
      fetchMarkedLocations();
    }
    else if (tab === "recents") {
      fetchRecents();
    }
  };

  return (
    <div style={{ display: "flex", flexDirection: "column", alignItems: "center" }}>
      <ul
        className="list-group mg"
        style={{ width: "200px", border: "none", borderRadius: "0%", paddingTop: "48px" }}
      >
        <li
          className={`list-group-item ${activeTab === "recents" ? "current-tab" : ""} custom-list-group`}
          aria-current={activeTab === "recents"}
          onClick={() => handleTabClick("recents")}
        >
          <IconRecentsTab />&nbsp;&nbsp;&nbsp;Recent
        </li>
        <li
          className={`list-group-item ${activeTab === "marked" ? "current-tab" : ""} custom-list-group`}
          onClick={() => handleTabClick("marked")}
        >
          <IconSavedTab />&nbsp;&nbsp;&nbsp;Marked
        </li>
        <li
          className={`list-group-item ${activeTab === "Customize" ? "current-tab" : ""} custom-list-group`}
          onClick={() => setActiveTab("Customize")}
        >
          <IconCustomize />&nbsp;&nbsp;&nbsp;Customize
        </li>
      </ul>
      <div className="row" style={{ paddingTop: "12px" }}>
        <div className="card display-card" style={{ width: "200px" }}>
          <ul className="list-group list-group-flush">
            {activeTab === "recents" && (
              <>
                {recentSearches.length > 0 ? (
                  recentSearches.map((search, index) => (
                    <li key={index} className="list-group-item custom-list-group2 list-group">
                      <div style={{ display: "grid", gridTemplateColumns: "1fr auto" }}>
                        {`${search[0]} to ${search[1]}`}
                        <DeleteData onDelete={() => handleDeleteRecent(search)} />
                      </div>
                    </li>
                  ))
                ) : (
                  <li className="list-group-item custom-list-group2">No recent searches</li>
                )}
              </>
            )}
            {activeTab === "marked" && (
              <>
                {savedLocations.length > 0 ? (
                  savedLocations.map((location, index) => (
                    <li key={index} className="list-group-item custom-list-group2 list-group">
                      <div style={{ display: "grid", gridTemplateColumns: "1fr auto" }}>
                        {location || 'Unnamed Location'}
                        <DeleteData onDelete={() => handleDelete(location)} />
                      </div>
                    </li>
                  ))
                ) : (
                  <li className="list-group-item custom-list-group2">No saved locations</li>
                )}
              </>
            )}
            {activeTab === "Customize" && (
              <>
                <li className="list-group-item custom-list-group2">
                  <h5>Customize</h5>
                </li>
                <li className="list-group-item custom-list-group2">
                  <button style={{ border: "none", backgroundColor: "#D9D9D9" }} onClick={handleThemeChange}>
                    <IconTheme />
                  </button>
                </li>
              </>
            )}
          </ul>
        </div>
      </div>
    </div>
  );
};

export default Tabs;
