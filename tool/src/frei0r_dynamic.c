#include "base.h"
#include "frei0r_dynamic.h"
#include "tbf.h"

static void* dlwrap(void *obj) {
  char* error = dlerror();
  if (error != NULL) {
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }
  return obj;
}

static void fill_f0r_from_handle(f0r_t *f0r, void *handle) {
  f0r->init =
    (init_t) dlwrap(dlsym(handle, "f0r_init"));
  f0r->deinit =
    (deinit_t) dlwrap(dlsym(handle, "f0r_deinit"));
  f0r->get_plugin_info =
    (get_plugin_info_t) dlwrap(dlsym(handle, "f0r_get_plugin_info"));
  f0r->get_param_info =
    (get_param_info_t) dlwrap(dlsym(handle, "f0r_get_param_info"));
  f0r->construct =
    (construct_t) dlwrap(dlsym(handle, "f0r_construct"));
  f0r->destruct =
    (destruct_t) dlwrap(dlsym(handle, "f0r_destruct"));
  f0r->set_param_value =
    (set_param_value_t) dlwrap(dlsym(handle, "f0r_set_param_value"));
  f0r->get_param_value =
    (get_param_value_t) dlwrap(dlsym(handle, "f0r_get_param_value"));
  f0r->update =
    (update_t) dlwrap(dlsym(handle, "f0r_update"));
}

static char* get_so_path(char* arg, tbf_t *tbf) {
  if (arg[0] == '/') {
    return arg;
  } else {
    /* Prefix the relative path with "./" to ensure that 'dlsym' treats it as a
       path. */
    char* path = (char*) malloc(sizeof(char) * (2 + 1 + strlen(arg)));
    tbf_add(tbf, path, &free);
    path[0] = '.';
    path[1] = '/';
    path[2] = '\0';
    return strcat(path, arg);
  }
}

void fill_f0r(f0r_t *f0r, char* so_path_base, tbf_t *tbf) {
  char* so_path = get_so_path(so_path_base, tbf);
  void *handle = dlopen(so_path, RTLD_NOW);
  if (handle == NULL) {
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }
  handle = dlwrap(handle);
  tbf_add(tbf, handle, (free_fun_t) &dlclose);
  fill_f0r_from_handle(f0r, handle);
}
