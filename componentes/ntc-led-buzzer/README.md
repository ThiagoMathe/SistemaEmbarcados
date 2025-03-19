# Descrição do Código

Este código implementa um sistema de monitoramento de temperatura utilizando um **sensor NTC** e um **LED RGB** para indicar a faixa de temperatura. Além disso, um **buzzer** é ativado quando a temperatura ultrapassa um limite crítico. O ESP32 é utilizado para ler a temperatura do NTC, controlar o LED RGB e ativar o buzzer conforme necessário.

## Funcionalidades

1. **Leitura de Temperatura**:
   - Lê a resistência do NTC usando o ADC do ESP32.
   - Converte a resistência em temperatura utilizando a equação do NTC e o coeficiente beta.

2. **Indicação Visual com LED RGB**:
   - **Temperatura abaixo de 30°C**: LED verde ligado.
   - **Temperatura entre 30°C e 40°C**: LED amarelo ligado.
   - **Temperatura acima de 40°C**: LED vermelho ligado.

3. **Alerta Sonoro com Buzzer**:
   - Quando a temperatura ultrapassa 40°C, o buzzer é ativado por 500ms.

4. **Exibição no Terminal**:
   - A temperatura é exibida no terminal a cada segundo.

## Simulação no Wokwi

Você pode testar e simular este código no Wokwi usando o link abaixo:

[![Simulação no Wokwi](https://img.shields.io/badge/Simulação-Wokwi-blue)](https://wokwi.com/projects/425519760237309953)