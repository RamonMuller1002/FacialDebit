Projeto FacialDebit
Por: Ramon Müller (coordenador da equipe), Elias Almenda, Mateus Müller
ALERTA DO AUTOR: De modo inicial irei criar vários arquivos formato "txt" para anotação e ideias de como fazer funcionar.
Projeto consiste em criar um aplicativo para utilizar de reconhecimento facial para poder substituir os cartões passes de ônibus no débito de tarifa de maneira automática, para otimizar tempo e comodidade na rotina de pessoas que necessitam do transporte publico.

Existem 4 etapas base para o funcionamento do projeto:

Captura da imagem
Envio da imagem para API
Reconhecimento e cadastro de dados biométricos como ID
Funções de operações monetárias dentro do App do cliente.
Utilizando de uma placa arduino, ESP8266 ou ESP32 e um dos modulos de video OV7670 (básico, mas requer mais configuração), ou o ArduCAM Mini (mais fácil de usar, suporta SPI e tem bibliotecas prontas).

As especificações podem variar dentre as opções dependendo da placa ou do módulo (ainda não decidido).

Bibliotecas para a captura de imagem
Verificar melhor arquivo envio-dados.txt para cada caso
Para ESP2866 ou ESP32:

Wire.h
ArduCAM.h
SPI.h
WiFi.h
Mensagem do autor: Transmissão de dados provavelmente será feita via wifi
