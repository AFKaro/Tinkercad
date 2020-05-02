/* 
*  PROJETO: MICROONDAS [SEM TIMER] - Karolayne Silva
* FUNCIONAMENTO: Ao apertar o botão a 1º vez o forno liga,
* e depois do tempoLimite (30 seg) ele se desliga automaticamente.
*   Se o botão for apertado enquando o forno está ligado ele pausa
*   o tempo. Se o botão for segurado por tempoReset (2 seg)
*   o tempo será resetado.
*/

#include <LiquidCrystal.h>
//#include <TimerOne.h>
#define pinBotao 11
#define tempoReset 2000
#define pinMotor 9
#define pinLed1 13
#define pinLed2 12
#define alarme 8

//Controle Tempo
int horas = 0;
int minutos = 0;
int segundos = 0;
bool flag = false;
int limiteSegundos = 30; //Tempo em que o microondas para por padrao

//Motor Controle
bool fornoAberto = false;
int pot = 1000;

//Alarme
unsigned long tempoAnterior = 0; // Armazena o último momento em que o Buzzer foi atualizado
const long PERIODO = 60;//Tempo em que o alarme toca
int estadoAlarme = LOW;

int estadoBotaoControlador = -1;

//LCD
String mensagem = "MICROONDAS";
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//PROCEDIMENTOS e FUNÇÕES
void incrementarTempo();
void pausarTempo();
void resetarTempo();
int pinBotaoStartApertado();
void ligarForno();
void desligarForno();
void lcdZerado(String);
void fornoEstaAberto();

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(mensagem);
  lcd.setCursor(4, 1);
  lcd.print("00:00:00");
  
  pinMode(pinBotao, INPUT_PULLUP);
  pinMode(pinLed1,OUTPUT);
  pinMode(pinLed2,OUTPUT);
  pinMode(pinMotor,OUTPUT);
  pinMode(alarme, OUTPUT);
  digitalWrite(pinLed1,HIGH);

/*O timer seria inicializado:
*Timer1.initialize(1000000);   
*Timer1.attachInterrupt('metodo que iria controlar o timer');
*/
}

void loop()  {
  estadoBotaoControlador = pinBotaoStartApertado();

  if (estadoBotaoControlador == 0) {
    if (flag) {
      mensagem = "    -LIGADO-     ";
      lcd.setCursor(0, 0);
      lcd.print(mensagem);
      ligarForno();
      incrementarTempo();
      estadoAlarme=LOW;
    }
    else if(!flag && fornoAberto){
      fornoEstaAberto();
      segundos=0;
    }
  }
  if (estadoBotaoControlador == 1) {
    if (!flag) {
      desligarForno();
      pausarTempo();
    }
  }
  if (estadoBotaoControlador == 2) {
    resetarTempo();
    desligarForno();
  }
}

void incrementarTempo() {
  
/*Timer inicia a contagem
*Timer1.start();
*Timer1.attachInterrupt(contaTempo);
*/  
  static unsigned long delayTempo;
  fornoAberto=false;
  
  if ( ((millis() - delayTempo) >= 1000)&&fornoAberto==false) {
    segundos++;
    delayTempo = millis();
    if(segundos == limiteSegundos){
      fornoAberto = true;
      flag=false;
    }
    atualizarPainel();
  }
}

void pausarTempo() {
  
  //quando o metodo é acionado, o timer para sem resetar o tempo, ou
  //seja, ficaria pausado:
  //Timer1.stop();
  
  mensagem = "-PAUSADO-";

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(mensagem);
  lcd.setCursor(4, 1);
  if (horas < 10) {
    lcd.print("0");
  }
  lcd.print(horas);
  lcd.print(":");

  if (minutos < 10) {
    lcd.print("0");
  }
  lcd.print(minutos);
  lcd.print(":");

  if (segundos < 10) {
    lcd.print("0");
  }
  lcd.print(segundos);

}

void resetarTempo() {
  
  //quando o metodo é acionado ele para o contador
  //e zera todos os valores 
  //Timer1.stop();
  //horas = minutos = segundos = 0;
  
  mensagem = "   MICROONDAS";
  lcdZerado(mensagem);
  horas = minutos = segundos = 0;

}

void atualizarPainel() {
  lcd.setCursor(4, 1);
  if (horas < 10) {
    lcd.print("0");
  }
  lcd.print(horas);
  lcd.print(":");

  if (minutos < 10) {
    lcd.print("0");
  }
  lcd.print(minutos);
  lcd.print(":");

  if (segundos < 10) {
    lcd.print("0");
  }
  lcd.print(segundos);
}


int pinBotaoStartApertado() {
#define tempoDebounce 50 //(tempo para eliminar o efeito Bounce EM MILISEGUNDOS)

  static bool estadoBotaoAnt;
  static unsigned long delayBotao = 0;
  static unsigned long botaoApertado;
  static byte fase = 0;

  bool estadoBotao;
  byte estadoRet;

  estadoRet = 0;
  if ( (millis() - delayBotao) > tempoDebounce ) {
    estadoBotao = digitalRead(pinBotao);
    if ( !estadoBotao && (estadoBotao != estadoBotaoAnt) ) {
      delayBotao = millis();
      botaoApertado = millis();
      fase = 1;
    }

    if ( (fase == 1) && ((millis() - botaoApertado) > tempoReset) ) {
      fase = 0;
      estadoRet = 2;
      flag = false;
    }

    if ( estadoBotao && (estadoBotao != estadoBotaoAnt) ) {
      delayBotao = millis();

      if ( fase == 1 ) {
        estadoRet = 1;
        flag = !flag;
      }
      fase = 0;
    }

    estadoBotaoAnt = estadoBotao;
  }

  return estadoRet;
}
void ligarForno(){
  digitalWrite(pinMotor,pot);
  digitalWrite(pinLed2,HIGH);
  digitalWrite(pinLed1,LOW);
}
void desligarForno(){
  digitalWrite(pinMotor,0);
  digitalWrite(pinLed2,LOW);
  digitalWrite(pinLed1,HIGH);
}
void lcdZerado(String mensagem){
  lcd.setCursor(0, 0);
  lcd.print(mensagem);
  lcd.setCursor(4, 1);
  lcd.print("00:00:00"); 
}
void fornoEstaAberto(){
  mensagem = "-Forno Aberto-";
  /*lcd.begin(16, 2);
  lcd.clear();*/
  lcd.setCursor(1, 0);
  lcd.print(mensagem);
  desligarForno();
  
  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoAnterior >= PERIODO) {
    tempoAnterior = tempoAtual;
    if (estadoAlarme == LOW) {
      tone(alarme,1319);
      estadoAlarme=HIGH;
    } 
    else {
      noTone(alarme);
    }
  }
}
