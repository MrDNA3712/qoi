#pragma once

#include <cstdint>

void* qoi_encode(const void* data, uint32_t width, uint32_t height, uint8_t channels, uint8_t colorspace, uint32_t* size);
void write_qoi(const void* data, uint32_t width, uint32_t height, uint8_t channels, uint8_t colorspace, const char* filename);

void* qoi_decode(const void* data, uint32_t* width, uint32_t* height, uint8_t* channels, uint8_t* colorspace, uint32_t* size);
void* read_qoi(const char* filename, uint32_t* width, uint32_t* height, uint8_t* channels, uint8_t* colorspace, uint32_t* size);