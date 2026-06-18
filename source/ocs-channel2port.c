// Copyright 2014-2016 David Robillard <d@drobilla.net>
// SPDX-License-Identifier: ISC

#include "./uris.h"

#include "lv2/atom/atom.h"
#include "lv2/atom/util.h"
#include "lv2/core/lv2.h"
#include "lv2/core/lv2_util.h"
#include "lv2/log/log.h"
#include "lv2/log/logger.h"
#include "lv2/midi/midi.h"
#include "lv2/urid/urid.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#define DRUM_START 36

void debugMidiNote(LV2_Handle instance, LV2_Atom_Event * event, int8_t  msg[]);

enum { CHN2PATH_IN = 0, CHN2PATH_OUT = 1 };

typedef struct {
  // Features
  LV2_URID_Map*  map;
  LV2_Log_Logger logger;

  // Ports
  const LV2_Atom_Sequence* in_port;
  LV2_Atom_Sequence*      out_port[4];
  const float* chnz[4];

  // URIs
  Chn2pathURIs uris;
} Chn2path;

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  Chn2path* self = (Chn2path*)instance;
  if (port==0) {
    self->in_port = (const LV2_Atom_Sequence*)data;
  } 
  else if (port<5) {
    self->chnz[port-1] = (const float*)data;
  }
  else if (port<9) {
    self->out_port[port-5] = (LV2_Atom_Sequence*)data;
  } 
  else {
    return;
  }

}


static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               path,
            const LV2_Feature* const* features)
{
  // Allocate and initialise instance structure.
  Chn2path* self = (Chn2path*)calloc(1, sizeof(Chn2path));
  if (!self) {
    return NULL;
  }

  // Scan host features for URID map
  // clang-format off
  const char*  missing = lv2_features_query(
    features,
    LV2_LOG__log,  &self->logger.log, false,
    LV2_URID__map, &self->map,        true,
    NULL);
  // clang-format on

  lv2_log_logger_set_map(&self->logger, self->map);
  if (missing) {
    lv2_log_error(&self->logger, "Missing feature <%s>\n", missing);
    free(self);
    return NULL;
  }

  

  map_chn2path_uris(self->map, &self->uris);

  return (LV2_Handle)self;
}

static void
cleanup(LV2_Handle instance)
{
  free(instance);
}

static void
run(LV2_Handle instance, uint32_t sample_count)
{
  Chn2path*     self = (Chn2path*)instance;
  Chn2pathURIs* uris = &self->uris;
  
  const uint32_t out_capacity = self->out_port[0]->atom.size;

  for (int i=0;i<4;i++)  {
    if (*self->chnz[i] >=0 && *self->chnz[i] <16 ) 
    {
    lv2_atom_sequence_clear(self->out_port[i]);
    self->out_port[i]->atom.type = self->in_port->atom.type;
    }
  }
  // Read incoming events
  LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev) {
    if (ev->body.type == uris->midi_Event) {
      const uint8_t* const msg = (const uint8_t*)(ev + 1);
      switch (lv2_midi_message_type(msg)) {
      case LV2_MIDI_MSG_NOTE_ON:
      case LV2_MIDI_MSG_NOTE_OFF:
        // Forward note to output
        //lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
          {
           
          const uint8_t incoming_channel =  msg[0] & 15; // Channel number
          for(int i=0; i<4; i++)
          {
            if (*self->chnz[i] <0 ) continue;
            if (incoming_channel == (uint8_t) *self->chnz[i])
            {
              lv2_atom_sequence_append_event(
                  self->out_port[i], out_capacity, ev);
              } 
            }
            break; 
          }
      default:
        break;
      }
    }
  }
}



static const void*
extension_data(const char* uri)
{
  return NULL;
}

static const LV2_Descriptor descriptor = {CHN2PATH_URI,
                                          instantiate,
                                          connect_port,
                                          NULL, // activate,
                                          run,
                                          NULL, // deactivate,
                                          cleanup,
                                          extension_data};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  return index == 0 ? &descriptor : NULL;
}