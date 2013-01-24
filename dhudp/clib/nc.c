#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "nc.h"
#include "time.h"
#include "math.h"
#include "randomlib.h"

int preNC()
{
    return initMulDivTab(MUL_DIV_TABLE_FN);
}

int initNC(NCCB* cb,char* fName,ulong blockSize,ulong blockNum,ulong d)
{
	FILE* f;
	ulong flen,i,t;
	strcpy(cb->fileName,fName);
	if((f=fopen(cb->fileName,"rb"))==NULL)
		return cFALSE;
	cb->blockSize=blockSize;
	cb->blockNum=blockNum;
	cb->d=d;
	cb->holdNum=0;
    //如果整个文件分为n块，则每块就附加上n字节的系数
    cb->size = cb->blockNum + cb->blockSize;
	fseek(f,0,SEEK_END);
	flen=ftell(f);
	fseek(f,0,SEEK_SET);

	cb->nonZero=(long*)malloc(sizeof(long)*cb->blockNum);
    for(i=0;i<cb->blockNum;i++)
        cb->nonZero[i]=-1;

	cb->cMatrix=(FIELD**)malloc(sizeof(FIELD*)*cb->blockNum);
    for(i=0;i<flen/cb->size;i++)    //对已经收到的部分,从每块里读取系数
	{
        //每块带的系数阵，n行8列(bit)，即n字节
        FIELD* block=(FIELD*)malloc(sizeof(FIELD)*cb->blockNum);
        fseek(f,i*cb->size,SEEK_SET);   //系数在编码块头部, i*块大小作为偏移
        fread(block,cb->blockNum,1,f);  //read coeff, which is save in the file
        if(appendM(cb,block)==cFALSE)    	//append coeff matrix from this block
            return cFALSE;   //blocks in already have part must be nonlinear correlation
	}
	fclose(f);
	t=time(NULL);
	RandomInitialise((t/30000)%30000,t%30000);	//set rand seed for function encode()
	return cTRUE;
}

FIELD* encode(NCCB* cb,ulong from,ulong to)
{
	FILE* f;
	ulong i;
    FIELD *block,*temp;
	
	if((f=fopen(cb->fileName,"rb"))==NULL)
		return NULL;
    if(from > cb->holdNum)  //from block (include)
		from=cb->holdNum;
    if(to > cb->holdNum)    //to block, (exclude)
		to=cb->holdNum;
    temp=(FIELD*)malloc(sizeof(FIELD)*cb->size);    //temp 1 block
    block=(FIELD*)malloc(sizeof(FIELD)*cb->size);   //target 1 block (cb->size bytes)
	memset(block,0,sizeof(FIELD)*cb->size);

    /* generate a radom coefficient matrix and encode (from, to) blocks into a single block
     * block = e1*srcBlock(from+0) + e2*srcBlock(from+1) +  ... + e(to-from)*srcBlock(to-1)
     * parameter d (together with total source block number t) is used to control encoding density:
     * e.g. if t =256，d =10，coeff matrix has a Probability of 0.99991 being enough to resolve original data
    */
    for(i=from; i<to; i++)
	{
        double rd=RandomUniform();  //between 0,1
        if( rd < ( log((double)cb->blockNum) + cb->d ) / (to-from) )	//exp=log(blockNum) + d
        {   // coefficient > 0 ( Probability: p = Ln(k) + d / k )
			fseek(f,i*cb->size,SEEK_SET);
			fread(temp,cb->size,1,f);						//load data
            cMulvAdd(block,temp,cb->size,(FIELD)RandomInt(0,255)); // block += temp*random_int
		}
        else{
            //do nothing = let this block coefficient is 0 ( Probability = 1 - p )
        }

	}
	free(temp);
	fclose(f);
	return block;		/*should be free later*/
}

/* simply return block i = from */
FIELD* encodeIdentity(NCCB* cb, ulong from)
{
    FILE* f;
    FIELD *block;

    if((f=fopen(cb->fileName,"rb"))==NULL)
        return NULL;

    block=(FIELD*)malloc(sizeof(FIELD)*cb->blockSize);   //target 1 block (cb->size bytes)
    memset(block,0,sizeof(FIELD)*cb->blockSize);

            fseek(f,from*cb->blockSize,SEEK_SET);
            fread(block,cb->blockSize,1,f);

    fclose(f);
    return block;		/*should be free later*/
}

