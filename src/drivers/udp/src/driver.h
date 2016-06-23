/***************************************************************************

    file                 : driver.h
    created              : 2006-08-31 01:21:49 UTC
    copyright            : (C) Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "globaldefinitions.h"
#include "Vec2d.h"
#include <fstream>



class TDriver {
  public:
  TDriver(int index);
  ~TDriver();

  const char* MyBotName;                      // Name of this bot 

  void InitTrack(PTrack Track, PCarHandle CarHandle, PCarSettings *CarParmHandle, PSituation Situation);
  void NewRace(PtCarElt Car, PSituation Situation, int index);
  void Drive(int index);
  void EndRace();
  void Shutdown(int index);

  private:
  // Utility functions
  void updateTime();
  void updateTimer();
  void setControls(int index);
  double getBrake(double maxspeed);
  double getAccel(double maxspeed);
  double getSteer();
  int getGear();
  double getClutch();
  bool hysteresis(bool lastout, double in, double hyst);

  // Per robot global data
  int mDrvPath;
  int prev_mDrvPath;
  enum {PATH_O, PATH_L, PATH_R}; // States for mDrvPath
  int mDrvState;
  int prev_mDrvState;
  enum {STATE_RACE, STATE_STUCK, STATE_OFFTRACK, STATE_PITLANE, STATE_PITSTOP}; // States for mDrvState

  tSituation* oSituation;
  double oCurrSimTime;
  
  PTrack mTrack;
  int mCarIndex;
  std::string mCarType;

  double mOppDist;
  double mOppSidedist;
  bool mOppAside;
  bool mOppLeft;
  bool mOppLeftHyst;
  bool mOppLeftOfMe;
  bool mOppLeftOfMeHyst;
  bool mOppInFrontspace;
  bool mBackmarkerInFrontOfTeammate;
  bool mTwoOppsAside;
  bool mOppComingFastBehind;
  bool prev_mOppComingFastBehind;
  bool mLearning;
  bool mTestpitstop;
  int mTestLine;
  int mDriverMsgLevel;
  int mDriverMsgCarIndex;
  double mTankvol;
  double mFuelPerMeter;
  double mMu;    // friction coefficient
  double mMass;  // mass of car + fuel
  double mSpeed;
  double mClutchtime;
  int mPrevgear;
  bool mControlAttackAngle;
  bool prev_mControlAttackAngle;
  double mAttackAngle;
  bool mControlYawRate;
  bool prev_mControlYawRate;
  bool mBumpSpeed;
  bool prev_mBumpSpeed;
  double mSectorTime;
  double mOldTimer;
  bool mTenthTimer;
  int mShiftTimer;
  int mGear;
  bool mStuck;
  int mStuckcount;
  bool mStateChange;
  bool mPathChange;
  bool mOvertake;
  bool prev_mOvertake;
  int mOvertakeTimer;
  bool mLetPass;
  bool prev_mLetPass;
  bool mLeavePit;
  double mFriction;
  double mCentrifugal;
  double mBrakeFriction;
  double mBrakeforce;
  double mBorderdist;
  bool mOnLeftSide;
  int mTrackType;
  double mTrackRadius;
  bool mOnCurveInside;
  double mAngleToTrack;
  bool mAngleToLeft;
  bool mPointingToWall;
  double mWallToMiddleAbs;
  double mWalldist;
  int mLastDamage;
  int mDamageDiff;
  int mPrevRacePos;
  int mRacePosChange;
  double mAccel;
  double mAccelAvg;
  double mAccelAvgSum;
  int mAccelAvgCount;
  double mMaxspeed;
  int mSector;
  int prev_mSector;
  double mSectSpeedfactor;
  bool mCurveAhead;
  bool prev_mCurveAhead;
  double mCurveAheadFromStart;
  bool mDrivingFast;
  bool prev_mDrivingFast;
  int mDrivingFastCount;
  bool mCatchingOpp;
  bool mLearnSectTime;
  bool mGetLapTime;
  double mLastLapTime;
  double mBestLapTime;
  bool mLearnLap;
  bool mAllSectorsFaster;
  bool mLearnSingleSector;
  int mLearnSector;
  bool mLearnedAll;
  bool mOfftrackInSector;
  bool mFinalLearnLap;
  double mFromStart;
  double mToMiddle;
  double mTargetFromstart;
  double mTargetToMiddle;
  double mNormalTargetToMiddle;
  double mPrevTargetdiff;
  double mTargetAngle;
  bool mMaxSteerAngle;
  bool prev_mMaxSteerAngle;
  Vec2d mGlobalCarPos;
  Vec2d mGlobalTarget;
  bool mCatchedRaceLine;
  bool prev_mCatchedRaceLine;
  double mCatchedRaceLineTime;
  double mAbsFactor;
  double mTclFactor;
  double mFrontCollFactor;
  bool mColl;
  bool mWait;
  double mFuelStart;
  double mPathOffs;
  double mAccelX;
  double mAccelXSum;
  int mAccelXCount;
  double mSkillGlobal;
  double mSkillDriver;
  int mWatchdogCount;
  
  double mFRONTCOLL_MARGIN;
};

#pragma pack(push, 1)
struct udpCmdType{
    float steeringCmd;
	float gasCmd;
	float brakeCmd;
	__int32 gearCmd;
	unsigned __int8 CtrlMode;
	unsigned __int8 ctrlCounter;
};
#pragma pack(pop)

#endif // _DRIVER_H_
