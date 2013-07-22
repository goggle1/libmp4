
#include <stdio.h>
#include <sys/types.h>

#include "Mp4File.h"

void show_usage(char* program_name)
{
	fprintf(stdout, "%s [in.mp4] [out.mp4]\n", program_name);
}

int main(int argc, char* argv[])
{
	int ret = 0;
	
	if(argc < 3)
	{
		show_usage(argv[0]);
		return -1;	
	}
	
	char* in_file = argv[1];
	char* out_file = argv[2];
		
	CMp4File mp4file;
	ret = mp4file.Read(in_file);
	ret = mp4file.Write(out_file);
	
	return ret;
}

