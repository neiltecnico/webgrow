#include <EEPROM.h>
#include <SPI.h>
#include <DS1307.h>      //Carrega a biblioteca do RTC DS1307
#include <Ethernet.h>
#include <String.h>
#include <Wire.h>
#include "RTClib.h"
#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#define DHTPIN 4     // O pino onde o DHT vai esta conectado
#define DHTTYPE DHT22   // DHT 22 - RHT03 - AM2302
#define ONE_WIRE_BUS 3  // Conectar o pino central dos sensores ao pino 2 do Arduino
//Modulo RTC DS1307 ligado as portas A4 e A5 do Arduino
DS1307 rtc(A4, A5);
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1 = { 0x28, 0xFF, 0xD4, 0xB4, 0x61, 0x16, 0x03, 0x3C};

byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0xA6, 0x09 }; // mac address
byte ip[] = {192, 168, 0, 100 }; // ip lan
byte gateway[] = {192, 168, 0, 1 }; // gateway
byte subnet[] = {255, 255, 255, 0 }; //subnet mask
EthernetServer server(80); //server port
DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 RTC;
/********VARIÁVEIS PARA CONTROLE DO SENSOR DE FLUXO******/
float vazao; //Variável para armazenar o valor em L/min
float media=0; //Variável para tirar a média a cada 1 minuto
int contaPulso; //Variável para a quantidade de pulsos
int i=0; //Variável para contagem
/*float vazao; //Variável para armazenar o valor em L/min
float media = 0; //Variável para fazer a média
int contaPulso; //Variável para a quantidade de pulsos
int i = 0; //Variável para segundos
int Min = 00; //Variável para minutos
float Litros = 0; //Variável para Quantidade de agua
float MiliLitros = 0; //Variavel para Conversão
/********************************************************/
float temp1,  temp2, umid_ar, ecc ;
int umidade_solo;
int saida1 = 8;  //int saida1 = 2; //
int saida2 = 9;  //int saida2 = 3; //
int saida3 = 10; //int saida3 = 4; //
int saida4 = 11; //int saida4 = 5; //
int s1 = EEPROM.read(1);
int s2 = EEPROM.read(2);
int s3 = EEPROM.read(3);
int s4 = EEPROM.read(4);

String readString;

void setup(){

  rtc.halt(false);  //Aciona o relogio
  //As linhas abaixo setam a data e hora do modulo
  //e podem ser comentada apos a primeira utilizacao
  // rtc.setDOW(WEDNESDAY);      //Define o dia da semana
 // rtc.setTime(19, 41, 0);    //Define o horario
 // rtc.setDate(18, 7, 2018);   //Define o dia, mes e ano
  //Definicoes do pino SQW/Out
  rtc.setSQWRate(SQW_RATE_1);
  rtc.enableSQW(true);

  Serial.begin(9600);

  pinMode(2, INPUT);
  attachInterrupt(0, incpulso, RISING); //Configura o pino 2(Interrupção 0) para trabalhar como interrupção

  sensors.begin();

  sensors.setResolution(sensor1, 10);   // set the resolution to 10 bit (good enough?)

  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();

  dht.begin()
  ;
  Serial.begin(9600);
  pinMode(saida1, OUTPUT);
  pinMode(saida2, OUTPUT);
  pinMode(saida3, OUTPUT);
  pinMode(saida4, OUTPUT);
 }

 void loop(){
sensor_temperatura1();
sensor_temperatura2();
sensor_solo();
sensor_fluxo();
displayWeb();
}

