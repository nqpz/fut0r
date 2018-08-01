#include "../base.h"
#include "../frei0r_dynamic.h"
#include "../pam_image.h"
#include "../tbf.h"

int apply_on_image(int argc, char* argv[]) {
  tbf_t tbf;
  tbf_init(&tbf);
  int ret = EXIT_SUCCESS;

  if (argc < 3) {
    if (argc == 0) {
      fputs("error: missing plugin argument\n", stderr);
    } else if (argc == 1) {
      fputs("error: missing input pam file\n", stderr);
    } else if (argc == 2) {
      fputs("error: missing output pam file\n", stderr);
    }
    ret = EXIT_FAILURE;
    goto end;
  }

  f0r_t f0r;
  f0r_plugin_info_t plugin_info;

  fill_f0r(&f0r, argv[0], &tbf);

  if (!f0r.init()) {
    fputs("error: plugin could not be inited\n", stderr);
    ret = EXIT_FAILURE;
    goto end;
  }
  tbf_add(&tbf, NULL, (free_fun_t) f0r.deinit);

  f0r.get_plugin_info(&plugin_info);
  if (argc - 3 < plugin_info.num_params) {
    fputs("error: not enough arguments\n", stderr);
    ret = EXIT_FAILURE;
    goto end;
  }

  char* pam_inpath = argv[1];
  FILE* pam_in;
  if (strcmp(pam_inpath, "-") == 0) {
    pam_in = stdin;
  } else {
    pam_in = fopen(pam_inpath, "rb");
    if (pam_in == NULL) {
      perror(NULL);
      ret = EXIT_FAILURE;
      goto end;
    }
    tbf_add(&tbf, (void*) pam_in, (free_fun_t) &fclose);
  }

  unsigned int width;
  unsigned int height;
  uint32_t* image_in = pam_load(pam_in, plugin_info.color_model, &width, &height);
  if (image_in == NULL) {
    fputs("error: could not allocate memory for image\n", stderr);
    ret = EXIT_FAILURE;
    goto end;
  }
  tbf_add(&tbf, image_in, &free);

  uint32_t* image_out = (uint32_t*) malloc(width * height * sizeof(uint32_t));
  if (image_out == NULL) {
    fputs("error: could not allocate memory for image\n", stderr);
    ret = EXIT_FAILURE;
    goto end;
  }
  tbf_add(&tbf, image_out, &free);

  f0r_instance_t instance = f0r.construct(width, height);
  tbf_add(&tbf, instance, f0r.destruct);

  for (int i = 0; i < plugin_info.num_params; i++) {
    f0r_param_info_t param_info;
    f0r_param_t value = NULL;
    char* arg = argv[3 + i];

    f0r.get_param_info(&param_info, i);
    if (param_info.type == F0R_PARAM_BOOL || param_info.type == F0R_PARAM_DOUBLE) {
      double d;
      sscanf(arg, "%lf", &d);
      value = (f0r_param_t) &d;
    } else if (param_info.type == F0R_PARAM_COLOR) {
      f0r_param_color_t color;
      sscanf(arg, "%f,%f,%f", &color.r, &color.g, &color.b);
      value = (f0r_param_t) &color;
    } else if (param_info.type == F0R_PARAM_POSITION) {
      f0r_param_position_t position;
      sscanf(arg, "%lf,%lf", &position.x, &position.y);
      value = (f0r_param_t) &position;
    } else if (param_info.type == F0R_PARAM_STRING) {
      value = (f0r_param_t) &arg;
    }
    f0r.set_param_value(instance, value, i);
  }

  double time = 0.0;
  f0r.update(instance, time, image_in, image_out);

  char* pam_outpath = argv[2];
  FILE* pam_out;
  if (strcmp(pam_outpath, "-") == 0) {
    pam_out = stdout;
  } else {
    pam_out = fopen(pam_outpath, "wb");
    if (pam_out == NULL) {
      perror(NULL);
      ret = EXIT_FAILURE;
      goto end;
    }
    tbf_add(&tbf, (void*) pam_out, (free_fun_t) &fclose);
  }

  pam_save(pam_out, plugin_info.color_model, image_out, width, height);

 end:
  tbf_free_all(&tbf);
  return ret;
}
