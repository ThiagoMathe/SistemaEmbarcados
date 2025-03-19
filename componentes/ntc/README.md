# Descrição do Código

Este código implementa a leitura de temperatura utilizando um **sensor NTC** conectado ao ESP32. O NTC é um resistor cuja resistência varia com a temperatura, e o código utiliza o ADC (Conversor Analógico-Digital) do ESP32 para medir a resistência do NTC e calcular a temperatura em Celsius, Kelvin e Fahrenheit.

## Funcionalidades

1. **Leitura de Temperatura**:
   - Lê a resistência do NTC usando o ADC do ESP32.
   - Converte a resistência em temperatura utilizando a equação do NTC e o coeficiente beta.

2. **Conversão de Unidades**:
   - Converte a temperatura lida para as escalas Celsius, Kelvin e Fahrenheit.

3. **Exibição no Terminal**:
   - Exibe a temperatura em Celsius no terminal a cada segundo.

## Simulação no Wokwi

Você pode testar e simular este código no Wokwi usando o link abaixo:

[![Simulação no Wokwi](https://img.shields.io/badge/Simulação-Wokwi-blue)](https://wokwi.com/projects/425520029900674049)