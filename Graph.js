import React, { useState, useEffect } from 'react';
import { db, ref, onValue } from './firebaseConfig';
import { Line } from 'react-chartjs-2';
import { Chart as ChartJS, CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend } from 'chart.js';
import './styles.css';

// Register chart.js components
ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend);

const Graph = () => {
  const [sensorData, setSensorData] = useState({ temperatures: [], humidities: [], illuminance: [] });
  const [latestSensorData, setLatestSensorData] = useState(null); 

  useEffect(() => {
    const dataRef = ref(db, 'sensor/weather'); 
    onValue(dataRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        const keys = Object.keys(data);
        const lastTenKeys = keys.slice(-25);
        
       
        const temperatures = lastTenKeys.map(key => data[key].temperature);
        const humidities = lastTenKeys.map(key => data[key].humidity);
        const illuminance = lastTenKeys.map(key => data[key].lightIntensity);        
        
        setSensorData({
          temperatures,
          humidities,
          illuminance
        });
       
        const lastKey = keys[keys.length - 1];
        setLatestSensorData(data[lastKey]);
      }
    });
  }, []);

   const chartData = {
    labels: Array.from({ length: sensorData.temperatures.length }, (_, i) => i + 1),     datasets: [
      {
        label: 'Temperature (°C)',
        data: sensorData.temperatures, 
        borderColor: 'rgba(75,192,192,1)',
        backgroundColor: 'rgba(75,192,192,0.2)',
        fill: true,
      },
      {
        label: 'Humidity (%)',
        data: sensorData.humidities
        borderColor: 'rgba(153,102,255,1)',
        backgroundColor: 'rgba(153,102,255,0.2)',
        fill: true,
      },
      {
        label: 'Light Illuminance (lx)',
        data: sensorData.illuminance, 
        borderColor: 'rgba(255,206,86,1)',
        backgroundColor: 'rgba(255,206,86,0.2)',
        fill: true,
      }
    ]
  };

  const chartOptions = {
    responsive: true,
    plugins: {
      legend: {
        position: 'top',
      },
      title: {
        display: true,
        text: 'Temperature, Humidity, and Light Illuminance Over Time',
      },
    },
    scales: {
      x: {
        title: {
          display: true,
          text: 'Reading Index',
        },
      },
      y: {
        title: {
          display: true,
          text: 'Sensor Values',
        },
      },
    },
  };

  return (
    <div>
      <h1>Sensor Data Dashboard</h1>
          {latestSensorData ? (
        <div>
          <p><strong>Latest Temperature:</strong> {latestSensorData.temperature} °C</p>
          <p><strong>Latest Humidity:</strong> {latestSensorData.humidity} %</p>
          <p><strong>Latest Light Illuminance:</strong> {latestSensorData.lightIntensity} lx</p>
        </div>
      ) : (
        <p>Loading latest sensor data...</p>
      )}
      
          {sensorData.temperatures.length > 0 ? (
        <div>
          <Line data={chartData} options={chartOptions} />
        </div>
      ) : (
        <p>Loading chart...</p>
      )}
    </div>
  );
};

export default Graph;
