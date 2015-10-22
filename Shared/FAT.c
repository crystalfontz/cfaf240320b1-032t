
#include"FAT.h"

DWORD FirstDirClust;    //first directory cluster
DWORD FirstDataSector;	// The first sector number of data
WORD BytesPerSector;	// Bytes per sector
WORD FATsectors;		// The amount sector a FAT occupied
WORD SectorsPerClust;	// Sector per cluster
DWORD FirstFATSector;	// The first FAT sector
DWORD FirstDirSector;	// The first Dir sector
DWORD RootDirSectors;	// The sector number a Root dir occupied 
DWORD RootDirCount;		// The count of directory in root dir
BYTE FAT32_Enable;

BYTE (* FAT_ReadSector)(DWORD,BYTE *);
BYTE (* FAT_WriteSector)(DWORD,BYTE *);

//function pointer to the sd card read & write single block
//write sector are not use in this viewer
BYTE (* FAT_ReadSector)(DWORD sector, BYTE * buffer)=MMC_SD_ReadSingleBlock;	//device read
BYTE (* FAT_WriteSector)(DWORD sector, BYTE * buffer)=MMC_SD_WriteSingleBlock;	//device write

//temporarily buffer for file information
struct FileInfoStruct FileInfo;

//Initialization of FAT, need initialized SD first
unsigned char FAT_Init()
{
	struct bootsector710 *bs  = 0;
	struct bpb710        *bpb = 0;
//	struct partsector    *ps  = 0;
	struct partrecord    *pr  = 0;

	BYTE buffer[512];
	DWORD hidsec=0;
	DWORD Capacity;

	Capacity = MMC_SD_ReadCapacity();
	if(Capacity<0xff)
		return 1;

	if(FAT_ReadSector(0,buffer))
		return 1;
	bs = (struct bootsector710 *)buffer;

	//first partition
	pr = (struct partrecord *)((struct partsector *)buffer)->psPart;
	//the hidden sectors
	hidsec = pr->prStartLBA;
	if(hidsec >= Capacity/512)
	{
		hidsec = 0;
	}
	else 
	{
		if(FAT_ReadSector(pr->prStartLBA,buffer))
			return 1;	//read the bpb sector
		bs = (struct bootsector710 *)buffer;
		if(bs->bsJump[0]!=0xE9 && bs->bsJump[0]!=0xEB)
		{
			hidsec = 0;
			if(FAT_ReadSector(0,buffer))
				return 1;	//read the bpb sector
			bs = (struct bootsector710 *)buffer;	
		}
	}

	//deal with card that have no boot sector
	if(bs->bsJump[0]!=0xE9 && bs->bsJump[0]!=0xEB)
	{
		return 1;
	}
	bpb = (struct bpb710 *)bs->bsBPB;

	
	//determine the FAT type  this does not support FAT12
	if(bpb->bpbFATsecs)
	{
		FAT32_Enable = 0;	//FAT16
		//The sectors number occupied by one fat table
		FATsectors = bpb->bpbFATsecs;
		FirstDirClust = 2;
	}
	else
	{
		FAT32_Enable = 1;	//FAT32
		//The sectors number occupied by one fat table
		FATsectors = bpb->bpbBigFATsecs;
		FirstDirClust = bpb->bpbRootClust;
	}

	BytesPerSector	= bpb->bpbBytesPerSec;
	SectorsPerClust	= (BYTE)bpb->bpbSecPerClust;
	FirstFATSector	= bpb->bpbResSectors+hidsec;
	RootDirCount	= bpb->bpbRootDirEnts;
	RootDirSectors	= (RootDirCount*32)>>9;
	FirstDirSector	= FirstFATSector+bpb->bpbFATs*FATsectors;
	FirstDataSector	= FirstDirSector+RootDirSectors;
	return 0;
}

//read one sector of one cluster, parameter part indicate which sector
unsigned char FAT_LoadPartCluster(unsigned long cluster,unsigned part,BYTE * buffer)
{
	DWORD sector;
	//calculate the actual sector number
	sector=FirstDataSector+(DWORD)(cluster-2)*(DWORD)SectorsPerClust;
	if(FAT_ReadSector(sector+part,buffer))
		return 1;
	else
		return 0;
}

//Return the cluster number of next cluster of file
//Suitable for system which has limited RAM
unsigned long FAT_NextCluster(unsigned long cluster)
{
	BYTE buffer[512];
	DWORD sector;
	DWORD offset;
	if(FAT32_Enable)
		offset = cluster/128;
	else
		offset = cluster/256;

	if(cluster<2)
		return 0x0ffffff8;
	//calculate the actual sector
	sector=FirstFATSector+offset;
	if(FAT_ReadSector(sector,buffer))
	{
		//read fat table / return 0xfff8 when error occurred
		return 0x0ffffff8;
	}

	if(FAT32_Enable)
	{
		//find the position
		offset=cluster%128;
		sector=((unsigned long *)buffer)[offset];	
	}
	else
	{
		//find the position
		offset=cluster%256;
		sector=((unsigned int *)buffer)[offset];
	}
	//return the cluster number
	return (unsigned long)sector;
}

