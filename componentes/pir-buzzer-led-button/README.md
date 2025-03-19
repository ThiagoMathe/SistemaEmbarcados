# Descrição do Código

Este código implementa um **sistema de detecção de movimento** com temporização e alerta sonoro. Ele utiliza um **sensor PIR** para detectar movimento, **LEDs** para indicar o estado do sistema (espera, temporização ou alerta) e um **buzzer** para emitir um alerta sonoro. O sistema também permite ajustar o tempo de temporização via comunicação UART e desativar o buzzer manualmente com um botão.

## Funcionalidades

1. **Detecção de Movimento**:
   - O sensor PIR detecta movimento e inicia uma temporização configurável.
   - Durante a temporização, o LED amarelo é aceso.

2. **Alerta Sonoro e Visual**:
   - Se o movimento persistir após a temporização, o LED vermelho é aceso e o buzzer é ativado.
   - O buzzer pode ser desativado manualmente pressionando um botão por 3 segundos.

3. **Ajuste de Temporização**:
   - O tempo de temporização pode ser ajustado via comunicação UART, aceitando valores de 1 a 10 segundos.

4. **Indicação de Estado com LEDs**:
   - **LED Verde**: Modo de espera (sem movimento detectado).
   - **LED Amarelo**: Temporização ativa (movimento detectado).
   - **LED Vermelho**: Alerta acionado (movimento persistente).

5. **Controle Manual do Buzzer**:
   - O botão permite desativar o buzzer manualmente se pressionado por 3 segundos.

## Simulação no Wokwi

Você pode testar e simular este código no Wokwi usando o link abaixo:

[![Simulação no Wokwi](https://img.shields.io/badge/Simulação-Wokwi-blue)](https://wokwi.com/projects/425537309055173633)