#ifndef _COMMON_H_
#define _COMMON_H_


#include <vector>
using namespace std;

// common definitions, structures, ...
//#define MS_TEST
#define PSO_TEST


// define maximum camera resolution: 720P
#define MAX_CAM_WIDTH			1280
#define MAX_CAM_HEIGHT			720

// actual camera resolution
#define CAM_WIDTH				1280
#define CAM_HEIGHT				720


///////////////////////////////////////////////////////////////////////////////////////////////////
// Html dialog size
///////////////////////////////////////////////////////////////////////////////////////////////////
#define HTML_DLG_TOP_DIV_HEIGHT			80
#define HTML_DLG_CENTER_DIV_HEIGHT		730
#define HTML_DLG_BOTTOM_DIV_HEIGHT		80

#define HTML_DLG_GAP					20
#define HTML_DLG_WIDTH					(MAX_CAM_WIDTH+HTML_DLG_GAP)
#define HTML_DLG_HEIGHT					(HTML_DLG_TOP_DIV_HEIGHT+HTML_DLG_CENTER_DIV_HEIGHT+HTML_DLG_BOTTOM_DIV_HEIGHT)
#define HTML_DLG_LEFT					0
#define HTML_DLG_TOP					0


///////////////////////////////////////////////////////////////////////////////////////////////////
// CAST dialog
///////////////////////////////////////////////////////////////////////////////////////////////////
#define CAST_DLG_X_GAP				16			
#define CAST_DLG_Y_GAP				38	
#define CAST_DLG_LEFT				0
#define CAST_DLG_TOP				0
#define CAST_DLG_WIDTH				(HTML_DLG_WIDTH+CAST_DLG_X_GAP)
#define CAST_DLG_HEIGHT				(HTML_DLG_HEIGHT+CAST_DLG_Y_GAP)


///////////////////////////////////////////////////////////////////////////////////////////////////
// Opengl dialog
///////////////////////////////////////////////////////////////////////////////////////////////////
#define OPENGL_DLG_X_GAP			20
#define OPENGL_DLG_Y_GAP			30
#define OPENGL_DLG_LEFT			8
#define OPENGL_DLG_TOP			(HTML_DLG_TOP_DIV_HEIGHT+OPENGL_DLG_Y_GAP)
#define OPENGL_DLG_WIDTH		(MAX_CAM_WIDTH+OPENGL_DLG_X_GAP)
#define OPENGL_DLG_HEIGHT		(HTML_DLG_CENTER_DIV_HEIGHT)

///////////////////////////////////////////////////////////////////////////////////////////////////
// OPENGL CANVAS in OpenGl Dialog
///////////////////////////////////////////////////////////////////////////////////////////////////
#define OPENGL_CANVAS_LEFT		( (OPENGL_DLG_WIDTH-MAX_CAM_WIDTH)/2 )
#define OPENGL_CANVAS_TOP		( (OPENGL_DLG_HEIGHT-MAX_CAM_HEIGHT)/2 )
#define OPENGL_CANVAS_WIDTH		MAX_CAM_WIDTH
#define OPENGL_CANVAS_HEIGHT	MAX_CAM_HEIGHT










//////////////////////////////////////////////////////////////////////////////////////////////////
// Hardware related
///////////////////////////////////////////////////////////////////////////////////////////////////
#define NUM_OF_FIXTURES			2
#define NUM_OF_ENCODERS			4

//////////////////////////////////////////////////////////////////////////////////////////////////
// Position related
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	double x;
	double y;
} XY;

typedef struct
{
	int x;
	int y;
} XY_INT;

///////////////////////////////////////////////////////////////////////////////////////////////////
// for fixture
///////////////////////////////////////////////////////////////////////////////////////////////////
enum FIXTURE 
{
	NONE=-1, LEFT, RIGHT
};

enum AXIS
{
	YAW=0, INS, PIT, ROL
};







///////////////////////////////////////////////////////////////////////////////////////////////////
// RETURN type
///////////////////////////////////////////////////////////////////////////////////////////////////
#define _OK_	0
#define _FAIL_ -1


