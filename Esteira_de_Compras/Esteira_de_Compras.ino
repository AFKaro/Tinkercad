//BIBLIOTECAS
#include <Servo.h>

//CD4511 - DISPLAY's
int unidades[] = {6, 8, 9, 7};
int dezenas[] = {10,12,13,11};
const int l = 1, d = 0; //l = ligado; d = desligado
byte digito[10][4] = { 
 { d,d,d,d },  //DIGITO 0
 { l,d,d,d },  //DIGITO 1
 { d,l,d,d },  //DIGITO 2
 { l,l,d,d},  //DIGITO 3
 { d,d,l,d },  //DIGITO 4
 { l,d,l,d },  //DIGITO 5
 { d,l,l,d },  //DIGITO 6
 { l,l,l,d },  //DIGITO 7
 { d,d,d,l },  //DIGITO 8
 { l,d,d,l }  //DIGITO 9
};
const int SEGMENTOS = 4;
const int NUMEROS = 10;
int num1 = 0; //Unidade
int num2 = 0; //Dezena

//Pinos 74HC585
#define pinSH_CP 14 //Pino Clock
#define pinST_CP 15  //Pino Latch
#define pinDS    16  //Pino Data
#define qtdeCI   1

//BOTÂO
#define pinBotao 5
#define tempoReset 2000
int estadoBotaoControlador = -1;
bool flag = false;

//BUZZER
#define alarme 19
unsigned long tempoAnterior = 0; // Armazena o último momento em que o Buzzer foi atualizado
const long PERIODO = 15;//Tempo em que o alarme toca
int estadoAlarme = LOW;

//MOTOR CC
#define pinMotor 17
int pot = 1000;

//SERVO
int pos = 0;
Servo servos[5];
unsigned long agora = 0;

//PRODUTOS
bool produtoNoFim;
int totalProdutos=0;

//PROCEDIMENTOS E FUNÇÕES
void moverProduto(int, int);
void servosOff();
void alarmeOn();
void ligaLed(int);
void desligaLed();
void displayPrint();
void acende(int*, int);
int pinBotaoStartApertado();
void ci74HC595Write(byte, bool);

void setup() {
   pinMode(pinSH_CP, OUTPUT);
   pinMode(pinST_CP, OUTPUT);
   pinMode(pinDS, OUTPUT);
   for(int i = 0; i < SEGMENTOS; i++) {
       pinMode(dezenas[i], OUTPUT);
       pinMode(unidades[i], OUTPUT);
   }
   acende(unidades, 0);
   acende(dezenas, 0);
  
   pinMode(pinBotao, INPUT_PULLUP);
   pinMode(pinMotor,OUTPUT);
   pinMode(alarme, OUTPUT);
   
   for(int i=0;i<5;i++){
     servos[i].attach(i); 
   }
   servosOff();
}

void loop()
{
  estadoBotaoControlador = pinBotaoStartApertado();
  
  if (estadoBotaoControlador == 0) {
    if (flag && !produtoNoFim) {
      moverProduto(400,-90);
        estadoAlarme=LOW;
    }
    else if(!flag && produtoNoFim){
      alarmeOn();
    }
  }
  if(estadoBotaoControlador==1){
    if(!flag){
    servosOff();
    produtoNoFim=false;
    }
  }
  if(estadoBotaoControlador==2){
   resetar(); 
  }
}
//*** FUNCIONALIDADES ***
//MOVENDO O PRODUTO
void moverProduto(int tempo, int angulo){
  digitalWrite(pinMotor,pot); //Liga Motor
  for(int i=0;i<5;i++){
  servos[i].write(angulo);
  ligaLed(i+1);
  agora = millis();
  while(millis() < agora + tempo) {
    // Pausa 
  }
  }
  ligaLed(6);
  flag = false;
  produtoNoFim = true;
  digitalWrite(pinMotor,0); //Desliga Motor
  displayPrint();
  
}

