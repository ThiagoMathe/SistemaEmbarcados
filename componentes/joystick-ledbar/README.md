# Descrição do Código

Este código implementa um sistema de controle de **LEDs** com base na leitura de um **joystick analógico** utilizando o ESP32. Ele lê os valores dos eixos X e Y do joystick através do ADC (Conversor Analógico-Digital) e controla o número de LEDs acesos conforme a direção do joystick. Os valores brutos e a direção são exibidos no terminal.

## Funcionalidades

1. **Leitura Analógica**:
   - Lê os valores dos eixos X e Y do joystick usando o ADC do ESP32.
   - Os valores brutos variam de 0 a 4095 (12 bits de resolução).

2. **Controle de LEDs**:
   - Um conjunto de 10 LEDs é controlado com base na direção do joystick.
   - Movimentar o joystick para a **direita** ou para **cima** aumenta o número de LEDs acesos.
   - Movimentar o joystick para a **esquerda** ou para **baixo** diminui o número de LEDs acesos.

3. **Exibição no Terminal**:
   - Os valores brutos dos eixos X e Y são exibidos no terminal.
   - A direção do joystick é exibida com base nos valores lidos.

## Simulação no Wokwi

Você pode testar e simular este código no Wokwi usando o link abaixo:

[![Simulação no Wokwi](https://img.shields.io/badge/Simulação-Wokwi-blue)](https://wokwi.com/projects/425519869038614529)