///////////////////////////////////////////////////////////////////////////////////////////////////
// State Maching states
///////////////////////////////////////////////////////////////////////////////////////////////////
#define STATE_NULL							0x00000000
// L0 layer
#define L0_BOOTING							0x10000000
#define L0_PLATFORM							0x20000000
#define L0_READY							0x30000000

// L1 layer [READY]-[ ]
#define L1_PRACTICE							0x01000000
#define L1_LAB								0x02000000
#define L1_SETTINGS							0x03000000

// L2 layer - [READY]-[LAB]-[ ]
#define L2_CTRLLAB							0x00100000
#define L2_CAMLAB							0x00200000

// L2 layer - [READY]-[SETTINGS]-[ ]
#define L2_FIXCALIB							0x00100000
#define L2_CAMCALIB							0x00200000



// Root
#define STATE_ROOT							0x00000000

// [Booting]
// upper state
#define BOOT_INIT							0x00000100
#define BOOT_SERVERCHECK					0x00000200
#define BOOT_SERVERCHECK_RTHREAD			0x00000010
#define BOOT_SERVERCHECK_WAIT				0x00000020
#define BOOT_SERVERCHECK_DONE				0x00000030
#define BOOT_USB4SCAN						0x00000300
#define BOOT_USB4SCAN_RTHREAD				0x00000010
#define BOOT_USB4SCAN_WAIT					0x00000020
#define BOOT_USB4SCAN_DONE					0x00000030
#define BOOT_USB4INIT						0x00000400
#define BOOT_USB4INIT_RTHREAD				0x00000010
#define BOOT_USB4INIT_WAIT					0x00000020
#define BOOT_USB4INIT_DONE					0x00000030
#define BOOT_CAMINIT						0x00000500
#define BOOT_CAMINIT_RTHREAD				0x00000010
#define BOOT_CAMINIT_WAIT					0x00000020
#define BOOT_CAMINIT_DONE					0x00000030
#define BOOT_DONE							0x00000600
#define BOOT_DONE_RTHREAD					0x00000010
#define BOOT_DONE_WAIT						0x00000020
#define BOOT_DONE_DONE						0x00000030

// [Platform]
// upper state
#define PLATFORM_INIT						0x00000100
#define PLATFORM_WAIT						0x00000200
#define PLATFORM_DONE						0x00000300

// [Ready]
// upper state
#define READY_INIT							0x00000100
#define READY_WAIT							0x00000200
#define READY_DONE							0x00000300

// [Ready] - [Practice]
#define PRACTICE_STEP1						0x00000100
#define PRACTICE_STEP1_INIT					0x00000010
#define PRACTICE_STEP1_WAIT					0x00000020
#define PRACTICE_STEP1_DONE					0x00000030

#define PRACTICE_STEP2						0x00000200
#define PRACTICE_STEP2_INIT					0x00000010
#define PRACTICE_STEP2_WAIT					0x00000020
#define PRACTICE_STEP2_DONE					0x00000030

#define PRACTICE_STEP3						0x00000300
#define PRACTICE_STEP3_INIT					0x00000010
#define PRACTICE_STEP3_WAIT1				0x00000020
#define PRACTICE_STEP3_RTIM					0x00000030
#define PRACTICE_STEP3_WAIT2				0x00000040
#define PRACTICE_STEP3_DONE					0x00000050

#define PRACTICE_STEP4						0x00000400
#define PRACTICE_STEP4_INIT					0x00000010
#define PRACTICE_STEP4_WAIT1				0x00000020
#define PRACTICE_STEP4_WAIT2				0x00000030
#define PRACTICE_STEP4_DONE					0x00000040

#define PRACTICE_STEP5						0x00000500
#define PRACTICE_STEP5_INIT					0x00000010
#define PRACTICE_STEP5_WAIT					0x00000020
#define PRACTICE_STEP5_DONE					0x00000030

#define PRACTICE_DONE						0x00000300
#define PRACTICE_STOP						0x00000F00

