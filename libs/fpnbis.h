#ifndef FPNBIS_H
#define FPNBIS_H


int computeRawNfiq(int *nfiq,float *conf,unsigned char * idata,const int iw, const int ih, const int id, const int ippi, int * flags);
int encodeRawWsq(unsigned char * image,int width,int height,int depth, int ppi,unsigned char * wsq,int * size,float bitrate);
int decodeRawWsq(unsigned char * wsqdat,int wsqsize,unsigned char * imagedata,int *width,int *height,int *depth, int *ppi,int *lossy);


#endif // FPNBIS_H
