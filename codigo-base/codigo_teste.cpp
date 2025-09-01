#include <WiFi.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <HTTPClient.h>

// Configurações da câmera
#define CS_PIN 5
ArduCAM myCAM(OV2640, CS_PIN);

// Configurações da rede Wi-Fi
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";

// URL da API que receberá a imagem
const char* serverUrl = "http://server/api/reconhecimento";

// ID biométrico simulado (poderia vir de um leitor biométrico)
String biometricID = "usuario_123";

void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado");

  // Inicializa a câmera
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  myCAM.write_reg(ARDUCHIP_MODE, 0x00);
  myCAM.OV2640_set_JPEG_size(OV2640_320x240); // resolução leve
  delay(1000);

  Serial.println("Câmera inicializada!");
}

void loop() {
  Serial.println("Capturando imagem...");
  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();
  myCAM.start_capture();

  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
    delay(10);
  }

  Serial.println("Imagem capturada!");

  uint32_t len = myCAM.read_fifo_length();
  Serial.printf("Tamanho da imagem: %lu bytes\n", len);

  if (len >= 100000 || len == 0) {
    Serial.println("Erro: imagem muito grande ou vazia.");
    return;
  }

  uint8_t* imageBuffer = (uint8_t*)malloc(len);
  if (!imageBuffer) {
    Serial.println("Erro de memória.");
    return;
  }

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  for (uint32_t i = 0; i < len; i++) {
    imageBuffer[i] = SPI.transfer(0x00);
  }
  myCAM.CS_HIGH();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/octet-stream");
    http.addHeader("Biometric-ID", biometricID);

    int httpResponseCode = http.POST(imageBuffer, len);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.printf("Resposta da API: %s\n", response.c_str());

      // Supondo que a API retorne algo como {"match":true,"user":"Elias"}
      if (response.indexOf("\"match\":true") != -1) {
        Serial.println(" Rosto reconhecido, Acesso liberado.");
      } else {
        Serial.println(" Rosto não compatível, Acesso negado");
      }

    } else {
      Serial.printf("Erro ao enviar: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  }

  free(imageBuffer);
  delay(5000); // espera antes da próxima captura
}
