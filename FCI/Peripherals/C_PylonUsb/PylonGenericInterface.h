//-----------------------------------------------------------------------------
//  (c) 2008 by Basler Vision Technologies
//  Section:  Basler Components
//  Project:  Pylon 2.0 Generic Interface
//-----------------------------------------------------------------------------
//
//		******************************************
//		* NON SUPPORTED PROGRAMMING SAMPLE ONLY! *
//		*        ABSOLUTELY NO WARRANTY!         *
//		******************************************
//
//-----------------------------------------------------------------------------
/**
  \file     PylonGenericInterface.h
  \brief    Interface of simple DLL wrapper for Pylon 2.0 generic interface
*/


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PYLONDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PYLONDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef PYLONDLL_EXPORTS
#define PYLONDLL_API __declspec(dllexport)
#else
#define PYLONDLL_API __declspec(dllimport)
#endif

#define INFINITE 0xFFFFFFFF // Infinite timeout

// Camera Handle
typedef struct 
{
	long dummy;
} *PYLONCAMHANDLE;

// Imagebuffer Handle
typedef struct 
{
	long dummy;
} *PYLONBUFFERHANDLE;

// ColorConverter Handle
typedef struct 
{
	long dummy;
} *PYLONCCHANDLE;

// ParamChange CallbackHandle
typedef struct 
{
	long dummy;
} *PYLONCALLBACKHANDLE;

typedef int BOOL;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// Pylon GenICam Interface Types
typedef enum 
{
	_UndefinedInterface=-1,	// Not Implemented
	IPylonNode,				// INode
	IPylonEnumeration,		// IEnumeration
	IPylonInteger,			// IInteger
	IPylonFloat,			// IFloat
	IPylonBoolean,			// IBoolean
	IPylonString,			// IString
	IPylonCommand			// ICommand
} EPylonInterface;

// Possible grab status values
typedef enum 
{
	InitGrabStatus=-100,
	_UndefinedGrabStatus=-1,
    Idle,       // currently not used
    Queued,     // in the input queue
    Grabbed,    // filled with data
    Canceled,   // request was canceled
    Failed,      // request failed
	TimeOut
} EPylonGrabStatus;

// access mode of a feature
typedef enum 
{
    NI,        // Not implemented
    NA,        // Not available
    WO,        // Write Only
    RO,        // Read Only
    RW,        // Read and Write
    _UndefinedAccesMode // Object is not yet initialized
} EPylonAccessMode; 

// Camera or StreamGrabber Feature
typedef enum 
{
    CAM_FEATURE,	// Camera Feature
    STREAM_FEATURE, // StreamGrabber Feature
	EVENT_FEATURE   // EventGrabber Feature
} EPylonFeatureType; 

// Recommended visibility of a node
typedef enum 
{
    Beginner = 0,				// Always visible
    Expert = 1,					// Visible for experts or Gurus
    Guru = 2,					// Visible for Gurus
    Invisible = 3,				// Not Visible
    _UndefinedVisibility  = 99	// Object is not yetinitialized
} EPylonVisibility;

// Structure for device information
typedef struct
{
	char FriendlyName[50];	  // e.g. "Basler scA1390-17gc (20610330)"
	char FullName[50];		  // e.g. "Basler scA1390-17gc#00305309501A#192.168.0.110:39ÌscA1390-17gc"
	char ModelName[50];		  // e.g. "scA1399-17gc"
	char SerialNumber[50];	  // SerialNumber, could be "N/A" for Basler 1394 Cameras
	char UserDefinedName[50]; // Name for Camera which can be defined by the user
	char VendorName[50];	  // e.g. "Basler"
} SCameraInfo;



