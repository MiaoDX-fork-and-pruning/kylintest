#include "main.h"


#define COM_PORT 1 //"COM7"
#define BUF_LEN 256
#define TIMEOUT 30
#define FRAME_N 20000

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
	//printf("*************************************VDBUS********************************************\n");
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
	//printf("*************************************CALIB********************************************\n");
	//printf("calib_msg=%x\n", calibMsg->auto_cali_flag);
	//printf("kp=%d,ki=%d,kd=%d,eh=%d,el=%d,ph=%d,pl=%d\n", calibMsg->pid.kp, calibMsg->pid.ki, calibMsg->pid.kd, calibMsg->pos.elevator_h, calibMsg->pos.elevator_l, calibMsg->pos.pwm_h, calibMsg->pos.pwm_l);
	//printf("*************************************CALIB********************************************\n");
}

static void Dnl_ProcMotorMsg(const MotorMsg_t* motorMsg)
{
	//printf("*************************************MOTOR********************************************\n");
	printf("id=%d,ecd_angle=%d,angle=%d,rate=%d,current=%d\n", motorMsg->id, motorMsg->ecd_angle, motorMsg->angle, motorMsg->rate, motorMsg->current);
}

static void Dnl_ProcOdomeMsg(const OdomeMsg_t* odomeMsg)
{
	//printf("*************************************ODOME********************************************\n");
	printf("px=%d,py=%d,pz=%d,vx=%d,vy=%d,vz=%d\n", odomeMsg->px, odomeMsg->py, odomeMsg->pz, odomeMsg->vx, odomeMsg->vy, odomeMsg->vz);
}

static void Dnl_ProcStatuMsg(const StatuMsg_t* statuMsg)
{
	//printf("*************************************STATU********************************************\n");
	printf("wgd=%d,ini=%d\n", statuMsg->wdg, statuMsg->ini);
}

static void Dnl_ProcZGyroMsg(const ZGyroMsg_t* zgyroMsg)
{
	//printf("*************************************ZGYRO********************************************\n");
	printf("angle=%d,rate=%d\n", zgyroMsg->angle, zgyroMsg->rate);
}

static void Dnl_ProcKylinMsg(const KylinMsg_t* kylinMsg)
{
        //printf("*************************************KYLIN********************************************\n");
        printf("fs=%x,px=%d,py=%d,pz=%d,pe=%d,pc=%d,vx=%d,vy=%d,vz=%d,ve=%d,vc=%d\n", kylinMsg->fs,
                kylinMsg->cp.x, kylinMsg->cp.y, kylinMsg->cp.z, kylinMsg->gp.e, kylinMsg->gp.c,
                kylinMsg->cv.x, kylinMsg->cv.y, kylinMsg->cv.z, kylinMsg->gv.e, kylinMsg->gv.c);
}

uint8_t exit_flag = 0;

FIFO_t rx_fifo;
uint8_t rx_buf[2][BUF_LEN];

VirtualDBUS_t vdbus;
VirtualCBUS_t vcbus;
CalibMsg_t calibMsg;
MotorMsg_t motorMsg;
ZGyroMsg_t zgyroMsg;
OdomeMsg_t odomeMsg;
GraspMsg_t graspMsg;
KylinMsg_t kylinMsg;

void PullMsg()
{
	// Get fifo free space
	int len = FIFO_GetFree(&rx_fifo);
	// If fifo free space insufficient, pop one element out
	if (!len) {
		uint8_t b;
		len = FIFO_Pop(&rx_fifo, &b, 1);
	}
	// Read input stream according to the fifo free space left
	len = read_serial(rx_buf[1], len, TIMEOUT);
	//printf("PULL LEN= %d\n", len);
	// Push stream into fifo
	FIFO_Push(&rx_fifo, rx_buf[1], len);
	// Check if any message received
	if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_vrc, &vdbus.rcp)) {
		//VRC_Proc(&vdbus.rcp);
	}
	else if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_vhc, &vdbus.hcp)) {
		//VHC_Proc(&vdbus.hcp);
	}
	else if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_vdbus, &vdbus)) {
		//VDBUS_Proc(&vdbus);
	}
	else if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_vcbus, &vcbus)) {
		//VCBUS_Proc(&vcbus);
	}
	else if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_calib, &calibMsg)) {
		//Dnl_ProcCalibMsg(&calibMsg);
	}
	else if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_motor, &motorMsg)) {
		//Dnl_ProcMotorMsg(&motorMsg);
	}
	else if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_zgyro, &zgyroMsg)) {
		//Dnl_ProcZGyroMsg(&zgyroMsg);
	}
	else if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_odome, &odomeMsg)) {
		//Dnl_ProcOdomeMsg(&odomeMsg);
	}
	else if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_grasp, &graspMsg)) {
		//Dnl_ProcGraspMsg(&graspMsg);
	}
	else if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_kylin, &kylinMsg)) {
		//printf("Before pop -> size: %d, used: %d, free: %d\n", FIFO_GetSize(&rx_fifo), FIFO_GetUsed(&rx_fifo), FIFO_GetFree(&rx_fifo));
		//printf("Before pop: %d\n", FIFO_GetFree(&rx_fifo));
		Dnl_ProcKylinMsg(&kylinMsg);
		//printf("After pop: %d\n", FIFO_GetFree(&rx_fifo));
	}
	else {
		//uint8_t b;
		//len = FIFO_Pop(&rx_fifo, &b, 1);
	}
}

void *KylinBotMsgPullerThreadFunc(void* param)
{
    while (exit_flag == 0) {
	      PullMsg();
	      usleep(4000);
	}
	return NULL;
}

