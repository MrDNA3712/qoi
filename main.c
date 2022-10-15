#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_LINEAR
#include "stb_image.h"
#include "convert.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("int size: %u, char size: %u, long size: %u\n",(unsigned int)sizeof(unsigned int), (unsigned int)sizeof(unsigned char), (unsigned int)sizeof(unsigned long long));
    printf("struct size: %u, width offset: %u, height offset: %u, channels offset: %u, cp offset: %u\n", (unsigned int)sizeof(qoi_desc), offsetof(qoi_desc, width), offsetof(qoi_desc, height), offsetof(qoi_desc, channels), offsetof(qoi_desc, colorspace));
    if (argc < 2) {
        printf("not enough arguments");
        exit(1);
    }

	void *pixels = NULL;
	int w, h, channels;
    if(!stbi_info(argv[1], &w, &h, &channels)) {
        printf("Couldn't read header %s\n", argv[1]);
        exit(2);
    }

    // Force all odd encodings to be RGBA
    if(channels != 3) {
        channels = 4;
    }

    pixels = (void *)stbi_load(argv[1], &w, &h, NULL, channels);

    qoi_desc desc;
    desc.height=h;
    desc.width=w;
    desc.channels=channels;
    desc.colorspace=0;

    printf("read image %i x %i px %i channels\n",w,h,channels);

    int out_size=0;
    
    printf("pixel address %llx\nout_size address %llx\ndesc address %llx\n", pixels, &out_size, &desc);


    void * out = encode(pixels,&out_size,&desc);

    printf("output size: %i\n",out_size);
    printf("out address: %llx\n",out);

    return 0;
}
