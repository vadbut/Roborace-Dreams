////////  UDP Roborace for Speed Dreams  \\\\\\\\\\

// Useful funcs
// printout to console:
// GfOut("%s %s\n", "Hello world", "!!!");

#include <Ws2tcpip.h>
#include <fstream>

#include <tgf.h>
#include "CarControl.h"
#include <windows.h>
#include "driver.h"
#include "Utils.h"

/*** definitions for UDP *****/
#define UDP_MSGLEN 1000
const int UDP_RECEIVE_PORT[20] = {3001,3002,3003,3004,3005,3006,3007,3008,3009,3010,3011,3012,3013,3014,3015,3016,3017,3018,3019,3020};
SOCKET ReceiveSocket[20];
struct sockaddr_in RecvAddr[20];      /* our address */
struct sockaddr_in RecvRemaddr[20];     /* remote address */
socklen_t RecvAddrlen = sizeof(RecvRemaddr);            /* length of addresses */
int recvlen;
udpCmdType udpCmd[20];
int receiveSkipped[20];
unsigned __int8 prevctr[20];
unsigned char udp_ctr[20];

const int UDP_SEND_PORT[20] = {4001,4002,4003,4004,4005,4006,4007,4008,4009,4010,4011,4012,4013,4014,4015,4016,4017,4018,4019,4020};
SOCKET SendSocket[20];
struct sockaddr_in SendAddr[20];      /* our address */
struct sockaddr_in SendRemaddr[20];     /* remote address */

char Ncars = 0; //number of participants
char car_indxs[20]; //participants indexes
tCarElt* oCar[20];        // pointer to tCarElt struct
float engineMap[100][50];
char engineMapNvert = 0;
char engineMapNhoriz = 0;
/************************/


TDriver::TDriver(int index)
{
  mCarIndex = index;
  oCar[index] = NULL;
  mOldTimer = 0.0;
}


TDriver::~TDriver()
{
}


void TDriver::InitTrack(PTrack Track, PCarHandle CarHandle, PCarSettings *CarParmHandle, PSituation Situation)
{
  mTrack = Track;
  mTankvol = GfParmGetNum(CarHandle, SECT_CAR, PRM_TANK, (char*)NULL, 50);

  // Get file handles
  char* trackname = strrchr(Track->filename, '/') + 1;
  char buffer[256];

  // Discover the car type used
  void* handle = NULL;
  std::sprintf(buffer, "drivers/%s/%s.xml", MyBotName, MyBotName);
  handle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
  std::sprintf(buffer, "%s/%s/%d", ROB_SECT_ROBOTS, ROB_LIST_INDEX, mCarIndex);
  mCarType = GfParmGetStr(handle, buffer, (char*)ROB_ATTR_CAR, "no good");

  // Parameters that are the same for all tracks
  handle = NULL;
  std::sprintf(buffer, "drivers/%s/%s/_all_tracks.xml", MyBotName, mCarType.c_str());
  handle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
  if (handle == NULL) {
    mLearning = 0;
    mTestpitstop = 0;
    mTestLine = 0;
    mDriverMsgLevel = 0;
    mDriverMsgCarIndex = 0;
    mFRONTCOLL_MARGIN = 4.0;
  } else {
    mLearning = GfParmGetNum(handle, "private", "learning", (char*)NULL, 0.0) != 0;
    //mLearning = 1;
    mTestpitstop = GfParmGetNum(handle, "private", "test pitstop", (char*)NULL, 0.0) != 0;
    //mTestpitstop = 1;
    mTestLine = (int)GfParmGetNum(handle, "private", "test line", (char*)NULL, 0.0);
    mDriverMsgLevel = (int)GfParmGetNum(handle, "private", "driver message", (char*)NULL, 0.0);
    mDriverMsgCarIndex = (int)GfParmGetNum(handle, "private", "driver message car index", (char*)NULL, 0.0);
    mFRONTCOLL_MARGIN = GfParmGetNum(handle, "private", "frontcollmargin", (char*)NULL, 4.0);
  }

  // Parameters that are track specific
  *CarParmHandle = NULL;
  switch (Situation->_raceType) {
    case RM_TYPE_QUALIF:
      std::sprintf(buffer, "drivers/%s/%s/qualifying/%s", MyBotName, mCarType.c_str(), trackname);
      *CarParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
      break;
    default:
      break;
  }
  if (*CarParmHandle == NULL) {
    std::sprintf(buffer, "drivers/%s/%s/%s", MyBotName, mCarType.c_str(), trackname);
    *CarParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
  }
  if (*CarParmHandle == NULL) {
    std::sprintf(buffer, "drivers/%s/%s/default.xml", MyBotName, mCarType.c_str());
    *CarParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
  }
  mFuelPerMeter = GfParmGetNum(*CarParmHandle, "private", "fuelpermeter", (char*)NULL, 0.001f);
  
  // Set initial fuel
  double distance = Situation->_totLaps * mTrack->length;
  if (mTestpitstop) {
    distance = 1.9 * mTrack->length;
  }

  if (mLearning) {
    mFuelStart = mTankvol;
  }
  GfParmSetNum(*CarParmHandle, SECT_CAR, PRM_FUEL, (char*)NULL, (tdble) mFuelStart);
  
  // Get skill level
  handle = NULL;
  std::sprintf(buffer, "%sconfig/raceman/extra/skill.xml", GetLocalDir());
  handle = GfParmReadFile(buffer, GFPARM_RMODE_REREAD);
  double globalskill = 0.0;
  if (handle != NULL) {
    // Skill levels: 0 pro, 3 semi-pro, 7 amateur, 10 rookie
    globalskill = GfParmGetNum(handle, "skill", "level", (char*)NULL, 0.0);
  }
  mSkillGlobal = MAX(0.9, 1.0 - 0.1 * globalskill / 10.0);
  //load the driver skill level, range 0 - 1
  handle = NULL;
  std::sprintf(buffer, "drivers/%s/%d/skill.xml", MyBotName, mCarIndex);
  handle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
  double driverskill = 0.0;
  if (handle != NULL) {
    driverskill = GfParmGetNum(handle,"skill","level", (char*)NULL, 0.0);
  }
  mSkillDriver = MAX(0.95, 1.0 - 0.05 * driverskill);
}


