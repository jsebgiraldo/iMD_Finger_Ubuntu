#ifndef FPDEVICE_H
#define FPDEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRUE 1
#define FALSE 0

#define WINAPI
typedef int     BOOL;
typedef int     LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int UINT;
typedef unsigned char BYTE;

//////////////////////////////////////////////////////////////////////////////////////////
// Struct
// Device Infomation
typedef struct _FPDEVINFO {
    int iFPMax;
    int iSecLevel;
    UINT uAddr;
    int iPackSize;
    int iBaud;
    BYTE SN[8];
}FPDEVINFO, * LPFPDEVINFO;

//////////////////////////////////////////////////////////////////////////////////////////
// Funcation

//Open Device
int WINAPI OpenDevice(int port, int val, int style);
//Link Device
int WINAPI LinkDevice(UINT password);
//Close Device
int WINAPI CloseDevice();

//Get Fingerprint Width and Height
BOOL WINAPI GetImageSize(int* width, int* height);
UINT WINAPI GetImageWidth();
UINT WINAPI GetImageHeight();
//Is Support Store In Device
BOOL WINAPI IsSupportStore();
//Get Fingerprint Sensor Type	Return 1=FAP10,2=FAP20,3=FAP30
int WINAPI GetSensorType();
//Get Fingerprint Template Size
int WINAPI GetTemplateSize();
//Read Device Information
BOOL WINAPI ReadDeviceInfo(LPFPDEVINFO lpDevInfo);

//Fingerprint Image(RAW) to Bitmap Data(8Bit)
void WINAPI ImageToBitmap(BYTE* pImage, int width, int height, BYTE* pBmp, int uType);
//Fingerprint Image(RAW) to Bitmap Data(32Bit)
void WINAPI ImageToBitmap32(BYTE* pImage, int width, int height, BYTE* pBmp, int clr, int bk);

//Get Image from Device
BOOL WINAPI GetDeviceImage();
//Up Image to Host
BOOL WINAPI UpDeviceImage(unsigned char* pImageData, int* iImageLength);
//Generate Template in Device	idBuffer=1 or 2
BOOL WINAPI GenerateDeviceTemplate(int idBuffer);
//Up Template to Host
BOOL WINAPI UpDeviceTemplate(int iBufferID, unsigned char* pTemplet, int* iTempletLength);
// Combine Template in Template
BOOL WINAPI CombineDeviceTemplate();

//Get Template Count in Device
BOOL WINAPI GetDeviceTemplateCount(int* count);
//Get Template List in Device
BOOL WINAPI ReadListInDevice(int iMbMax, BYTE* list);

//Clear All Template In Device
BOOL WINAPI ClearInDevice();
//Delete Template In Device
BOOL WINAPI DeleteInDevice(int id);
//Save Buffer to Device Flash
BOOL WINAPI SaveInDevice(int iBufferID, int id);
//Serach in Device
BOOL WINAPI SearchInDevice(int iBufferID, int iNum, int* mbAddr, int* sc);
//Match Template in Device Buffer
BOOL WINAPI MatchInDevice(int iBufferID, int iBufferPage, int pageid, int* sc);

//Download Host to Device
BOOL WINAPI SaveToDevice(int id, BYTE* tpbuf, int size);
//Upload Host from Device
BOOL WINAPI LoadFromDevice(int id, BYTE* tpbuf, int* size);

//Get Device Random Num
BOOL WINAPI GetRandomNum(UINT* rand);
//Get Device Handle
//HANDLE WINAPI GetDeviceHandle();
//Get Device Address
UINT WINAPI GetDeviceAddress();

//Create Fingerprint Template from Fingerprint Image
int WINAPI CreateTemplate(BYTE* lpImage, int width, int height, BYTE* lpTemplate, int* iTemplateSize);
//Fingerprint Template Format(ISO to Private)
void WINAPI ISOtoPrivate(BYTE* pSrc, BYTE* pDst, int type);


#ifdef __cplusplus
}
#endif

#endif // FPENGINE_H
