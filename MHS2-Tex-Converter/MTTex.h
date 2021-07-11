#pragma once
#include <cstdint>
#include <filesystem>

enum MTTexFormats {
	MT_R8G8B8A8 = 9,
	MT_BC1_LIN = 19,
	MT_BC1_SRGB = 20,
	MT_BC2_LIN = 21,
	MT_BC2_SRGB = 22,
	MT_BC3_LIN = 23,
	MT_BC3_SRGB = 24,
	MT_BC4_UNORM = 25,
	MT_BC5_UNORM = 26,
	MT_BC7_LIN = 55,
	MT_BC7_SRGB = 56
};

class MTTex {
public:
	const char magic[4] = { 'T', 'E', 'X', 0 };
	uint16_t version;
	uint16_t unkn1;
	uint8_t unused1;
	uint8_t alphaFlags;

	uint8_t mipCount;
	uint16_t width;
	uint16_t height;

	uint8_t unkn2;
	uint8_t Format;
	uint16_t unkn3;
	
	unsigned char* data;
	uint64_t mipOffsets[16];//max I've seen is 11, 16 lets us match the max of dds
	uint32_t size;

	MTTex(const char* path);
	MTTex(uint16_t vers, uint16_t widt, uint16_t heigh, uint8_t mipC, uint32_t mipOffset[], uint8_t format, unsigned char* dataOff, size_t nSize);
	bool Export(std::filesystem::path path);
	char* GetTexData();
	unsigned int GetPitch();
};
