// Content.jsx
import React, { useState } from "react";
import MyGoogleMap from './MyGoogleMap';
import Tabs from './Tabs';
import trafficSpeedIcon from "../assets/speedometerIcon.png";
import IconGlass from './IconGlass';

const Content = ({
  currentLocation,
  destination,
  setLocationName,
  setDistances,
  distances,
  trafficSpeeds,
  ETA1,
  ETA2,
  duration1,
  duration2,
  isTabColumnVisible,
  activeTab,
  setActiveTab,
  handleThemeChange
}) => {
  return (
    <div className="bg full-height parent-card">
      <div className="row lower-row">
        {(isTabColumnVisible || window.innerWidth >= 992) && (
          <div className="col-lg-2 tab-col justify-content-center">
            <div className="row">
              <Tabs
                activeTab={activeTab}
                setActiveTab={setActiveTab}
                handleThemeChange={handleThemeChange}
              />
            </div>
          </div>
        )}
        <div className="col-lg-8 pt-5 map-col" style={{ marginRight: "15px" }}>
          <div className="container-sm">
            <MyGoogleMap
              currentLocation={currentLocation}
              destination={destination}
              setLocationName={setLocationName}
              setDistances={setDistances}
            />
            <div className="pt-5">
              <div className="card w-100" style={{ width: "980px", border: "none" }}>
                <ul className="list-group" style={{ backgroundColor: "white" }}>
                  {distances.length > 0 && (
                    <>
                      <li className="list-group-item justify-content-center suggest-tab">
                        Main North Rd&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                        <img src={trafficSpeedIcon} alt="Traffic Speed Icon" style={{ width: "20px", height: "20px" }} />&nbsp;&nbsp;&nbsp;{Math.round(trafficSpeeds.speed1)} km/h
                        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Distance:&nbsp;&nbsp;{distances[0]} away&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Arrival Time: {ETA1}&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<IconGlass />{duration1} min
                      </li>
                      <li className="list-group-item suggest-tab">
                        Airport Rd &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                        <img src={trafficSpeedIcon} alt="Traffic Speed Icon" style={{ width: "20px", height: "20px" }} />&nbsp;&nbsp;&nbsp;{Math.round(trafficSpeeds.speed2)} km/h &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                        Distance:&nbsp;&nbsp;{distances[1]} away &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Arrival Time: {ETA2}&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<IconGlass />{duration2} min
                      </li>
                    </>
                  )}
                </ul>
              </div>
            </div>
          </div>
        </div>
        <div className="col-lg-2"></div>
      </div>
    </div>
  );
};

export default Content;
