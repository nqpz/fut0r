#ifndef FUT0R_PAM_IMAGE_HEADER
#define FUT0R_PAM_IMAGE_HEADER

uint32_t* pam_load(FILE *f, int color_model,
                   unsigned int *width, unsigned int *height);

void pam_save(FILE* f, int color_model, const uint32_t* image_pixels,
              unsigned int width, unsigned int height);

#endif
