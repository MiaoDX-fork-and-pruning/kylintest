/**
 * Copyright (c) 2011-2016, Jack Mo (mobangjack@foxmail.com).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "kylinbot.h"


static uint8_t exit_flag = 0;

static FIFO_t rx_fifo;
static uint8_t rx_buf[2][KYLINBOT_MSG_BUF_LEN];
static KylinMsg_t rxKylinMsg;

static FIFO_t tx_fifo;
static uint8_t tx_buf[2][KYLINBOT_MSG_BUF_LEN];
static KylinMsg_t txKylinMsg;

serial::Serial* my_serial = NULL; // our serial

void my_sleep(unsigned long milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}


std::string get_device_port()
{
	std::vector<serial::PortInfo> devices_found = serial::list_ports();

	for (auto device : devices_found) {
		printf("(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
			device.hardware_id.c_str());
	}

	if (devices_found.size() > 1) {
		std::cout << "Seems that there are more than one suitable available devices" << std::endl; // !!!!!!!!! EXCEPTION
		return std::string(); // this is empty string
	}

	if (devices_found.size() < 1) {
		std::cout << "Seems that there is no available devices !!" << std::endl;
		return std::string(); // !!!!!!!!! EXCEPTION
	}

	// ONLY ONE
	return devices_found.at(0).port.c_str();
}




bool connect_serial(std::string port, uint32_t baud)
{
	if (port.empty()) {
		std::cout << "seems no available port" << std::endl;
		return false;
	}

	my_serial = new serial::Serial(port, baud, serial::Timeout::simpleTimeout(KYLINBOT_SERIAL_IO_TIMEOUT)); // IT IS MORE TAHN CRUCIAL TO SET THIS TIMEOUT, SINC IT WILL MAKE SURE COMMAND WILL BE SENT, ESPECIALLY IN RASPBERRY, write method.

	std::cout << "Is the serial port open?";
	if (my_serial->isOpen()) {
		std::cout << " Yes." << std::endl;
		std::cout << "Going to return" << std::endl;
		return true;
	}
	else {
		std::cout << " No." << std::endl;
		return false;
	}
}

bool connect_serial(uint32_t baud)
{
	std::string port = get_device_port();
	return connect_serial(port, baud);
}

size_t read_serial(unsigned char* data, int max_size)
{
#ifdef _WIN32
	my_serial->flush(); // this library have not implement flushInput in windows
#else
	my_serial->flushInput();
#endif
	return my_serial->read(data, max_size);
}

size_t write_serial(unsigned char* buf, int len)
{
#ifdef _WIN32
	my_serial->flush(); // this library have not implement flushOutput in windows
#else
	my_serial->flushOutput();
#endif
	return my_serial->write(buf, len);
}

void disconnect_serial()
{
	my_serial->close();
}




static void init()
{
	FIFO_Init(&rx_fifo, rx_buf[0], KYLINBOT_MSG_BUF_LEN);
	FIFO_Init(&tx_fifo, tx_buf[0], KYLINBOT_MSG_BUF_LEN);
}

static void Dnl_ProcKylinMsg(const KylinMsg_t* kylinMsg)
{
        //printf("*************************************KYLIN********************************************\n");
        printf("fs=%x,px=%d,py=%d,pz=%d,pe=%d,pc=%d,vx=%d,vy=%d,vz=%d,ve=%d,vc=%d\n", kylinMsg->fs,
                kylinMsg->cp.x, kylinMsg->cp.y, kylinMsg->cp.z, kylinMsg->gp.e, kylinMsg->gp.c,
                kylinMsg->cv.x, kylinMsg->cv.y, kylinMsg->cv.z, kylinMsg->gv.e, kylinMsg->gv.c);
}

static void PullMsg()
{
	// Get fifo free space
	int len = FIFO_GetFree(&rx_fifo);
	// If fifo free space insufficient, pop one element out
	if (!len) {
		uint8_t b;
		len = FIFO_Pop(&rx_fifo, &b, 1);
	}
	// Read input stream according to the fifo free space left
	//len = read_serial(rx_buf[1], len, KYLINBOT_SERIAL_IO_TIMEOUT);
	
	len = read_serial(rx_buf[1], len);

	//printf("PULL LEN= %d\n", len);
	// Push stream into fifo
	FIFO_Push(&rx_fifo, rx_buf[1], len);
	// Check if any message received
	if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_kylin, &rxKylinMsg)) {
	    Dnl_ProcKylinMsg(&rxKylinMsg);
	}
}

void KylinBotMsgPullerThreadFunc()
{
    while (exit_flag == 0) {
		//std::cout << "puller" << std::endl;
		std::cout << ">>" << std::endl;
		PullMsg();
	      //usleep(4000);
		  my_sleep(4000);
	}

}

static void PushMsg()
{
	txKylinMsg.fs = 1;
	txKylinMsg.cv.x = 2000;
	uint32_t len = Msg_Push(&tx_fifo, tx_buf[1], & msg_head_kylin, &txKylinMsg);
	FIFO_Pop(&tx_fifo, tx_buf[1], len);
	//write_serial(tx_buf[1], len, KYLINBOT_SERIAL_IO_TIMEOUT);
	write_serial(tx_buf[1], len);
}

void KylinBotMsgPusherThreadFunc()
{
    while (exit_flag == 0) {
		std::cout << "<<" << std::endl;
	  PushMsg();
	  //usleep(4000);
	  my_sleep(4000);
    }
}

int Kylinbot_Connect(const char* port)
{
    if (connect_serial(port,KYLINBOT_SERIAL_BAUD_RATE) == false)
    {
	  printf("serial open error!\n");
	  return -1;
    }
    
	std::cout << "Serial connect done" << std::endl;

    //MyThread kylibotMsgPullerThread;
    //MyThread kylibotMsgPusherThread;

    //kylibotMsgPullerThread.create(KylinBotMsgPullerThreadFunc, NULL);
    //kylibotMsgPusherThread.create(KylinBotMsgPusherThreadFunc, NULL);

	std::thread t1(KylinBotMsgPullerThreadFunc);
	std::thread t2(KylinBotMsgPusherThreadFunc);
	std::cout << "kylintbot join start" << std::endl;
	
	t1.join();
	t2.join();

	std::cout << "kylintbot join done" << std::endl;
    while (1) {
		std::cout << "kylintbot" << std::endl;
	  //PullMsg();
    }
	
    return 0;
}


int Kylinbot_PullMsg(KylinMsg_t* kylinMsg)
{
  memcpy(&kylinMsg, &rxKylinMsg, sizeof(KylinMsg_t));
  return 0;
}

int Kylinbot_PushMsg(const KylinMsg_t* kylinMsg)
{
  memcpy(&txKylinMsg, kylinMsg, sizeof(KylinMsg_t));
  return 0;
}


int Kylinbot_Disconnect()
{
  disconnect_serial();
  exit_flag = 1;
  return 0;
}