#ifdef __cplusplus
extern "C" {
#endif

// Retrieves the error message of the most recent error.
// An empty string indicates no error
//
// Out Parameters:
// - char *ErrMessage -> Error message
//
PYLONDLL_API void _stdcall PylonGetLastErrorMsg (/*out*/ char *ErrMessage);

//-----------------------------------------------------------------------------
// Initializing / Uninitializing
//-----------------------------------------------------------------------------

// Must be called ONCE at beginning of any Pylon application
PYLONDLL_API void _stdcall PylonInitialize ();

// Must be called ONCE at end of any Pylon application
PYLONDLL_API void _stdcall PylonTerminate ();


//-----------------------------------------------------------------------------
// Information functions
//-----------------------------------------------------------------------------

// Yields number of all GenICam Cameras, attached to the PC
// (no matter if 1394 or GigE interface)
//
// Out Parameters:
// - long *NumberOfCameras  -> Numbers of Cameras found
//
// Return values:
// - TRUE if succeeding (even if NO camera was found)
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetNumCam (/*Out*/ long *NumberOfCameras);


// Yields the Basler Camera's serial number, Camera will NOT be opened
// IndexNumber must be 0...*NumberOfCameras-1
//
// In Parameters:
// - long Index -> Index of Camera for which SerialNumber shall be retrieved
//
// Out Parameters:
// - char *SerialNumber -> Camera's SerialNumber
//
// Return values:
// - TRUE if valid SerialNumber was found
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetSerialNumberByIndex (/*In*/ long Index, 
													    /*Out*/ char *SerialNumber);

// Yields Camera device information
// Camera must be opened before to get PYLONCAMHANDLE
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
//
// Out Parameters:
// - SCameraInfo *info -> Structure containing information about Camera
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetCameraInfo (/*In*/ PYLONCAMHANDLE hCamera, 
											   /*Out*/ SCameraInfo *info);


//-----------------------------------------------------------------------------
// Function related to camera opening
//-----------------------------------------------------------------------------

// Yields PYLONCAMHANDLE. Opens Camera and Streamgrabber
// Index must be 0...*NumberOfCameras-1
//
// In Parameters:
// - long Index -> Index of Camera to be opened
//
// Return values:
// - PYLONCAMHANDLE -> Handle of Camera which was successfully opened
// - NULL if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API PYLONCAMHANDLE _stdcall PylonCamOpen (/*In*/ long Index);


// Yields PYLONCAMHANDLE. Opens Camera and Streamgrabber by Camera's SerialNumber
//
// In Parameters:
// - const char *SerialNumber -> SerialNumber of Camera to be opened
//
// Return values:
// - PYLONCAMHANDLE -> Handle of Camera which was successfully opened
// - NULL if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API PYLONCAMHANDLE _stdcall PylonCamOpenBySerialNumber (/*In*/ const char *SerialNumber, int& cameraType);


//-----------------------------------------------------------------------------
// Function related to image grabbing
//-----------------------------------------------------------------------------

// Prepare Grabbing
// After this call, some Camera attributes cannot be modified anymore (e.g. setting AOI)
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonPrepareGrab (/*In*/ PYLONCAMHANDLE hCamera);


// Register Buffer at StreamGrabber
// Must be done before Buffer is queued using PylonQueueBuffer
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - unsigned char *ImageBuffer -> Buffer which must be allocated big enough to receive image
// - size_t BufferSize -> Size of Buffer
//
// Return values:
// - PYLONBUFFERHANDLE -> Handle to Buffer after successfully registering
// - NULL if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API PYLONBUFFERHANDLE _stdcall PylonRegisterBuffer (/*In*/ PYLONCAMHANDLE hCamera, unsigned char *ImageBuffer, size_t BufferSize);


// Queueing Buffer 
// Buffer must be registered before using PylonRegisterBuffer
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - PYLONBUFFERHANDLE hBuffer -> Handle to Buffer to be queued in
// - int BufferIndex -> Index of Buffer which is queued in (this must be a unique identifier of the buffer!)
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonQueueBuffer (/*In*/ PYLONCAMHANDLE hCamera, PYLONBUFFERHANDLE hBuffer, long BufferIndex);


// Yields ImageBuffer after acquiring image 
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - long timeout -> timeout for waiting for ImageBuffer (INFINITE = wait forever)
//
// Out Parameters:
// - int *BufferIndex -> Index of Buffer which was just grabbed
// - EPylonGrabStatus *status -> Structure containing information about grab result
// - char *ErrStr -> Contains Error Information if status == Failed
//
// Return values:
// - PYLONBUFFERHANDLE -> Handle to Buffer after successfully registering
// - NULL if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API PYLONBUFFERHANDLE _stdcall PylonGetBuffer (/*In*/ PYLONCAMHANDLE hCamera, long timeout,
														/*Out*/ long *BufferIndex, EPylonGrabStatus *status, char *ErrStr);


//-----------------------------------------------------------------------------
// Function related to camera closing
//-----------------------------------------------------------------------------

// Cancel grabbing
// All pending buffers are collected
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonCancelGrab (/*In*/ PYLONCAMHANDLE hCamera);


