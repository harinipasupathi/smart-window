import React, { useState, useEffect } from 'react';
import { db, ref, onValue } from './firebaseConfig';
import './styles.css';

function History() {
  const [historyData, setHistoryData] = useState([]);

  useEffect(() => {
    const historyRef = ref(db, 'sensor/weather');
    onValue(historyRef, (snapshot) => {
      const data = snapshot.val();
      const dataList = [];

      for (let key in data) {
        dataList.push(data[key]);
      }

      setHistoryData(dataList);
    });
  }, []);

  return (
    <div className="history-container">
      <h1>History Data</h1>
      <div className="table-readings">
        <table>
          <thead>
            <tr>
              <th>Timestamp</th>
              <th>Temperature (°C)</th>
              <th>Humidity (%)</th>
              <th>Light Intensity (lux)</th>
              <th>Status</th>  
            </tr>
          </thead>
          <tbody>
            {historyData.map((record, index) => (
              <tr key={index}>
                <td>{new Date(record.timestamp).toLocaleString()}</td>
                <td>{record.temperature}</td>
                <td>{record.humidity}</td>
                <td>{record.lightIntensity}</td>
                <td>{record.status}</td>  
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}

export default History;
