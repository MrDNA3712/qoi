typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned char channels;
	unsigned char colorspace;
} qoi_desc;

void * encode(void *pixels,int *out_size, qoi_desc *desc);
void * decode(void *data, int size, qoi_desc *qoi_desc);
