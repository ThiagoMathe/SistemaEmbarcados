# Descrição do Código

Este código implementa um **sistema de detecção de movimento** utilizando um **sensor PIR** e um **LED** para indicar a detecção. Quando o sensor PIR detecta movimento, ele aciona uma interrupção que faz o LED piscar repetidamente, simulando um alarme. O ESP32 é utilizado para gerenciar a interrupção e controlar o LED.

## Funcionalidades

1. **Detecção de Movimento**:
   - O sensor PIR é configurado para gerar uma interrupção na borda de subida (quando o movimento é detectado).
   - A interrupção envia um evento para uma fila, que é processado pela tarefa principal.

2. **Indicação Visual com LED**:
   - Quando o movimento é detectado, o LED pisca 30 vezes com um intervalo de 100ms entre cada estado (ligado/desligado).

3. **Comunicação via Fila**:
   - A interrupção do sensor PIR envia o número do GPIO para uma fila, e a tarefa principal processa o evento, garantindo um tratamento seguro e eficiente.

4. **Exibição no Terminal**:
   - Quando o movimento é detectado, uma mensagem "ALARME ACIONADO!" é exibida no terminal.

## Simulação no Wokwi

Você pode testar e simular este código no Wokwi usando o link abaixo:

[![Simulação no Wokwi](https://img.shields.io/badge/Simulação-Wokwi-blue)](https://wokwi.com/projects/425520100351354881)