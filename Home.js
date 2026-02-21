import React, { useState, useEffect } from 'react';
import { db, ref, onValue, query, limitToLast } from './firebaseConfig';
import './styles.css';
import temperatureIcon from './icons/temperature-icon.png'; 
import humidityIcon from './icons/humidity-icon.png';
import sunlightIcon from './icons/sunlight-icon.png';

function Home() {
  const [data, setData] = useState({
    temperature: 0,
    humidity: 0,
    pressure: 0,
    lightIntensity: 0,
    timestamp: '',
    status: '',  
  });

  useEffect(() => {
    const dataRef = query(ref(db, 'sensor/weather'), limitToLast(1));
    onValue(dataRef, (snapshot) => {
      snapshot.forEach((childSnapshot) => {
        const values = childSnapshot.val();
        
        // Set data including the status received from Firebase
        setData({
          temperature: values.temperature,
          humidity: values.humidity,
          pressure: values.pressure,
          lightIntensity: values.lightIntensity,
          timestamp: values.timestamp,
          status: values.status,  
        });
      });
    });
  }, []);

  const formattedTimestamp = new Date(data.timestamp).toLocaleString();

  return (
    <div className="dashboard-container">
      <h1>Dashboard</h1>
      <div className="data-section">
        <div className="data-box">
          <img src={temperatureIcon} alt="Temperature" className="data-icon" />
          <h5>Temperature</h5>
          <p>{data.temperature}°C</p>
        </div>

        <div className="data-box">
          <img src={humidityIcon} alt="Humidity" className="data-icon" />
          <h5>Humidity</h5>
          <p>{data.humidity}%</p>
        </div>
        
        <div className="data-box">
          <img src={sunlightIcon} alt="Light Intensity" className="data-icon" />
          <h5>Light Intensity</h5>
          <p>{data.lightIntensity} lux</p>
        </div>
      </div>

      <div className="status-section">
        <p>Last updated: {formattedTimestamp}</p>
        <p>Current Weather Status: <strong>{data.status}</strong></p>
      </div>
    </div>
  );
}

export default Home;
