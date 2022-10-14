#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_LINEAR
#include "stb_image.h"
#include "convert.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("int size: %u, char size: %u\n",(unsigned int)sizeof(unsigned int), (unsigned int) sizeof(unsigned char));

	void *pixels = NULL;
	int w, h, channels;
    if(!stbi_info(argv[1], &w, &h, &channels)) {
        printf("Couldn't read header %s\n", argv[1]);
        exit(1);
    }

    // Force all odd encodings to be RGBA
    if(channels != 3) {
        channels = 4;
    }

    pixels = (void *)stbi_load(argv[1], &w, &h, NULL, channels);

    qoi_desc desc;
    desc.channels=channels;
    desc.height=h;
    desc.width=w;
    desc.colorspace=0;

    printf("read image %i x %i px\n",w,h);

    int out_size;

    void * out = encode(pixels,&out_size,&desc);

    printf("output size: %i\n",out_size);

    return 0;
}
