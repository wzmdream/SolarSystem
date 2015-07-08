#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "type.h"

void LoadBmp(char *filename, TEXTUREIMAGE *textureImg)
{
	int i, j;
	FILE *file;
	BMPFILEHEADER bmpFile;
	BMPINFOHEADER bmpInfo;
	int pixel_size;

	// ��ʼ����������
	
	textureImg->imgWidth = 0;
	textureImg->imgHeight = 0;
	if (textureImg->data != NULL)
	{
		delete []textureImg->data;
	}

	// ���ļ�
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		return;
	}
	// ��ȡ�ļ�ͷ
	rewind(file);
	fread(&bmpFile, sizeof(BMPFILEHEADER), 1, file);
	fread(&bmpInfo, sizeof(BMPINFOHEADER), 1, file);
	// ��֤�ļ�����
	if (bmpFile.bfType != 0x4D42)
	{
		return;
	}
	// ��ȡͼ��ɫ����
	pixel_size = bmpInfo.biBitCount >> 3;
	
	// ��ȡ�ļ�����
	textureImg->data = new unsigned char[bmpInfo.biWidth * bmpInfo.biHeight * pixel_size];
	
	
	for(i = 0 ; i < bmpInfo.biHeight; i++)
	{
		fseek(file, bmpFile.bfOffBits + (bmpInfo.biHeight - i - 1) * bmpInfo.biWidth * pixel_size, SEEK_SET);	

		for (j = 0; j < bmpInfo.biWidth; j++)
		{
			// ��ɫ����
			fread(textureImg->data + (i * bmpInfo.biWidth + j) * pixel_size + 2, sizeof(unsigned char), 1, file);
			// ��ɫ����
			fread(textureImg->data + (i * bmpInfo.biWidth + j) * pixel_size + 1, sizeof(unsigned char), 1, file);
			// ��ɫ����
			fread(textureImg->data + (i * bmpInfo.biWidth + j) * pixel_size + 0, sizeof(unsigned char), 1, file);
			// Alpha����
			if (pixel_size == 4)
			{
				fread(textureImg->data + (i * bmpInfo.biWidth + j) * pixel_size + 3, sizeof(unsigned char), 1, file);
			}
		}
	}
	// ��¼ͼ����ز���
	textureImg->imgWidth = bmpInfo.biWidth;
	textureImg->imgHeight = bmpInfo.biHeight;
	textureImg->byteCount = pixel_size;
	fclose(file);
}