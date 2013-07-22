
#ifndef __MP4FILE_H__
#define __MP4FILE_H__

#include "deque.h"
#include "Mp4Box.h"

class CMp4File
{
	public:
		CMp4File();
		~CMp4File();
		int Read(char* file);
		int Write(char* file);		
		
	protected:		
		DEQUE_NODE* m_box_list;
};

#endif
