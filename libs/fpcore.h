#ifndef FPCORE_H
#define FPCORE_H

#ifdef __cplusplus
extern "C" {
#endif

    int MatchTemplate(unsigned char* pSrcData, int nSrcSize, unsigned char* pDstData, int nDstSize);
    int FAP20_MatchTemplate(unsigned char* pSrcData, int nSrcSize, unsigned char* pDstData, int nDstSize);
    int FAP30_MatchTemplate(unsigned char* pSrcData, int nSrcSize, unsigned char* pDstData, int nDstSize);

#ifdef __cplusplus
}
#endif

#endif
