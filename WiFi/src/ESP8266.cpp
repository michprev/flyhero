/*
 * ESP8266.cpp
 *
 *  Created on: 2. 4. 2017
 *      Author: michp
 */

#include <ESP8266.h>

namespace flyhero {

ESP& ESP8266::Instance() {
	static ESP8266 instance;

	return instance;
}

ESP8266::ESP8266() {
	this->readPos.pos = 0;
	this->processedLength = 0;
	this->ready = false;
	this->inIPD = false;
	this->IPD_callback = NULL;
	this->state = ESP_READY;
	this->timestamp = HAL_GetTick();
	this->link_ID = -1;
	this->wait_for_wrap = false;
	this->IPD_received = 0;
	this->IPD_size = 0;
	this->huart = UART_HandleTypeDef();
	this->hdma_usart3_rx = DMA_HandleTypeDef();
	this->hdma_usart3_tx = DMA_HandleTypeDef();
}

HAL_StatusTypeDef ESP8266::Init(void (*IPD_callback)(uint8_t linkID, uint8_t *data, uint16_t length)) {
	this->IPD_callback = IPD_callback;
	if (this->UART_Init(2000000) != HAL_OK) {
		//LEDs::TurnOn(LEDs::Green | LEDs::Orange | LEDs::Yellow);
		while (true);
	}

	// PB7 RST
	if (__GPIOB_IS_CLK_DISABLED())
		__GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef rst;
	rst.Pin = GPIO_PIN_7;
	rst.Mode = GPIO_MODE_OUTPUT_PP;
	rst.Pull = GPIO_PULLUP;;
	rst.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &rst);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);

	this->reset();

	HAL_Delay(1000);
	HAL_UART_Receive_DMA(&this->huart, this->buffer, this->BUFFER_SIZE);

	this->Send("ATE0\r\n");
	this->Send("AT+CWMODE=2\r\n");
	this->Send("AT+CWSAP=\"DRON_WIFI\",\"123456789\",5,3,1,1\r\n");
	this->Send("AT+CWDHCP=0,1\r\n");
	this->Send("AT+CIPMUX=1\r\n");
	this->Send("AT+CIPSERVER=1,80\r\n");
	this->Send("AT+CIPSTART=4,\"UDP\",\"0\",0,4789,1\r\n");
}

} /* namespace The_Eye */
