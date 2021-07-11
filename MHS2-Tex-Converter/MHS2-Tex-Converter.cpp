// MHS2-Tex-Converter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <filesystem>
#include "dds.h"
#include "MTTex.h"
using namespace std;
static const dds_u32 dds_supfmt[] = { DDS_FMT_B8G8R8A8, DDS_FMT_R8G8B8A8, DDS_FMT_BC1_LIN, DDS_FMT_BC1_SRGB, DDS_FMT_BC2_LIN, DDS_FMT_BC2_SRGB, DDS_FMT_BC3_LIN, DDS_FMT_BC3_SRGB, DDS_FMT_BC4, DDS_FMT_BC5, DDS_FMT_BC7_LIN, DDS_FMT_BC7_SRGB, 0 };
int main(int argc, char *argv[])
{
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if ((std::filesystem::path(argv[i]).extension()) == ".dds") {
                dds_info* dds = (dds_info*)malloc(sizeof(dds_info));
                int res = dds_load_from_file(argv[i], dds,dds_supfmt);
                if (res < 0) {
                    cout << "Failed to load DDS";
                    return 0;
                }
                unsigned char* ddsData = dds_read_all(dds);
                MTTex tex((uint16_t)163, (uint16_t)dds->image.width, (uint16_t)dds->image.height, (uint8_t)dds->mipcount, dds->mipoffsets, (uint8_t)dds->image.format, ddsData, dds->image.size);
                bool finalRes = tex.Export(std::filesystem::path(argv[i]).replace_extension(".tex"));
                //cout << dds->srcsize - dds->hdrsize;
            }
            else if ((std::filesystem::path(argv[i]).extension()) == ".tex") {
                MTTex tex(argv[i]);
                dds_info* dds = (dds_info*)malloc(sizeof(dds_info));
                dds->mipcount = tex.mipCount;
                dds->image.width = tex.width;
                dds->image.height = tex.height;
                dds->image.format = tex.Format;
                dds->image.pitch = tex.GetPitch();
                dds->image.size = tex.size;
                unsigned char* data = tex.data;
                bool finalRes = dds_write(dds, (dds_byte*)data, tex.size, std::filesystem::path(argv[i]).replace_extension(".dds"));
                //cout << tex.version;
            }
        }
    }
    
}

