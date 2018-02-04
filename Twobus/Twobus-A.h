#ifndef _TWOBUS_A_H
#define _TWOBUS_A_H
void __twobus_a_sendbit(bool bit);
void __twobus_a_sendbyte(uint8_t data);
void __twobus_a_broadcast();
void __twobus_a_listen();
void Twobus_A_init(uint8_t addr);
uint8_t Twobus_A_SendPacket(uint8_t destAddr, uint8_t encryption_key, uint16_t timeout, uint8_t data[], uint16_t dlen, bool encrypted);
#endif