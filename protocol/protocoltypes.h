/*
 * a protoType specify:
 * 1)data link type: UPD / TCP
 * 2)encoder - decoder
 *
 * one DataHandler deals with one protoType
 */

#ifndef PROTOCOLTYPES_H
#define PROTOCOLTYPES_H

enum eProtocTypes{
    PROTOC_NONE,
    PROTOC_UDP,  //data:UDP, coding:NC
    PROTOC_TCP   //data:TCP, coding:none
};

#endif // PROTOCOLTYPES_H
