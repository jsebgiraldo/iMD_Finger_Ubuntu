#ifndef FPDEVICE50_H
#define FPDEVICE50_H

#include "typedef.h"

#ifdef __cplusplus
extern "C"
{
#endif

	#define IMAGE_WIDTH					1600
        #define IMAGE_HEIGHT                                    1000
	#define RAW_WIDTH					1600
	#define RAW_HEIGHT					1024
	#define IMAGE_SIZE					(IMAGE_WIDTH * IMAGE_HEIGHT)
	#define RAW_SIZE					(RAW_WIDTH * RAW_HEIGHT)
        #define BMP_SIZE                                        (IMAGE_WIDTH * IMAGE_HEIGHT+1078)
	#define ROLL_WIDTH					800
	#define ROLL_HEIGHT					800
	#define ROLL_SIZE					(IMAGE_SIZE/2)

        #define FAP50_IMAGE_WIDTH                               1600
        #define FAP50_IMAGE_HEIGHT                              1000
        #define FAP50_RAW_SIZE					(RAW_WIDTH * RAW_HEIGHT)
        #define FAP50_BMP_SIZE                                  (IMAGE_WIDTH * IMAGE_HEIGHT+1078)
        #define FAP30_RAW_SIZE                                  200000
        #define FAP30_BMP_SIZE                                  201078

	enum E_HW_SCAN_MODE
	{
		HW_SCAN_MODE_FLAT,
		HW_SCAN_MODE_ROLL,
		HW_SCAN_MODE_FAST,
	};

	BOOL WINAPI FAP50_OpenDevice();
	BOOL WINAPI FAP50_CloseDevice();
	BOOL WINAPI FAP50_IsOpen();

	int WINAPI FAP50_ResetDevice();
	int WINAPI FAP50_ResetSensor();
	int WINAPI FAP50_GetChipID(WORD* wChipID);
	int WINAPI FAP50_GetSensorRegister(WORD wRegAddr, BYTE* lpValue);
	int WINAPI FAP50_SetSensorRegister(WORD wRegAddr, BYTE byValue);
	int WINAPI FAP50_GetFingerState(unsigned char* pbState);

	int WINAPI FAP50_CaptureImage(int hw_scan_mode, unsigned char* rawdata, int rawsize);
	int WINAPI FAP50_CheckFingerOn(int hw_scan_mode, unsigned char* rawdata);
	int WINAPI FAP50_ImageToBitmap(BYTE* pImage, BYTE* pBmp, int width, int height, UINT uType);

    int WINAPI FAP50_RollImageInit();
    int WINAPI FAP50_RollImageReset();
    int WINAPI FAP50_RollImageProcess(BYTE* lpImage);
    int WINAPI FAP50_RollImageFree();
    int WINAPI FAP50_GetRollImage(BYTE* lpRollImage, int type);
    int WINAPI FAP50_GetRollBitmap(BYTE* pBmp, UINT uType);

    int WINAPI FAP50_SetImageMode(int img_kind);

#ifdef __cplusplus
}
#endif

#endif