// [Reday] - [Lab]
#define LAB_INIT							0x00000100
#define LAB_WAIT							0x00000200
#define LAB_DONE							0x00000300

// [Ready] - [Lab] - [Control]
#define CTRLLAB_INIT						0x00000100
#define CTRLLAB_WAIT						0x00000200
#define CTRLLAB_DONE						0x00000300

// [Ready] - [Lab] - [Camera]
#define CAMLAB_INIT							0x00000100

#define CAMLAB_MAIN							0x00000200
#define CAMLAB_MAIN_INIT					0x00000010
#define CAMLAB_MAIN_RTIM					0x00000020
#define CAMLAB_MAIN_WAIT					0x00000030
#define CAMLAB_MAIN_DONE					0x00000040

#define CAMLAB_DONE							0x00000300
#define CAMLAB_STOP							0x00000F00

// [Ready] - [Settings]
#define SETTINGS_INIT						0x00000100
#define SETTINGS_WAIT						0x00000200
#define SETTINGS_DONE						0x00000300

// [Ready]-[Settings]-[Fixture Calibration]
#define FIXCALIB_STEP1						0x00000100
#define FIXCALIB_STEP1_INIT					0x00000010
#define FIXCALIB_STEP1_WAIT					0x00000020
#define FIXCALIB_STEP1_DONE					0x00000030

#define FIXCALIB_STEP2						0x00000200
#define FIXCALIB_STEP2_INIT					0x00000010
#define FIXCALIB_STEP2_WAIT					0x00000020
#define FIXCALIB_STEP2_DONE					0x00000030

#define FIXCALIB_STEP3						0x00000300
#define FIXCALIB_STEP3_INIT					0x00000010
#define FIXCALIB_STEP3_WAIT1				0x00000020
#define FIXCALIB_STEP3_RTIM					0x00000030
#define FIXCALIB_STEP3_WAIT2				0x00000040
#define FIXCALIB_STEP3_DONE					0x00000050

#define FIXCALIB_STEP4						0x00000400
#define FIXCALIB_STEP4_INIT					0x00000010
#define FIXCALIB_STEP4_WAIT1				0x00000020
#define FIXCALIB_STEP4_RTIM					0x00000030
#define FIXCALIB_STEP4_WAIT2				0x00000040
#define FIXCALIB_STEP4_DONE					0x00000050

#define FIXCALIB_STEP5						0x00000500
#define FIXCALIB_STEP5_INIT					0x00000010
#define FIXCALIB_STEP5_WAIT1				0x00000020
#define FIXCALIB_STEP5_RTHREAD				0x00000030
#define FIXCALIB_STEP5_WAIT2				0x00000040
#define FIXCALIB_STEP5_DONE					0x00000050

#define FIXCALIB_STEP6						0x00000600
#define FIXCALIB_STEP6_INIT					0x00000010
#define FIXCALIB_STEP6_WAIT					0x00000020
#define FIXCALIB_STEP6_DONE					0x00000030

#define FIXCALIB_DONE						0x00000700
#define FIXCALIB_STOP						0x00000F00

// [Ready] - [Settings] - [Camera Calibration]
#define CAMCALIB_INIT						0x00000100

#define CAMCALIB_MARK						0x00000200
#define CAMCALIB_MARK_INIT					0x00000010
#define CAMCALIB_MARK_WAIT1					0x00000020
#define CAMCALIB_MARK_RTIM					0x00000030
#define CAMCALIB_MARK_WAIT2					0x00000040
#define CAMCALIB_MARK_DONE					0x00000050

#define CAMCALIB_REVIEW						0x00000300
#define CAMCALIB_REVIEW_INIT				0x00000010
#define CAMCALIB_REVIEW_WAIT1				0x00000020
#define CAMCALIB_REVIEW_RTIM				0x00000030
#define CAMCALIB_REVIEW_WAIT2				0x00000040
#define CAMCALIB_REVIEW_DONE				0x00000050