//Find a item in the directory which specify by the parameter "cluster"
//Return the start cluster number
unsigned int FAT_FindItem(unsigned long cluster, BYTE *name, struct FileInfoStruct *FileInfo)
{
	BYTE *buffer;
	DWORD tempclust;
	DWORD sector;
	unsigned char cnt;
	unsigned int offset;
	unsigned char i;
	struct direntry *item = 0;

	// root directory
	if((cluster==0) && (FAT32_Enable == 0))
	{
		//try to allocate memory
		buffer=malloc(512);

		//if failed, bounce out
		if(buffer==0)
			return 1;

		for(cnt=0;cnt<RootDirSectors;cnt++)
		{
			if(FAT_ReadSector(FirstDirSector+cnt,buffer))
			{
				free(buffer);
				return 1;
			}
			for(offset=0;offset<512;offset+=32)
			{
				item=(struct direntry *)(&buffer[offset]);
				if((item->deName[0] != 0x00) & (item->deName[0] != 0xe5) & (item->deAttributes != 0x0f))
				{
					for(i=0;i<11;i++)
					{
						if(buffer[offset+i]!=name[i])
							break;
					}
					if(i==11)
					{
						//return the parameter of the item
						FileInfo->StartCluster = item->deStartCluster + (((unsigned long)item->deHighClust)<<16);//don't care
						FileInfo->Size         = item->deFileSize;
						FileInfo->Attr         = item->deAttributes;
						FileInfo->Sector       = FirstDirSector+cnt;
						FileInfo->Offset       = offset;
						free(buffer);
						return 0;
					}
				}
			}
		}
		free(buffer);	//release
	}
	else	//other folders
	{
		tempclust=cluster;
		while(1)
		{
			//calculate the actual sector number
			sector=FirstDataSector+(DWORD)(tempclust-2)*(DWORD)SectorsPerClust;

			//try to allocate memory
			buffer=malloc(512);

			//if failed, bounce out
			if(buffer==0)
				return 1;

			for(cnt=0;cnt<SectorsPerClust;cnt++)
			{
				if(FAT_ReadSector(sector+cnt,buffer))
				{
					free(buffer);
					return 1;
				}
				for(offset=0;offset<512;offset+=32)
				{
					item=(struct direntry *)(&buffer[offset]);
					if((item->deName[0] != 0x00) & (item->deName[0] != 0xe5) & (item->deAttributes != 0x0f))
					{
						for(i=0;i<11;i++)
						{
							if(buffer[offset+i]!=name[i])
								break;
						}
						if(i==11)
						{
							FileInfo->StartCluster = item->deStartCluster + (((unsigned long)item->deHighClust)<<16);	//don't care
							FileInfo->Size         = item->deFileSize;
							FileInfo->Attr         = item->deAttributes;
							FileInfo->Sector       = sector+cnt;
							FileInfo->Offset       = offset;
							free(buffer);
							return 0;
						}
					}
				}
			}
			free(buffer);	//release
			tempclust=FAT_NextCluster(tempclust);	//next cluster
			if(tempclust == 0x0fffffff || tempclust == 0x0ffffff8 || (FAT32_Enable == 0 && tempclust == 0xffff))
				break;
		}
	}
	return 1;
}

// find a directory with the given path
unsigned long FAT_OpenDir(BYTE * dir)
{
	BYTE name[11];
	BYTE *p=dir;
	BYTE deep=0;
	BYTE i,j;
	DWORD cluster=0;
	if(FAT32_Enable)
	{
		cluster = FirstDirClust;
	}

	//invalid path
	if(*p != '\\')
		return 1;

	while(*p)
	{
		if(*p == '\\')
		{
			deep++;
		}
		p++;
	}
	p=dir;
	for(i=0;i<deep-1;i++)
	{
		p++;
		for(j=0;j<11;j++)name[j]=0x20;
		j=0;
		while(*p != '\\')
		{
			if((*p) >= 'a' && (*p) <= 'z')
				name[j] = (*p++)-0x20;
			else
				name[j] = *p++;
			j++;
		}
		if(FAT_FindItem(cluster,name, &FileInfo))
		{
			//find the directory
			return 1;
		}
		cluster = FileInfo.StartCluster;
	}
	p++;
	for(j=0;j<11;j++)name[j]=0x20;
	j=0;
	while(*p)
	{
		if(*p>='a' && *p<='z')
			name[j]=(*p++)-0x20;
		else
			name[j]=*p++;
		j++;
	}
	if(j == 0)
		return cluster;
		
	if(FAT_FindItem(cluster,name, &FileInfo))
	{
		//find the final directory
		return 1;
	}
	cluster = FileInfo.StartCluster;
	return cluster;
}