int recv1(NCCB* cb,FIELD* block)
{
	FILE* f;
//	ulong i,j;
	int flag=appendM(cb,block);
	
	if(flag==cFALSE)		//coeff matrix must be nonsingular
		return cFALSE;
	if((f=fopen(cb->fileName,"rb+"))==NULL)
	{	
		return cFALSE;}
	fseek(f,0,SEEK_END);
	fwrite(block,cb->size,1,f);
	fclose(f);
	/*//printf("\nrecv\n");
	for (i=0;i<cb->holdNum;i++)
		{
			for (j=0;j<cb->blockNum;j++)
				//printf("%d ",cb->cMatrix[i][j]);
			//printf("\n");
		};*/
	return cTRUE;
}



int check(NCCB* cb,FIELD* block, ulong n)
{

	ulong i=0;
	ulong j=0;
	ulong jjj=0;
	int flag=0;
	FIELD coeff;
	ulong holdNum=cb->holdNum;
	FIELD** row=(FIELD**)malloc(sizeof(FIELD*)*n);
	FIELD** tMatrix=(FIELD**)malloc(sizeof(FIELD*)*cb->blockNum);
	
	//long* nonZero;		/*初等变换系数矩阵用到该数组*/
	long* tnonZero=(long*)malloc(sizeof(long)*cb->blockNum);
	for(i=0;i<n;i++)
	{
		FIELD* temp=(FIELD*)malloc(sizeof(FIELD)*cb->blockNum);
		row[i]=temp;
		memcpy(row[i],&block[i*cb->blockNum],cb->blockNum);

	}
	for (i=0;i<holdNum;i++)
	{
		tMatrix[i]=(FIELD*)malloc(sizeof(FIELD)*cb->blockNum);
		memcpy(tMatrix[i],cb->cMatrix[i],cb->blockNum);
	}
    for(i=0;i<cb->blockNum;i++)
	{
		tnonZero[i]=cb->nonZero[i];		


	}

	for (j=0;j<n;j++)
	{
		flag=0;

		for(i=0;i<cb->blockNum;i++)
		{
			
			long rindex;
			
			if(row[j][i]==0)
			{
				continue;
			}
			rindex=tnonZero[i];

			
			if(rindex<0)				//detect non singular,append it.
			{
				
				tMatrix[holdNum]=row[j];

				tnonZero[i]=holdNum;

				holdNum+=1;
				flag=1;
				
				break;
			
			}
			else
			{	

				coeff=gfsub(0,gfdiv(row[j][i],tMatrix[rindex][i]));
				
				
				cMulvAdd(row[j],tMatrix[rindex],cb->blockNum,coeff);

			}
		}
		if (flag==0)
			
			break;
	}
	

	
	for(i=0;i<holdNum;i++)
	{
		free(tMatrix[i]);
	}
	free(tMatrix);
	free(tnonZero);

	return flag;
}

/* extract coefficient matrix from block
 * try to Determine the linear correlation
 */
int appendM(NCCB* cb,FIELD* block)
{
	ulong i=0,j=0;
	int flag=0;
	FIELD* row=(FIELD*)malloc(sizeof(FIELD)*cb->blockNum);
    memcpy(row,block,cb->blockNum); //head "blockNum bytes" is the coefficient matrix (8*blockNum bits)

    for(i=0;i<cb->blockNum;i++) //for each row in this matrix (1 row = 8 bits = 1 bytes )
	{
		long rindex;
        if(row[i]==0)   //if: is a all zero row
			continue;

        rindex = cb->nonZero[i];    //read marks in nonZero array
        if(rindex<0) //never got this row (refer to a certain source block) before(mark is -1)
        {	////detect non singular,append it.
			////printf("detect non singular,append it.\n");
            ////for (j=0;j<cb->blockNum;j++){//printf("%d ",row[j]);cb->cMatrix[cb->holdNum][j]=row[j];}
            cb->cMatrix[cb->holdNum]=row;   //save this coeff matrix from this encoded block
			/*for (j=0;j<cb->blockNum;j++) 
				//printf("%d ",cb->cMatrix[cb->holdNum][j]);*/
            cb->nonZero[i]=cb->holdNum;
			cb->holdNum+=1;
			flag=1;

			break;
		}
        else //have got some encoded block before, which are generated with non-zero coeff for this raw block i
		{
			FIELD coeff=gfsub(0,gfdiv(row[i],cb->cMatrix[rindex][i]));
            cMulvAdd(row,cb->cMatrix[rindex],cb->blockNum,coeff); // this row data is Eliminated
		}
    }

	return flag;
}