//SERVOS
void servosOff(){ //todos os servos na posição inicial
  for (int i = 0; i <5; i++) {
    servos[i].write(90);
  }
  desligaLed();
}
void resetar(){
 servosOff();
 desligaLed();
 digitalWrite(pinMotor,0);
 totalProdutos = 0;
 acende(unidades, 0);
 acende(dezenas, 0);
 num1=0;
 num2=0;
}
//BUZZER
void alarmeOn(){
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
//CONTROLE LEDS
void ligaLed(int led){
  switch(led){
   case 1:
    ci74HC595Write(1, HIGH);
    ci74HC595Write(2, LOW);
    ci74HC595Write(3, LOW);
    ci74HC595Write(4, LOW);
    ci74HC595Write(5, LOW);
    ci74HC595Write(6, LOW);
    break;
   case 2:
    ci74HC595Write(1, LOW);
    ci74HC595Write(2, HIGH);
    ci74HC595Write(3, LOW);
    ci74HC595Write(4, LOW);
    ci74HC595Write(5, LOW);
    ci74HC595Write(6, LOW);
    break;
   case 3:
    ci74HC595Write(1, LOW);
    ci74HC595Write(2, LOW);
    ci74HC595Write(3, HIGH);
    ci74HC595Write(4, LOW);
    ci74HC595Write(5, LOW);
    ci74HC595Write(6, LOW);
    break;
   case 4:
    ci74HC595Write(1, LOW);
    ci74HC595Write(2, LOW);
    ci74HC595Write(3, LOW);
    ci74HC595Write(4, HIGH);
    ci74HC595Write(5, LOW);
    ci74HC595Write(6, LOW);
    break;
   case 5:
    ci74HC595Write(1, LOW);
    ci74HC595Write(2, LOW);
    ci74HC595Write(3, LOW);
    ci74HC595Write(4, LOW);
    ci74HC595Write(5, HIGH);
    ci74HC595Write(6, LOW);
    break;
    case 6:
    ci74HC595Write(1, LOW);
    ci74HC595Write(2, LOW);
    ci74HC595Write(3, LOW);
    ci74HC595Write(4, LOW);
    ci74HC595Write(5, LOW);
    ci74HC595Write(6, HIGH);
    break;
  }
}
void desligaLed(){
  ci74HC595Write(1, LOW);
  ci74HC595Write(2, LOW);
  ci74HC595Write(3, LOW);
  ci74HC595Write(4, LOW);
  ci74HC595Write(5, LOW);
  ci74HC595Write(6, LOW); 
}
//DISPLAY
void displayPrint(){
  num1++;
  if(num1==10&&num2>=10){
    acende(unidades, 0);
    acende(dezenas, 0);
  }
  else{
  if(num1==10){
    num1 = 0;
    acende(unidades, num1);
    num2++;
    if(num2>=10){
      acende(dezenas, 9);
    }
    else{
      acende(dezenas, num2);
    }
  }else{
       acende(unidades, num1);
    }
  }
}
void acende(int* pino, int dig) {
    for(int i = 0; i < SEGMENTOS; i++) {
       digitalWrite(pino[i], digito[dig][i]);
    }
} 

//BOTAO MULTIFUNÇÕES
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
//"Digital Write" do 74HC565
void ci74HC595Write(byte pino, bool estado) {
static byte ciBuffer[qtdeCI];

bitWrite(ciBuffer[pino / 8], pino % 8, estado);

digitalWrite(pinST_CP, LOW); //Inicia a Transmissão

digitalWrite(pinDS, LOW);    //Apaga Tudo para Preparar Transmissão
digitalWrite(pinSH_CP, LOW);

for (int nC = qtdeCI-1; nC >= 0; nC--) {
    for (int nB = 7; nB >= 0; nB--) {

        digitalWrite(pinSH_CP, LOW);  //Baixa o Clock      
        
        digitalWrite(pinDS,  bitRead(ciBuffer[nC], nB) );     //Escreve o BIT
        
        digitalWrite(pinSH_CP, HIGH); //Eleva o Clock
        digitalWrite(pinDS, LOW);     //Baixa o Data para Previnir Vazamento      
    }  
}

digitalWrite(pinST_CP, HIGH);  //Finaliza a Transmissão

}
