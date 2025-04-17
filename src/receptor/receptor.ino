#include <RF24.h>
#include <nRF24L01.h>
#include <Servo.h>

#define MOTOR_DIREITO_HORARIO 46 
#define MOTOR_DIREITO_ANTIH 47
#define MOTOR_ESQUERDO_HORARIO 48
#define MOTOR_ESQUERDO_ANTIH 49
#define VEL_MOTORD 5  
#define VEL_MOTORE 6

#define MOTOR_BASE_HORARIO 44
#define MOTOR_BASE_ANTIH 45             
#define VEL_MOTOR_BASE 11  

#define PIN_BASE 4    
#define PIN_MEIO 3
#define PIN_ROTACIONA_GARRA 2             
#define PIN_GARRA 9  

struct Dados {
    int motores_y;
    int motores_x;
    int braco_x;
    int braco_y;
    bool botao1;
    bool botao2;              
};

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

Servo servoBase;
Servo servoMeio;
Servo servoRotacionaGarra;
Servo servoGarra;

Dados pacoteRecebido;

void setup() {
    //Serial.begin(9600);

    radio.begin();
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_HIGH);
    radio.startListening();

    pinMode(MOTOR_ESQUERDO_HORARIO, OUTPUT);
    pinMode(MOTOR_ESQUERDO_ANTIH, OUTPUT);
    pinMode(MOTOR_DIREITO_HORARIO, OUTPUT);
    pinMode(MOTOR_DIREITO_ANTIH, OUTPUT);
    pinMode(VEL_MOTORD, OUTPUT);
    pinMode(VEL_MOTORE, OUTPUT);

    pinMode(MOTOR_BASE_HORARIO, OUTPUT);
    pinMode(MOTOR_BASE_ANTIH, OUTPUT);
    pinMode(VEL_MOTOR_BASE, OUTPUT);

    servoBase.attach(PIN_BASE);
    servoMeio.attach(PIN_MEIO);
    servoRotacionaGarra.attach(PIN_ROTACIONA_GARRA);
    servoGarra.attach(PIN_GARRA);

}

void loop() {
    delay(15);

    if (radio.available()) {
        radio.read(&pacoteRecebido, sizeof(Dados));

        int eixoX = pacoteRecebido.motores_x;
        int eixoY = pacoteRecebido.motores_y;
        bool botao1 = pacoteRecebido.botao1;
        bool botao2 = pacoteRecebido.botao2;
        int valorBase;
        int velocidade = 0;
        
        /*Serial.println("=== Dados Recebidos ===");
        Serial.print("Motores X: "); Serial.println(eixoX);
        Serial.print("Motores Y: "); Serial.println(eixoY);
        Serial.print("Botao1: "); Serial.println(botao1);
        Serial.print("Botao2: "); Serial.println(botao2);
        Serial.print("Braco X: "); Serial.println(pacoteRecebido.braco_x);
        Serial.print("Braco Y: "); Serial.println(pacoteRecebido.braco_y);*/

        if (!botao1 && botao2) {

            if(eixoY < 450){
                digitalWrite(MOTOR_BASE_HORARIO, HIGH);
                digitalWrite(MOTOR_BASE_ANTIH, LOW);
                analogWrite(VEL_MOTOR_BASE, map(eixoY, 0, 1023, 255, 60));
            }else if(eixoY > 550){
                digitalWrite(MOTOR_BASE_HORARIO, LOW);
                digitalWrite(MOTOR_BASE_ANTIH, HIGH);
                analogWrite(VEL_MOTOR_BASE, map(eixoY, 0, 1023, 60, 255));
            }else{
                digitalWrite(MOTOR_BASE_HORARIO, LOW);
                digitalWrite(MOTOR_BASE_ANTIH, LOW);
                analogWrite(VEL_MOTOR_BASE, 0);
            }

            valorBase=map(eixoX, 0, 1023, 0, 180);
            servoBase.write(valorBase > 69 ? valorBase : 69);


        }else if (eixoX < 450) {

            digitalWrite(MOTOR_ESQUERDO_HORARIO, HIGH);
            digitalWrite(MOTOR_ESQUERDO_ANTIH, LOW);
            digitalWrite(MOTOR_DIREITO_HORARIO, LOW);
            digitalWrite(MOTOR_DIREITO_ANTIH, HIGH);
            velocidade = map(eixoX, 0, 1023, 255, 60);

        } else if (eixoX > 550) {

            digitalWrite(MOTOR_ESQUERDO_HORARIO, LOW);
            digitalWrite(MOTOR_ESQUERDO_ANTIH, HIGH);
            digitalWrite(MOTOR_DIREITO_HORARIO, HIGH);
            digitalWrite(MOTOR_DIREITO_ANTIH, LOW);
            velocidade = map(eixoX, 0, 1023, 60, 255);

        } else if (eixoY < 450) {

            digitalWrite(MOTOR_ESQUERDO_HORARIO, LOW);
            digitalWrite(MOTOR_ESQUERDO_ANTIH, HIGH);
            digitalWrite(MOTOR_DIREITO_HORARIO, LOW);
            digitalWrite(MOTOR_DIREITO_ANTIH, HIGH);
            velocidade = map(eixoY, 0, 1023, 255, 60);

        } else if (eixoY > 550) {

            digitalWrite(MOTOR_ESQUERDO_HORARIO, HIGH);
            digitalWrite(MOTOR_ESQUERDO_ANTIH, LOW);
            digitalWrite(MOTOR_DIREITO_HORARIO, HIGH);
            digitalWrite(MOTOR_DIREITO_ANTIH, LOW);
            velocidade = map(eixoY, 0, 1023, 60, 255);

        } else {

            digitalWrite(MOTOR_ESQUERDO_HORARIO, LOW);
            digitalWrite(MOTOR_ESQUERDO_ANTIH, LOW);
            digitalWrite(MOTOR_DIREITO_HORARIO, LOW);
            digitalWrite(MOTOR_DIREITO_ANTIH, LOW);
            velocidade = 0;
        }

        analogWrite(VEL_MOTORD, velocidade);
        analogWrite(VEL_MOTORE, velocidade);

        if (botao1 && !botao2) {
            servoGarra.write(70);
        } else if (botao1 && botao2) {
            servoGarra.write(180);
        } else {
            servoGarra.write(88);
        }

        servoMeio.write(map(pacoteRecebido.braco_x, 0, 1023, 0, 180));
        servoRotacionaGarra.write(map(pacoteRecebido.braco_y, 0, 1023, 0, 180));
    }
}
