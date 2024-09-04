// SearchBar.jsx
import React from 'react';
import IconSaveButton from './IconSaveButton';
import IconTabs from './IconTabs';
import IconLogo from "./IconLogo"

const SearchBar = ({ currentLocation, setCurrentLocation, destination, setDestination, locationName, handleSaveLocation, toggleTabColumn, isTabColumnVisible }) => {
  return (
    <div className="container upper-row">
      <div className="row" id="parent_col">
        <div className="col-2 logo">
          <div style={{ paddingLeft: "23px", color: "#2BAF6A", fontWeight: "100px", width: "100%", marginLeft: "12px" }}>
            <IconLogo /><p style={{ fontFamily: "sans-serif" }}>TKT Smart Traffic</p>
          </div>
        </div>
        <div className="col-2 input-group-parent" style={{ marginLeft: "0px" }}>
          <div className="input-group mb-3" style={{ width: "100%" }}>
            <button className="btn btn-primary d-lg-none" onClick={toggleTabColumn}>
              {isTabColumnVisible ? <IconTabs /> : <IconTabs />}
            </button>
            <input
              type="text"
              className="form-control"
              placeholder="Current Location"
              aria-label="Current Location"
              value={currentLocation}
              onChange={(e) => setCurrentLocation(e.target.value)}
              id="location_id"
            />
            <input
              type="text"
              className="form-control margin-input2"
              placeholder="Destination"
              aria-label="Destination"
              value={destination}
              onChange={(e) => setDestination(e.target.value)}
              id="destination_id"
            />
            &nbsp;
            <div className="col-1">
              <button style={{ border: "none", borderRadius: "100%" }} onClick={handleSaveLocation}>
                <IconSaveButton />
              </button>
            </div>
            <div className="col">
              <h3 style={{ fontSize: "20px" }}>{locationName}</h3>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default SearchBar;
