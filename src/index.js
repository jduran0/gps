const express = require('express');
const engine = require('ejs-mate');
const path = require('path');
const WebSocket = require('ws');
const http = require('http');
const socketIO = require('socket.io');

const app = express();
app.engine('ejs', engine);
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));
app.use(express.static(path.join(__dirname, 'public')));

const server = http.createServer(app);
const io = socketIO(server);

const wss = new WebSocket.Server({ port: 1234 });

wss.on('connection', function connection(ws) {
  console.log('Cliente conectado');


  ws.on('message', function incoming(message) {
    console.log('Mensaje recibido: %s', message);
    const coords = JSON.parse(message);

    // Emitir los datos GPS al cliente web
    io.emit('gpsData', coords);
  });

  ws.send('Hola, ESP-32!');
});

console.log('Servidor WebSocket escuchando en el puerto 1234');

app.get('/', (req, res) => {
  res.render('index');
});

server.listen(3000, () => {
  console.log('Servidor Express y WebSocket escuchando en el puerto 3000');
});