// Finish grabbing
// Once PylonPrepareGrab was called, you have to call this function
// if you want to modify some Camera attribute (e.g. setting the AOI)
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonFinishGrab (/*In*/ PYLONCAMHANDLE hCamera);


// Deregistrering of a PYLONBUFFERHANDLE
// Must be done before closing the camera!
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - PYLONBUFFERHANDLE hBuffer -> Handle of Buffer to be deregistered
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonDeregisterBuffer (/*In*/ PYLONCAMHANDLE hCamera, PYLONBUFFERHANDLE hBuffer);


// Closing of Camera and Streamgrabber
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera to be closed (will be obtained by PylonCamOpen)
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonCamClose (/*In*/ PYLONCAMHANDLE hCamera);


//-----------------------------------------------------------------------------
// Feature Setter
//-----------------------------------------------------------------------------

// Setting the Enum value of an Enumeration
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be set
// - const char *FeatureName -> Name of Enumeration to be set (e.g. "PixelFormat")
// - const char *EntryName -> Enumeration entry to be set (e.g."Mono8")
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonSetEnumFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, const char *EntryName);


// Setting the Enum value of an Enumeration by the Entry's Index
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be set
// - const char *FeatureName -> Name of Enumeration to be set (e.g. "PixelFormat")
// - long Index -> Index of Enumeration entry to be set
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonSetEnumFeatureByIndex (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, long Index);


// Setting an Integer Value
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be set
// - const char *FeatureName -> Name of Integer Feature to be set (e.g. "OffsetX")
// - long Val -> Value to be set
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonSetIntFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, long Val);


// Setting a Float Value
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be set
// - const char *FeatureName -> Name of Float Feature to be set (e.g. "ExposureTimeAbs")
// - double Val -> Value to be set
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonSetFloatFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, double Val);


// Setting a String Value
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be set
// - const char *FeatureName -> Name of Float Feature to be set (e.g. "DeviceUserID")
// - const char * -> Value to be set
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonSetStringFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, const char *Val);


// Setting a Boolean Value
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be set
// - const char *FeatureName -> Name of Float Feature to be set (e.g. "ChunkModeActive")
// - BOOL Val -> Value to be set
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonSetBoolFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, BOOL Val);


//-----------------------------------------------------------------------------
// Feature Getter
//-----------------------------------------------------------------------------

// Getting the Enum Entry of an Enumeration
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of Enumeration to get (e.g. "PixelFormat")
//
// Out Parameters:
// - char *EntryName -> Current Enumeration Entry (value) to get
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetEnumFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
											    /*Out*/ char *EntryName);

// Getting the Index of an Enum Entry of an Enumeration
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of Enumeration to get (e.g. "PixelFormat")
//
// Out Parameters:
// - long *Index -> Index of currently active EnumEntry value
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetEnumFeatureIndex (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
													 /*Out*/ long *Index);

// Getting an Integer value
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of Integer to get (e.g. "OffsetX")
//
// Out Parameters:
// - long *Val -> Current value of Integer feature to get
// - long *Min -> if  Min != NULL: Min value of Integer feature
// - long *Max -> if  Max != NULL: Max value of Integer feature
// - long *Inc -> if  Inc != NULL: Inc value of Integer feature
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetIntFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
											   /*Out*/ long *Val, long *Min, long *Max, long *Inc);

// Getting a Float value
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of Float to get (e.g. "ExposureTimeAbs")
//
// Out Parameters:
// - double *Val -> Current value of Float feature
// - double *Min -> if  Min != NULL: Min value of Float feature
// - double *Max -> if  Max != NULL: Max value of Float feature
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetFloatFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
												 /*Out*/ double *Val, double *Min, double *Max);


// Getting a String value
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of String to get (e.g. "DeviceID")
//
// Out Parameters:
// - char *Val -> Current value of String feature
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetStringFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
												  /*Out*/ char *Val);

// Getting a Boolean value
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of Boolean to get (e.g. "ChunkModeActive")
//
// Out Parameters:
// - BOOL *Val -> Current value of Boolean feature
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetBoolFeature (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
											    /*Out*/ BOOL *Val);


// Getting the number of entries of an Enumeration
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of Enumeration to get (e.g. "PixelFormat")
//
// Out Parameters:
// -  long *Num -> Number of Enumeration entries
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetNumEnumEntries (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
												   /*Out*/ long *Num);


