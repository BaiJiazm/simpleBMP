#ifndef SIMPLEBMP_H
#define SIMPLEBMP_H

#include <cstdio>
#include <cstdint>
#include <cstdlib> 

//BMP structure infomation : http://www.cnblogs.com/xiekeli/archive/2012/05/09/2491191.html

#pragma pack(1) // For MSVC,disable struct Pack,or short will take 32bit mem as int32_t
typedef struct {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} ClBitMapFileHeader;

typedef struct {
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} ClBitMapInfoHeader;

typedef struct {
	uint8_t rgbBlue;
	uint8_t rgbGreen;
	uint8_t rgbRed;
	uint8_t rgbReserved;
} ClrgbMap;

class ClImgBMP {
	public:
		ClBitMapFileHeader bmpFileHeaderData;
		ClBitMapInfoHeader bmpInfoHeaderData;
		ClrgbMap colorMap[256];
		uint8_t* imgData;
		uint32_t dataLength;

		inline bool LoadImage(const char* path);
		inline bool SaveImage(const char* path);
};
#pragma pack()// reset to default


bool ClImgBMP::LoadImage(const char* path) {
	FILE* pFile;
	pFile = fopen(path, "rb");
	if (!pFile) {
		printf("open file %s failed !\n", path);
		return 0;
	}
	// Processing
	fread(&bmpFileHeaderData, sizeof(ClBitMapFileHeader), 1, pFile);
	if (bmpFileHeaderData.bfType != 0x4D42) { // Check is it an RGB file
		printf("%s not a bmp file !\n", path);
		fclose(pFile);
		return 0;
	}

	// Get Channel num of a pixel
	int channels = 0;
	fread(&bmpInfoHeaderData, sizeof(ClBitMapInfoHeader), 1, pFile);

	if (bmpInfoHeaderData.biBitCount == 8) { // grayscale format
		channels = 1;
		fread(&colorMap, sizeof(ClrgbMap), 256, pFile);
	} else if (bmpInfoHeaderData.biBitCount == 24) { // RGB format
		channels = 3;
	}

	// Get offset of every scanline,length(scanline)=length(pixel)+offset
	int offset = 0;
	int linelength = bmpInfoHeaderData.biWidth * channels;
	offset = linelength % 4;
	if (offset > 0) {
		offset = 4 - offset;
	}

	//new space for data
	dataLength=(linelength+offset)*bmpInfoHeaderData.biHeight;
	imgData=(uint8_t*)malloc(dataLength);
	
	// Read Pixel
	uint8_t* pData=imgData;
	for (int i = 0; i < bmpInfoHeaderData.biHeight; i++) {
		for (int j = 0; j < linelength; j++) {
			fread(pData, sizeof(uint8_t), 1, pFile);
			pData+=sizeof(uint8_t);
		}
		for (int k = 0; k < offset; k++) {
			fread(pData, sizeof(uint8_t), 1, pFile);
			pData+=sizeof(uint8_t);
		}
	}

	fclose(pFile);
	return true;
}

bool ClImgBMP::SaveImage(const char* path) {
	FILE* pFile;
	pFile = fopen(path, "wb");
	if (!pFile) {
		return 0;
	}

	// Processing
	fwrite(&bmpFileHeaderData, sizeof(ClBitMapFileHeader), 1, pFile);
	fwrite(&bmpInfoHeaderData, sizeof(ClBitMapInfoHeader), 1, pFile);
	// Get Channel num of a pixel
	int channels = 0;
	if (bmpInfoHeaderData.biBitCount == 8) {
		channels = 1;
		fwrite(&colorMap, sizeof(ClrgbMap), 256, pFile);
	} else if (bmpInfoHeaderData.biBitCount == 24) {
		channels = 3;
	}
	// Get offset of every scanline,length(scanline)=length(pixel)+offset
	int offset = 0;
	int linelength = bmpInfoHeaderData.biWidth * channels;
	offset = (channels * bmpInfoHeaderData.biWidth) % 4;
	if (offset > 0) {
		offset = 4 - offset;
	}

	// Write Pixel
	uint8_t* pData=imgData;
	for (int i = 0; i < bmpInfoHeaderData.biHeight; i++) {
		for (int j = 0; j < linelength; j++) {
			fwrite(pData, sizeof(uint8_t), 1, pFile);
			pData+=sizeof(uint8_t);
		}
		for (int k = 0; k < offset; k++) {
			fwrite(pData, sizeof(uint8_t), 1, pFile);
			pData+=sizeof(uint8_t);
		}
	}

	fclose(pFile);
	return true;
}


#endif
