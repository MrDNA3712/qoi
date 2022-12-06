
#include "qoi_imp.h"
#define QOI_OP_INDEX  0x00 /* 00xxxxxx */
#define QOI_OP_DIFF   0x40 /* 01xxxxxx */
#define QOI_OP_LUMA   0x80 /* 10xxxxxx */
#define QOI_OP_RUN    0xc0 /* 11xxxxxx */
#define QOI_OP_RGB    0xfe /* 11111110 */
#define QOI_OP_RGBA   0xff /* 11111111 */

#include <cstring>
#include <fstream>

using namespace std;

union pixel
{
	struct { 
		uint8_t r, g, b, a;
	} rgba;
	uint32_t v;
};

void* qoi_encode(const void* data, const uint32_t width, const uint32_t height, const uint8_t channels, const uint8_t colorspace, uint32_t *size) {
	uint32_t max_size = width * height * (channels + 1) + 14 + 8; // 14 byte header + 8 byte end marker
	uint8_t* bytes = new uint8_t[max_size];
	pixel array[64] = {0};
	pixel cur, prev;
	uint8_t run = 0;
	uint32_t length, end, pos,p=0;
	const uint8_t *pixels = (const uint8_t *)data;


	length = width * height * channels;
	end = length - channels;
	cur.v = 0;
	cur.rgba.a = 255;

	bytes[p++] = 'q';
	bytes[p++] = 'o';
	bytes[p++] = 'i';
	bytes[p++] = 'f';
	bytes[p++] = (0xff000000 & width) >> 24;
	bytes[p++] = (0x00ff0000 & width) >> 16;
	bytes[p++] = (0x0000ff00 & width) >> 8;
	bytes[p++] = (0x000000ff & width);
	bytes[p++] = (0xff000000 & height) >> 24;
	bytes[p++] = (0x00ff0000 & height) >> 16;
	bytes[p++] = (0x0000ff00 & height) >> 8;
	bytes[p++] = (0x000000ff & height);
	bytes[p++] = channels;
	bytes[p++] = colorspace;

	for (pos = 0; pos < length; pos += channels) {
		prev = cur;
		cur.rgba.r = pixels[pos + 0];
		cur.rgba.g = pixels[pos + 1];
		cur.rgba.b = pixels[pos + 2];

		if (channels == 4) {
			cur.rgba.a = pixels[pos + 3];
		}

		if (prev.v==cur.v)
		{
			run++;
			if (run == 62 || pos >= end) {
				bytes[p++] = QOI_OP_RUN | (run - 1);
				run = 0;
			}
			continue;
			
		}
		else if (run>0) {
			bytes[p++] = QOI_OP_RUN | (run - 1);
			run = 0;
		}
		uint8_t index = (cur.rgba.r * 3 + cur.rgba.g * 5 + cur.rgba.b * 7 + cur.rgba.a * 11) % 64;
		if (array[index].v == cur.v) {
			bytes[p++] = QOI_OP_INDEX | index;
			continue;
		}
		array[index] = cur;

		if (cur.rgba.a == prev.rgba.a) {
			int8_t dr = cur.rgba.r - prev.rgba.r;
			int8_t dg = cur.rgba.g - prev.rgba.g;
			int8_t db = cur.rgba.b - prev.rgba.b;

			if (dr >= -2 && dr <= 1 &&
				dg >= -2 && dg <= 1 &&
				db >= -2 && db <= 1) {
				bytes[p++] = QOI_OP_DIFF | (dr + 2) << 4 | (dg + 2) << 2 | (db + 2);
				continue;
			}
			if (dg >= -32 && dg <= 31 &&
				(dr - dg) >= -8 && (dr - dg) <= 7 &&
				(db - dg) >= -8 && (db - dg) <= 7) {
				bytes[p++] = QOI_OP_LUMA | (dg + 32);
				bytes[p++] = (dr - dg + 8) << 4 | (db - dg + 8);
				continue;
			}
			bytes[p++] = QOI_OP_RGB;
			bytes[p++] = cur.rgba.r;
			bytes[p++] = cur.rgba.g;
			bytes[p++] = cur.rgba.b;
		}
		else
		{
			bytes[p++] = QOI_OP_RGBA;
			bytes[p++] = cur.rgba.r;
			bytes[p++] = cur.rgba.g;
			bytes[p++] = cur.rgba.b;
			bytes[p++] = cur.rgba.a;
		}
	}
	bytes[p++] = 0;
	bytes[p++] = 0;
	bytes[p++] = 0;
	bytes[p++] = 0;

	bytes[p++] = 0;
	bytes[p++] = 0;
	bytes[p++] = 0;
	bytes[p++] = 1;
	*size = p;
	return bytes;
}

