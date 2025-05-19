#include <WiFi.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <HTTPClient.h>

// Configurações da câmera
#define CS_PIN 5 // GPIO5 é comum no ESP32 para CS
ArduCAM myCAM(OV2640, CS_PIN);

// Configurações da rede Wi-Fi
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";

// URL da API que receberá a imagem
const char* serverUrl = "http_do_server";

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
  myCAM.initCAM();
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  myCAM.OV2640_set_JPEG_size(OV2640_320x240); // Resolução mais leve
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

  // Lê os dados da imagem da FIFO
  uint32_t len = myCAM.read_fifo_length();
  Serial.printf("Tamanho da imagem: %lu bytes\n", len);

  if (len >= 100000 || len == 0) {
    Serial.println("Erro: imagem muito grande ou vazia.");
    return;
  }

  // Aloca buffer para imagem
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

  // Envia a imagem para a API
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/octet-stream");
    http.addHeader("Biometric-ID", biometricID); // Cabeçalho customizado

    int httpResponseCode = http.POST(imageBuffer, len);
    if (httpResponseCode > 0) {
      Serial.printf("Imagem enviada! Código de resposta: %d\n", httpResponseCode);
    } else {
      Serial.printf("Erro ao enviar: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  }

  free(imageBuffer); // Libera memória
  delay(5000); // Aguarda 5s para próxima captura
}