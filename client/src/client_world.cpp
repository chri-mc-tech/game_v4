#include "client_world.h"

#include "client_logger.h"

namespace world {
  void render_chunk(const string& input_string) {
    log_debug(input_string);

    string chunk = input_string.substr(0, input_string.find_first_of(';'));
    log_debug(chunk);
    string chunk_x = chunk.substr(0, input_string.find(' '));
    string chunk_y = chunk.substr(input_string.find(' '));

    string blocks_string = input_string.substr(input_string.find_first_of(';'));

    while (blocks_string.find(';') != std::string::npos) {
      string block = blocks_string.substr(0, blocks_string.find_first_of(';'));
      blocks_string = blocks_string.substr(blocks_string.find_first_of(';') + 1);
      log_debug(block);
    }

  }
}