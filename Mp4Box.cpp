
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "Mp4Box.h"

BOX_CONTAINER_T box_container_list[] = 
{
	{BOX_FTYP, false},
	{BOX_PDIN, false},
	{BOX_MOOV, true },
	{BOX_MVHD, false},
	{BOX_TRAK, true },
	{BOX_TKHD, false},
	{BOX_TREF, false},
	{BOX_EDTS, true },
	{BOX_ELST, false},
	{BOX_MDIA, true },
	{BOX_MDHD, false},
	{BOX_HDLR, false},
	{BOX_MINF, true },
	{BOX_VMHD, false},
	{BOX_SMHD, false},
	{BOX_HMHD, false},
	{BOX_NMHD, false},
	{BOX_DINF, true },
	{BOX_DREF, false},
	{BOX_STBL, true },
	{BOX_STSD, false},
	{BOX_STTS, false},
	{BOX_CTTS, false},
	{BOX_STSC, false},
	{BOX_STSZ, false},
	{BOX_STZ2, false},
	{BOX_STCO, false},
	{BOX_CO64, false},
	{BOX_STSS, false},
	{BOX_STSH, false},
	{BOX_PADB, false},
	{BOX_STDP, false},
	{BOX_SDTP, false},
	{BOX_SBGP, false},
	{BOX_SGPD, false},
	{BOX_SUBS, false},
	{BOX_MVEX, true },
	{BOX_MEHD, false},
	{BOX_TREX, false},
	{BOX_IPMC, false},
	{BOX_MOOF, true },
	{BOX_MFHD, false},
	{BOX_TRAF, true },
	{BOX_TFHD, false},
	{BOX_TRUN, false},
	{BOX_SDTP, false},
	{BOX_SBGP, false},
//	{BOX_SUBS, false},
	{BOX_MFRA, true },
	{BOX_TFRA, false},
	{BOX_MFRO, false},
	{BOX_MDAT, false},
	{BOX_FREE, false},
	{BOX_SKIP, true },
	{BOX_UDTA, true },
	{BOX_CPRT, false},
	{BOX_META, true },
	{BOX_HDLR, false},
	{BOX_DINF, true },
	{BOX_DREF, false},
//	{BOX_IPMC, false},
	{BOX_ILOC, false},
	{BOX_IPRO, true },
	{BOX_SINF, true },
	{BOX_FRMA, false},
	{BOX_IMIF, false},
	{BOX_SCHM, false},	
	{BOX_SCHI, false},
	{BOX_IINF, false},
	{BOX_XML , false},	
	{BOX_BXML, false},
	{BOX_PITM, false},
	{BOX_FIIN, true },
	{BOX_PAEN, true },
	{BOX_FPAR, false},
	{BOX_FECR, false},
	{BOX_SEGR, false},
	{BOX_GITN, false},
	{BOX_TSEL, false},
	{BOX_MECO, true },
	{BOX_MERE, false},
};

bool box_is_container(u_int32_t box_type)
{
	unsigned int index = 0;
	for(index=0; index<sizeof(box_container_list)/sizeof(box_container_list[0]); index++)
	{
		if(box_container_list[index].type == box_type)
		{
			return box_container_list[index].is_container;
		}
	}

	return false;
}

bool box_is_fullheader(u_int32_t box_type)
{
	return false;
}


CMp4Box::CMp4Box()
{
	m_filepos	= 0;
	m_size		= 0;
	m_type  	= 0;
	m_version 	= 0;
	memset(m_flags, 0, sizeof(m_flags));
	m_largesize = 0;
	memset(m_usertype, 0, sizeof(m_usertype));
	m_header_len= 0;

	m_is_container = false;
	m_data		= NULL;
	
}

CMp4Box::~CMp4Box()
{
	if(!m_is_container)
	{
		if(m_data != NULL)
		{
			free(m_data);
			m_data = NULL;
		}
	}
	else
	{
		if(m_box_list != NULL)
		{
			deque_release(m_box_list, mp4box_release);
			m_box_list = NULL;
		}
	}
}

bool CMp4Box::IsEmpty()
{
	if(GetType() != 0)
	{
		return false;
	}
	
	return true;
}

int CMp4Box::Read(FILE* filep)
{
	int ret = 0;

	ret = ReadHeaderBase(filep);
	if(ret != 0)
	{
		return ret;
	}

	u_int32_t type = GetType();
	bool is_fullheader = box_is_fullheader(type);
	SetIsFullHeader(is_fullheader);
	if(is_fullheader)
	{
		ret = ReadHeaderFull(filep);
		if(ret != 0)
		{
			return ret;
		}
	}
	
	bool is_container = box_is_container(type);
	SetIsContainer(is_container);

	ret = ReadHeaderExtend(filep);
	if(ret != 0)
	{
		return ret;
	}

	if(GetIsContainer())
	{
		ret = ReadSubBox(filep);		
	}
	else
	{
		ret = ReadData(filep);
	}
	
	if(ret != 0)
	{
		return ret;
	}
	
	return ret;
}

