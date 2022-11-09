#include <Arduino.h>
const char indexHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html lang="pt-BR">
<html>

<head>
    <meta charset="utf-8">
    <title>Cavalo Vendado Sat</title>
    <link rel="stylesheet" href="style.css">
    <script src="script.js"></script>
</head>

<body>
    <div class="header">
        <h1>Equipe Cavalo Vendado</h1>
    </div>
    <p id="updateCounter"> Sem informação sobre atualizações.</p>
    <p>Informações dos sensores: </p>
    <div class="flex-container">
        <div>
            <p>Bateria</p>
            <p id="bateria">data</p>
        </div>
        <div>
            <p>Temperatura</p>
            <p id="temperatura">data</p>
        </div>
        <div>
            <p>Pressão</p>
            <p id="pressao">data</p>
        </div>
        <div>
            <p>Acelerômetro</p>
            <p id="acelerometro">data</p>
        </div>
        <div>
            <p>Giroscópio</p>
            <p id="giroscopio">data</p>
        </div>
        <div>
            <p>Magnetômetro</p>
            <p id="mag">data</p>
        </div>
        <div>
            <p>CO2</p>
            <p id="co2">data</p>
        </div>
        <div>
            <p>Humidade</p>
            <p id="hum">data</p>
        </div>
        <div>
            <p>nº de fotos</p>
            <p id="fotos">data</p>
        </div>
    </div>
    <div class="viewport">
        <img src="frame.jpg" alt="imagem obtida do satélite" id="viewportImg"/>
    </div>
</body>

</html>
)rawliteral";
const char indexJS[] PROGMEM = R"rawliteral(
socket = new WebSocket("ws://192.168.4.1/ws")

socket.addEventListener('close', (event) => {
    statusText = document.getElementById("updateCounter")
    statusText.innerHTML = "Satélite desconectado!";
});
socket.addEventListener('message', (event) => {
    sateliteData = JSON.parse(event.data);
    console.log(sateliteData);
    if(sateliteData != null){
        statusText = document.getElementById("updateCounter")
        bat = document.getElementById("bateria")
        temp = document.getElementById("temperatura")
        bar = document.getElementById("pressao")
        accel = document.getElementById("acelerometro")
        gyro = document.getElementById("giroscopio")
        mag = document.getElementById("mag")
        co = document.getElementById("co2")
        hum = document.getElementById("hum")
        pics = document.getElementById("fotos")
        image = document.getElementById("viewportImg")
        newImage.src = "/frame.jpg?" + new Date().getTime();
        statusText.innerHTML = "Dados atualizados em: " + Date.toString(Date.now());
        bat.innerHTML = sateliteData.bateria + "V"
        temp.innerHTML = sateliteData.temperatura + "°c"
        bar.innerHTML = sateliteData.pressao + "mmHg"
        co.innerHTML = sateliteData.payload.CO2 + "ppm"
        hum.innerHTML = sateliteData.payload.hum + "%"
        pics.innerHTML = sateliteData.payload.fotos
        accel.innerHTML = "x:" + sateliteData.acelerometro[0] + "\ny:" + sateliteData.acelerometro[1] + "\nz:" + sateliteData.acelerometro[2]
        gyro.innerHTML = "x:" + sateliteData.giroscopio[0] + "\ny:" + sateliteData.giroscopio[1] + "\nz:" + sateliteData.giroscopio[2]
        mag.innerHTML = "x:" + sateliteData.payload.mag[0] + "\ny:" + sateliteData.payload.mag[1] + "\nz:" + sateliteData.payload.mag[2]
    }
});
)rawliteral";
const char indexCSS[] PROGMEM = R"rawliteral(
:root {
    background-color: #8b59c4;
}

.flex-container {
    display: flex;
    flex-wrap: wrap;
    background-color: #8b59c4;
    width:100%;
}

.flex-container>div {
    background-color: #e1c9f8;
    width: max-content;
    min-height: 5%;
    line-height: 0.15;
    margin: 5px;
    text-align:start;
    font-size: 30px;
    border-radius: 20px;
    padding: 10px;
    box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);
}
.viewport{
    border-style: solid;
    margin: auto;
    border-radius: 5%;
    width: 100%;
    border-width: 2px;
    border-color: #42297c;
    height: fit-content;
}
.viewport>img{
    width: 100%;
    border-radius:inherit;
}
.header{
    margin: auto;
    border-bottom: solid;
    border-color: #328bff;
}
.header>h1{
    text-align: center;
}
)rawliteral";