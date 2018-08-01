#include "../base.h"
#include "../frei0r_dynamic.h"
#include "../tbf.h"

static char* get_type_name(int type) {
  char* type_name = NULL;
  if (type == F0R_PARAM_BOOL) {
    type_name = "double";
  } else if (type == F0R_PARAM_DOUBLE) {
    type_name = "double";
  } else if (type == F0R_PARAM_COLOR) {
    type_name = "f0r_param_color_t";
  } else if (type == F0R_PARAM_POSITION) {
    type_name = "f0r_param_position_t";
  } else if (type == F0R_PARAM_STRING) {
    type_name = "char*";
  }
  return type_name;
}

int generate_c_futhark_wrapper(int argc, char* argv[]) {
  tbf_t tbf;
  tbf_init(&tbf);
  int ret = EXIT_SUCCESS;

  if (argc < 2) {
    if (argc == 0) {
      fputs("error: missing plugin argument\n", stderr);
    } else if (argc == 1) {
      fputs("error: missing port author name\n", stderr);
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

  f0r_instance_t inst = f0r.construct(16, 16); // Not perfect
  tbf_add(&tbf, inst, free);

  f0r.get_plugin_info(&plugin_info);

  puts("#include <stdlib.h>");
  puts("#include \"frei0r.h\"");
  puts("#include \"fut.h\"");
  puts("");
  puts("typedef struct {");
  puts("  unsigned int width;");
  puts("  unsigned int height;");
  for (int i = 0; i < plugin_info.num_params; i++) {
    f0r_param_info_t param_info;
    f0r.get_param_info(&param_info, i);
    char* type_name = get_type_name(param_info.type);
    if (type_name == NULL) {
      fprintf(stderr, "error: invalid param type %d\n", param_info.type);
      ret = EXIT_FAILURE;
      goto end;
    }
    printf("  %s param%d;\n", type_name, i);
  }
  puts("} local_instance_t;");
  puts("");
  puts("int f0r_init() { return 1; }");
  puts("void f0r_deinit() { }");
  puts("");
  puts("void f0r_get_plugin_info(f0r_plugin_info_t* info) {");
  printf("  info->name = \"%s\";\n", plugin_info.name);
  char* port_author = argv[1];
  printf("  info->author = \"%s (original frei0r plugin by %s)\";\n",
         port_author, plugin_info.author);
  printf("  info->plugin_type = %d;\n", plugin_info.plugin_type);
  printf("  info->color_model = %d;\n", plugin_info.color_model);
  printf("  info->frei0r_version = %d;\n", plugin_info.frei0r_version);
  printf("  info->major_version = %d;\n", plugin_info.major_version);
  printf("  info->minor_version = %d;\n", plugin_info.minor_version);
  printf("  info->num_params = %d;\n", plugin_info.num_params);
  printf("  info->explanation = \"%s\";\n", plugin_info.explanation);
  puts("}");
  puts("");
  puts("void f0r_get_param_info(f0r_param_info_t* info, int param_index) {");
  puts("  switch(param_index) {");
  for (int i = 0; i < plugin_info.num_params; i++) {
    f0r_param_info_t param_info;
    f0r.get_param_info(&param_info, i);
    printf("  case %d:\n", i);
    printf("    info->name = \"%s\";\n", param_info.name);
    printf("    info->type = %d;\n", param_info.type);
    printf("    info->explanation = \"%s\";\n", param_info.explanation);
    puts("    break;");
  }
  puts("  }");
  puts("}");
  puts("");
  puts("f0r_instance_t f0r_construct(unsigned int width, unsigned int height) {");
  puts("  local_instance_t* inst = (local_instance_t*) malloc(sizeof(local_instance_t));");
  puts("  inst->width = width;");
  puts("  inst->height = height;");
  for (int i = 0; i < plugin_info.num_params; i++) {
    f0r_param_info_t param_info;
    f0r.get_param_info(&param_info, i);
    if (param_info.type == F0R_PARAM_BOOL || param_info.type == F0R_PARAM_DOUBLE) {
      double value;
      f0r.get_param_value(inst, (f0r_param_t) &value, i);
      printf("  inst->param%d = %lf;\n", i, value);
    } else if (param_info.type == F0R_PARAM_COLOR) {
      f0r_param_color_t color;
      f0r.get_param_value(inst, (f0r_param_t) &color, i);
      printf("  inst->param%d.r = %f;\n", i, color.r);
      printf("  inst->param%d.g = %f;\n", i, color.g);
      printf("  inst->param%d.b = %f;\n", i, color.b);
    } else if (param_info.type == F0R_PARAM_POSITION) {
      f0r_param_position_t position;
      f0r.get_param_value(inst, (f0r_param_t) &position, i);
      printf("  inst->param%d.x = %lf;\n", i, position.x);
      printf("  inst->param%d.y = %lf;\n", i, position.y);
    } else if (param_info.type == F0R_PARAM_STRING) {
      char* string;
      f0r.get_param_value(inst, (f0r_param_t) &string, i);
      printf("  inst->param%d = \"%s\";\n", i, string);
    } else {
      fprintf(stderr, "error: invalid param type %d\n", param_info.type);
      ret = EXIT_FAILURE;
      goto end;
    }
  }
  puts("  return (f0r_instance_t) inst;");
  puts("}");
  puts("");
  puts("void f0r_destruct(f0r_instance_t instance) {");
  puts("  free(instance);");
  puts("}");
  puts("");
  puts("void f0r_set_param_value(f0r_instance_t inst_f0r, f0r_param_t param, int param_index) {");
  puts("  local_instance_t *inst = (local_instance_t*) inst_f0r;");
  puts("  switch(param_index) {");
  for (int i = 0; i < plugin_info.num_params; i++) {
    f0r_param_info_t param_info;
    f0r.get_param_info(&param_info, i);
    char* type_name = get_type_name(param_info.type);
    if (type_name == NULL) {
      fprintf(stderr, "error: invalid param type %d\n", param_info.type);
      ret = EXIT_FAILURE;
      goto end;
    }
    printf("  case %d:\n", i);
    printf("    inst->param%d = *((%s*) param);\n", i, type_name);
    puts("    break;");
  }
  puts("  }");
  puts("}");
  puts("");
  puts("void f0r_get_param_value(f0r_instance_t inst_f0r, f0r_param_t param, int param_index) {");
  puts("  local_instance_t *inst = (local_instance_t*) inst_f0r;");
  puts("  switch(param_index) {");
  for (int i = 0; i < plugin_info.num_params; i++) {
    f0r_param_info_t param_info;
    f0r.get_param_info(&param_info, i);
    char* type_name = get_type_name(param_info.type);
    if (type_name == NULL) {
      fprintf(stderr, "error: invalid param type %d\n", param_info.type);
      ret = EXIT_FAILURE;
      goto end;
    }
    printf("  case %d:\n", i);
    printf("    *((%s*) param) = inst->param%d;\n", type_name, i);
    puts("    break;");
  }
  puts("  }");
  puts("}");
  puts("");
  puts("void f0r_update(f0r_instance_t inst_f0r, double time, const uint32_t* inframe, uint32_t* outframe) {");
  puts("  local_instance_t *inst = (local_instance_t*) inst_f0r;");
  puts("  struct futhark_context_config *cfg = futhark_context_config_new();");
  puts("  struct futhark_context *ctx = futhark_context_new(cfg);");
  puts("  struct futhark_u32_2d *in_arr = futhark_new_u32_2d(ctx, (uint32_t*) inframe, inst->height, inst->width);");
  puts("  struct futhark_u32_2d *out_arr;");
  puts("  futhark_entry_f0r_update(ctx, &out_arr,");
  for (int i = 0; i < plugin_info.num_params; i++) {
    f0r_param_info_t param_info;
    f0r.get_param_info(&param_info, i);
    if (param_info.type == F0R_PARAM_BOOL || param_info.type == F0R_PARAM_DOUBLE) {
      printf("    inst->param%d,\n", i);
    } else if (param_info.type == F0R_PARAM_COLOR) {
      printf("    inst->param%d.r, inst->param%d.g, inst->param%d.b,\n", i, i, i);
    } else if (param_info.type == F0R_PARAM_POSITION) {
      printf("    inst->param%d.x, inst->param%d.y,\n", i, i);
    } else if (param_info.type == F0R_PARAM_STRING) {
      // FIXME
    } else {
      fprintf(stderr, "error: invalid param type %d\n", param_info.type);
      ret = EXIT_FAILURE;
      goto end;
    }
  }
  puts("    time, in_arr);");
  puts("  futhark_values_u32_2d(ctx, out_arr, outframe);");
  puts("  futhark_free_u32_2d(ctx, in_arr);");
  puts("  futhark_free_u32_2d(ctx, out_arr);");
  puts("  futhark_context_free(ctx);");
  puts("  futhark_context_config_free(cfg);");
  puts("  futhark_context_sync(ctx);");
  puts("}");

 end:
  tbf_free_all(&tbf);
  return ret;
}
