#ifndef FUT0R_FREI0R_DYNAMIC_HEADER
#define FUT0R_FREI0R_DYNAMIC_HEADER

#include "tbf.h"

typedef int (*init_t)();
typedef void (*deinit_t)();
typedef void (*get_plugin_info_t)(f0r_plugin_info_t*);
typedef void (*get_param_info_t)(f0r_param_info_t*, int);
typedef f0r_instance_t (*construct_t)(unsigned int, unsigned int);
typedef void (*destruct_t)(f0r_instance_t);
typedef void (*set_param_value_t)(f0r_instance_t, f0r_param_t, int);
typedef void (*get_param_value_t)(f0r_instance_t, f0r_param_t, int);
typedef void (*update_t)(f0r_instance_t, double, uint32_t*, uint32_t*);
// FIXME: Also handle f0r_update2?

typedef struct {
  init_t init;
  deinit_t deinit;
  get_plugin_info_t get_plugin_info;
  get_param_info_t get_param_info;
  construct_t construct;
  destruct_t destruct;
  set_param_value_t set_param_value;
  get_param_value_t get_param_value;
  update_t update;
} f0r_t;

void fill_f0r(f0r_t *f0r, char* so_path_base, tbf_t *tbf);

#endif
