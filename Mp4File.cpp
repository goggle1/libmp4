#include <string.h>

#include "Mp4File.h"

CMp4File::CMp4File()
{
	m_box_list = NULL;
}

CMp4File::~CMp4File()
{
	if(m_box_list != NULL)
	{
		deque_release(m_box_list, mp4box_release);
		m_box_list = NULL;
	}
}

int CMp4File::Read(char* file_name)
{
	int ret = 0;
	
	FILE* filep = fopen(file_name, "rb");
	if(filep == NULL)
	{
		fprintf(stderr, "%s: can not open file %s\n", __PRETTY_FUNCTION__, file_name);
		return -1;
	}
	
	while ( !feof( filep) )
	{
		DEQUE_NODE* nodep = (DEQUE_NODE*)malloc(sizeof(DEQUE_NODE));
		if(nodep == NULL)
		{
			ret = -1;
			break;
		}
		memset(nodep, 0, sizeof(DEQUE_NODE));
		
		CMp4Box* mp4boxp = new CMp4Box();
		if(mp4boxp == NULL)
		{
			ret = -1;
			break;
		}
		nodep->datap = mp4boxp;
		m_box_list = deque_append(m_box_list, nodep);
		
		ret = mp4boxp->Read(filep);		
		if(ret != 0)
		{
			break;
		}		
	}

	if(filep != NULL)
	{
		fclose(filep);
		filep = NULL;
	}
	
	return ret;
}

int CMp4File::Write(char* file_name)
{
	int ret = 0;

	if(m_box_list == NULL)
	{
		fprintf(stderr, "%s: box list is null\n", __PRETTY_FUNCTION__);
		return -1;
	}
	
	FILE* filep = fopen(file_name, "wb");
	if(filep == NULL)
	{
		fprintf(stderr, "%s: can not open file %s\n", __PRETTY_FUNCTION__, file_name);
		return -1;
	}

	DEQUE_NODE* nodep = m_box_list;
	while(1)
	{
		CMp4Box* mp4boxp = (CMp4Box*)(nodep->datap);
		if(mp4boxp!=NULL && !mp4boxp->IsEmpty())
		{
			ret = mp4boxp->Write(filep);
			if(ret != 0)
			{
				break;
			}
		}
		
		if(nodep->nextp == m_box_list)
		{
			break;
		}
		nodep = nodep->nextp;
	}

	if(filep != NULL)
	{
		fclose(filep);
		filep = NULL;
	}
	
	return ret;
}


