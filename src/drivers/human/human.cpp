/***************************************************************************

    created              : Sat Mar 18 23:16:38 CET 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id$

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** @file

    @author	<a href=mailto:torcs@free.fr>Eric Espie</a>
    @version	$Id$
*/

/* 2013/3/21 Tom Low-Shang
 *
 * Moved original contents of
 *
 * drivers/human/human.cpp,
 * drivers/human/human.h,
 * drivers/human/pref.cpp,
 * drivers/human/pref.h,
 *
 * to libs/robottools/rthumandriver.cpp.
 *
 * CMD_* defines from pref.h are in interfaces/playerpref.h.
 *
 * Robot interface entry points are still here.
 */


#define UDP_STREAM
#ifdef UDP_STREAM
	#include <Ws2tcpip.h>

	#include <tgf.h>
	#include "CarControl.h"
	#include <windows.h>

	#define UDP_SEND_PORT 5001
	SOCKET SendSocket = INVALID_SOCKET;
	struct sockaddr_in SendAddr;      /* our address */
	struct sockaddr_in SendRemaddr;     /* remote address */
#endif


#include <humandriver.h>

static HumanDriver robot("human");

static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s);
static void drive_mt(int index, tCarElt* car, tSituation *s);
static void drive_at(int index, tCarElt* car, tSituation *s);
static void newrace(int index, tCarElt* car, tSituation *s);
static void resumerace(int index, tCarElt* car, tSituation *s);
static int  pitcmd(int index, tCarElt* car, tSituation *s);

#ifdef _WIN32
/* Must be present under MS Windows */
BOOL WINAPI DllEntryPoint (HINSTANCE hDLL, DWORD dwReason, LPVOID Reserved)
{
    return TRUE;
}
#endif


static void
shutdown(const int index)
{
    robot.shutdown(index);

	#ifdef UDP_STREAM
		closesocket(SendSocket);
		WSACleanup();
	#endif
}//shutdown


/**
 *
 *	InitFuncPt
 *
 *	Robot functions initialisation.
 *
 *	@param pt	pointer on functions structure
 *  @return 0
 */
static int
InitFuncPt(int index, void *pt)
{
	tRobotItf *itf = (tRobotItf *)pt;

    robot.init_context(index);

	itf->rbNewTrack = initTrack;	/* give the robot the track view called */
	/* for every track change or new race */
	itf->rbNewRace  = newrace;
	itf->rbResumeRace  = resumerace;

	/* drive during race */
	itf->rbDrive = robot.uses_at(index) ? drive_at : drive_mt;
	itf->rbShutdown = shutdown;
	itf->rbPitCmd   = pitcmd;
	itf->index      = index;

	return 0;
}//InitFuncPt


/**
 *
 * moduleWelcome
 *
 * First function of the module called at load time :
 *  - the caller gives the module some information about its run-time environment
 *  - the module gives the caller some information about what he needs
 * MUST be called before moduleInitialize()
 *
 * @param	welcomeIn Run-time info given by the module loader at load time
 * @param welcomeOut Module run-time information returned to the called
 * @return 0 if no error occured, not 0 otherwise
 */
extern "C" int
moduleWelcome(const tModWelcomeIn* welcomeIn, tModWelcomeOut* welcomeOut)
{
	welcomeOut->maxNbItf = robot.count_drivers();

	return 0;
}//moduleWelcome


/**
 *
 * moduleInitialize
 *
 * Module entry point
 *
 * @param modInfo	administrative info on module
 * @return 0 if no error occured, -1 if any error occured
 */
extern "C" int
moduleInitialize(tModInfo *modInfo)
{
    return robot.initialize(modInfo, InitFuncPt);
}//moduleInitialize


/**
 * moduleTerminate
 *
 * Module exit point
 *
 * @return 0
 */
extern "C" int
moduleTerminate()
{
        robot.terminate();

	return 0;
}//moduleTerminate


