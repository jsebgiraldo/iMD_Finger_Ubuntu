#ifndef FPSTATE_H
#define FPSTATE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void  (*HOTPLUGCALLBACK)(unsigned char,unsigned char,unsigned short int,unsigned short int);

int fpstate_start(HOTPLUGCALLBACK lpCallBack);
void fpstate_stop();

#ifdef __cplusplus
}
#endif

#endif // FPSTATE_H
