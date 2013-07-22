
#ifndef __MP4BOX_H__
#define __MP4BOX_H__

#include <stdio.h>
#include <sys/types.h>

#include "deque.h"

// http://blog.csdn.net/pirateleo/article/details/7061452
/*
aligned(8) class Box (unsigned int(32) boxtype,optional unsigned int(8)[16] extended_type) 
{ 
    unsigned int(32) size; 
    unsigned int(32) type = boxtype; 
    if (size==1) 
    { 
        unsigned int(64) largesize; 
    } 
    else if (size==0) 
    { 
        // box extends to end of file 
    } 
    if (boxtype==¡®uuid¡¯) 
    { 
        unsigned int(8)[16] usertype = extended_type; 
    } 
}
 */

/*
 #define FOURCC(a,b,c,d) \
( ((u_int32_t)d) | ( ((u_int32_t)c) << 8 ) | ( ((u_int32_t)b) << 16 ) | ( ((u_int32_t)a) << 24 ) )
*/

 #define FOURCC(a,b,c,d) \
( ((u_int32_t)a) | ( ((u_int32_t)b) << 8 ) | ( ((u_int32_t)c) << 16 ) | ( ((u_int32_t)d) << 24 ) )

// all the mp4 box type
// http://blog.csdn.net/pirateleo/article/details/7061452
#define BOX_FTYP FOURCC('f', 't', 'y', 'p')
#define BOX_PDIN FOURCC('p', 'd', 'i', 'n')
#define BOX_MOOV FOURCC('m', 'o', 'o', 'v')
#define BOX_MVHD FOURCC('m', 'v', 'h', 'd')
#define BOX_TRAK FOURCC('t', 'r', 'a', 'k')
#define BOX_TKHD FOURCC('t', 'k', 'h', 'd')
#define BOX_TREF FOURCC('t', 'r', 'e', 'f')
#define BOX_EDTS FOURCC('e', 'd', 't', 's')
#define BOX_ELST FOURCC('e', 'l', 's', 't')
#define BOX_MDIA FOURCC('m', 'd', 'i', 'a')
#define BOX_MDHD FOURCC('m', 'd', 'h', 'd')
#define BOX_HDLR FOURCC('h', 'd', 'l', 'r')
#define BOX_MINF FOURCC('m', 'i', 'n', 'f')
#define BOX_VMHD FOURCC('v', 'm', 'h', 'd')
#define BOX_SMHD FOURCC('s', 'm', 'h', 'd')
#define BOX_HMHD FOURCC('h', 'm', 'h', 'd')
#define BOX_NMHD FOURCC('n', 'm', 'h', 'd')
#define BOX_DINF FOURCC('d', 'i', 'n', 'f')
#define BOX_DREF FOURCC('d', 'r', 'e', 'f')
#define BOX_STBL FOURCC('s', 't', 'b', 'l')
#define BOX_STSD FOURCC('s', 't', 's', 'd')
#define BOX_STTS FOURCC('s', 't', 't', 's')
#define BOX_CTTS FOURCC('c', 't', 't', 's')
#define BOX_STSC FOURCC('s', 't', 's', 'c')
#define BOX_STSZ FOURCC('s', 't', 's', 'z')
#define BOX_STZ2 FOURCC('s', 't', 'z', '2')
#define BOX_STCO FOURCC('s', 't', 'c', 'o')
#define BOX_CO64 FOURCC('c', 'o', '6', '4')
#define BOX_STSS FOURCC('s', 't', 's', 's')
#define BOX_STSH FOURCC('s', 't', 's', 'h')
#define BOX_PADB FOURCC('p', 'a', 'd', 'b')
#define BOX_STDP FOURCC('s', 't', 'd', 'p')
#define BOX_SDTP FOURCC('s', 'd', 't', 'p')
#define BOX_SBGP FOURCC('s', 'b', 'g', 'p')
#define BOX_SGPD FOURCC('s', 'g', 'p', 'd')
#define BOX_SUBS FOURCC('s', 'u', 'b', 's')
#define BOX_MVEX FOURCC('m', 'v', 'e', 'x')
#define BOX_MEHD FOURCC('m', 'e', 'h', 'd')
#define BOX_TREX FOURCC('t', 'r', 'e', 'x')
#define BOX_IPMC FOURCC('i', 'p', 'm', 'c')
#define BOX_MOOF FOURCC('m', 'o', 'o', 'f')
#define BOX_MFHD FOURCC('m', 'f', 'h', 'd')
#define BOX_TRAF FOURCC('t', 'r', 'a', 'f')
#define BOX_TFHD FOURCC('t', 'f', 'h', 'd')
#define BOX_TRUN FOURCC('t', 'r', 'u', 'n')
#define BOX_SDTP FOURCC('s', 'd', 't', 'p')
#define BOX_SBGP FOURCC('s', 'b', 'g', 'p')
//#define BOX_SUBS FOURCC('s', 'u', 'b', 's')
#define BOX_MFRA FOURCC('m', 'f', 'r', 'a')
#define BOX_TFRA FOURCC('t', 'f', 'r', 'a')
#define BOX_MFRO FOURCC('m', 'f', 'r', 'o')
#define BOX_MDAT FOURCC('m', 'd', 'a', 't')
#define BOX_FREE FOURCC('f', 'r', 'e', 'e')
#define BOX_SKIP FOURCC('s', 'k', 'i', 'p')
#define BOX_UDTA FOURCC('u', 'd', 't', 'a')
#define BOX_CPRT FOURCC('c', 'p', 'r', 't')
#define BOX_META FOURCC('m', 'e', 't', 'a')
//#define BOX_HDLR FOURCC('h', 'd', 'l', 'r')
#define BOX_DINF FOURCC('d', 'i', 'n', 'f')
#define BOX_DREF FOURCC('d', 'r', 'e', 'f')
//#define BOX_IPMC FOURCC('i', 'p', 'm', 'c')
#define BOX_ILOC FOURCC('i', 'l', 'o', 'c')
#define BOX_IPRO FOURCC('i', 'p', 'r', 'o')
#define BOX_SINF FOURCC('s', 'i', 'n', 'f')
#define BOX_FRMA FOURCC('f', 'r', 'm', 'a')
#define BOX_IMIF FOURCC('i', 'm', 'i', 'f')
#define BOX_SCHM FOURCC('s', 'c', 'h', 'm')
#define BOX_SCHI FOURCC('s', 'c', 'h', 'i')
#define BOX_IINF FOURCC('i', 'i', 'n', 'f')
#define BOX_XML  FOURCC('x', 'm', 'l', '\0')
#define BOX_BXML FOURCC('b', 'x', 'm', 'l')
#define BOX_PITM FOURCC('p', 'i', 't', 'm')
#define BOX_FIIN FOURCC('f', 'i', 'i', 'n')
#define BOX_PAEN FOURCC('p', 'a', 'e', 'n')
#define BOX_FPAR FOURCC('f', 'p', 'a', 'r')
#define BOX_FECR FOURCC('f', 'e', 'c', 'r')
#define BOX_SEGR FOURCC('s', 'e', 'g', 'r')
#define BOX_GITN FOURCC('g', 'i', 't', 'n')
#define BOX_TSEL FOURCC('t', 's', 'e', 'l')
#define BOX_MECO FOURCC('m', 'e', 'c', 'o')
#define BOX_MERE FOURCC('m', 'e', 'r', 'e')

