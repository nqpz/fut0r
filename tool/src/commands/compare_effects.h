#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "../base.h"
#include "../frei0r_dynamic.h"
#include "../tbf.h"

#define NUM_PLUGINS 2

int compare_effects(int argc, char* argv[]) {
  tbf_t tbf;
  tbf_init(&tbf);
  int ret = EXIT_SUCCESS;

  if (argc < NUM_PLUGINS) {
    fputs("error: missing plugins\n", stderr);
    ret = EXIT_FAILURE;
    goto end;
  }

  f0r_t effect[NUM_PLUGINS];
  f0r_instance_t instance[NUM_PLUGINS];
  f0r_plugin_info_t plugin_info[NUM_PLUGINS];
  f0r_param_info_t param_info[NUM_PLUGINS];
  uint32_t* image_out[NUM_PLUGINS];

  unsigned int width = 64;
  unsigned int height = 64;
  uint32_t* image_in = (uint32_t*) malloc(width * height * sizeof(uint32_t));
  if (image_in == NULL) {
    fputs("error: could not allocate memory for image\n", stderr);
    ret = EXIT_FAILURE;
    goto end;
  }

  unsigned int seed;
  FILE* f = fopen("/dev/urandom", "rb");
  fread(&seed, sizeof(unsigned int), 1, f);
  fclose(f);
  seed ^= time(NULL);
  seed ^= getpid();
  seed ^= getppid();
  srand(seed);

  for (unsigned int j = 0; j < width * height; j++) {
    /* Assumes RAND_MAX is 2**31 - 1. */
    int r31bits = rand();
    int r1bit = rand() & 1;
    image_in[j] = (((uint32_t) r31bits) << 1) | ((uint32_t) r1bit);
  }
  double time = (double) rand() / (double) RAND_MAX;

  for (int i = 0; i < NUM_PLUGINS; i++) {
    fill_f0r(&effect[i], argv[i], &tbf);

    if (!effect[i].init()) {
      fprintf(stderr, "error: plugin '%s' could not be inited\n", argv[i]);
      ret = EXIT_FAILURE;
      goto end;
    }
    tbf_add(&tbf, NULL, (free_fun_t) effect[i].deinit);

    instance[i] = effect[i].construct(width, height);
    tbf_add(&tbf, instance[i], effect[i].destruct);

    effect[i].get_plugin_info(&plugin_info[i]);

    image_out[i] = (uint32_t*) malloc(width * height * sizeof(uint32_t));
    if (image_out[i] == NULL) {
      fputs("error: could not allocate memory for image\n", stderr);
      ret = EXIT_FAILURE;
      goto end;
    }
  }

  if (plugin_info[0].num_params != plugin_info[1].num_params) {
    fputs("error: plugins take different number of arguments\n", stderr);
    ret = EXIT_FAILURE;
    goto end;
  }

  for (int j = 0; j < plugin_info[0].num_params; j++) {
    for (int i = 0; i < NUM_PLUGINS; i++) {
      effect[i].get_param_info(&param_info[i], j);
    }
    if (param_info[0].type != param_info[1].type) {
      fprintf(stderr, "error: params are of different type at %d\n", j);
      ret = EXIT_FAILURE;
      goto end;
    }

    f0r_param_t value = NULL;
    if (param_info[0].type == F0R_PARAM_BOOL) {
      double b = (double) (rand() & 1);
      value = (f0r_param_t) &b;
    } else if (param_info[0].type == F0R_PARAM_DOUBLE) {
      double d = (double) rand() / (double) RAND_MAX;
      value = (f0r_param_t) &d;
    } else if (param_info[0].type == F0R_PARAM_COLOR) {
      f0r_param_color_t color;
      color.r = (float) rand() / (float) RAND_MAX;
      color.g = (float) rand() / (float) RAND_MAX;
      color.b = (float) rand() / (float) RAND_MAX;
      value = (f0r_param_t) &color;
    } else if (param_info[0].type == F0R_PARAM_POSITION) {
      f0r_param_position_t position;
      position.x = (double) rand() / (double) RAND_MAX;
      position.y = (double) rand() / (double) RAND_MAX;
      value = (f0r_param_t) &position;
    } else if (param_info[0].type == F0R_PARAM_STRING) {
      char* string = "FIXME";
      value = (f0r_param_t) &string;
    } else {
      fputs("error: invalid param type\n", stderr);
      ret = EXIT_FAILURE;
      goto end;
    }
    for (int i = 0; i < NUM_PLUGINS; i++) {
      effect[i].set_param_value(instance[i], value, j);
    }
  }

  for (int i = 0; i < NUM_PLUGINS; i++) {
    effect[i].update(instance[i], time, image_in, image_out[i]);
  }

  for (unsigned int j = 0; j < width * height; j++) {
    if (image_out[0][j] != image_out[1][j]) {
      fprintf(stderr, "error: result values at index %u do not match: %u, %u\n",
              j, image_out[0][j], image_out[1][j]);
      ret = EXIT_FAILURE;
      goto end;
    }
  }

 end:
  tbf_free_all(&tbf);
  return ret;
}
