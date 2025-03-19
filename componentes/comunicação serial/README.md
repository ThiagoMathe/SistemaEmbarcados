# Repositório de Componentes para Sistemas Embarcados
# Descrição do Código

Este código é para controlar um **display de 7 segmentos** e um **LCD via I2C** utilizando o ESP32. Ele permite exibir números no display de 7 segmentos e caracteres no LCD, seguindo uma estrutura modular e fácil de integrar em projetos.

## Funcionalidades

1. **Display de 7 Segmentos**:
   - Exibe números de 0 a 9.
   - Configuração dos pinos GPIO para cada segmento (A-G).

2. **LCD via I2C**:
   - Exibe caracteres alfanuméricos.
   - Suporta comandos básicos como limpar a tela e definir a posição do cursor.

3. **Inicialização**:
   - Configura os pinos do display de 7 segmentos.
   - Inicializa a comunicação I2C para o LCD.

4. **Uso Simples**:
   -  Exibe um número no display de 7 segmentos.
   -  Exibe um caractere no LCD.

## Simulação no Wokwi

Você pode testar e simular este código no Wokwi usando o link abaixo:

[![Simulação no Wokwi](https://img.shields.io/badge/Simulação-Wokwi-blue)](https://wokwi.com/projects/425500873324288001)