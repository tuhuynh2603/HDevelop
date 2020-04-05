#pragma once

#include "dask.h"

extern "C" int _stdcall InitIODLL(long lReserved);

extern "C" int _stdcall FreeIODLL(long lReserved);

extern "C" int _stdcall RegisterCard(long lCardNo,
								  long lReserved);

extern "C" int _stdcall FreeCard(long lCardNo,
								   long lReserved);

extern "C" int _stdcall ConfigIOPort(long lCardNo,
								     long lPortID,
								     long lIOMode,
								     long lReserved);

extern "C" int _stdcall FreeIOPort(long lCardNo,
								   long lPortID,
								   long lReserved);



extern "C" int _stdcall OutPortWrite(long lCardNo,
							         long lPortID,
								     long lData,
								     long lReserved);
extern "C" int _stdcall OutPortWriteBit(long lCardNo, 
									long lPortID, 
									long lBitPos, 
									long lData);

extern "C" int _stdcall InPortRead(long lCardNo,
							       long lPortID,
								   long *plData,
								   long lReserved);

extern "C" int _stdcall SetDIInterrupt(long lCardNo,
							         long lPortID,
								     long lIntMask,
								     long lReserved);

extern "C" int _stdcall WaitForActiveDIInterrupt(long lCardNo,
							         long lPortID,
								     long lData,
								     long lBitPos);

extern "C" int _stdcall ClearActiveDIInterrupt(long lBitPos);