// Getting the Enumeration entry of an Enumeration by Enumerationindex
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of Enumeration to get (e.g. "PixelFormat")
// - long Index -> Enumerationindex of entry to get 
//
// Out Parameters:
// - char *EntryName -> Enumeration Entry of specified Index
// - char *DisplayName -> Display NAme of Enumeration Entry of specified Index
// - EPylonAccessMode *Mode -> if Mode != NULL: AccessMode of Entry (e.g. RW)
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetEnumEntryByIndex (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, long Index, 
													 /*Out*/ char *EntryName, char *DisplayName, EPylonAccessMode *Mode);


//-----------------------------------------------------------------------------
// Feature Tester
//-----------------------------------------------------------------------------

// Checking the availability and AccessMode of a feature
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of feature to check
//
// Out Parameters:
// - EPylonAccessMode *Mode -> AccessMode of feature (e.g. RW)
//
// Return values:
// - TRUE if feature is available
// - FALSE if feature is NOT available or if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetFeatureAvailability (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
													    /*Out*/ EPylonAccessMode *mode);


// Checking the availability and AccessMode of an Enumeration entry
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber feature has to be got
// - const char *FeatureName -> Name of Enumeration to check (e.g. "PixelFormat")
// - const char *EntryName -> Enumeration entry to check (e.g."Mono8")
//
// Out Parameters:
// - EPylonAccessMode *Mode -> AccessMode of feature (e.g. RW)
//
// Return values:
// - TRUE if feature is available
// - FALSE if feature is NOT available or if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetEnumEntryAvailability (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, const char *EntryName,
														  /*Out*/ EPylonAccessMode *mode);


//-----------------------------------------------------------------------------
// Command execution
//-----------------------------------------------------------------------------

// Executing a command
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber command has to be executed
// - const char *Command -> Name of command to be executed (e.g. "AcquisitionStart")
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonExecCommand (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *Command);


// Checking the status of a command
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber command has to be executed
// - const char *Command -> Name of command to be checked (e.g. "AcquisitionStart")
//
// Out Parameters:
//  BOOL *bDone -> TRUE, if command has already been executed, otherwise FALSE
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetCommandStatus (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *Command, 
												  /*Out*/ BOOL *bDone);



// -------------------------------------------------------------
// Nodemap Functions
// -------------------------------------------------------------

// Getting Number of ChildNodes belonging to a ParentNode
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber command has to be executed
// - const char *NodeName -> Name of ParentNode
//
// Out Parameters:
//  long *NumChildNodes -> Number of ChildNodes
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetNumChildNodes (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *NodeName, 
												  /*Out*/ long *NumChildNodes);


// Retrieving a ChildRoot of specified ParentRoot by ParentRootIndex
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber command has to be executed
// - const char *ParentNodeName -> Name of ParentNode
// - long Index -> Index of ParentNode's ChildNodeList
//
// Out Parameters:
//  char *ChildNodeName -> Name Of Childnode
//  EPylonVisibility *Visibility -> Visibility of Childnode
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetChildNodeByIndex (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *ParentNodeName, long Index, 
													 /*Out*/ char *ChildNodeName, EPylonVisibility *Visibility);


// Retrieving Information of a Node 
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber command has to be executed
// - const char *NodeName -> Name of Node
//
// Out Parameters:
// - EPylonInterface *PylonInterface -> if PylonInterface!= NULL: GenICam Interface Type
// - EPylonAccessMode *AccessMode -> if AccessMode != NULL: AccessMode of Node (e.g. RW)
// - EPylonVisibility *Visibility -> if Visibility != NULL: Visibility of Node (e.g. Guru)
// - char *DisplayName -> Display Name of Node
// - char *ToolTip -> Tooltip of Node
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetNodeInfo (/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *NodeName, 
											 /*Out*/ EPylonInterface *PylonInterface, EPylonAccessMode *AccessMode, EPylonVisibility *Visibility, char *DisplayName, char *ToolTip);


// -------------------------------------------------------------
// Color Conversion
// -------------------------------------------------------------

// Creates and opens a ColorConverter to convert any camera pixel format into
// Mono8 (for monochrome camera input formats such as Mono16) or to RGBA (for 
// colour camera input formats such as BayerBG12)
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - const char *PixelFormat -> GenICam PixelFormat name
// - long Width -> Width in Pixel of Image
// - long Height -> Height in Pixel of Image
//
// Out Parameters:
//  BOOL *bIsColorFormat -> If TRUE, image will be converted to RGBA, otherwise to Mono8
//
// Return values:
// - PYLONCCHANDLE -> Handle to ColorConerter object
// - NULL if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API PYLONCCHANDLE _stdcall PylonOpenColorConverter (/*In*/ PYLONCAMHANDLE hCamera, const char *PixelFormat, long Width, long Height,
															 /*Out*/ BOOL *bIsColorFormat);


