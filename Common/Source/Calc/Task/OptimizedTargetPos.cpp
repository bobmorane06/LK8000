/*
   LK8000 Tactical Flight Computer -  WWW.LK8000.IT
   Released under GNU/GPL License v.2
   See CREDITS.TXT file for authors and copyrights

   $Id$
*/

#include "externs.h"
#include "AATDistance.h"
#include "Waypointparser.h"
#include "NavFunctions.h"
#include "PGTask/PGTaskMgr.h"

PGTaskMgr gPGTask; // This Is Shared ressource, never use without Locking Task Data ( LockTaskData()/UnlockTaskData() )!

void CalculateOptimizedTargetPos(NMEA_INFO *Basic, DERIVED_INFO *Calculated) {

	if (!DoOptimizeRoute())
		return;

	LockTaskData();

	gPGTask.Optimize(Basic, Calculated);

	for(size_t i=0; i<gPGTask.Count(); ++i) {
        gPGTask.UpdateTaskPoint(i, Task[i]);
	}

	int stdwp=Task[ActiveTaskPoint].Index;

	WayPointList[RESWP_OPTIMIZED].Latitude = Task[ActiveTaskPoint].AATTargetLat;
	WayPointList[RESWP_OPTIMIZED].Longitude = Task[ActiveTaskPoint].AATTargetLon;
	WayPointList[RESWP_OPTIMIZED].Altitude = Task[ActiveTaskPoint].AATTargetAltitude;

	_sntprintf(WayPointList[RESWP_OPTIMIZED].Name, NAME_SIZE, _T("!%s"),WayPointList[stdwp].Name);
    WayPointList[RESWP_OPTIMIZED].Name[NAME_SIZE] = _T('\0');

	UnlockTaskData();
}

// Clear PG
void ClearOptimizedTargetPos() {

	if (!DoOptimizeRoute())
		return;

	LockTaskData();

	WayPointList[RESWP_OPTIMIZED].Latitude=RESWP_INVALIDNUMBER;
	WayPointList[RESWP_OPTIMIZED].Longitude=RESWP_INVALIDNUMBER;
	WayPointList[RESWP_OPTIMIZED].Altitude=RESWP_INVALIDNUMBER;
	// name will be assigned by function dynamically
	_tcscpy(WayPointList[RESWP_OPTIMIZED].Name, _T("OPTIMIZED") );

	for(int i = 0; ValidWayPoint(Task[i].Index); ++i) {
		Task[i].AATTargetLat = WayPointList[Task[i].Index].Latitude;
		Task[i].AATTargetLon = WayPointList[Task[i].Index].Longitude;
        Task[i].AATTargetAltitude = WayPointList[Task[i].Index].Altitude;
		Task[i].AATTargetLocked = false;
	}

	gPGTask.Initialize();

	UnlockTaskData();
}