FIFO_t tx_fifo;
uint8_t tx_buf[2][BUF_LEN];
KylinMsg_t txKylinMsg;
void PushMsg()
{
	//txKylinMsg.fs = 1;
	//txKylinMsg.cv.x = 2000;
	uint32_t len = Msg_Push(&tx_fifo, tx_buf[1], & msg_head_kylin, &txKylinMsg);
	FIFO_Pop(&tx_fifo, tx_buf[1], len);
	write_serial(tx_buf[1], len, TIMEOUT);
}

void *KylinBotMsgPusherThreadFunc(void* param)
{
  while (exit_flag == 0) {
		PushMsg();
		usleep(4000);
	}
}

void init()
{
	FIFO_Init(&rx_fifo, rx_buf[0], BUF_LEN);
	FIFO_Init(&tx_fifo, tx_buf[0], BUF_LEN);
}

typedef struct
{
  uint8_t dir;
  uint32_t cnt;
  uint32_t scl;
}Tri_t;

void Tri_Init(Tri_t* tri, uint32_t scl)
{
  tri->scl = scl;
  tri->cnt = 0;
  tri->dir = 0;
}

float Tri_Proc(Tri_t* tri)
{
  if (tri->dir == 0) {
     if (tri->cnt < tri->scl) {
       tri->cnt++;
     }
     else {
       tri->dir = 1;
     }
  }
  else if (tri->dir == 1) {
     if (tri->cnt > 0) {
       tri->cnt--;
     }
  }
  return (float)tri->cnt / (float)tri->scl;
}

void Tri_Reset(Tri_t* tri)
{
  tri->cnt = 0;
  tri->dir = 0;
  //memset(tri, 0, sizeof(Tri_t));
}

#define MAF_BUF_LEN 20
static uint32_t cnt = 0;
static Rmp_t rmp;
static Tri_t tri;
static Maf_t maf;
static float maf_buf[MAF_BUF_LEN];
static uint8_t dir = 0;
void kylinbot_control()
{
  ++cnt;
  
  if (cnt < 5e4) {
    if (dir != 1) {
      Rmp_Reset(&rmp);
      Tri_Reset(&tri);
      dir = 1;
    }
    // Move forward
    printf("Move forward\n");
    memset(&txKylinMsg, 0, sizeof(KylinMsg_t));
    txKylinMsg.cp.y = 1000;
    txKylinMsg.cv.y = 1000 * Tri_Proc(&tri); //* Rmp_Calc(&rmp);
  }
  else if (cnt < 10e4) {
    if (dir != 2) {
      Rmp_Reset(&rmp);
      Tri_Reset(&tri);
      dir = 2;
    }
    // Move right
    printf("Move right\n");
    memset(&txKylinMsg, 0, sizeof(KylinMsg_t));
    txKylinMsg.cp.x = 1000;
    txKylinMsg.cv.x = 1000 * Tri_Proc(&tri); // * Rmp_Calc(&rmp);;
  }
  else if (cnt < 15e4) {
    if (dir != 3) {
      Rmp_Reset(&rmp);
      Tri_Reset(&tri);
      dir = 3;
    }
    // Move backward
    printf("Move backward\n");
    memset(&txKylinMsg, 0, sizeof(KylinMsg_t));
    txKylinMsg.cp.y = -1000;
    txKylinMsg.cv.y = -1000 * Tri_Proc(&tri); // * Rmp_Calc(&rmp);;
  }
  else if (cnt < 20e4) {
    if (dir != 4) {
      Rmp_Reset(&rmp);
      Tri_Reset(&tri);
      dir = 4;
    }
    // Move left
    printf("Move left\n");
    memset(&txKylinMsg, 0, sizeof(KylinMsg_t));
    txKylinMsg.cp.x = -1000;
    txKylinMsg.cv.x = -1000 * Tri_Proc(&tri); // * Rmp_Calc(&rmp);;
  }
  else {
    cnt = 0;
  }
  
  printf("cnt=%d\n", tri.cnt);
}

int main(int argc, char** argv)
{
        init();
	uint32_t cnt = 0;
	Rmp_Config(&rmp, 50000);
	Maf_Init(&maf, maf_buf, MAF_BUF_LEN);
	Tri_Init(&tri, 2.5e4);
        if (argc < 2) {
            printf("Please input device name\n");
            return 0;
        }
        char* device = argv[1];
        if (connect_serial(device,115200) == -1)
	{
		printf("serial open error!\n");
		return -1;
	}

	MyThread kylibotMsgPullerTread;
	MyThread kylibotMsgPusherTread;
	
	kylibotMsgPullerTread.create(KylinBotMsgPullerThreadFunc, NULL);
	kylibotMsgPusherTread.create(KylinBotMsgPusherThreadFunc, NULL);
	
	//KylinBotMsgPullerThreadFunc(NULL);
	while (!exit_flag) {
	  txKylinMsg.cp.x = 0;
	  txKylinMsg.cv.x = -1000;
	  txKylinMsg.cp.y = 0;
	  txKylinMsg.cv.y = 1000;
	  txKylinMsg.cp.z = 0;
	  txKylinMsg.cv.z = 1000;
	  usleep(10000);
	  //kylinbot_control();
	  //PullMsg();
	}
	
	disconnect_serial();

	//while (getchar() == '\0');
	return 0;
}

int main__(int argc, char** argv)
{
  int flag = Kylinbot_Connect();
  if (flag == -1) {
    return -1;
  }
  
  while (flag != -1) {
    usleep(10000);
  }
  
  Kylinbot_Disconnect();
  
  return 0;
}