void displayWeb(){
  s1 == EEPROM.read(1);
  s2 == EEPROM.read(2);
  s3 == EEPROM.read(3);
  s4 == EEPROM.read(4);
  Serial.println(s1);
  Serial.println(s2);
  Serial.println(s3);
  Serial.println(s4);
  Serial.println(" teste de impressão ");
EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
        }
        //RECEBE A INFORMAÇÃO PELA URL PARA ENVIAR AS INFORMAÇÕES PARA AS PORTAS UTILIZADAS
        if (c == '\n') {
          Serial.println(readString);
          if(readString.indexOf("?S1Ligado") >0) {
            EEPROM.write(1, 1);
            digitalWrite(saida1, HIGH);
          }
          if(readString.indexOf("?S1Desligado") >0) {
            EEPROM.write(1, 0);
            digitalWrite(saida1, LOW);
          }
          if(readString.indexOf("?S2Ligado") >0) {
            EEPROM.write(2, 1);
            digitalWrite(saida2, HIGH);
          }
          if(readString.indexOf("?S2Desligado") >0) {
            EEPROM.write(2, 0);
            digitalWrite(saida2, LOW);
          }
          if(readString.indexOf("?S3Ligado") >0) {
            EEPROM.write(3, 1);
            digitalWrite(saida3, HIGH);
          }
          if(readString.indexOf("?S3Desligado") >0) {
            EEPROM.write(3, 0);
            digitalWrite(saida3, LOW);
          }
          if(readString.indexOf("?S4Ligado") >0) {
            EEPROM.write(4, 1);
            digitalWrite(saida4, HIGH);
          }
          if(readString.indexOf("?S4Desligado") >0) {
            EEPROM.write(4, 0);
            digitalWrite(saida4, LOW);
          }
          //S serve para apontar no codigo html se esta ligado ou desligado;
          //int S1 = digitalRead(saida1);
          //int S2 = digitalRead(saida2);
          //int S3 = digitalRead(saida3);
          //int S4 = digitalRead(saida4);
      int S1 = EEPROM.read(1);
          int S2 = EEPROM.read(2);
          int S3 = EEPROM.read(3);
          int S4 = EEPROM.read(4);
      Serial.println(S1);
      Serial.println(S2);
      Serial.println(S3);
      Serial.println(S4);
          client.println(F("HTTP/1.1 200 OK")); //send new page
          client.println(F("Content-Type: text/html"));
          client.println();
          client.println(F("<HTML>"));
          client.println(F("<HEAD>"));
          client.println(F("<title>Controle do Grow</title>"));
          //client.println(F("<link rel='stylesheet' href='http://noxweb.com.br/arduinoAquario/interface.css'>"));
            //  client.println(F("<title>Controle da Horta</title>"));
           //   client.println(F("<style>table {background-color: #FFFFFF;}th {background-color: #3E4551;color: #FFFFFF;}</style>"));
          //
          client.println(F("<META HTTP-EQUIV='refresh' CONTENT='2'>")); //reload de 10 segundos
          client.println(F("<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"));
          client.println(F("<link href='http://noxweb.com.br/bootstrap/3.3.1/css/bootstrap.min.css' rel='stylesheet'></link>"));
//codigo js para informar data e hora
          client.println(F("<script language=javascript type='text/javascript'>"));
          client.println(F("dayName = new Array ('domingo', 'segunda', 'terça', 'quarta', 'quinta', 'sexta', 'sábado')"));
          client.println(F("monName = new Array ('janeiro', 'fevereiro', 'março', 'abril', 'maio', 'junho', 'julho', 'agosto', 'setembro', 'outubro', 'novembro', 'dezembro')"));
          client.println(F("now = new Date"));
          client.println(F("</script>"));
          client.println(F("</head>"));
          client.println(F("<body>"));
// menu
          client.println(F("<nav class='navbar navbar-default navbar-fixed-top' role='navigation'>"));
          client.println(F("<div class='navbar-header'>"));
          client.println(F("<a class='navbar-brand' rel='home' href=''>MonitorHorta</a>"));
          client.println(F("</div>"));
          client.println(F("<div class='collapse navbar-collapse'>"));
          client.println(F("<ul class='nav navbar-nav'>"));
          client.println(F("<li><a href='#'>Menu Principal</a></li>"));
         // client.println(F("<li><a target='_blank' href='http://monitoraquarium.com.br/landing/index.php'>MonitorAquarium</a></li>"));
         // client.println(F("<li><a target='_blank' href='http://blog.monitoraquarium.com.br/'>Blog</a></li>"));
          client.println(F("</ul>"));
          client.println(F("</div>"));
          client.println(F("</nav>"));
// final menu
//painel de botões
          client.println(F("<div class='container-fluid'>"));
          client.println(F("<div class='col-sm-4'>"));  //  client.println(F("<div class='col-sm-8'>"));
         client.println(F("<h2>Painel controle</h2>"));
          client.println(F("<div class='panel panel-default'>"));
          client.println(F("<div class='panel-heading'>LEITURA DOS SENSORES DO GROW</div>"));
          client.println(F("<div class='panel-body'>"));
//Mostra data com JS
          client.println(F("<script language=javascript type='text/javascript'>"));
          client.println(F("now = new Date"));
          client.println(F("document.write ('Hoje é ' + dayName[now.getDay() ] + ', ' + now.getDate() + ' de ' + monName[now.getMonth() ] + ' de ' + now.getFullYear() )"));
//   client.println(F("document.write ('Hoje é ' + dayName[now.getDay() ] + ', ' + now.getDate() + ' de ' + monName[now.getMonth() ] + ' de ' + now.getFullYear() + '  e são ' + now.getHours() + ' horas e  ' + now.getMinutes()+ ' minutos ' + now.getSeconds() + ' segundos  ' )"));
          client.println(F("</script>"));
          client.println(F("<table class='table table-bordered'>"));
          client.println(F("<tbody>"));
//*************************************************************************************************************************
          client.println(F("<tr><td width=230px>Data e Hora do controlador "));
          client.println(F("</td><td>"));
          client.println("Dia");
          client.println(rtc.getDateStr(FORMAT_LONG));
          client.println("         ");
          client.println(rtc.getTimeStr());
          client.println("Horas");
          client.println(F("</td></tr>"));
          client.println(F("<tr><td width=230px>Temperatura do Ambiente"));
          client.println(F("</td><td>"));
          client.println(temp2);
          client.println("ºC");
          client.println(F("</td></tr>"));
          client.println(F("<tr><td width=175px>Temperatura da Água"));
          client.println(F("</td><td>"));
          client.println(temp1);
          client.println("ºC");
          client.println(F("</td></tr>"));
          client.println(F("<tr><td width=175px>Condutividade so Solo"));
          client.println(F("</td><td>"));
          client.println(ecc);
          client.println("%");
          client.println(F("</td></tr>"));
          client.println(F("<tr><td width=175px>Vazão da irrigação"));
          client.println(F("</td><td>"));
          client.println(vazao);
          client.println("Litros ");
          client.println(F("</td><td>"));
          client.println(media);
          //client.println(F("</td><td>"));
          client.println("L/Min");
          client.println(F("</td></tr>"));
          client.println(F("<tr><td width=175px>Dióxido de Carbono"));
          client.println(F("</td><td>"));
          client.println(temp2);
          client.println("ppm");
          client.println(F("</td></tr>"));
          client.println(F("<tr><td width=175px>Umidade do Ar"));
          client.println(F("</td><td>"));
          client.println(umid_ar);
          client.println("%");
          client.println(F("</td></tr>"));
          client.println(F("</td></tr>"));
          client.println(F("</tbody>"));
          client.println(F("</table>"));
          client.println(F("</div>"));
          client.println(F("</div>"));
          client.println(F("<hr>"));
          //Final painel de botões
//****************************************************************************************************************************/
          //painel de retorno
          client.println(F("<div class='panel panel-default'>"));
          client.println(F("<div class='panel-heading'>CONTROLE DA HORTA</div>"));
          client.println(F("<div class='panel-body'>"));
          client.println(F("<table class='table table-bordered'>"));
          client.println(F("<tbody>"));
          //Saida 1
          client.println(F("<tr><td width=175px>Iluminacao.: ->"));
           EEPROM.read(0);
          //if(S1 == HIGH){
      if(S1 == 1){
            client.println(F("Ligado"));
            client.println(F("</td><td>"));
            client.println(F("<a class='btn btn-danger btn-lg' href='?S1Desligado'>Desligar</buttLigado>"));
          }else{
            client.println(F("Desligado"));
            client.println(F("</td><td>"));
            client.println(F("<a class='btn btn-success btn-lg' href='?S1Ligado'>Ligar</buttLigado>"));
          }
          client.println(F("</td></tr>"));
          //Saida 2
          client.println(F("<tr><td width=175px>Ventilacao.: ->"));
           EEPROM.read(1);
          //if(S2 == HIGH){
      if(S2 == 1){
            client.println(F("Ligado"));
            client.println(F("</td><td>"));
            client.println(F("<a class='btn btn-danger btn-lg' href='?S2Desligado'>Desligar</buttLigado>"));
          }else{
            client.println(F("Desligado"));
            client.println(F("</td><td>"));
            client.println(F("<a class='btn btn-success btn-lg' href='?S2Ligado'>Ligar</buttLigado>"));
          }
          client.println(F("</td></tr>"));
          //Saida 3
          client.println(F("<tr><td width=175px>Exautao....: ->"));
           EEPROM.read(2);
          //if(S3 == HIGH){
      if(S3 == 1){
            client.println(F("Ligado"));
            client.println(F("</td><td>"));
            client.println(F("<a class='btn btn-danger btn-lg' href='?S3Desligado'>Desligar</buttLigado>"));
          }else{
            client.println(F("Desligado"));
            client.println(F("</b></td><td>"));
            client.println(F("<a class='btn btn-success btn-lg' href='?S3Ligado'>Ligar</buttLigado>"));
          }
          client.println(F("</td></tr>"));
          //Saida 4
          client.println(F("<tr><td width=175px>  Irrigacao...: ->"));
          EEPROM.read(3);
          //if(S4 == HIGH){
      if(S4 == 1){
            client.println(F("Ligado"));
            client.println(F("</td><td>"));
            client.println(F("<a class='btn btn-danger btn-lg' href='?S4Desligado'>Desligar</buttLigado>"));
          }else{
            client.println(F("Desligado"));
            client.println(F("</td><td>"));
            client.println(F("<a class='btn btn-success btn-lg' href='?S4Ligado'>Ligar</buttLigado>"));
          }
          client.println(F("</tbody>"));
          client.println(F("</table>"));
          client.println(F("</div>"));
          client.println(F("</div>"));
          //client.println(F("</div>"));
          client.println(F("<hr>"));
//Final painel de retorno
          client.println(F("</body>"));
          client.println(F("</html>"));

          delay(1);
//stopping client
          client.stop();

//clearing string for next read
          readString="";
        }
      }
    }
  }
}
void sensor_temperatura1(){
  delay(1000); // delay para garantir no minimo 2s entre cada uma das leituras
  sensors.requestTemperatures();
  temp1 = sensors.getTempC(sensor1); // leitura de temperatura da água
}
void sensor_temperatura2(){
  delay(2000); // delay para garantir no minimo 2s entre cada uma das leituras
  umid_ar = dht.readHumidity(); // leitura de umidade
  temp2 = dht.readTemperature(); // leitura da temperatura
}
void sensor_solo() {
  umidade_solo = analogRead(A0);
  ecc = map(umidade_solo, 1023, 0, 0, 100);
}
void sensor_fluxo () {
  contaPulso = 0;   //Zera a variável para contar os giros por segundos
  sei();      //Habilita interrupção
  delay (1000); //Aguarda 1 segundo
  cli();      //Desabilita interrupção
  vazao = contaPulso / 7.5; //Converte para L/min
  media=media+vazao; //Soma a vazão para o calculo da media
  i++;
  Serial.print(vazao); //Imprime na serial o valor da vazão
  Serial.print(" L/min - "); //Imprime L/min
  Serial.print(i); //Imprime a contagem i (segundos)
  Serial.println("s"); //Imprime s indicando que está em segundos
  if(i==60){
    media = media/60; //Tira a media dividindo por 60
  //Serial.print("\nMedia por minuto = "); //Imprime a frase Media por minuto =
  // Serial.print(media); //Imprime o valor da media
   // Serial.println(" L/min - "); //Imprime L/min
    media = 0; //Zera a variável media para uma nova contagem
    i=0; //Zera a variável i para uma nova contagem
    Serial.println("\n\nInicio\n\n"); //Imprime Inicio indicando que a contagem iniciou
  }
}
void incpulso () {
 contaPulso++; //Incrementa a variável de pulsos
}
