#ifndef FPIMAGE50_H
#define FPIMAGE50_H

#include "typedef.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct tagHistogram
{
	DWORD	dwHistogram[256];
	DWORD	dwHistogramPeak;
	WORD	HigramShrinkPixelNum;
	DWORD	HigramTotalPixelNum;
} Histogram;

typedef struct tagWindowingWOI
{
	BOOL	WinFunEnable; //ref: WOI_ENABLE
	BOOL    SWWinFunEnable; //Glass series sensor SF20x use software WOI function
	INT		img_width, img_height;
	INT		WOI_StartX, WOI_StartY;
	INT		WOI_LengthX, WOI_LengthY;
	INT     PixelXstep;
} WindowingWOI, * WindowingWOI_ptr;

typedef struct tagFPContrast
{
	BYTE	Contrast;
	FLOAT	fStd;
	FLOAT	fMean;
	BYTE	ContrastStart, ContrastEnd;

	WindowingWOI CstWOI;

	BYTE	wMinHist;
	DWORD	dwMinHistPixelNum;
	BYTE	wMaxHist;
	DWORD	dwMaxHistPixelNum;

	WORD	dwEffectHistPixelNum;
	BYTE	bCriteriaLow, bCriteriaHigh, ContrastCriteria;
	FLOAT	StdCriteria;

} FPContrast, * pFPContrast;

typedef union {
	struct {
		BYTE iG1 : 1;
		BYTE iG2 : 1;
		BYTE iG3 : 1;
		BYTE iG4 : 1;
	};
	BYTE all;
}iGCaliStatus;

typedef struct {
	int line;
	BOOL is_roll_init;
	BOOL is_roll_done;
	BOOL is_finger_on;	
	BYTE* roll_img;	
	BYTE* avgimg;
	BYTE* roll_done_img;
} ImageRollItem;

void CalculateHistogram(Histogram* myHistogram, LPBYTE lpImage, DWORD dwImageSize, UINT uWidth);
DWORD ContrastCalculation(FPContrast* myContrast, LPBYTE lpImage, LPDWORD a_lpdwHistogram, FPContrast* a_pstFPContrast);

void get_histogram_fap50(BYTE* img, int* black, int* white, int* middle, int hw_scan_mode);
BOOL get_line(BYTE* pImg, long finger_on_th, long* pLine, long* pLeft_x, long* pRight_x, long* pFinger_width);
int rolling_finger_fap50(BYTE* pImg, ImageRollItem* img_status);

/*
int WINAPI FAP50_RollImageInit();
int WINAPI FAP50_RollImageReset();
int WINAPI FAP50_RollImageProcess(BYTE* lpImage);
int WINAPI FAP50_RollImageFree();
int WINAPI FAP50_GetRollImage(BYTE* lpRollImage, int type);
int WINAPI FAP50_GetRollBitmap(BYTE* pBmp, UINT uType);
*/

int WINAPI FAP50_SeparateImage(BYTE* lpImage, BYTE* lpSI0, BYTE* lpSI1, BYTE* lpSI2, BYTE* lpSI3, int* num);


void WINAPI FAP50_SmoothImage(BYTE* lpOrgFinger, BYTE* lpTemp, INT IMGW, INT IMGH, INT r, INT d);
void WINAPI FAP50_ZoomOutImage(BYTE* lpOrgFinger, BYTE* lpTemp, INT IMGW, INT IMGH);
void WINAPI FAP50_GetOrientMap(BYTE* lpOrient, BYTE* lpDivide, BYTE* lpTemp, INT IMGW, INT IMGH, INT  r);
int WINAPI FAP50_DivideImage(BYTE* lpDivide, BYTE* lpTemp, INT IMGW, INT IMGH, INT  r, INT threshold);

void WINAPI FAP50_ImageDivide(BYTE* lpImage, BYTE* lpDivide);

void WINAPI FAP50_FingerCheck(BYTE* lpImage, INT checkwidth, INT* pl, INT* count);

void WINAPI Lightness(unsigned char* piImage, int wImageSize, int Amount);
void WINAPI Contrast(unsigned char* piImage, int wImageSize, int Amount);

#ifdef __cplusplus
}
#endif

#endif
