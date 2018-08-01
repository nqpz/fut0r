#include "base.h"

#include "commands/describe_plugin.h"
#include "commands/apply_on_image.h"
#include "commands/compare_effects.h"
#include "commands/generate_c_futhark_wrapper.h"

void show_help() {
  puts("usage: tool COMMAND [ARG...]\n"
       "\n"
       "Commands:\n"
       "  describe-plugin SO_PATH: Print plugin info and parameters info\n"
       "  apply-on-image SO_PATH PAM_INPATH PAM_OUTPATH [ARG...]: Apply plugin effect on image\n"
       "  compare-effects SO_PATH1 SO_PATH2: Compare the outputs of two supposedly equivalent effect implementations\n"
       "  generate-c-futhark-wrapper SO_PATH PORT_AUTHOR_NAME: Generate C code for interfacing with a Futhark port of a frei0r effect");
}

int main(int argc, char* argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    show_help();
    return EXIT_SUCCESS;
  }

  char* command = argv[1];
  int argc_rest = argc - 2;
  char** argv_rest = argv + 2;
  if (strcmp(command, "describe-plugin") == 0) {
    return describe_plugin(argc_rest, argv_rest);
  } else if (strcmp(command, "apply-on-image") == 0) {
    return apply_on_image(argc_rest, argv_rest);
  } else if (strcmp(command, "compare-effects") == 0) {
    return compare_effects(argc_rest, argv_rest);
  } else if (strcmp(command, "generate-c-futhark-wrapper") == 0) {
    return generate_c_futhark_wrapper(argc_rest, argv_rest);
  } else {
    fprintf(stderr, "error: invalid command '%s'\n", command);
    return EXIT_FAILURE;
  }
}