#define CAMCALIB_RESULT						0x00000400
#define CAMCALIB_RESULT_INIT				0x00000010
#define CAMCALIB_RESULT_WAIT				0x00000020
#define CAMCALIB_RESULT_DONE				0x00000030

#define CAMCALIB_DONE						0x00000500
#define CAMCALIB_STOP						0x00000F00

#define STOP_WAIT							0x00000001
#define STOP_CONFIRM						0x00000002
#define STOP_RESUME							0x00000003





//////////////////////////////////////////////////////////////////////////////////////////////////
// commend sets
///////////////////////////////////////////////////////////////////////////////////////////////////
#define USR_CTRL_MSG						(WM_USER + 11)
#define CMD_NULL							0



// state transition related cmd
#define JMP_MASK							0xFFFFFF00
#define JMP_UNIT							0x00000100
#define CMD_JMP_FW							0x00000100
#define CMD_JMP_BW							0x00000200
#define CMD_JMP_BW2							0x00000300
#define JMP_STOP_MASK						0xFFFFF000
#define CMD_JMP_MV_STOP						0x00000F00

#define STEP_MASK							0xFFFFFFF0
#define STEP_UNIT							0x00000010
#define CMD_STEP_FW							0x00000010
#define CMD_STEP_BW							0x00000020


#define MSTEP_MASK							0xFFFFFFFF
#define MSTEP_UNIT							0x00000001
#define CMD_MSTEP_FW						0x00000001
#define CMD_MSTEP_BW						0x00000002
#define CMD_MSTEP_FW2						0x00000003
#define CMD_STOP_RESUME						0x00000004
#define CMD_STOP_CONFIRM					0x00000005

#define CMD_L0_MASK							0xF0000000 
#define CMD_L0_UNIT							0x10000000
#define CMD_L0_NEXT							0x10000000
#define CMD_L0_BACK							0x20000000

#define CMD_L1_MASK							0xFF000000
#define CMD_L1_TO_L0						0x0F000000
#define CMD_L2_MASK							0xFFF00000
#define CMD_L2_TO_L1						0x00F00000


#define CMD_L1_MV_PRACTICE					0x31000000
#define CMD_L1_MV_LAB						0x32000000
#define CMD_L1_MV_SETTINGS					0x33000000

#define CMD_L2_MV_CTRL_LAB					0x32100000
#define CMD_L2_MV_CAM_LAB					0x32200000

#define CMD_L2_MV_FIXCALIB					0x33100000
#define CMD_L2_MV_CAMCALIB					0x33200000

#define CMD_L2_MV_CAMCALIB_MARK				0x33200300
#define CMD_L2_MV_CAMCALIB_REVIEW			0x33200400
#define CMD_L2_MV_CAMCALIB_ALGO				0x33200500

// gui cmd
// booting
#define GUI_L0_BOOTING_UPDATEPROGRESSBAR	0x10000010
// platform
#define GUI_L0_PLATFORM_LOADPAGE			0x20000010
// ready
#define GUI_L0_READY_LOADPAGE				0x30000010


// [Ready] - [Practice]
#define GUI_L1_PRACTICE_STEP1_LOADPAGE		0x31000110
#define GUI_L1_PRACTICE_STEP2_LOADPAGE		0x31000210
#define GUI_L1_PRACTICE_STEP3_LOADPAGE		0x31000310
#define GUI_L1_PRACTICE_STEP3_UPDATE		0x31000320
#define GUI_L1_PRACTICE_STEP4_LOADPAGE		0x31000410
#define GUI_L1_PRACTICE_STEP4_UPDATE		0x31000420

// [Ready] - [Lab]
#define GUI_L1_LAB_LOADPAGE					0x32000010
// [Ready] - [Settings]
#define GUI_L1_SETTINGS_LOADPAGE			0x33000010




// [Ready]-[Lab]-[Control]
#define GUI_L2_CTRL_LAB_LOADPAGE			0x32100010

// [Ready]-[Lab]-[Camera]
#define GUI_L2_CAM_LAB_LOADPAGE				0x32200010
#define GUI_L2_CAM_LAB_UPDATE				0x32200020

