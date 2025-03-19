# Descrição do Código

Este código implementa a leitura de um **joystick analógico** utilizando o ESP32. Ele lê os valores dos eixos X e Y do joystick através do ADC (Conversor Analógico-Digital) e determina a direção do joystick com base nos valores lidos. Os valores brutos e a direção são exibidos no terminal.

## Funcionalidades

1. **Leitura Analógica**:
   - Lê os valores dos eixos X e Y do joystick usando o ADC do ESP32.
   - Os valores brutos variam de 0 a 4095 (12 bits de resolução).

2. **Determinação da Direção**:
   - Com base nos valores lidos, o código determina a direção do joystick:
     - **Direita**: Valor de X menor que 1200.
     - **Esquerda**: Valor de X maior que 2800.
     - **Baixo**: Valor de Y menor que 1200.
     - **Cima**: Valor de Y maior que 2800.
     - **Centro**: Valores intermediários.

3. **Exibição no Terminal**:
   - Os valores brutos dos eixos X e Y são exibidos no terminal.
   - A direção do joystick é exibida com base nos valores lidos.

## Simulação no Wokwi

Você pode testar e simular este código no Wokwi usando o link abaixo:

[![Simulação no Wokwi](https://img.shields.io/badge/Simulação-Wokwi-blue)](https://wokwi.com/projects/425519914062376961)