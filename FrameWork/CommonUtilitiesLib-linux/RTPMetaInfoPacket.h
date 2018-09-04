#ifndef __QTRTP_META_INFO_PACKET_H__
#define __QTRTP_META_INFO_PACKET_H__

#include <stdlib.h>
#include "SafeStdLib.h"
#include "OSHeaders.h"
#include "StrPtrLen.h"

class RTPMetaInfoPacket
{
    public:
    
        //
        // RTP Meta Info Fields
        
        enum
        {
            kPacketPosField         = 0, //TW0_CHARS_TO_INT('p', 'p'),
            kTransTimeField         = 1, //TW0_CHARS_TO_INT('t', 't'),
            kFrameTypeField         = 2, //TW0_CHARS_TO_INT('f', 't'),
            kPacketNumField         = 3, //TW0_CHARS_TO_INT('p', 'n'),
            kSeqNumField            = 4, //TW0_CHARS_TO_INT('s', 'q'),
            kMediaDataField         = 5, //TW0_CHARS_TO_INT('m', 'd'),
            
            kIllegalField           = 6,
            kNumFields              = 6
        };
        typedef UInt16 FieldIndex;
        
        //
        // Types
        
        typedef UInt16 FieldName;
        typedef SInt32 FieldID;
        
        //
        // Special field IDs
        enum
        {
            kUncompressed = -1,     // No field ID (not a compressed field)
            kFieldNotUsed = -2      // This field is not being used
        };
        
        //
        // Routine that converts the above enum items into real field names
        static FieldName GetFieldNameForIndex(FieldIndex inIndex) { return kFieldNameMap[inIndex]; }
        static FieldIndex GetFieldIndexForName(FieldName inName);
        
        //
        // Routine that constructs a standard FieldID Array out of a x-RTP-Meta-Info header
        static void ConstructFieldIDArrayFromHeader(StrPtrLen* inHeader, FieldID* ioFieldIDArray);
        
        //
        // Values for the Frame Type Field  
        enum
        {
            kUnknownFrameType   = 0,
            kKeyFrameType       = 1,
            kBFrameType         = 2,
            kPFrameType         = 3
        };
        typedef UInt16 FrameTypeField;
        
        
        //
        // CONSTRUCTOR
        
        RTPMetaInfoPacket() :   fPacketBuffer(NULL),
                                fPacketLen(0),
                                fTransmitTime(0),
                                fFrameType(kUnknownFrameType),
                                fPacketNumber(0),
                                fPacketPosition(0),
                                fMediaDataP(NULL),
                                fMediaDataLen(0),
                                fSeqNum(0)          {}
        ~RTPMetaInfoPacket() {}
        
        //
        // Call this to parse the RTP-Meta-Info packet.
        // Pass in an array of FieldIDs, make sure it is kNumFields in length.
        // This function will use the array as a guide to tell which field IDs in the
        // packet refer to which fields.
        Bool16  ParsePacket(UInt8* inPacketBuffer, UInt32 inPacketLen, FieldID* inFieldIDArray);
        
        //
        // Call this if you would like to rewrite the Meta-Info packet
        // as a normal RTP packet (strip off the extensions). Note that
        // this will overwrite data in the buffer!
        // Returns a pointer to the new RTP packet, and its length
        UInt8*          MakeRTPPacket(UInt32* outPacketLen);
        
        //
        // Field Accessors
        SInt64          GetTransmitTime()       { return fTransmitTime; }
        FrameTypeField  GetFrameType()          { return fFrameType; }
        UInt64          GetPacketNumber()       { return fPacketNumber; }
        UInt64          GetPacketPosition()     { return fPacketPosition; }
        UInt8*          GetMediaDataP()         { return fMediaDataP; }
        UInt32          GetMediaDataLen()       { return fMediaDataLen; }
        UInt16          GetSeqNum()             { return fSeqNum; }
    
    private:
    
        UInt8*          fPacketBuffer;
        UInt32          fPacketLen;
        
        SInt64          fTransmitTime;
        FrameTypeField  fFrameType;
        UInt64          fPacketNumber;
        UInt64          fPacketPosition;
        UInt8*          fMediaDataP;
        UInt32          fMediaDataLen;
        UInt16          fSeqNum;
        
        static const FieldName kFieldNameMap[];
        static const UInt32 kFieldLengthValidator[];
};

#endif // __QTRTP_META_INFO_PACKET_H__
