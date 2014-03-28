#include <MeetAndroid.h>
#include <Servo.h>
#include <Ultrasonic.h>
#include <SoftwareSerial.h>

const int VEL_MAX = 255;
const int VEL_MIN = 10;
const int STOP = 0;
const int STOP_DELAY = 5000;

// Motor
int motorDirPinI = 5;
int motorDirPin1 = 6;
int motorDirPin2 = 7;

int motorEsqPinI = 8;
int motorEsqPin1 = 9;
int motorEsqPin2 = 10;
unsigned long lastSignalTime;

// Servo e Ultrason
//Ultrasonic ultrasonic(4, 3);
//Servo servo1;

//MeetAndroid
MeetAndroid meetAndroid;

const float DIFF = 2;
const int X = 0;
const int Y = 1;
const int Z = 2;

float eixos[3]= {-5.0,0.0,0.0};
//---


void setup() {


  //Inicialização dos Servos
  pinMode(motorDirPin1, OUTPUT);  
  pinMode(motorDirPin2, OUTPUT); 
  
  pinMode(motorEsqPin1, OUTPUT);
  pinMode(motorEsqPin2, OUTPUT); 
  delay(10);
  parar();
  
  //Servo
  //servo1.attach(14);

  //Inicialização do Amarino
  Serial.begin(115200);
  meetAndroid.registerFunction(acelerometro, 'A'); // a string
  //---
  meetAndroid.send("start");
}

void loop() {
  meetAndroid.receive(); // you need to keep this in your loop() to receive events
  if (millis()-lastSignalTime>STOP_DELAY){
    parar();
    meetAndroid.send("NAO RECEBENDO PARAR");
  }else{
    andar();
  }
  delay(10);
}

/*
*/
void acelerometro(byte flag, byte numOfValues)
{
  lastSignalTime=millis();  
  int length = meetAndroid.stringLength();
  char data[length];
  meetAndroid.getString(data);
  String leitura = data;

  //Separa a string recebida do Amarino em 3 strings
  //Ex: 1.42342;-3.43242;-1.34232
  int offset = 0;
  int count = 0;
  for(int i = 0; i < length; i++) {
    //Toda vez que encontrar um ';', le de 'offset' até 'i'
    if(data[i] == ';') {
      eixos[count] = stringToFloat(leitura.substring(offset, i));
      offset = i + 1;
      count++;
      
      //Se o último ';' na string tiver o mesmo índice de 'i', então pega
      //o restante da string no próximo elemento do array.
      if(leitura.lastIndexOf(';') == i) {
        eixos[count] = stringToFloat(leitura.substring(offset, length));
      }
    }
  }
}

void andar() {
   if (eixos[Y] > DIFF) {
    meetAndroid.send("ESQUERDA");
    esquerda(eixos[Y]);
  } else if(eixos[Y] < -DIFF) {
    meetAndroid.send("DIREITA");
    direita(eixos[Y]*-1);
  } else if(eixos[Z] < 10 && eixos[Z] > 4) {
    meetAndroid.send("FRENTE");
    frente(eixos[Z]);
  }else if(eixos[Z] > -10 && eixos[Z] < -4) {
    meetAndroid.send("RECUAR");
    recuar(abs(eixos[Z]));
  } else {
    meetAndroid.send("PARAR");
    parar();
  }
}

void parar() {
  digitalWrite(motorDirPin1, STOP);
  digitalWrite(motorDirPin2, STOP);
  
  digitalWrite(motorEsqPin1, STOP);
  digitalWrite(motorEsqPin2, STOP);
}

void frente(int eixo) {
  int intensidade = (eixo -4);
  float prop = (float)VEL_MAX/5.0;
  
  if(intensidade > 5)
    intensidade = 5;

  meetAndroid.send(prop*intensidade);
  digitalWrite(motorDirPin1, HIGH);
  digitalWrite(motorDirPin2, LOW);
  digitalWrite(motorDirPinI, prop*intensidade);
 
  digitalWrite(motorEsqPin2, LOW);
  digitalWrite(motorEsqPin1, HIGH);
  digitalWrite(motorEsqPinI, prop*intensidade);
}

void recuar(int eixo) {
  int intensidade = (eixo -4);
  float prop = (float)VEL_MAX/5.0;
  
  if(intensidade > 5)
    intensidade = 5;

  meetAndroid.send(prop*intensidade);
  digitalWrite(motorDirPin1, LOW);
  digitalWrite(motorDirPin2, HIGH);
  digitalWrite(motorDirPinI, prop*intensidade);
  
  digitalWrite(motorEsqPin1, LOW);
  digitalWrite(motorEsqPin2, HIGH);  
  digitalWrite(motorEsqPinI, prop*intensidade);
}

void esquerda(int eixo) {
  int intensidade = eixo -DIFF +1;
  int maxI = (5.0-DIFF);
  float prop = (float)VEL_MAX/maxI;
  
  if(intensidade > maxI)
    intensidade = maxI;
    
  meetAndroid.send(prop*intensidade);
   
  digitalWrite(motorEsqPin1, HIGH);
  digitalWrite(motorEsqPin2, LOW);
  digitalWrite(motorEsqPinI, prop*intensidade);
}

void direita(int eixo) {
  int intensidade = eixo -DIFF +1;
  int maxI = (5.0-DIFF);
  float prop = (float)(VEL_MAX)/maxI;
  
  if(intensidade > maxI)
    intensidade = maxI;
  
  meetAndroid.send(prop*intensidade);
  
  digitalWrite(motorDirPin1, HIGH);
  digitalWrite(motorDirPin2, LOW);
  digitalWrite(motorDirPin1, prop*intensidade);
}

/**
* Converts a String to a float
*/
float stringToFloat(String str) {
  char arr[str.length()];
  str.toCharArray(arr, sizeof(arr));
  return atof(arr);
}
