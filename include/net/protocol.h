#ifndef PROTOCOL_H
#define PROTOCOL_H

enum PROTOCOL_COMMANDS{
	PC_CONTEST_STATE = 0,
	PC_AUTHENTICATE,
	PC_QDATA,
	PC_ADATA,
	PC_ERROR = 100
};

#endif //PROTOCOL_H