// Closes and destroyes the ColorConverter
//
// In Parameters:
// - PYLONCCHANDLE hColorConverter -> Handle of ColorConverter object (will be obtained by PylonOpenColorConverter)
//
// Return values:
// - TRUE (always)
//
PYLONDLL_API BOOL _stdcall PylonCloseColorConverter (/*In*/ PYLONCCHANDLE hColorConverter);


// Converts Input Imagebuffer (of any camera pixel format) into
// Mono8 (for monochrome camera input formats such as Mono16) or to RGBA (for 
// colour camera input formats such as BayerBG12)
//
// In Parameters:
// - PYLONCCHANDLE hColorConverter -> Handle of ColorConverter object (will be obtained by PylonOpenColorConverter)
// - unsigned char *InBuffer -> Buffer of input image
// - long InBufSize -> Size of Input Image
// - long OutBufSize -> Size of Output Image
//
// Out Parameters:
// - unsigned char *OutBuffer -> Buffer of input image (must be allocated before)
// 
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonConvertImageFormat (/*In*/ PYLONCCHANDLE hColorConverter, unsigned char *InBuffer, long InBufSize, long OutBufSize, 
													/*Out*/ unsigned char *OutBuffer);


// -------------------------------------------------------------
// Callbacks on Parameter Changes
// -------------------------------------------------------------

// Registers a function of the following Prototype as a Parameter Change Callback Function:
//
// void _stdcall myFunction (void *myGenICamNode) 
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber command has to be executed
// - const char *NodeName -> Name of Node (GenICam Feature) which Parameter Changes shall be observed
// - void *pFunc -> Pointer to Callback Function
//
// Return values:
// - PYLONCALLBACKHANDLE -> Handle to CallbackFunction
// - NULL if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API PYLONCALLBACKHANDLE _stdcall PylonRegisterParamCB (/*in*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *NodeName, void *pFunc);


// Unregisters the Parameter Change Callback Function:
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber command has to be executed
// - PYLONCALLBACKHANDLE hParamCB -> Handle to Callback Function
// - const char *NodeName -> Name of Node (GenICam Feature) which Parameter Changes shall be observed
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonUnRegisterParamCB (/*in*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, PYLONCALLBACKHANDLE hParamCB, const char *NodeName);


// Retrieves the current value of a GenICam Node (returned in Callback Function) as String
//
// In Parameters:
// - void *pInNode -> Pointer to GenICam Node in memory (returned in Callback Fuction)
//
// Out Parameters:
// - EPylonInterface *PylonInterface -> GenICam Interface Type
// - char *FeatureName -> Name of Node (GenICam Feature) which Value shall be retrieved
// - char *CurrVal -> Node's value as String
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetFeatureByNode (/*in*/ void *pInNode, 
												  /*out*/ EPylonInterface *PylonInterface, char *FeatureName,  char *CurrVal);


// Retrieves the current memory Address of a GenICam Node
//
// In Parameters:
// - PYLONCAMHANDLE hCamera -> Handle of Camera (will be obtained by PylonCamOpen)
// - EPylonFeatureType ftype -> Determine, if a Camera, StreamGrabber or EventGrabber command has to be executed
// - const char *NodeName -> Name of Node (GenICam Feature) which Address shall be retrieved
//
// Out Parameters:
// long *Address -> Physical Address of Node
//
// Return values:
// - TRUE if succeeding
// - FALSE if any error occured (retrieve with PylonGetLastErrorMsg)
//
PYLONDLL_API BOOL _stdcall PylonGetNodeAddress (PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *NodeName, long *Address);


// Converts a void Pointer returned in Callback Function to the Address of GenApi::INode Object
//
// In Parameter:
// - void *pInNode -> Node returend in Callback Function
//
// Out Parameters:
// long *Address -> Physical Address of GenApi::INode Object
//
// Return values:
// - Always TRUE
//
PYLONDLL_API BOOL _stdcall PylonGetNodeByNode (void *pInNode, long *Address);

#ifdef __cplusplus
} // extern "C"
#endif