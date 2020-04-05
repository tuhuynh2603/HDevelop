#pragma once

// Note that each port represents 8 bits only
const long PORT_1A			= 1;
const long PORT_1B			= 2;
const long PORT_1C			= 3;

const long PORT_2A			= 4;
const long PORT_2B			= 5;
const long PORT_2C			= 6;

const long PORT_3A			= 7;
const long PORT_3B			= 8;
const long PORT_3C			= 9;

const long PORT_4A			= 10;
const long PORT_4B			= 11;
const long PORT_4C			= 12;

const long IO_MODE_IN		= 1;
const long IO_MODE_OUT		= 2;

const long PORT_1750A		= 1;
const long PORT_1750B		= 2;

enum IO_Interface
{
	REGISTER_CARD,
	RELEASE_CARD,
	CONFIG_PORT,
	SET_INTERRUPT,
	WRITE_DATA,
	WRITE_DATA_BIT,
	READ_DATA,
	IO_RESPONSE,
	CLOSE_IO
};
