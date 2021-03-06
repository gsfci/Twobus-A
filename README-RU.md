# Twobus-A

Twobus-A - это децентрализованный протокол передачи данных для микроконтроллеров AVR.
## Преимущества
 1. Только два провода данных и два питания (VCC, GND, CLK, DAT)
 2. Не зависит от источника тактирования (может работать на нестабильном внутреннем генераторе 1/4/8 МГц)
 3. До 255 устройств на одной шине
 4. Децентрализованность
 5. Малый объём скомпилированной программы
 6. Высокая скорость
 7. Встроенное шифрование XOR
## Недостатки
 1. Нет сигнала подтверждения получения. Один неправильный пакет пожет поломать все тайминги (будет реализовано в Twobus-B)
 2. Нет защиты от помех (будет реализовано в Twobus-D)
 3. Только односторонняя передача данных. Например, если устройство А хочет запросить данные у Б, то сначала А отправит пакет с запросом к Б, а затем Б отправит пакет с ответом к А, но в это время другие устройства на шине могут передавать данные друг другу, что может вызвать задержки (будет реализовано в Twobus-B)
 ## Схема подключения с 4-мя ATmega8
 ![Схема подключения с 4-мя ATmega8](https://github.com/gsfci/Twobus-A/blob/master/Twobus_A_mega8.png "Схема подключения с 4-мя ATmega8")