void TDriver::NewRace(PtCarElt Car, PSituation Situation, int index)
{
  //initialize engine map
  ifstream enginefile ("cars/models/Robocar/engine_map.csv");
  string line, sub_line;
  if (enginefile.is_open())
  {
	while ( getline (enginefile,line) )
    {
		stringstream stream_line(line);
		engineMapNhoriz = 0;
		while (getline(stream_line,sub_line,',')){
			engineMap[engineMapNvert][engineMapNhoriz] = atof(sub_line.c_str());
			engineMapNhoriz++;
		}
		engineMapNvert++;
    }
    enginefile.close();
  }

  // index corresponds to the drivers from the list. E.g. 0 for udp1, 1 for udp2 regardless of who participates in the race. Good
  car_indxs[Ncars] = index;
  Ncars++;

  receiveSkipped[index] = 0;
  udp_ctr[index] = 0;
  prevctr[index] = 0;

  oCar[index] = Car;
  oSituation = Situation;
	
  // Init UDP
  int iResult;
  WSADATA wsaData;
  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != NO_ERROR) {
	  GfOut("%s\n", "WSAStartup failed");
  }

  // Create a socket for receiving data
  ReceiveSocket[index] = INVALID_SOCKET;
  ReceiveSocket[index] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (ReceiveSocket[index] == INVALID_SOCKET) {
	  GfOut("%s\n", "Socket assignment failed");
      WSACleanup();
  }
  // Create a socket for sending data
  SendSocket[index] = INVALID_SOCKET;
  SendSocket[index] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (SendSocket[index] == INVALID_SOCKET) {
	  GfOut("%s\n", "Socket assignment failed");
      WSACleanup();
  }

  //Set SendSocket to Broadcast mode
  int broadcastPermission = 1;
  if (setsockopt(SendSocket[index], SOL_SOCKET, SO_BROADCAST, (char *) &broadcastPermission,sizeof(broadcastPermission)) < 0){
       GfOut("%s\n", "Error when switching to broadcast mode for Send port");
  }
  
  //create addresses for Receive and Send sockets
  RecvAddr[index].sin_family = AF_INET;
  RecvAddr[index].sin_addr.s_addr = htonl(INADDR_ANY);
  RecvAddr[index].sin_port = htons(UDP_RECEIVE_PORT[index]);
  SendAddr[index].sin_family = AF_INET;
  SendAddr[index].sin_addr.s_addr = inet_addr("255.255.255.255");//htonl(INADDR_ANY);
  SendAddr[index].sin_port = htons(UDP_SEND_PORT[index]);

  if (bind(ReceiveSocket[index], (struct sockaddr *)&RecvAddr[index], sizeof(RecvAddr[index])) < 0) {
      GfOut("%s\n", "Socket bind failed");
	  closesocket(ReceiveSocket[index]);
	  WSACleanup();
  }

  GfOut("%s %i %s%i\n", "Waiting for UDP Controller", index+1, "at Port=", UDP_RECEIVE_PORT[index]);
  recvfrom(ReceiveSocket[index], (char*)(&udpCmd[index]), sizeof(udpCmd[index]), 0, (struct sockaddr *)&RecvRemaddr[index], &RecvAddrlen);	
  if (!((udpCmd[index].CtrlMode == 3)||(udpCmd[index].CtrlMode == 11)||(udpCmd[index].CtrlMode == 12))){
	GfOut("%s\n", "Controller status is invalid");
	closesocket(ReceiveSocket[index]);
	WSACleanup();
	udpCmd[index].brakeCmd = 1;
	udpCmd[index].gasCmd = 0;
  }

  //assign the socket to be non-blocking. Otherwise recvfrom will be blocking execution until it receives smth
  u_long iMode = 1;
  ioctlsocket(ReceiveSocket[index], FIONBIO, &iMode); 
}