void write_qoi(const void* data, uint32_t width, uint32_t height, uint8_t channels, uint8_t colorspace, const char* filename) {
	uint32_t size = 0;
	void* enc = qoi_encode(data, width, height, channels, colorspace,&size);
	ofstream file;
	file.open(filename, ios::binary | ios::out);
	file.write((char*)enc, size);
	file.close();
}

void* qoi_decode(const void* data, uint32_t * width,  uint32_t * height,  uint8_t * channels,  uint8_t * colorspace, uint32_t* size) {
	const uint8_t* qoi = (const uint8_t*)data;
	uint32_t p = 0;
	char magic[4];
	pixel array[64] = { 0 };
	pixel cur, prev;
	cur.v = 0;
	cur.rgba.a = 255;
	
	//reading header
	magic[0] = (char)qoi[p++];
	magic[1] = (char)qoi[p++];
	magic[2] = (char)qoi[p++];
	magic[3] = (char)qoi[p++];
	if (strncmp(magic, "qoif", 4) != 0) {
		return NULL;
	}

	*width  = qoi[p++] << 24;
	*width |= qoi[p++] << 16;
	*width |= qoi[p++] << 8;
	*width |= qoi[p++];

	*height  = qoi[p++] << 24;
	*height |= qoi[p++] << 16;
	*height |= qoi[p++] << 8;
	*height |= qoi[p++];

	*channels   = qoi[p++];
	*colorspace = qoi[p++];

	uint8_t* image = new uint8_t[(*width) * (*height) * (*channels)];

	// reading image
	for (uint32_t pos=0; pos < (*width) * (*height) * (*channels); p++) {
		prev = cur;
		*size = pos;

		if (qoi[p] == QOI_OP_RGB) {
			cur.rgba.r = qoi[++p];
			cur.rgba.g = qoi[++p];
			cur.rgba.b = qoi[++p];
		}
		else if (qoi[p] == QOI_OP_RGBA) {
			cur.rgba.r = qoi[++p];
			cur.rgba.g = qoi[++p];
			cur.rgba.b = qoi[++p];
			cur.rgba.a = qoi[++p];
		}
		else if ((qoi[p] & 0b11000000) == QOI_OP_INDEX) {
			uint8_t index = qoi[p] & 0b00111111;
			cur = array[index];
		}
		else if ((qoi[p] & 0xc0) == QOI_OP_DIFF) {
			int8_t dr = qoi[p] >> 4 & 0b00000011;
			int8_t dg = qoi[p] >> 2 & 0b00000011;
			int8_t db = qoi[p]      & 0b00000011;
			cur.rgba.r += dr - 2;
			cur.rgba.g += dg - 2;
			cur.rgba.b += db - 2;
		}
		else if ((qoi[p] & 0xc0) == QOI_OP_LUMA) {
			int8_t dg = qoi[p++]     & 0b00111111;
			int8_t drg = qoi[p] >> 4 & 0b00001111;
			int8_t dbg = qoi[p]      & 0b00001111;
			cur.rgba.r += drg + dg -32 - 8;
			cur.rgba.g += dg - 32;
			cur.rgba.b += dbg + dg -32 - 8;
		}
		else if ((qoi[p] & 0xc0) == QOI_OP_RUN) {
			uint8_t run = qoi[p] & 0b00111111;
			run++;
			for (uint8_t i = 0; i < run; i++) {
				image[pos++] = cur.rgba.r;
				image[pos++] = cur.rgba.g;
				image[pos++] = cur.rgba.b;
				if (*channels == 4) {
					image[pos++] = cur.rgba.a;
				}
			}
			continue;
		}
		image[pos++] = cur.rgba.r;
		image[pos++] = cur.rgba.g;
		image[pos++] = cur.rgba.b;
		if (*channels == 4) {
			image[pos++] = cur.rgba.a;
		}
		uint8_t index = (cur.rgba.r * 3 + cur.rgba.g * 5 + cur.rgba.b * 7 + cur.rgba.a * 11) % 64;
		array[index] = cur;
	}
	return image;
}

void* read_qoi(const char* filename, uint32_t * width, uint32_t * height, uint8_t * channels, uint8_t * colorspace, uint32_t* size) {
	std::streampos fsize;
	char* data;
	ifstream file;
	file.open(filename, ios::in | ios::binary | ios::ate);
	if (file.is_open()) {
		fsize = file.tellg();
		data = new char[fsize];
		file.seekg(0, ios::beg);
		file.read(data, fsize);
		file.close();
		void* image;
		image = qoi_decode(data, width, height, channels, colorspace, size);
		delete[] data;
		return image;
	}
	else {
		return NULL;
	}
}