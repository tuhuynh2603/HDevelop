/// GardaLight: GardaSoft Light Control interface
/// Udupa; May 2012;

#pragma once

extern "C" int _stdcall InitIODLL(long lReserved);

extern "C" int _stdcall FreeIODLL(long lReserved);

extern "C" int _stdcall RegIOCard(long lCardNo,
								  long lReserved);

extern "C" int _stdcall FreeIOCard(long lCardNo,
								   long lReserved);

extern "C" int _stdcall ConfigIOPortLight(long lCardNo,
								     CString lPortID,
								     long lIOMode,
								     long lReserved);

extern "C" int _stdcall FreeIOPort(long lCardNo,
								   long lPortID,
								   long lReserved);

extern "C" int _stdcall OutPortWriteString(long lCardNo, 
											long lPortID, 
											CString* strData, 
											long lReserved);