// [Ready]-[Settings]-[Fixture Calibration]
#define GUI_L2_FIXCALIB_STEP1_LOADPAGE		0x33100110
#define GUI_L2_FIXCALIB_STEP2_LOADPAGE		0x33100210
#define GUI_L2_FIXCALIB_STEP3_LOADPAGE		0x33100220
#define GUI_L2_FIXCALIB_STEP3_UPDATEENC		0x33100310
#define GUI_L2_FIXCALIB_STEP4_LOADPAGE		0x33100410
#define GUI_L2_FIXCALIB_STEP4_UPDATEIMG		0x33100420
#define GUI_L2_FIXCALIB_STEP5_LOADPAGE		0x33100510
#define GUI_L2_FIXCALIB_STEP5_UPDATE		0x33100520
#define GUI_L2_FIXCALIB_STEP6_LOADPAGE		0x33100610
//#define CMD_L2_FIXCALIB_UPDATEALGOPROG		0x33100030
//#define CMD_L2_FIXCALIB_SHOWSUBDLG			0x33100040
//#define CMD_L2_FIXCALIB_HIDESUBDLG			0x33100050
//#define CMD_L2_FIXCALIB_RESETENCODER		0x33100060

// [Ready]-[Settings]-[Camera Calibration]
#define GUI_L2_CAMCALIB_MARK_LOADPAGE		0x33200310
#define GUI_L2_CAMCALIB_UPDATEIMGATMARK		0x33200320
#define GUI_L2_CAMCALIB_REVIEW_LOADPAGE		0x33200410
#define GUI_L2_CAMCALIB_UPDATEIMGATREVIEW	0x33200420
#define GUI_L2_CAMCALIB_RESULT_LOADPAGE		0x33200510

#define CMD_RESETENCODER					0xA0000000

#define CMD_GUI_TEST						0x00000001



//////////////////////////////////////////////////////////////////////////////////////////////////
// Platform
///////////////////////////////////////////////////////////////////////////////////////////////////
#define UA_PLATFORM							1
#define UNC_PLATFORM						2
#define UA_PLATFORM_PATH					".\\server\\platform\\ua\\"
#define UNC_PLATFORM_PATH					".\\server\\platform\\unc\\"

//////////////////////////////////////////////////////////////////////////////////////////////////
// Config folder path
//////////////////////////////////////////////////////////////////////////////////////////////////
#define CONFIG_FOLDER						"config\\"

//////////////////////////////////////////////////////////////////////////////////////////////////
// fixture configuration file
///////////////////////////////////////////////////////////////////////////////////////////////////
#define FIXTURE_CONFIG_FILE_NAME			"fixture_config.json"

//////////////////////////////////////////////////////////////////////////////////////////////////
// camera configuration file
///////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMERA_CONFIG_FILE_NAME				"camera_config.json"
#define CAMERA_MATRIX_LR					1
#define CAMERA_MATRIX_LLS					2

//////////////////////////////////////////////////////////////////////////////////////////////////
// for camera calibration
//////////////////////////////////////////////////////////////////////////////////////////////////
#define CAM_WIDTH_CALIB			640
#define CAM_HEIGHT_CALIB		360
#define IMG_WIDTH_CALIB			640
#define IMG_HEIGHT_CALIB		360
#define CAM_CALIB_FOLDER		"cam_calib\\"


///////////////////////////////////////////////////////////////////////////////////////////////////
// for MM Timer
///////////////////////////////////////////////////////////////////////////////////////////////////
#define MM_TIMER_INTERVAL	50
#define TARGET_RESOLUTION       1   // 1-millisecond target resolution

typedef struct
{
	LARGE_INTEGER freq;	//freq.
	LARGE_INTEGER t1;	// start count
	LARGE_INTEGER t2;	// end count
	double interval;	//
	double minInterval;	// for test
	double maxInterval;	// for test
	double totalTime;
	long totalTimeInt;
	double timer;
	int period;
} TIME_INTERVAL;





#endif