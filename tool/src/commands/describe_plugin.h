#include "../base.h"
#include "../frei0r_dynamic.h"
#include "../tbf.h"

int describe_plugin(int argc, char* argv[]) {
  tbf_t tbf;
  tbf_init(&tbf);
  int ret = EXIT_SUCCESS;

  if (argc < 1) {
    fputs("error: missing plugin argument\n", stderr);
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
  printf("Plugin '%s': %s\nTakes %d parameters:\n",
         plugin_info.name, plugin_info.explanation, plugin_info.num_params);
  for (int i = 0; i < plugin_info.num_params; i++) {
    f0r_param_info_t param_info;
    f0r.get_param_info(&param_info, i);
    char* type_name;
    if (param_info.type == F0R_PARAM_BOOL) {
      type_name = "bool";
    } else if (param_info.type == F0R_PARAM_DOUBLE) {
      type_name = "double";
    } else if (param_info.type == F0R_PARAM_COLOR) {
      type_name = "color";
    } else if (param_info.type == F0R_PARAM_POSITION) {
      type_name = "position";
    } else if (param_info.type == F0R_PARAM_STRING) {
      type_name = "string";
    } else {
      fprintf(stderr, "error: invalid param type %d\n", param_info.type);
      ret = EXIT_FAILURE;
      goto end;
    }
    printf("  Param %d, '%s' (%s): %s\n", i,
           param_info.name, type_name, param_info.explanation);
  }
  printf("Author: %s\n", plugin_info.author);

 end:
  tbf_free_all(&tbf);
  return ret;
}
