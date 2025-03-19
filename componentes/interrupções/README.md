# Descrição do Código

Este código implementa um sistema simples de controle de um **LED** utilizando um **botão** no ESP32. Quando o botão é pressionado, ele aciona uma interrupção que alterna o estado do LED (ligado/desligado).

## Funcionalidades

1. **Controle do LED**:
   - O LED é ligado ou desligado toda vez que o botão é pressionado.
   - O estado do LED é alternado a cada pressionamento.

2. **Interrupção do Botão**:
   - O botão é configurado com uma interrupção na borda de subida (quando o botão é pressionado).
   - A interrupção envia o número do GPIO para uma fila, que é processada pela task principal.

3. **Debouncing**:
   - Um pequeno delay é implementado para evitar múltiplos acionamentos da interrupção devido ao ruído mecânico do botão (debouncing).

4. **Comunicação via Fila**:
   - A interrupção envia o número do GPIO para uma fila, e a task principal processa o evento, garantindo que o código seja seguro e eficiente.

## Simulação no Wokwi

Você pode testar e simular este código no Wokwi usando o link abaixo:

[![Simulação no Wokwi](https://img.shields.io/badge/Simulação-Wokwi-blue)](https://wokwi.com/projects/425519640834902017)