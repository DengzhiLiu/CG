#include "Dib.h"
#include <fstream>
#include <windows.h>

CDIB::CDIB(void)
{
	image = NULL;
}

CDIB::~CDIB(void)
{
	if (image)
		delete image;
}

bool CDIB::readBMP(char* filename)
{
	//临时存放信息的变量
	int m = 0;
	u_char* pData;
	BITMAPINFOHEADER bmpInfo;
	BITMAPFILEHEADER bmpFileHead;

	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "ReadBMP failed: Can't open bmp file \"%s\".\n", filename);
		exit(1);
	}

	fread(&bmpFileHead, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	
	// Error Checking!
	if (bmpFileHead.bfType != 0x4d42)
		return false;
	if (bmpInfo.biBitCount != 24)
		return false;

	width = bmpInfo.biWidth;
	height = bmpInfo.biHeight;
	int	sizeBit = bmpInfo.biBitCount;
	int delta = sizeBit / 8;
	//int lineBytes = (width * bmpInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	long lFileSize = bmpFileHead.bfSize - bmpFileHead.bfOffBits;

	pData = (u_char *)new char[lFileSize];
	if (!pData)		return false;
	int f = fread(pData, sizeof(u_char), lFileSize, fp);
	fclose(fp);

	image = (u_char *)new char[height * width * 3];
	for(int j = height - 1; j >= 0; j--){
		for(int i = 0; i < width; i++){
			image[(j * width + i) * 3] = pData[m + 2];
			image[(j * width + i) * 3 + 1] = pData[m + 1];
			image[(j * width + i) * 3 + 2] = pData[m];	
			m += delta;	
		}
	}
	free(pData);

	return true;
}

void CDIB::buildMap()
{
	//int height,wide,i,j,level;
	//int LineBytes;
	//int off = 512;
	//float avg;

	//map = new BYTE*[1024];
	//for(i = 0;i < 1024;i++)
	//	map[i] = new BYTE[1024];
	//BYTE* lpSrc,*lpDibBits;
	//lpDibBits = pLpImage;
	////一行像素占用的字节数
	//LineBytes = (pLpBMIH->biWidth * pLpBMIH->biBitCount + 31)/32*4;
	//for(i = 0;i < 512;i++)
	//{
	//	for(j = 0;j < 512;j++)
	//	{
	//		lpSrc = lpDibBits + i*LineBytes + 3*j;
	//		map[i][j + off] = *(lpSrc + 2);
	//		map[i + off][j + off] = *(lpSrc + 1);
	//		map[i + off][j] = *lpSrc;
	//		/*if((i/8+j/8)%2 == 0)
	//		{
	//		map[i][j + off] = 0;
	//		map[i + off][j + off] = 0;
	//		map[i + off][j] = 0;
	//		*(lpSrc + 2) = 0;
	//		*(lpSrc + 1) = 0;
	//		*lpSrc = 0;
	//		}else{
	//		map[i][j + off] = 255;
	//		map[i + off][j + off] = 255;
	//		map[i + off][j] = 255;
	//		*(lpSrc + 2) = 255;
	//		*(lpSrc + 1) = 255;
	//		*lpSrc = 255;
	//		}*/
	//	}
	//}
	//for(level = 1;level <= 9;level++)
	//{
	//	off >>= 1;
	//	for(i = 0;i < off;i++)
	//		for(j = 0;j < off;j++)
	//		{
	//			int row,col,lastoff;
	//			row = 2*i;
	//			col = 2*j;
	//			lastoff = 2*off;
	//			avg = map[row][col + lastoff] + map[row][col + 1 + lastoff] + map[row + 1][col + lastoff] + map[row + 1][col + 1 +lastoff];
	//			avg = avg/4;
	//			map[i][j + off] = avg;

	//			avg = map[row+lastoff][col + lastoff] + map[row+lastoff][col + 1 + lastoff] + map[row + 1 +lastoff][col + lastoff] + map[row + 1+lastoff][col + 1 +lastoff];
	//			avg = avg/4;
	//			map[i + off][j + off] = avg;

	//			avg = map[row+lastoff][col] + map[row+lastoff][col+1] + map[row + 1+lastoff][col] + map[row + 1+lastoff][col + 1];
	//			avg = avg/4;
	//			map[i+off][j] = avg;
	//		}

	//}
}