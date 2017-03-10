#include "main.h"

#define COM_PORT "COM7"
#define BUF_LEN 256
#define TIMEOUT 30
#define FRAME_N 10000

void VRC_Proc(const VirtualRC_t* vrc)
{
	printf("*************************************VRC********************************************\n");
	printf("ch0=%d,ch1=%d,ch2=%d,ch3=%d,s1=%d,s2=%d\n",
		vrc->ch[0], vrc->ch[1], vrc->ch[2], vrc->ch[3], vrc->sw[0], vrc->sw[1]);
	//printf("*************************************VRC********************************************\n");
}

void VHC_Proc(const VirtualHC_t* vhc)
{
	printf("*************************************VHC********************************************\n");
	printf("k=%d,x=%d,y=%d,z=%d,l=%d,r=%d\n",
		vhc->key.val, vhc->mouse.x, vhc->mouse.y, vhc->mouse.z,
		vhc->mouse.b[0], vhc->mouse.b[1]);
	//printf("*************************************VHC********************************************\n");
}

void VDBUS_Proc(const VirtualDBUS_t* vdbus)
{
	printf("*************************************VDBUS********************************************\n");
	printf("ch0=%d,ch1=%d,ch2=%d,ch3=%d,s1=%d,s2=%d,k=%d,x=%d,y=%d,z=%d,l=%d,r=%d\n",
		vdbus->rcp.ch[0], vdbus->rcp.ch[1], vdbus->rcp.ch[2], vdbus->rcp.ch[3], vdbus->rcp.sw[0],
		vdbus->rcp.sw[1], vdbus->hcp.key.val, vdbus->hcp.mouse.x, vdbus->hcp.mouse.y, vdbus->hcp.mouse.z,
		vdbus->hcp.mouse.b[0], vdbus->hcp.mouse.b[1]);
	//printf("*************************************VDBUS********************************************\n");
}

void VCBUS_Proc(const VirtualCBUS_t* vcbus)
{
}

static void Dnl_ProcCalibMsg(const CalibMsg_t* calibMsg)
{
	printf("*************************************CALIB********************************************\n");
	printf("kp=%d,ki=%d,kd=%d,eh=%d,el=%d,ph=%d,pl=%d\n", calibMsg->pid.kp, calibMsg->pid.ki, calibMsg->pid.kd, calibMsg->pos.elevator_h, calibMsg->pos.elevator_l, calibMsg->pos.pwm_h, calibMsg->pos.pwm_l);
	//printf("*************************************CALIB********************************************\n");
}

static void Dnl_ProcMotorMsg(const MotorMsg_t* motorMsg)
{
	printf("*************************************MOTOR********************************************\n");
	printf("id=%d,ecd_angle=%d,angle=%d,rate=%d,current=%d\n", motorMsg->id, motorMsg->ecd_angle, motorMsg->angle, motorMsg->rate, motorMsg->current);
}

static void Dnl_ProcOdomeMsg(const OdomeMsg_t* odomeMsg)
{
	printf("*************************************ODOME********************************************\n");
	printf("px=%d,py=%d,pz=%d,vx=%d,vy=%d,vz=%d\n", odomeMsg->px, odomeMsg->py, odomeMsg->pz, odomeMsg->vx, odomeMsg->vy, odomeMsg->vz);
}

static void Dnl_ProcStatuMsg(const StatuMsg_t* statuMsg)
{
	printf("*************************************STATU********************************************\n");
	printf("wgd=%d,ini=%d\n", statuMsg->wdg, statuMsg->ini);
}

static void Dnl_ProcZGyroMsg(const ZGyroMsg_t* zgyroMsg)
{
	printf("*************************************ZGYRO********************************************\n");
	printf("angle=%d,rate=%d\n", zgyroMsg->angle, zgyroMsg->rate);
}

int main()
{
	if (connect_serial(COM_PORT) == -1)
	{
		printf("serial open error!\n");
		return -1;
	}

	FIFO_t rx_fifo;
	uint8_t buf[2][BUF_LEN];
	FIFO_Init(&rx_fifo, buf[0], BUF_LEN);

	VirtualDBUS_t vdbus;
	VirtualCBUS_t vcbus;
	CalibMsg_t calibMsg;
	MotorMsg_t motorMsg;
	ZGyroMsg_t zgyroMsg;
	OdomeMsg_t odomeMsg;

	for (uint32_t i = 0; i < FRAME_N; i++) {
		// Get fifo free space
		uint32_t len = FIFO_GetFree(&rx_fifo);
		// If fifo free space insufficient, pop one element out
		if (!len) {
			uint8_t b;
			len = FIFO_Pop(&rx_fifo, &b, 1);
		}
		// Read input stream according to the fifo free space left
		len = read_serial(buf[1], len, TIMEOUT);
		// Push stream into fifo
		FIFO_Push(&rx_fifo, buf[1], len);
		// Check if any message received
		if (Msg_Pop(&rx_fifo, &msg_header_vrc, &vdbus.rcp)) {
			VRC_Proc(&vdbus.rcp);
		}
		else if (Msg_Pop(&rx_fifo, &msg_header_vhc, &vdbus.hcp)) {
			VHC_Proc(&vdbus.hcp);
		}
		else if (Msg_Pop(&rx_fifo, &msg_header_vdbus, &vdbus)) {
			VDBUS_Proc(&vdbus);
		}
		else if (Msg_Pop(&rx_fifo, &msg_header_vcbus, &vcbus)) {
			VCBUS_Proc(&vcbus);
		}
		else if (Msg_Pop(&rx_fifo, &msg_header_calib, &calibMsg)) {
			Dnl_ProcCalibMsg(&calibMsg);
		}
		else if (Msg_Pop(&rx_fifo, &msg_header_motor, &motorMsg)) {
			Dnl_ProcMotorMsg(&motorMsg);
		}
		else if (Msg_Pop(&rx_fifo, &msg_header_zgyro, &zgyroMsg)) {
			Dnl_ProcZGyroMsg(&zgyroMsg);
		}
		else if (Msg_Pop(&rx_fifo, &msg_header_odome, &odomeMsg)) {
			Dnl_ProcOdomeMsg(&odomeMsg);
		}
	}
	disconnect_serial();
	//printf("msg_header_vrc=%x\n", MSG_HEADER_VALUE_VRC);
	while (getchar() == '\0');
	return 0;
}