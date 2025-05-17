/* NÃO É PROTÓTIPO AINDA, TEM MUITO CÓDIGO E ESTOU ANALIZANDO AINDA*/


#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>

#define CS_PIN 10 // Pino CS do ArduCAM

ArduCAM myCAM(OV2640, CS_PIN);

void setup() {
    Serial.begin(9600); // Comunicação Serial
    Wire.begin();
    SPI.begin();

    // Inicializa a câmera
    myCAM.initCAM();
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    Serial.println("Câmera inicializada!");
}

void loop() {
    // Captura a imagem
    myCAM.flush_fifo();
    myCAM.start_capture();

    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
        delay(10); // Aguarda a captura
    }

    Serial.println("Imagem capturada!");

    // Envia os dados da imagem via Serial
    uint8_t temp;
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();
    while (SPI.transfer(0x00) != 0xD9) { // 0xD9 é o marcador de fim de imagem JPEG
        temp = SPI.transfer(0x00);
        Serial.write(temp); // Envia byte a byte
    }
    myCAM.CS_HIGH();

    delay(3000); // Aguarda 3 segundos antes de capturar novamente
}
