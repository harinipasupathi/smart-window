import React from 'react';
import './styles.css';
import window from './icons/window-iot.png';

function AboutUs() {
  return (
    <div className="about-container">
      <div className="about-text">
        <h2>About Us</h2>
        <p>
          Welcome to Smart Window Monitor, where we provide real-time weather monitoring for areas with connected smart windows. Stay updated on temperature, humidity, and sunlight intensity to optimize comfort.
        </p>
      </div>
      <div className="about-image">
        <img src={window} alt="Smart Window" />
      </div>
    </div>
  );
}

export default AboutUs;