#define BOX_UUID FOURCC('u', 'u', 'i', 'd')

typedef struct box_fullheader_t
{
	u_int32_t 	type;
	bool 		is_fullheader;
} BOX_FULLHEADER_T;


typedef struct box_container_t
{
	u_int32_t 	type;
	bool 		is_container;
} BOX_CONTAINER_T;

bool box_is_fullheader(u_int32_t box_type);
bool box_is_container(u_int32_t box_type);

class CMp4Box
{
	public:
		CMp4Box();
		~CMp4Box();
		int  Read(FILE* filep);
	 	int  Write(FILE* filep);
	 	bool IsEmpty();
	 	
	protected:
		int  ReadHeaderBase(FILE* filep);
		u_int32_t GetType() { return m_type; };
		void SetIsFullHeader(bool is_fullheader) { m_is_fullheader = is_fullheader;};
		bool GetIsFullHeader() { return m_is_fullheader; };
		int  ReadHeaderFull(FILE* filep);
		int  ReadHeaderExtend(FILE* filep);
		
		void SetIsContainer(bool is_container) { m_is_container = is_container;};
		bool GetIsContainer() { return m_is_container; };
		int  ReadSubBox(FILE* filep);
		bool ReadSubBoxDone(FILE* filep);
		int  ReadData(FILE* filep);

		int  WriteHeaderBase(FILE* filep);
		int  WriteHeaderFull(FILE* filep);
		int  WriteHeaderExtend(FILE* filep);
		int  WriteSubBox(FILE* filep);
		int  WriteData(FILE* filep);
	protected:
		// for debug.
		long	  m_filepos;
		
		// base header
		u_int32_t m_size;
		union
		{
			u_int32_t m_type;
			char  	  m_4cc[4];
		};
		// full header.
		bool	  m_is_fullheader;
		u_int8_t  m_version;
		u_int8_t  m_flags[3];
		// extended header.
		u_int64_t m_largesize;
		u_int8_t  m_usertype[16];
		// header_len = base_header_len + full_header_len + extended_header_len
		int 	  m_header_len;
		
		// data
		bool	  m_is_container;
		union
		{
			u_int8_t* m_data;
			DEQUE_NODE* m_box_list;
		};
		long	  m_data_size;
		
};

inline void mp4box_release(void* datap)
{
	CMp4Box* boxp = (CMp4Box*)datap;
	if(boxp != NULL)
	{
		delete boxp;
	}
}



#endif