/* load coeff and file data */
int loadData(char* file,FIELD*** data,ulong bSize,ulong bNum)
{
	FILE* f;
	ulong flen,i;
	FIELD** blocks=*data;
	if((f=fopen(file,"rb"))==NULL)
		return cFALSE;
	fseek(f,0,SEEK_END);
	flen=ftell(f);
	fseek(f,0,SEEK_SET);
	if((bSize+bNum)*bNum!=flen)	//error format
	{   
        printf("ERROR file length for loading data for decoding!\n");
		fclose(f);
		return cFALSE;
	}
	for(i=0;i<bNum;i++)
	{
		fseek(f,i*(bNum+bSize),SEEK_SET);
		fread(blocks[i],(bNum+bSize),1,f);
	}
	fclose(f);
	return cTRUE;
}

int decodeFile(char* sourceFile,char* destFile,ulong bSize,ulong bNum,ulong chunkNum)
{
	ulong i,j;
	FILE* ofile;
	FIELD** data;
	int flag;
    if((ofile=fopen(destFile,"wb"))==NULL)
		  return cFALSE;
    data=(FIELD**)malloc(sizeof(FIELD*)*(bNum+bSize));
    for(i=0;i<bNum;i++)
		data[i]=(FIELD*)malloc(sizeof(FIELD)*(bNum+bSize));
	for(i=0;i<chunkNum;i++){

		char chunkName[72],chunkNo[4];
		sprintf(chunkNo,"%d",i);
		strcpy(chunkName,sourceFile);
		strcat(chunkName,chunkNo);
		strcat(chunkName,".dat");

        //load whole file in RAM! maybe too large...
        flag=loadData(chunkName,&data,bSize,bNum);
		if(flag==cFALSE){
			printf("loading chunk %d failed.\n",i);
			return cFALSE;
		}

	    flag=identical(data,bNum,bNum+bSize);
		if(flag==cFALSE){
			printf("decoding chunk %d failed.\n",i);
		    return cFALSE;
		}
        printf("chunk %d decoding ok\n",i);
	    for(j=0;j<bNum;j++)
            fwrite(data[j]+bNum,bSize,1,ofile);
	   }
        		 
		for(j=0;j<bNum;j++)
		free(data[j]);

	fclose(ofile);
	free(data);
	return cTRUE;
}


int genSeedFile(char* rfName,char* sfName,ulong chunkSize, ulong blockSize)
{   
    ulong flen,chunkNum,blockNum = chunkSize/blockSize,i,j;
	FILE* ifile,*ofile,*infofile;
	FIELD* block;
	char infoName[64];
	strcpy(infoName,sfName);
	strcat(infoName,".snc");

	if((ifile=fopen(rfName,"rb"))==NULL)
		return cFALSE;
	if((infofile=fopen(infoName,"wb"))==NULL)
		return cFALSE;

	fseek(ifile,0,SEEK_END);
	flen=ftell(ifile);
	fseek(ifile,0,SEEK_SET);
	if(flen==0)
		chunkNum=0;
	else
		chunkNum=1+(flen-1)/(long)chunkSize;
	for(i=0;i<chunkNum;i++){

		 char chunkName[72],chunkNo[4];
		 sprintf(chunkNo,"%d",i);
		 strcpy(chunkName,sfName);
		 strcat(chunkName,chunkNo);
		 strcat(chunkName,".dat");
         
		 if((ofile=fopen(chunkName,"wb"))==NULL)
		 return cFALSE;

	     block=(FIELD*)malloc(sizeof(FIELD)*(blockSize+blockNum));
         for(j=0;j<blockNum;j++)
		 {
			memset(block,0,sizeof(FIELD)*blockNum);
            block[j]=1; //coefficient, this byte= 0x01
		    fread(block+blockNum,blockSize,1,ifile);
		    fwrite(block,blockSize+blockNum,1,ofile);
		 }
	     fclose(ofile);
         free(block);
	}
	fclose(ifile);
	fprintf(infofile,"[FileInfo]\n");
	fprintf(infofile,"file_name=%s\n",rfName);
	fprintf(infofile,"file_len=%ld\n",flen);
	fprintf(infofile,"[SNCInfo]\n");
	fprintf(infofile,"chunk_size=%ld\n",chunkSize);
	fprintf(infofile,"chunk_num=%ld\n",chunkNum);
	fprintf(infofile,"block_size=%ld\n",blockSize);
	fprintf(infofile,"block_num=%ld\n",blockNum);
	fclose(infofile);
	return cTRUE;
}

void clearNC(NCCB* cb)			//for python
{
	ulong i;
	for(i=0;i<cb->holdNum;i++)
		free(cb->cMatrix[i]);
	free(cb->cMatrix);
	free(cb->nonZero);

}

