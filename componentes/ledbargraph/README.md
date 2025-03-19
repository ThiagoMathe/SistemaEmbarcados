# Descrição do Código

Este código implementa uma **barra de LEDs** que acende progressivamente e depois apaga todos os LEDs, criando um efeito visual de "onda". O código utiliza o ESP32 para controlar 10 LEDs conectados a diferentes pinos GPIO.

## Funcionalidades

1. **Controle de LEDs**:
   - Controla 10 LEDs conectados a pinos GPIO específicos.
   - Acende os LEDs um por um, criando um efeito progressivo.
   - Apaga todos os LEDs após o acendimento completo.

2. **Efeito Visual**:
   - O efeito de "onda" é criado ao acender cada LED com um intervalo de 200ms.
   - Após todos os LEDs estarem acesos, eles são apagados e o ciclo se repete após 500ms.

## Simulação no Wokwi

Você pode testar e simular este código no Wokwi usando o link abaixo:

[![Simulação no Wokwi](https://img.shields.io/badge/Simulação-Wokwi-blue)](https://wokwi.com/projects/423720209071389697)
