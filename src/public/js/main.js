const map = L.map('map-template').setView([0, 0], 2);
const titleURL = 'https://maps.wikimedia.org/osm-intl/{z}/{x}/{y}.png';
L.tileLayer(titleURL).addTo(map);

const socket = io();
let marker;

socket.on('connect', () => {
  console.log('Conectado al servidor WebSocket');
});

socket.on('gpsData', (data) => {
  if (data.lat && data.lng) {
    if (marker) {
      map.removeLayer(marker);
    }
    marker = L.marker([data.lat, data.lng]);

    const popupContent = `
      <b>Latitud:</b> ${data.lat}<br>
      <b>Longitud:</b> ${data.lng}<br>
      <b>Grados Latitud:</b> ${data.latDeg}<br>
      <b>Minutos Latitud:</b> ${data.latMin}<br>
      <b>Grados Longitud:</b> ${data.lngDeg}<br>
      <b>Minutos Longitud:</b> ${data.lngMin}<br>
      <b>Velocidad:</b> ${data.speed} km/h<br>
      <b>Altitud:</b> ${data.altitude} m<br>
      <b>Fecha:</b> ${data.year}/${data.month}/${data.day}<br>
      <b>Hora:</b> ${data.hour}:${data.minute}:${data.second}<br>
      <b>Satélites:</b> ${data.satellites}
    `;

    marker.bindPopup(popupContent);
    map.addLayer(marker);
  }
});