void TDriver::Drive(int index)
{
#ifdef TIME_ANALYSIS
  struct timeval tv;
  gettimeofday(&tv, NULL); 
  double usec1 = tv.tv_usec;
#endif
  updateTime();
  updateTimer();
  setControls(index);
#ifdef TIME_ANALYSIS
  gettimeofday(&tv, NULL); 
  double usec2 = tv.tv_usec;
#endif
}


void TDriver::EndRace()                             // Stop race
{
  // This is never called by TORCS! Don't use it!
}


void TDriver::Shutdown(int index)                            // Cleanup
{
	closesocket(ReceiveSocket[index]);

	//Place data for sending
	float tempF = 42.42;
	std::string SendBuf;
	const unsigned int sizeString = 794;
	SendBuf.reserve( sizeString );
	for (int j=1; j<=32; j++){//fill it in with whatever
		SendBuf.append( (const char*) &tempF,  sizeof(float));//Message size: 4
	}
	char Ncars = 0;	float V2x[39]; char V2xchar[39];
	SendBuf.append( (const char*) &Ncars,  sizeof(char));//125
	SendBuf.append( (const char*) &V2xchar,  sizeof(V2xchar));//201
	SendBuf.append( (const char*) &V2x,  sizeof(V2x));//201
	SendBuf.append( (const char*) &V2x,  sizeof(V2x));//277
	SendBuf.append( (const char*) &V2x,  sizeof(V2x));//353
	SendBuf.append( (const char*) &V2x,  sizeof(V2x));//429

	unsigned char status = 5; //we are stopping
	SendBuf.append( (const char*) &status,  sizeof(char));//125
	SendBuf.append( (const char*) &udp_ctr,  sizeof(char));//126

	//send out the data
	int iResult = sendto(SendSocket[index], (char*)&SendBuf[0], sizeString, 0, (SOCKADDR *) & SendAddr[index], sizeof (SendAddr[index]));
    if (iResult == SOCKET_ERROR) {
		GfOut("Sendto failed %d\n", WSAGetLastError());
        closesocket(SendSocket[index]);
        WSACleanup();
    }
	closesocket(SendSocket[index]);
	WSACleanup();

}


void TDriver::updateTime()
{
  mOldTimer = oCurrSimTime;
  oCurrSimTime = oSituation->currentTime;
}


void TDriver::updateTimer()
{
  double diff = oCurrSimTime - mOldTimer;
  //GfOut("diff of time: %g\n", diff);
  if (diff >= 0.1) {
    mOldTimer += 0.1;
    mTenthTimer = true;
  } else {
    mTenthTimer = false;
  }
}

