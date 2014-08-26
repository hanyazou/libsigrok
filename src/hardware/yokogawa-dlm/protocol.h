/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2014 abraxa (Soeren Apel) <soeren@apelpie.net>
 * Based on the Hameg HMO driver by poljar (Damir Jelić) <poljarinho@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBSIGROK_HARDWARE_YOKOGAWA_DLM_PROTOCOL_H
#define LIBSIGROK_HARDWARE_YOKOGAWA_DLM_PROTOCOL_H

#include <glib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "libsigrok.h"
#include "libsigrok-internal.h"
#include "protocol_wrappers.h"

#define LOG_PREFIX "yokogawa-dlm"
#define MAX_INSTRUMENT_VERSIONS 4

/* See Communication Interface User's Manual on p. 268 (:WAVeform:ALL:SEND?). */
#define DLM_MAX_FRAME_LENGTH (12500)
/* See Communication Interface User's Manual on p. 269 (:WAVeform:SEND?). */
#define DLM_DIVISION_FOR_WORD_FORMAT (3200)
#define DLM_DIVISION_FOR_BYTE_FORMAT (12.5)


enum trigger_slopes {
	SLOPE_POSITIVE,
	SLOPE_NEGATIVE
};

struct scope_config {
	const char *model_id[MAX_INSTRUMENT_VERSIONS];
	const char *model_name[MAX_INSTRUMENT_VERSIONS];
	const uint8_t analog_channels;
	const uint8_t digital_channels;
	const uint8_t pods;

	const char *(*analog_names)[];
	const char *(*digital_names)[];

	const int32_t (*hw_caps)[];
	const uint8_t num_hwcaps;

	const int32_t (*analog_hwcaps)[];
	const uint8_t num_analog_hwcaps;

	const char *(*coupling_options)[];
	const uint8_t num_coupling_options;

	const char *(*trigger_sources)[];
	const uint8_t num_trigger_sources;

	const char *(*trigger_slopes)[];

	const uint64_t (*timebases)[][2];
	const uint8_t num_timebases;

	const uint64_t (*vdivs)[][2];
	const uint8_t num_vdivs;

	const uint8_t num_xdivs;
	const uint8_t num_ydivs;

	const char *(*scpi_dialect)[];
};

struct analog_channel_state {
	int coupling;

	int vdiv;
	float vertical_offset, waveform_range, waveform_offset;

	gboolean state;
};

struct scope_state {
	struct analog_channel_state *analog_states;
	gboolean *digital_states;
	gboolean *pod_states;

	int timebase;
	float horiz_triggerpos;

	int trigger_source;
	int trigger_slope;
	uint64_t sample_rate;
};

/** Private, per-device-instance driver context. */
struct dev_context {
	void *model_config;
	void *model_state;

	struct sr_channel_group **analog_groups;
	struct sr_channel_group **digital_groups;

	GSList *enabled_channels;
	GSList *current_channel;
	uint64_t num_frames;

	uint64_t frame_limit;
};

/*--- api.c -----------------------------------------------------------------*/
SR_PRIV int dlm_data_request(const struct sr_dev_inst *sdi);

/*--- protocol.c ------------------------------------------------------------*/
SR_PRIV int dlm_model_get(char *model_id, char **model_name, int *model_index);
SR_PRIV int dlm_device_init(struct sr_dev_inst *sdi, int model_index);
SR_PRIV int dlm_data_receive(int fd, int revents, void *cb_data);

SR_PRIV void dlm_scope_state_destroy(struct scope_state *state);
SR_PRIV int dlm_scope_state_query(struct sr_dev_inst *sdi);
SR_PRIV int dlm_sample_rate_query(const struct sr_dev_inst *sdi);

#endif