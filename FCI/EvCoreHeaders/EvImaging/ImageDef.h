#pragma once



#define MVS_LIVE_BUG	0
#define MVS_DEAD_BUG	1


#define TOP		0
#define BOTTOM	1
#define LEFT	2
#define RIGHT	3

#define MM		1
#define MIL		2
#define MICRON	3
#define PIXEL	4

#define LCC		1
#define MLF		2

#define MAX_PROJ					1024
//#define MAX_BLOB					256
#define MAX_BLOB					2000
#define NORMAL_BLOB_SIZE			400
#define MAX_GRD_EDGE				50
#define MAX_EDGE					400
#define MAX_LINE_COUNT				400
#define MAX_POINT					400
#define MAX_ARROW_SAVED				10
#define MAX_MEM_ALLOC_SIZE			9000000.0

#define HOUGHSPACE_THETA_DIMENSION	400
#define HOUGHSPACE_RHO_DIMENSION    1024	
#define MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM	2000   


#ifdef ROW
#undef ROW
#endif

#define ROW							1
#define COLUMN						2

#define POS							0
#define NEG							1
#define ANY							2

#define FORWARD						0
#define BACKWARD					1

#define WHITE						0
#define BLACK						1
#define COLOR_ANY					2

#define THRESHOLD					1
#define LEVEL						2

#define MAXIMUM						1
#define AVERAGE						0
#define MINIMUM						-1


#define BINOMIAL_SMOOTHING			1
#define NO_SMOOTHING				2
#define GAUSSIAN_SMOOTHING			3
#define AVERAGE_SMOOTHING			4
#define SMOOTHING_MASK				5

#define DEBUG_OFF					0x00000000	// No Debugging 
#define DEBUG_DISP_IMAGE			0x00000001	// Dsiplay Image
#define DEBUG_DRAW					0x00000002	// Draw Data using line and rect
#define DEBUG_PRINT					0x00000004	// Print Data
#define DEBUG_PRINT_EXT				0x00000008	// Print Extra Data 
#define DEBUG_EDGE					0x00000010	// Debug Edge Detection
#define DEBUG_BLOB					0x00000020	// Debug Blob Detection
#define DEBUG_HIST					0x00000040	// Debug Histogram
#define DEBUG_TIME					0x00000080	// Debug Timing
#define DEBUG_TIME_EXT				0x00000100	// Debug Timing Extension
#define DEBUG_STEP_MODE				0x00000200	// Step by Step Mode
#define DEBUG_CHECKBLOB				0x00000400	
#define DEBUG_CHECKAREA				0x00000800	
#define DEBUG_STOP_ON_GOOD			0x00001000	
#define DEBUG_STOP_ON_BAD			0x00002000	
#define DEBUG_SAVE_BAD_IMAGE        0x00004000
#define DEBUG_SAVE_GOOD_IMAGE		0x00008000
#define DEBUG_ALL_DEFECTS			0x00010000
#define DEBUG_SAVE_PASS_IMAGE       0x00008000
#define DEBUG_DATA_MODE				0x00020000	// Data Step Mode
