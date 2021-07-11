#include <fstream>
#include <iostream>
#include "bitter/lsb0_reader.hpp"
#include "bitter/lsb0_writer.hpp"
#include "MTTex.h"



MTTex::MTTex(const char* path)
{
	std::ifstream file(path, std::ios::in|std::ios::binary);
	if (!file) {
		std::cout << "Failed to open file.";
		return;
	}

	uint32_t header1 = 0;
	uint32_t header2 = 0;
	uint32_t header3 = 0;

	char magic[4];
	file.seekg(0, SEEK_END);
	int fileSize = file.tellg();
	file.seekg(0, SEEK_SET);
	file.read(magic, 4);
	file.read((char*)&header1, sizeof(uint32_t));
	file.read((char*)&header2, sizeof(uint32_t));
	file.read((char*)&header3, sizeof(uint32_t));

	auto head1 = bitter::lsb0_reader<uint32_t, 12, 12, 4, 4>(header1);
	auto ver = head1.field<0>().as<uint16_t>();
	auto unk1 = head1.field<1>().as<uint16_t>();
	auto unu1 = head1.field<2>().as<uint8_t>();
	auto alpF = head1.field<3>().as<uint8_t>();
	version = ver;
	unkn1 = unk1;
	unused1 = unu1;
	alphaFlags = alpF;

	auto head2 = bitter::lsb0_reader<uint32_t, 6, 13, 13>(header2);
	auto mips = head2.field<0>().as<uint8_t>();
	auto widt = head2.field<1>().as<uint16_t>();
	auto heig = head2.field<2>().as<uint16_t>();
	mipCount = mips;
	width = widt;
	height = heig;

	auto head3 = bitter::lsb0_reader<uint32_t, 8, 8, 16>(header3);
	auto unk2 = head3.field<0>().as<uint8_t>();
	auto form = head3.field<1>().as<uint8_t>();
	auto unk3 = head3.field<2>().as<uint16_t>();

	unkn2 = unk2;
	Format = form;
	unkn3 = unk3;
	for (int i = 0; i < mipCount; i++) {
		file.read((char*)&mipOffsets[i], sizeof(uint64_t));
	}
	size = fileSize - 16 - (8 * mipCount);
	data = (unsigned char*)malloc(size);
	file.read((char*)data, size);
	file.close();
}

MTTex::MTTex(uint16_t vers, uint16_t widt, uint16_t heigh, uint8_t mipC, uint32_t mipOffset[], uint8_t format, unsigned char* dataOff, size_t nSize)
{
	version = vers;
	unkn1 = 3;
	unused1 = 0;
	alphaFlags = 2;//this isn't always 2, but for the textures this supports it is
	width = widt;
	height = heigh;
	mipCount = mipC;
	unkn2 = 1;
	Format = format;
	unkn3 = 1;
	for (int i = 0; i < 16; i++) {
		mipOffsets[i] = (uint64_t)mipOffset[i]+ 16 + (8*mipCount);
	}
	data = dataOff;
	size = nSize;
}

unsigned int MTTex::GetPitch() {
	switch (Format) {
	case 7:
	case 9:
		return height * width * 4;
	case 19:
	case 20:
		return height * width / 2;
	default:
		return height * width;
	}
}

bool MTTex::Export(std::filesystem::path path) {
	std::ofstream file(path, std::ios::out | std::ios::binary);
	if (!file) {
		std::cout << "Failed to open file.";
		return false;
	}
	auto head1 = bitter::lsb0_writer<uint32_t, 12, 12, 4, 4>();
	head1.field<0>(version);
	head1.field<1>(unkn1);
	head1.field<2>(unused1);
	head1.field<3>(alphaFlags);
	auto head2 = bitter::lsb0_writer<uint32_t, 6, 13, 13>();
	head2.field<0>(mipCount);
	head2.field<1>(width);
	head2.field<2>(height);
	auto head3 = bitter::lsb0_writer<uint32_t, 8, 8, 16>();
	head3.field<0>(unkn2);
	head3.field<1>(Format);
	head3.field<2>(unkn3);
	uint32_t header1 = head1.data();
	uint32_t header2 = head2.data();
	uint32_t header3 = head3.data();
	//now start actual file writing
	file.write((char*)&magic, sizeof(magic));
	file.write((char*)&header1, sizeof(uint32_t));
	file.write((char*)&header2, sizeof(uint32_t));
	file.write((char*)&header3, sizeof(uint32_t));
	for (int i = 0; i < mipCount; i++) {
		file.write((char*)&mipOffsets[i], sizeof(uint64_t));
	}
	file.write((char*)data, size);
	file.close();
	return true;
}
