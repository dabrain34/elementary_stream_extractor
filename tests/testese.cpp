/* ESExtractor
 * Copyright (C) 2022 Igalia, S.L.
 *     Author: Stephane Cerveau <scerveau@igalia.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You
 * may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <fstream>
#include <algorithm>
#include <cassert>

#include "esextractor.h"
#include "eselogger.h"


static void
dump_packet (ESExtractor * esextractor, ESEPacket* pkt)
{
  const char *packet_type_name = es_extractor_video_codec_name (esextractor);
  INFO ("Got a %s packet of size %d pts=%lld", packet_type_name, pkt->data_size, pkt->pts);
  MEM_DUMP (pkt->data, pkt->data_size, "Buffer=");
}

/// @brief Return a new esextactor
/// @param fileName the file path to extract frames from
/// @param options Options used by the esextractor
/// @param debug_level set the debug level of the esextractor
/// @return a new esextractor object or nullptr if the path is not a valid media file.
ESExtractor*
create_es_extractor (const char *fileName, const char* options, uint8_t debug_level) {
  es_extractor_set_log_level (debug_level);
  INFO ("Extracting packets from %s with options %s", fileName, options);
  ESExtractor *esextractor = es_extractor_new (fileName, options);
  return esextractor;
}


/// @brief  Returns the number of frame found
/// @param extractor a valid extractor
/// @return the number of frames
int
parse (ESExtractor* esextractor)
{
  ESEResult res;
  ESEPacket *pkt;
  int packet_count;

  while ((res =
          es_extractor_read_packet (esextractor,
              &pkt)) < ESE_RESULT_EOS) {

    dump_packet (esextractor, pkt);
    es_extractor_clear_packet (pkt);
  }

  packet_count = es_extractor_packet_count (esextractor);
  INFO ("Got %d packet(s)", packet_count);
  return packet_count;
}

int parse_file (const char *fileName, const char* options, uint8_t debug_level) {
  ESExtractor* esextractor = create_es_extractor (fileName, options, debug_level);
  if (!esextractor) {
    ERR ("Unable to discover a compatible stream. Exit");
    return -1;
  }
  int packet_count = parse(esextractor);
  es_extractor_teardown (esextractor);
  return packet_count;
}