void TDriver::setControls(int index)
{
	/**********************************************************************
     ****************** Building state string *****************************
     **********************************************************************/
	udp_ctr[index]++;
	//Place data for sending
	float tempF, rpm; __int32 tempInt, collision;
	std::string SendBuf;
	const unsigned int sizeString = 794;
	SendBuf.reserve( sizeString );

	tempF = float(oCar[index]->_curLapTime);
	SendBuf.append( (const char*) &tempF,  sizeof(float));//Message size: 4
	SendBuf.append( (const char*) &oCar[index]->race.distFromStartLine,  sizeof(float));//8
	tempF = float(oCar[index]->_lastLapTime);
	SendBuf.append( (const char*) &tempF,  sizeof(float));//12
	tempInt = (__int32)oCar[index]->race.pos;
	SendBuf.append( (const char*) &tempInt,  sizeof(__int32));//16
	rpm = float(oCar[index]->_enginerpm*10);
	SendBuf.append( (const char*) &rpm,  sizeof(float));//20
	SendBuf.append( (const char*) &oCar[index]->_fuel,  sizeof(float));//24
	tempInt = (__int32)oCar[index]->_gear;
	SendBuf.append( (const char*) &tempInt,  sizeof(__int32));//28
	SendBuf.append( (const char*) &oCar[index]->_wheelSpinVel(0),  sizeof(float));//32
	SendBuf.append( (const char*) &oCar[index]->_wheelSpinVel(1),  sizeof(float));//36
	SendBuf.append( (const char*) &oCar[index]->_wheelSpinVel(2),  sizeof(float));//40
	SendBuf.append( (const char*) &oCar[index]->_wheelSpinVel(3),  sizeof(float));//44
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.pos.x,  sizeof(float));//48
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.pos.y,  sizeof(float));//52
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.pos.z,  sizeof(float));//56
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.pos.ax,  sizeof(float));//60
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.pos.ay,  sizeof(float));//64
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.pos.az,  sizeof(float));//68
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.vel.ax,  sizeof(float));//72
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.vel.ay,  sizeof(float));//76
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.vel.az,  sizeof(float));//80
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.vel.x,  sizeof(float));//84
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.vel.y,  sizeof(float));//88
	SendBuf.append( (const char*) &oCar[index]->pub.DynGCg.vel.z,  sizeof(float));//92
	SendBuf.append( (const char*) &oCar[index]->pub.DynGC.acc.x,  sizeof(float));//96
	SendBuf.append( (const char*) &oCar[index]->pub.DynGC.acc.y,  sizeof(float));//100
	SendBuf.append( (const char*) &oCar[index]->pub.DynGC.acc.z,  sizeof(float));//104
	SendBuf.append( (const char*) &oCar[index]->_steerCmd,  sizeof(float));//108
	SendBuf.append( (const char*) &oCar[index]->priv.reaction[0],  sizeof(float));//112
	SendBuf.append( (const char*) &oCar[index]->priv.reaction[1],  sizeof(float));//116
	SendBuf.append( (const char*) &oCar[index]->priv.reaction[2],  sizeof(float));//120
	SendBuf.append( (const char*) &oCar[index]->priv.reaction[3],  sizeof(float));//124
	collision = (__int32)oCar[index]->priv.collision;
	SendBuf.append( (const char*) &collision,  sizeof(__int32));//128
	//V2V
	char V2xType[39];// 19 other cars + 20 other obstacles
	float V2xXpos[39];
	float V2xYpos[39];
	float V2xSpeed[39];
	float V2xHeading[39];
	char ind = 0;				
	float x_ego = oCar[index]->pub.DynGCg.pos.x;
	float y_ego = oCar[index]->pub.DynGCg.pos.y;

	for (char i=0; i<Ncars; i++){
		float x_opp = oCar[car_indxs[i]]->pub.DynGCg.pos.x;
		float y_opp = oCar[car_indxs[i]]->pub.DynGCg.pos.y;
		float xd = pow(float(x_ego-x_opp) , float(2.0));
		float yd = pow(float(y_ego-y_opp) , float(2.0));
		if ((pow( float(xd + yd) , float(0.5)) < 500) && (index != car_indxs[i])){// the car is close to us, and it's not us
			V2xType[ind] = 1; //normal race participant
			V2xXpos[ind] = x_opp;
		    V2xYpos[ind] = y_opp;
			float V2xspeed2 = pow(oCar[car_indxs[i]]->pub.DynGCg.vel.x, 2) + pow(oCar[car_indxs[i]]->pub.DynGCg.vel.y, 2);
			V2xSpeed[ind] = pow( V2xspeed2, float(0.5));
			V2xHeading[ind] = oCar[car_indxs[i]]->pub.DynGCg.pos.az;
			ind++;
		}
	}
	SendBuf.append( (const char*) &ind,  sizeof(char));//129
	SendBuf.append( (const char*) &V2xType,  sizeof(V2xType));//168
	SendBuf.append( (const char*) &V2xXpos,  sizeof(V2xXpos));//324
	SendBuf.append( (const char*) &V2xYpos,  sizeof(V2xYpos));//480
	SendBuf.append( (const char*) &V2xSpeed,  sizeof(V2xSpeed));//636
	SendBuf.append( (const char*) &V2xHeading,  sizeof(V2xHeading));//792

	unsigned char status = 3;
	SendBuf.append( (const char*) &status,  sizeof(char));//793
	SendBuf.append( (const char*) &udp_ctr,  sizeof(char));//794

	//send out the data
	int iResult = sendto(SendSocket[index], (char*)&SendBuf[0], sizeString, 0, (SOCKADDR *) & SendAddr[index], sizeof (SendAddr[index]));
    if (iResult == SOCKET_ERROR) {
		GfOut("Sendto failed %d\n", WSAGetLastError());
        closesocket(SendSocket[index]);
        WSACleanup();
    }

	//read until there are packets in the queue
    int read = 0;//number of reads
	while ((read >= 0)&&(receiveSkipped[index] <= 100)){
		recvlen = recvfrom(ReceiveSocket[index], (char*)(&udpCmd[index]), sizeof(udpCmd[index]), 0, (struct sockaddr *)&RecvRemaddr[index], &RecvAddrlen);
		if (recvlen > 0){
			read++;}
		else{
			//GfOut("number of read %u\n", read);
			if (read>0){
				if (udpCmd[index].ctrlCounter != prevctr[index]) {
					receiveSkipped[index] = 0;
					prevctr[index] = udpCmd[index].ctrlCounter;
				}else{
					receiveSkipped[index]++;//if counter didn't change - udp controller might have froze
					if (receiveSkipped[index] > 100)
					{
						GfOut("UDP controller seems to be stuck. Closing UDP connection and applying vehicle hard stop\n");
						closesocket(ReceiveSocket[index]);
						WSACleanup();
						udpCmd[index].gasCmd = 0;
						udpCmd[index].brakeCmd = 1;
					}	
				}
			}else{
				receiveSkipped[index]++;
				//GfOut("Didn't receive a UDP command\n");
				if (receiveSkipped[index] > 100)
				{
					GfOut("Didn't receive a UDP cmd for too long. Closing UDP connection and applying vehicle hard stop\n");
					closesocket(ReceiveSocket[index]);
					WSACleanup();
					udpCmd[index].gasCmd = 0;
					udpCmd[index].brakeCmd = 1;
				}
			}
			read = -1;}
		}

        // Set controls command and store them in variables
	    if (udpCmd[index].CtrlMode == 11){//gas cmd
			oCar[index]->_accelCmd = udpCmd[index].gasCmd;
		}else if ((udpCmd[index].CtrlMode == 3)||(udpCmd[index].CtrlMode == 12)){//torque cmd, need to interpolate		
			int i,j;
			for (i=1; i<engineMapNvert; i++){
				if (rpm < engineMap[i][0]){
					break;
				}
			}
			float Trpm[50], rpm1, rpm2;
			// interpolate to get Torque map for the RPM value
			rpm1 = engineMap[i-1][0]; rpm2 = engineMap[i][0];
			for (j=1; j<engineMapNhoriz; j++){
				Trpm[j]=(rpm-rpm1)/(rpm2-rpm1)*engineMap[i][j] + (rpm2-rpm)/(rpm2-rpm1)*engineMap[i-1][j];
			}
			for (i=1; i<engineMapNhoriz; i++){
				if (udpCmd[index].gasCmd < Trpm[i]){
					break;
				}
			}
			float Trpm1, Trpm2, g1, g2, gas;
			Trpm1 = Trpm[i-1]; Trpm2 = Trpm[i];
			g1 = engineMap[0][i-1]; g2 = engineMap[0][i];
			gas = (udpCmd[index].gasCmd-Trpm1)/(Trpm2-Trpm1)*(g2-g1)+g1;
			oCar[index]->_accelCmd = gas;
			if (gas>1){
				gas = 1;
			}else if (gas<0){
				gas = 0;
			}
			if (udpCmd[index].CtrlMode == 12){
				GfOut("Input Torque: %f, RPM: %f Corresponding gas: %f\n",udpCmd[index].gasCmd, rpm, gas);
			}
		}
		// steer_ang: receive in deg -> convert to rads -> [-1,1]
		float ang = (udpCmd[index].steeringCmd/180 * 3.14159) / oCar[car_indxs[index]]->info.steerLock;
		if (ang > 1){
			ang = 1;
		}else if (ang < -1){
			ang = -1;
		}
		oCar[index]->_steerCmd = ang;
        oCar[index]->_brakeCmd = udpCmd[index].brakeCmd;
        oCar[index]->_gearCmd  = udpCmd[index].gearCmd;      
        oCar[index]->_clutchCmd = 0;

  oCar[index]->_lightCmd = RM_LIGHT_HEAD1 | RM_LIGHT_HEAD2;
}

