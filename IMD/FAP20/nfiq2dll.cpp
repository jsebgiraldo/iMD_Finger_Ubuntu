


void Nfiq2Dll::FreeIntPtr(char* ptr)
{
    free(ptr);
}

char* Nfiq2Dll::ConvertToIntPtr(std::byte array[])
{
    char* ptr = static_cast<char*>(malloc(strlen((char *)array)));
    memcpy(ptr, array, strlen((char *)array));
    return ptr;
}

int Nfiq2Dll::SafeComputeNfiq2Score(int fpos, char *pixels,
                                  int size, int width, int height, int ppi){
    return  ComputeNfiq2Score(fpos,pixels,size,  width,  height, ppi);

}
