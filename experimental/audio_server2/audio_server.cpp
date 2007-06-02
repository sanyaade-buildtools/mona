#include "audio_server.h"
#include "audio_driver.h"
#include "servers/audio.h"
#include <stdlib.h>
#include <monapi/Stream.h>
#include <monapi/Message.h>

AudioServer audio_server_new()
{
	struct audio_server *ret;
	ret = (struct audio_server*)calloc(1, sizeof(struct audio_server));
	if( ret == NULL ) return NULL;
	ret->driver = audio_driver_factory("es1370");
	if( ret->driver == NULL )
	{
		free(ret);
		return NULL;
	}
	ret->stream = new MonAPI::Stream;
	if( ret->stream == NULL )
	{
		free(ret->driver);
		free(ret);
		return NULL;
	}
	return (AudioServer)ret;
}

void audio_server_delete(AudioServer o)
{
	struct audio_server *serv;
	serv = (struct audio_server*)o;
	if( serv->device != NULL )
	{
		serv->driver->driver_delete(serv->device);
		serv->device = NULL;
	}
	if( serv->stream != NULL ) delete serv->stream;
}

/* Function: MSG_AUDIO_NEW_CHANNEL
 * To create a new channel.
 * Params:
 * 	type: A type of the channel. It's described by the enum `ChannelType`.
 * Returns: number of the channel, handle of the stream.
 */
int audio_new_channel(AudioServer o, MessageInfo *msg)
{
	if( o->channels != 0 )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
	o->channels = 1;
	MonAPI::Message::reply(msg, 1, o->stream->handle());

	return 0;
}

/* Function: MSG_AUDIO_DELETE_CHANNEL
 * To delete a channel.
 * Params:
 * 	channel: number of a channel.
 * Retruns: None
 */
int audio_delete_channel(AudioServer o, MessageInfo *msg)
{
	if( o->channels == 0 )
	{
		MonAPI::Message::reply(msg, 0);
		return -1;
	}
	o->channels = 0;
	return 0;
}

/* Function: MSG_AUDIO_START
 * To start playing or captureing.
 * Params:
 *	channel: number of a channel.
 * Returns: None
 */
int audio_start(AudioServer o, MessageInfo *msg)
{
	if( o->device == NULL || o->driver == NULL )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
	o->driver->driver_set_render_callback(o->device, &audio_render_callback, o);
	o->driver->driver_start(o->device);
	MonAPI::Message::reply(msg, 0);
	return 0;
}

/* Function: MSG_AUDIO_STOP
 * To stop playing or captureing.
 * Params:
 * 	channel: number of a channel.
 * Returns: None
 */
int audio_stop(AudioServer o, MessageInfo *msg)
{
	if( o->device == NULL || o->driver == NULL )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
	o->driver->driver_stop(o->device);
	MonAPI::Message::reply(msg, 0);
	return 0;
}

/* Function: MSG_AUDIO_SET_FORMAT
 * To set a audio data format.
 * Params:
 * 	channels: number of audio channels
 * 	bits: bits per sample
 * 	rate: samples per second
 * 	byteorder: Byte ordering in sample. But now, It's a little endian only.
 * 	           Therefore you don't have to set this param.
 * Returns: Error Status
 */
int audio_set_format(AudioServer o, MessageInfo *msg)
{
	if( o->device != NULL )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
	o->outputFormat.channels = msg->arg1;
	o->outputFormat.bits = msg->arg2;
	o->outputFormat.sample_rate = msg->arg3;
	o->device = o->driver->driver_new(&o->outputFormat);
	if( o->device == NULL )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
	MonAPI::Message::reply(msg, 0);
	return 0;
}

/* Function: MSG_AUDIO_SERVER_VERSION
 * To get a version number of this server.
 * Prams: None
 * Retruns: version number
 */

/* Function: MSG_AUDIO_KILL
 * To kill this server.
 * Params: None
 * Returns: None
 */

/* Function: MSG_AUDIO_RESET
 * To reest this server.
 * Params: None
 * Returns: None
 */


int audio_render_callback(void *ref, void *buf, size_t size, size_t *wrote)
{
	AudioServer serv = (AudioServer)ref;
	MonAPI::Stream *stream = serv->stream;
	*wrote = stream->read((uint8_t*)buf, (uint32_t)size);
	return OK;
}
