#include "base.h"
#include "pam_image.h"


/* Netpbm PAM graphics format */
uint32_t* pam_load(FILE *f, int color_model,
                   unsigned int *width, unsigned int *height) {
  assert(0 == fscanf(f, "P7\n"));
  int c = fgetc(f);
  if (c == '#') {
    while (fgetc(f) != '\n');
  } else {
    ungetc(c, f);
  }
  assert(1 == fscanf(f, "WIDTH %u\n", width));
  assert(1 == fscanf(f, "HEIGHT %u\n", height));
  unsigned int depth;
  assert(1 == fscanf(f, "DEPTH %u\n", &depth));
  assert(4 == depth);
  assert(0 == fscanf(f, "MAXVAL 255\n"));
  assert(0 == fscanf(f, "TUPLTYPE RGB_ALPHA\n"));
  assert(0 == fscanf(f, "ENDHDR\n"));
  uint8_t* image = (uint8_t*) malloc(*width * *height * sizeof(uint32_t));
  if (image == NULL) {
    return NULL;
  }
  for (unsigned int i = 0; i < *width * *height; i++) {
    uint8_t r, g, b, a;
    r = (uint8_t) fgetc(f);
    g = (uint8_t) fgetc(f);
    b = (uint8_t) fgetc(f);
    a = (uint8_t) fgetc(f);
    if (color_model == F0R_COLOR_MODEL_RGBA8888) {
      image[i * 4 + 0] = r;
      image[i * 4 + 1] = g;
      image[i * 4 + 2] = b;
      image[i * 4 + 3] = a;
    } else { /* BGRA8888 or PACKED32 */
      image[i * 4 + 0] = b;
      image[i * 4 + 1] = g;
      image[i * 4 + 2] = r;
      image[i * 4 + 3] = a;
    }
  }
  return (uint32_t*) image;
}

void pam_save(FILE* f, int color_model, const uint32_t* image_pixels,
              unsigned int width, unsigned int height) {
  fprintf(f, "P7\n");
  fprintf(f, "WIDTH %u\n", width);
  fprintf(f, "HEIGHT %u\n", height);
  fprintf(f, "DEPTH 4\n");
  fprintf(f, "MAXVAL 255\n");
  fprintf(f, "TUPLTYPE RGB_ALPHA\n");
  fprintf(f, "ENDHDR\n");
  uint8_t* image = (uint8_t*) image_pixels;
  for (unsigned int i = 0; i < width * height; i++) {
    uint8_t r, g, b, a;
    if (color_model == F0R_COLOR_MODEL_RGBA8888) {
      r = image[i * 4 + 0];
      g = image[i * 4 + 1];
      b = image[i * 4 + 2];
      a = image[i * 4 + 3];
    } else { /* BGRA8888 or PACKED32 */
      b = image[i * 4 + 0];
      g = image[i * 4 + 1];
      r = image[i * 4 + 2];
      a = image[i * 4 + 3];
    }
    fputc(r, f);
    fputc(g, f);
    fputc(b, f);
    fputc(a, f);
  }
}