/**
 * initTrack
 *
 * Search under robots/human/cars/<carname>/<trackname>.xml
 *
 * @param index
 * @param track
 * @param carHandle
 * @param carParmHandle
 * @param s situation provided by the sim
 *
 */
static void
initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s)
{
    robot.init_track(index, track, carHandle, carParmHandle, s);
}//initTrack


/**
 *
 * newrace
 *
 * @param index
 * @param car
 * @param s situation provided by the sim
 *
 */
void
newrace(int index, tCarElt* car, tSituation *s)
{
    robot.new_race(index, car, s);

	#ifdef UDP_STREAM
	      // Init UDP
		  int iResult;
		  WSADATA wsaData;
		  // Initialize Winsock
		  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		  if (iResult != NO_ERROR) {
			  GfOut("%s\n", "WSAStartup failed");
		  }

		  // Create a socket for sending data
		  SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		  if (SendSocket == INVALID_SOCKET) {
			  GfOut("%s\n", "Socket assignment failed");
			  WSACleanup();
		  }

		  SendAddr.sin_family = AF_INET;
		  SendAddr.sin_addr.s_addr = inet_addr("255.255.255.255");//htonl(INADDR_ANY);
		  SendAddr.sin_port = htons(UDP_SEND_PORT);

		    //Set SendSocket to Broadcast mode
		  int broadcastPermission = 1;
		  if (setsockopt(SendSocket, SOL_SOCKET, SO_BROADCAST, (char *) &broadcastPermission,sizeof(broadcastPermission)) < 0){
			   GfOut("%s\n", "Error when switching to broadcast mode for Send port");
		  }


	#endif
}//newrace


void
resumerace(int index, tCarElt* car, tSituation *s)
{
    robot.resume_race(index, car, s);
}

/*
 * Function
 *
 *
 * Description
 *
 *
 * Parameters
 *
 *
 * Return
 *
 *
 * Remarks
 *
 */
static void
drive_mt(int index, tCarElt* car, tSituation *s)
{
    robot.drive_mt(index, car, s);
}//drive_mt


/*
 * Function
 *
 *
 * Description
 *
 *
 * Parameters
 *
 *
 * Return
 *
 *
 * Remarks
 *
 */
static void
drive_at(int index, tCarElt* car, tSituation *s)
{
	// seems like this one is being called to control the car
    robot.drive_at(index, car, s);

	#ifdef UDP_STREAM
		float tempF; __int32 tempInt;
		std::string SendBuf;
		const unsigned int sizeString = 17;
		SendBuf.reserve( sizeString );

		float x_opp = car->pub.DynGCg.pos.x;
		float y_opp = car->pub.DynGCg.pos.y;
		char V2xType = 2; //human race participant
		float V2xspeed2 = pow(car->pub.DynGCg.vel.x, 2) + pow(car->pub.DynGCg.vel.y, 2);
		float V2xSpeed = pow( V2xspeed2, float(0.5));
		float V2xHeading = car->pub.DynGCg.pos.az;
	
		SendBuf.append( (const char*) &V2xType,  sizeof(char));//Message size: 1
		SendBuf.append( (const char*) &x_opp,  sizeof(float));//5
		SendBuf.append( (const char*) &y_opp,  sizeof(float));//9
		SendBuf.append( (const char*) &V2xSpeed,  sizeof(float));//13
		SendBuf.append( (const char*) &V2xHeading,  sizeof(float));//17

		//send out the data
		int iResult = sendto(SendSocket, (char*)&SendBuf[0], sizeString, 0, (SOCKADDR *) & SendAddr, sizeof (SendAddr));
		if (iResult == SOCKET_ERROR) {
			GfOut("Sendto failed %d\n", WSAGetLastError());
			closesocket(SendSocket);
			WSACleanup();
		}

	#endif

}//drive_at


static int
pitcmd(int index, tCarElt* car, tSituation *s)
{
    return robot.pit_cmd(index, car, s);
}//pitcmd
