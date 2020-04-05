#pragma once

typedef int (CALLBACK* INIT_IO_DLL)(long lReserved);

typedef int (CALLBACK* FREE_IO_DLL)(long lReserved);

typedef int (CALLBACK* REG_IO_CARD)(long lCardNo,
									long lReserved);

typedef int (CALLBACK* FREE_IO_CARD)(long lCardNo,
									 long lReserved);

typedef int (CALLBACK* CONFIG_IO_PORT)(long lCardNo,
								       long lPortID,
								       long lIOMode,
								       long lReserved);

typedef int (CALLBACK* CONFIG_IO_PORT_LIGHT)(long lCardNo,
								       CString lPortID,
								       long lIOMode,
								       long lReserved);



typedef int (CALLBACK* CONFIG_COM_PORT)(long lCardNo,
										long lPortID,
										long lBaudRate, 
										long lReserved);

typedef int (CALLBACK* FREE_IO_PORT)(long lCardNo,
								     long lPortID,
								     long lReserved);

typedef int (CALLBACK* OUTPORT_WRITE)(long lCardNo,
							          long lPortID,
								      long lData,
								      long lReserved);

typedef int (CALLBACK* OUTPORT_WRITE_BIT)(long lCardNo,
							          long lPortID,
								      long lBitPos,
								      long lData);

typedef int (CALLBACK* OUTPORT_WRITE_STRING)(long lCardNo,
							          long lPortID,
								      CString* pstrData,
								      long lReserved);

typedef int (CALLBACK* OUTPORT_READ)(long lCardNo,
							         long lPortID,
								     long *plData,
								     long lReserved);

typedef int (CALLBACK* INPORT_READ)(long lCardNo,
							        long lPortID,
								    long *plData,
								    long lReserved);

typedef int (CALLBACK* INPORT_READ_STRING)(long lCardNo,
							        long lPortID,
								    CString* strMessage,
								    long lReserved);

typedef int (CALLBACK* INPORT_ABORT_READ)(long lCardNo,
							        long lPortID,
								    long lReserved);

typedef int (CALLBACK* SET_DI_INTERRUPT)(long lCardNo,
							          long lPortID,
								      long lIntMask,
								      long lReserved);

typedef int (CALLBACK* WAIT_FOR_ACTIVE_DI_INTERRUPT_IO)(long lCardNo,
							          long lPortID,
								      long lData,
								      long lBitPos);

typedef int (CALLBACK* CLEAR_ACTIVE_DI_INTERRUPT)(long lBitPos);

// Interface Callback Functions
// Declare the Callback functions definitions(characteristics)
typedef int (CALLBACK* INIT_IO_INT_DLL)(long lReserved);

typedef int (CALLBACK* FREE_IO_INT_DLL)(long lReserved);

typedef int (CALLBACK* REG_IO_INT_CARD)(long lCardNo,
									long lReserved);

typedef int (CALLBACK* FREE_IO_INT_CARD)(long lCardNo,
									 long lReserved);

typedef int (CALLBACK* CONFIG_IO_INT_PORT)(long lCardNo,
								       long lPortID,
								       long lIOMode,
								       long lReserved);

typedef int (CALLBACK* FREE_IO_INT_PORT)(long lCardNo,
								     long lPortID,
								     long lReserved);

typedef int (CALLBACK* INT_OUTPORT_WRITE)(long lCardNo,
							          long lPortID,
								      long lData,
								      long lReserved);

typedef int (CALLBACK* INT_OUTPORT_WRITE_BIT)(long lCardNo,
							          long lPortID,
								      long lBitPos,
								      long lData);

typedef int (CALLBACK* INT_OUTPORT_READ)(long lCardNo,
							         long lPortID,
								     long *plData,
								     long lReserved);

typedef int (CALLBACK* INT_INPORT_READ)(long lCardNo,
							        long lPortID,
								    long *plData,
								    long lReserved);



class CIOInterface
{
public :
	// Variables
	HINSTANCE			hDLL;				// Frame Grabber DLL Instance

	INIT_IO_DLL			lpInitIODLL;			// DLL InitDLL() Function
	FREE_IO_DLL			lpFreeIODLL;			// DLL FreeDLL() Function

	REG_IO_CARD			lpRegisterCard;		// DLL Register IO Card Function
	FREE_IO_CARD		lpFreeCard;		// DLL Free IO Card Function

	CONFIG_IO_PORT		lpConfigIOPort;		// DLL Configure IO Port Function
    CONFIG_IO_PORT_LIGHT		lpConfigIOPortLight;
	CONFIG_COM_PORT		lpConfigCOMPort;		// DLL Configure IO Port Function
	FREE_IO_PORT		lpFreeIOPort;		// DLL Configure IO Port Function
	
	OUTPORT_WRITE		lpOutPortWrite;		// DLL Write to Output Port Function
	OUTPORT_WRITE_BIT	lpOutPortWriteBit;	// DLL Write to Output Port Function
	OUTPORT_WRITE_STRING lpOutPortWriteString;	// DLL Write String to Output Port Function
	OUTPORT_READ		lpOutPortRead;		// DLL Read the Output Port Function(check on what is written)
	
	INPORT_READ			lpInPortRead;		// DLL Read the Input Port Function
	INPORT_READ_STRING	lpInPortReadString;	// DLL Read String from the Input Port Function
	INPORT_ABORT_READ	lpInPortAbortRead;	// DLL Abort Read from the Input Port Function
	
	SET_DI_INTERRUPT    lpSetDIInterrupt;
	WAIT_FOR_ACTIVE_DI_INTERRUPT_IO    lpWaitForActiveDIInterrupt;

	CLEAR_ACTIVE_DI_INTERRUPT lpClearActiveDIInterrupt;


	CIOInterface();



	int LoadIODll(CString strIOCard);

	int RegisterCard(long lCardNo, long lReserved);

	int FreeCard(long lCardNo, long lReserved);

	int ConfigIOPort(long lCardNo, long lPortID, long lIOMode, long lReserved);

    int ConfigIOPortLight(long lCardNo, CString lPortID, long lIOMode, long lReserved);

	int ConfigCOMPort(long lCardNo, long lPortID, long lBaudRate, long lReserved);

	int FreeIOPort(long lCardNo, long lPortID, long lReserved);

	int OutPortWrite(long lCardNo, long lPortID, long lData, long lReserved);

	int OutPortWriteBit(long lCardNo, long lPortID, long lData, long lReserved);

	int OutPortWriteString(long lCardNo, long lPortID, CString* pstrData, long lReserved);

	int OutPortRead(long lCardNo, long lPortID, long *plData, long lReserved);

	int InPortRead(long lCardNo, long lPortID, long *plData, long lReserved);
	int InPortReadString(long lCardNo, long lPortID, CString *strMessage, long lReserved);
	int SetDIInterrupt(long lCardNo, long lPortID, long lIntMask, long lReserved);

	int WaitForActiveDIInterrupt(long lCardNo, long lPortID, long lData, long lBitPos);

	int ClearActiveDIInterrupt(long lBitPos);

	long GetIOPortID(CString strPortName);
};