#include <Audio.h>
#include <monapi/Message.h>
#include <stdint.h>
#include "debug.h"

/* A version number is as the BCD. */
static uint32_t major_version = 0x00000000;
static uint32_t minor_version = 0x00000001;

#define NumOfMembers ((int)(sizeof(memberTable)/sizeof(memberTable[0])))

int (ServerCommand::*memberTable[])(MessageInfo*) = {
	&ServerCommand::Nop,
	&ServerCommand::GetServerVersion,
	&ServerCommand::Nop,
	&ServerCommand::AllocateChannel,
	&ServerCommand::PrepareChannel,
	&ServerCommand::Nop,
	&ServerCommand::Nop,
	&ServerCommand::ReleaseChannel,
};

ServerCommand::ServerCommand(Audio *_parent) : parent(_parent)
{
}

ServerCommand::~ServerCommand()
{
}

int ServerCommand::Nop(MessageInfo *msg)
{
	return 0;
}

int ServerCommand::caller(int number, MessageInfo *msg)
{
	if( number >= NumOfMembers ) return -1;
	if( memberTable[number] == NULL ) return -1;
	return (this->*memberTable[number])(msg);
}

int ServerCommand::GetServerVersion(MessageInfo *msg)
{
	/* arg2 = major_version, arg3 = minor_version */
	MonAPI::Message::reply(msg, major_version, minor_version);
	return 0;
}

int ServerCommand::AllocateChannel(MessageInfo *msg)
{
	ch_t ch;
	std::vector<struct driver_desc*>::iterator it;
	it = parent->drivers->begin();
	ch = (*it)->create_channel();
	MonAPI::Message::reply(msg, ch);
	return 0;
}

int ServerCommand::PrepareChannel(MessageInfo *msg)
{
	ch_t ch;
	int rate;
	int bits;
	int ret;
	ch = msg->arg1;
	rate = msg->arg2;
	bits = msg->arg3;
	std::vector<struct driver_desc*>::iterator it;
	it = parent->drivers->begin();
	ret = (*it)->prepare_channel(ch, rate, bits, 1);
	MonAPI::Message::reply(msg, ret);
	return ret;
}

int ServerCommand::ReleaseChannel(MessageInfo *msg)
{
	std::vector<struct driver_desc*>::iterator it;
	it = parent->drivers->begin();
	(*it)->destroy_channel(msg->arg1);
	MonAPI::Message::reply(msg, 0);
	return 0;
}
