#include <SoftwareSerial.h>
#include <Servo.h>

#define DEBUG true
//#define CH_PD 4

//Cria o objeto para usar a biblioteca Sotfware Serial
//RX do arduino é o pino 10
//TX do arduino é o pino 11
//O TX do esp8266 deve ser ligado no pino 2 do arduino
//O RX do esp8266 deve ser ligado no pino 3 do arduino

//SoftwareSerial esp8266(10, 11);
#define esp8266 Serial3

//declara os pinos do led rgb
int andasup = 12;
int garagem = 11;
int sala = 54;
int jardim = 9;
Servo servo1;

int   pos1 = 175;
//const char* ssid = "Kamuy";
//const char* password = "cgla48123456789";



void setup()
{
  servo1.attach(10);
  //um pequeno delay no arduino
  //para evitar que envie comandos antes do esp8266 dar o start
  delay(500);
  //Seta ambas seriais para a velocidade de 9600
  Serial.begin(9600);
  //(em alguns casos a velocidade do seu esp8266 pode estar diferente desta)
  esp8266.begin(9600);

  //reset8266();

  //declara os pinos como saida


  pinMode(andasup, OUTPUT);
  digitalWrite(andasup, LOW);

  pinMode(garagem, OUTPUT);
  digitalWrite(garagem, LOW);

  pinMode(sala, OUTPUT);
  digitalWrite(sala, LOW);

  pinMode(jardim, OUTPUT);
  digitalWrite(jardim, LOW);

  delay(2000);
  //liga o led azul para informar o inicio dos comandos AT
  //digitalWrite(blue, HIGH);


  //Envia o comandos AT

  // reseta o modulo
  sendData("AT+RST\r\n", 2000, DEBUG);



  //sendData("AT+CWQAP\r\n", 2000, DEBUG);
  // configure as access point e estação (ambos)
  //sendData("AT+CWJAP?\r\n", 3000, DEBUG);
  //sendData("AT+CWMODE=3\r\n", 1000, DEBUG);

  //sendData("AT+CWMODE_DEF=3\r\n", 3000, DEBUG);


  //conecta ao roteador com a senha
  //(esta configuração deve ser feita, pois o seu roteador tem nome diferente do meu e senha)


  sendData("AT+CWJAP=\"Kamuy\",\"cgla48123456789\"r\n", 10000, DEBUG);
  sendData("AT+CWJAP?\r\n", 3000, DEBUG);
  //Retorna o IP ao qual está conectado e o IP de Station
  sendData("AT+CIFSR\r\n", 5000, DEBUG);
  //Habilita multiplas conexões
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
  //Habilita ao servidor a porta 80
  sendData("AT+CIPSERVER=1,8085\r\n", 1000, DEBUG);

  delay(2000);
  //desliga led verde
  //digitalWrite(green, LOW);


}

void loop()
{
//verifica se o esp8266 esta enviando mensagem e esta disponivel
  if (esp8266.available())
  {
    if (esp8266.find("+IPD,"))
    {
      delay(500);
      //recupera apenas comando ASCII
      int connectionId = esp8266.read() - 48;

      //avança o cursor até a palavra  "pin="
      esp8266.find("pin=");

      //armazena o primeiro numero.. Ex: Se for pino 13 armazenara 1 e logo em seguida multiplica por 10 para o calculo do pino
      int pinNumber = (esp8266.read() - 48) * 10;
      //armazena o segundo numero e soma com o primeiro...  Ex : Se o numero for 13 a soma sera 10 + 3 que resultara no pino ao qual foi enviado pela pagina Web
      pinNumber += (esp8266.read() - 48);
      //armazena o terceiro dado que correponde ao status do led '1' para ligado e '0' para desligado
      int statusLed = (esp8266.read() - 48);


      digitalWrite(pinNumber, statusLed);
      if (statusLed == 10 ) {
         servo1.write(175);
      }

    /*  if(statusLed == 10){
        digitalWrite(serv1, pos1);
      }*/


      // finaliza o comando no esp8266
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId;
      closeCommand += "\r\n";

      //Encerra a conexao
      sendData(closeCommand, 1000, DEBUG);



    }


  }
}


//Metodo que envia os comandos para o esp8266
String sendData(String command, const int timeout, boolean debug)
{
  //variavel de resposta do esp8266
  String response = "";

  // send a leitura dos caracteres para o esp8266
  esp8266.println(command);

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {


      //Concatena caracter por caractere recebido do modulo esp8266
      char c = esp8266.read();
      response += c;
    }
  }

  //debug de resposta do esp8266
  if (debug)
  {
    //Imprime o que o esp8266 enviou para o arduino
    Serial.println("Arduino : " + response);
  }

  return response;
}