int CMp4Box::ReadHeaderBase(FILE* filep)
{
	int size = 0;
	int ret  = 0;

	m_filepos = ftell(filep);

	size = sizeof(m_size);
	ret = fread(&m_size, 1, size, filep);
	if(ret != size)
	{
		fprintf(stderr, "%s: fread size %d, return %d, errno %d %s\n", 
			__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
		return -1;
	}
	m_size = ntohl(m_size);

	size = sizeof(m_type);
	ret = fread(&m_type, 1, size, filep);
	if(ret != size)
	{
		fprintf(stderr, "%s: fread size %d, return %d, errno %d %s\n", 
			__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
		return -1;
	}

	m_header_len = sizeof(m_size) + sizeof(m_type);

	fprintf(stdout, "%s: file pos 0x%016lX, size 0x%08X, type %c%c%c%c\n", 
			__PRETTY_FUNCTION__, m_filepos, m_size, m_4cc[0], m_4cc[1], m_4cc[2], m_4cc[3]);

	return 0;
}

int CMp4Box::ReadHeaderFull(FILE* filep)
{
	int size = 0;
	int ret  = 0;

	size = sizeof(m_version);
	ret = fread(&m_version, 1, size, filep);
	if(ret != size)
	{
		fprintf(stderr, "%s: fread size %d, return %d, errno %d %s\n", 
			__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
		return -1;
	}
	
	size = sizeof(m_flags);
	ret = fread(&m_flags, 1, size, filep);
	if(ret != size)
	{
		fprintf(stderr, "%s: fread size %d, return %d, errno %d %s\n", 
			__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
		return -1;
	}

	m_header_len += sizeof(m_version);
	m_header_len += sizeof(m_flags);
	
	fprintf(stdout, "%s: file pos 0x%016lX, version %d, flags %c%c%c\n", 
			__PRETTY_FUNCTION__, m_filepos, m_version, m_flags[0], m_flags[1], m_flags[2]);

	return 0;
}


int CMp4Box::ReadHeaderExtend(FILE* filep)
{
	int size = 0;
	int ret  = 0;

	if(m_size == 1)
	{
		// read large_size
		char temp[8];
		size = sizeof(temp);
		ret = fread(&temp, 1, size, filep);
		if(ret != size)
		{
			fprintf(stderr, "%s: fread size %d, return %d, errno %d %s\n", 
				__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
			return -1;
		}

		u_int64_t large_temp = 0;
		for(int index=0; index<8; index++)
		{
			large_temp = temp[index];
			m_largesize = m_largesize || (large_temp<<((8-1-index)*8));
		}

		m_header_len += sizeof(m_largesize);

		fprintf(stdout, "%s: file pos 0x%016lX, size 0x%08X, largesize 0x%16lX\n", 
			__PRETTY_FUNCTION__, m_filepos, m_size, m_largesize);
	}
	
	
	if(m_type == BOX_UUID)
	{
		// read usertype
		size = sizeof(m_usertype);
		ret = fread(&m_usertype, 1, size, filep);
		if(ret != size)
		{
			fprintf(stderr, "%s: fread size %d, return %d, errno %d %s\n", 
				__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
			return -1;
		}

		m_header_len += sizeof(m_usertype);
		
		fprintf(stdout, "%s: file pos 0x%016lX, type %c%c%c%c, usertype %c%c%c%c %c%c%c%c %c%c%c%c %c%c%c%c\n", 
			__PRETTY_FUNCTION__, m_filepos, 
			m_4cc[0], m_4cc[1], m_4cc[2], m_4cc[3], 
			m_usertype[0], m_usertype[1], m_usertype[2], m_usertype[3],
			m_usertype[4], m_usertype[5], m_usertype[6], m_usertype[7],
			m_usertype[8], m_usertype[9], m_usertype[10], m_usertype[11],
			m_usertype[12], m_usertype[13], m_usertype[14], m_usertype[15]);
	}

	return 0;
}


int CMp4Box::ReadData(FILE* filep)
{
	int ret = 0;
	long read_size = 0;
	long data_size = 0;

	if(m_size == 0)
	{
		long now_pos = ftell(filep);		
		fseek(filep, 0, SEEK_END);	
		long end_pos = ftell(filep);
		data_size = end_pos - now_pos;
		fseek(filep, now_pos, SEEK_SET);	
	}
	else if(m_size == 1)
	{
		data_size = m_largesize - m_header_len;
	}
	else
	{	
		data_size = m_size - m_header_len;	
	}

	m_data_size = data_size;
	m_data = (u_int8_t*)malloc(data_size);
	if(m_data == NULL)
	{
		return -1;
	}
	read_size = fread(m_data, 1, data_size, filep);
	if(read_size == data_size)
	{
		return 0;
	}
	else
	{
		return -1;
	}

	return ret;
}

bool CMp4Box::ReadSubBoxDone(FILE* filep)
{
	long data_size = 0;
	if(m_size == 0)
	{
		return false;
	}
	else if(m_size == 1)
	{
		data_size = m_largesize - m_header_len;		
	}
	else
	{	
		data_size = m_size - m_header_len;		
	}

	long now_pos = ftell(filep);
	long read_size = now_pos - m_filepos;
	if(read_size < data_size)
	{
		return false;
	}
	else
	{
		return true;
	}

	// never run here.
	return false;
}

int CMp4Box::ReadSubBox(FILE* filep)
{
	int ret = 0;

	fprintf(stdout, "%s: BEGIN\n", __PRETTY_FUNCTION__);

	while ( !feof( filep) )
	{
		if(ReadSubBoxDone(filep))
		{
			break;
		}
		
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

	fprintf(stdout, "%s: END\n", __PRETTY_FUNCTION__);	
	
	return ret;
}


int CMp4Box::Write(FILE* filep)
{
	int ret = 0;

	fprintf(stdout, "%s: type %c%c%c%c\n", __PRETTY_FUNCTION__, 
		(m_type>>24) & 0x000000FF,
		(m_type>>16) & 0x000000FF,
		(m_type>>8) & 0x000000FF,
		(m_type>>0) & 0x000000FF
		);
	
	ret = WriteHeaderBase(filep);
	if(ret != 0)
	{
		return ret;
	}

	ret = WriteHeaderFull(filep);
	if(ret != 0)
	{
		return ret;
	}

	ret = WriteHeaderExtend(filep);
	if(ret != 0)
	{
		return ret;
	}

	if(GetIsContainer())
	{
		ret = WriteSubBox(filep);		
	}
	else
	{
		ret = WriteData(filep);
	}	
	if(ret != 0)
	{
		return ret;
	}
	
	return 0;
}

int CMp4Box::WriteHeaderBase(FILE* filep)
{
	int size = 0;
	int ret  = 0;

	u_int32_t size_big_endian = htonl(m_size);
	size = sizeof(size_big_endian);
	ret = fwrite(&size_big_endian, 1, size, filep);
	if(ret != size)
	{
		fprintf(stderr, "%s: fwrite size %d, return %d, errno %d %s\n", 
			__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
		return -1;
	}

	size = sizeof(m_type);
	ret = fwrite(&m_type, 1, size, filep);
	if(ret != size)
	{
		fprintf(stderr, "%s: fwrite size %d, return %d, errno %d %s\n", 
			__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
		return -1;
	}

	return 0;
}

int CMp4Box::WriteHeaderFull(FILE* filep)
{
	if(!GetIsFullHeader())
	{
		return 0;
	}
	
	int size = 0;
	int ret  = 0;

	size = sizeof(m_version);
	ret = fwrite(&m_version, 1, size, filep);
	if(ret != size)
	{
		fprintf(stderr, "%s: fwrite size %d, return %d, errno %d %s\n", 
			__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
		return -1;
	}
	
	size = sizeof(m_flags);
	ret = fwrite(&m_flags, 1, size, filep);
	if(ret != size)
	{
		fprintf(stderr, "%s: fread size %d, return %d, errno %d %s\n", 
			__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
		return -1;
	}

	return 0;

}

int CMp4Box::WriteHeaderExtend(FILE* filep)
{
	int size = 0;
	int ret  = 0;

	if(m_size == 1)
	{
		// read large_size
		char temp[8];
		size = sizeof(temp);
		
		u_int64_t large_temp = 0;
		for(int index=0; index<8; index++)
		{
			large_temp = (m_largesize >> ((8-1-index)*8))&0x00000000000000FF;
			temp[index] = large_temp;			
		}

		ret = fwrite(&temp, 1, size, filep);
		if(ret != size)
		{
			fprintf(stderr, "%s: fwrite size %d, return %d, errno %d %s\n", 
				__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
			return -1;
		}
	}
	
	
	if(m_type == BOX_UUID)
	{
		// write usertype
		size = sizeof(m_usertype);
		ret = fwrite(&m_usertype, 1, size, filep);
		if(ret != size)
		{
			fprintf(stderr, "%s: fwrite size %d, return %d, errno %d %s\n", 
				__PRETTY_FUNCTION__, size, ret, errno, strerror(errno));
			return -1;
		}
	}

	return 0;
}

int CMp4Box::WriteData(FILE* filep)
{
	long write_size = 0;
	
	write_size = fwrite(m_data, 1, m_data_size, filep);
	if(write_size != m_data_size)
	{
		fprintf(stderr, "%s: fwrite size %ld, return %ld, errno %d %s\n", 
				__PRETTY_FUNCTION__, m_data_size, write_size, errno, strerror(errno));
		return -1;
	}

	return 0;
}


int CMp4Box::WriteSubBox(FILE* filep)
{
	int ret = 0;

	fprintf(stdout, "%s: BEGIN\n", __PRETTY_FUNCTION__);
	
	DEQUE_NODE* nodep = m_box_list;
	while(1)
	{
		CMp4Box* mp4boxp = (CMp4Box*)(nodep->datap);
		if(mp4boxp!=NULL && mp4boxp->m_type!=0)
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

	fprintf(stdout, "%s: END\n", __PRETTY_FUNCTION__);

	return ret;
}