//copy item
void CopyDirEntryItem(struct direntry *Desti,struct direntry *Source)
{
	BYTE i;
	for(i=0;i<8;i++)
		Desti->deName[i] = Source->deName[i];
	for(i=0;i<3;i++)
	{
		Desti->deExtension[i] = Source->deExtension[i];
	}
	Desti->deAttributes = Source->deAttributes;
	Desti->deLowerCase = Source->deLowerCase;
	Desti->deCHundredth = Source->deCHundredth;
	for(i=0;i<2;i++)
	{
		Desti->deCTime[i] = Source->deCTime[i];
	}
	for(i=0;i<2;i++)
	{
		Desti->deCDate[i] = Source->deCDate[i];
	}
	for(i=0;i<2;i++)
	{
		Desti->deADate[i] = Source->deADate[i];
	}
	Desti->deHighClust = Source->deHighClust;
	for(i=0;i<2;i++)
	{
		Desti->deMTime[i] = Source->deMTime[i];
	}
	for(i=0;i<2;i++)
	{
		Desti->deMDate[i] = Source->deMDate[i];
	}
	Desti->deStartCluster = Source->deStartCluster;
	Desti->deFileSize = Source->deFileSize;
}

//search the file , when *count = 0 it will bring the number of files, when *cout != 0 the *FileInfo will bring the infomation of the file
BYTE Search(BYTE *dir,struct direntry *FileInfo,WORD *Count)
{
	BYTE *buffer;
	//BYTE buff[3];
	DWORD sector;
	DWORD cluster;
	DWORD tempclust;
	unsigned char cnt;
	unsigned int offset;
	unsigned int i=0;
	struct direntry *item = 0;
	cluster = FAT_OpenDir(dir);
	if(cluster == 1)
		return 1;
	
	// root directory
	if(cluster==0 && FAT32_Enable==0)
	{
		//try to allocate memory
		buffer=malloc(512);

		//if failed, bounce out
		if(buffer==0)
			return 1;

		for(cnt=0;cnt<RootDirSectors;cnt++)
		{
			if(FAT_ReadSector(FirstDirSector+cnt,buffer))
			{
				free(buffer);
				return 1;
			}
			for(offset=0;offset<512;offset+=32)
			{
				//pointer conversion
				item=(struct direntry *)(&buffer[offset]);
				
				//find a valid item and display it
				if((item->deName[0] != '.') & (item->deName[0] != 0x00) & (item->deName[0] != 0xe5) & (item->deAttributes != 0x0f))
				{
					if((item->deExtension[0] == 'B')&&(item->deExtension[1] == 'I')&&(item->deExtension[2] == 'N'))
					{
						CopyDirEntryItem(FileInfo,item);
						i++;
						if(i==*Count)
						{
							free(buffer);
							return 0;
						}
					}
				}
			}
		}
		//release
		free(buffer);
	}
	else //other folders
	{
		tempclust=cluster;
		while(1)
		{
			sector=FirstDataSector+(DWORD)(tempclust-2)*(DWORD)SectorsPerClust;//calculate the actual sector number
			
			//try to allocate memory
			buffer=malloc(512);

			//if failed, bounce out
			if(buffer==0)
				return 1;
			
			for(cnt=0;cnt<SectorsPerClust;cnt++)
			{
				if(FAT_ReadSector(sector+cnt,buffer))
				{
					free(buffer);
					return 1;
				}
				for(offset=0;offset<512;offset+=32)
				{
					item=(struct direntry *)(&buffer[offset]);
					if((item->deName[0] != '.') & (item->deName[0] != 0x00) & (item->deName[0] != 0xe5) & (item->deAttributes != 0x0f))
					{
						if((item->deExtension[0] == 'B')&&(item->deExtension[1] == 'I')&&(item->deExtension[2] == 'N'))
						{
							CopyDirEntryItem(FileInfo,item);
							i++;
							if(i==*Count)
							{
								free(buffer);
								return 0;
							}	
						}
					}
				}
			}
			
			//release
			free(buffer);
			
			//next cluster
			tempclust=FAT_NextCluster(tempclust);
			
			if(tempclust == 0x0fffffff || tempclust == 0x0ffffff8 || (FAT32_Enable == 0 && tempclust == 0xffff))
				break;
		}
	}
	
	if(*Count==0)
	{
		*Count=i;
	}
	
	return 0;	
}
