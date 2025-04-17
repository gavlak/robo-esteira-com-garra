#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>

#define JOYSTICK1_X   A0
#define JOYSTICK1_Y   A1
#define JOYSTICK1_BTN 5

#define JOYSTICK2_X   A2
#define JOYSTICK2_Y   A3
#define JOYSTICK2_BTN 6

struct Dados {
    int motores_y;
    int motores_x;
    int braco_x;
    int braco_y;
    bool botao1;
    bool botao2;
};

Dados pacoteDados;

RF24 radio(8, 7); // CE, CSN
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600); 
  radio.begin();
  radio.setAutoAck(false); //Quando você não precisa de confirmação de recebimento do sinal
  radio.setDataRate(RF24_250KBPS); //taxa de transmissão dos dados. Mais confiável, maior alcance, porém, menor velocidade
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH); //Aumenta a força do sinal, melhora o alcance e resistência ao ruído
  radio.stopListening();

  pinMode(JOYSTICK1_BTN, INPUT_PULLUP);
  pinMode(JOYSTICK2_BTN, INPUT_PULLUP);
}

void loop() {

  delay(15);

  pacoteDados.motores_x = analogRead(JOYSTICK1_X);
  pacoteDados.motores_y = analogRead(JOYSTICK1_Y);
  pacoteDados.braco_x   = analogRead(JOYSTICK2_X);
  pacoteDados.braco_y   = analogRead(JOYSTICK2_Y);
  pacoteDados.botao1    = !digitalRead(JOYSTICK1_BTN); //por padrão, pressionar o botão interrompe o sinal, portanto, para o resultado esperado, basta negar(!) a leitura
  pacoteDados.botao2    = !digitalRead(JOYSTICK2_BTN);

  // Envia os dados pelo rádio
  radio.write(&pacoteDados, sizeof(Dados));

  Serial.print("Motores X: "); Serial.print(pacoteDados.motores_x);
  Serial.print(" | Motores Y: "); Serial.print(pacoteDados.motores_y);
  Serial.print(" | Braco X: "); Serial.print(pacoteDados.braco_x);
  Serial.print(" | Braco Y: "); Serial.print(pacoteDados.braco_y);
  Serial.print(" | Botao1: "); Serial.print(pacoteDados.botao1);
  Serial.print(" | Botao2: "); Serial.println(pacoteDados.botao2);
}
