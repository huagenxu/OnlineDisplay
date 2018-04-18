/// -----  Separate histogramm producer for hfsepprodmap.file: serprod3.C ----- ///
/// --- borodina@ikp246:~/ROOT/ProdConsEx/SepPrRootCons/serprod3.C --- ///
/// ------------------ Last modification 17.07.2008 ------------------ ///
/// Modified on Nov. 13. 2012 by Huagen for day-1 online display ///



#include <TROOT.h>
#include <TPad.h>
#include <TMapFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <iostream>
#include <TRandom.h>
#include <TApplication.h>
#include <TFile.h>

#include "day_1_decoding.h"

using namespace std;

int main()
{
	TApplication app("app",0,0);
	TMapFile::SetMapAddress(0xb46a5000);   /// Must be here !!! Inache ne rabotaet :-( 
	TMapFile* mfile = TMapFile::Create("hfsepprod.map","RECREATE", 1000000,"Day-1 Online");
   
	TH1F *h102 = new TH1F("ADC1_ch2","Strip2",8192,0.,8192.);
	TH1F *h301 = new TH1F("ADC3_ch1","HV",8192,0.,8192.);
	TH2S *hits = new TH2S("HitsOnStrip","Spectrum on each strip",70,0.,70., 8192,0.,8192.);

	mfile->Print();

	FILE *pFile;

	int count=0;
	u_long32_t lSize;	
	u_int32_t result;
	char filename[40],filename2[50];

	printf("Type the file name to be decoded\n");
	scanf("%s",filename);

	char froot[10] = {'.','r','o','o','t'};
	printf("The filename is %s\n",filename);
    strcpy(filename2,filename);
	strcat(filename2,froot);
	
	TFile *file = new TFile(filename2,"RECREATE");

	pFile = fopen(filename, "rb");

	if (pFile == NULL) {
		fputs("Open file error",stderr);
		exit (1);
		}

	//obtain file size
	fseek(pFile, 0, SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);
	printf("the data size is %d\n",lSize);

	//allocate memory to contain the whole file
	int sizeofint = sizeof(u_int32_t);
	buffer = (u_int32_t*) malloc (sizeof(u_int32_t)*lSize);
	if (buffer == NULL) {
		fputs("Allocate memory error", stderr);
		exit(2);
		}		
	
	//copy the file into the buffer
	result = fread(buffer, 1, lSize, pFile);
	if(result !=lSize) {
		fputs("Read file error", stderr);
		exit (3);
		}


	int ii = 0;
	do  
	{

	int size = 0;//number of all following works
	const int endiantest = 0x12345678;

	for (int i= 0;i<result&i<3000000;i++) 
	{	
		printf("The Evt No. is %d\n",i);
	if(buffer[i]<8 ) {
		fputs("the data size is less than the size of cluster header, quit!\n",stderr);
		return 0;		
		//exit(4);
		}

	else if(buffer[i+1]!=endiantest){
		fputs("in decoding(): buffer[1] is not 0x12345678!\n",stderr);
		continue;//exit(5);
		}

	else if(buffer[i]<8 && buffer [i+1]!= endiantest){
		fputs("in decoding(): buffer[0] < 8 and buffer[1] is not 0x12345678!\n",stderr);
		return 0;//exit(5);
		} 
	else	size = buffer[i];
		printf("The size of cluster is %d\n",size);

    for(int k=0;k<size+1;k++) 
	{
	//	printf("The buffer[i+k], (%d + %d), buffer[%d] is 0x%08x\n",i,k,i+k,buffer[i+k]);
		if((buffer[i+k]&0xFF000000) == 0x40000000)
		{   //  cout<<"buffer["<<(i+k)<<"] is event header"<<endl;
				int adcres = buffer [i+k]>>12 & 0x7;
			//	printf("the ADC resolution is %d\n", adcres);
				int nrwords = buffer [i+k]&0xFFF;
			//	printf("the following words are %d \n",nrwords);
				int id = (buffer[i+k]>>16)&0xff;
			//    printf("the ADC id is %d \n",id);

				for (int m=1; m<=nrwords; m++)
					{
						if((buffer[i+k+m]&0xF0000000) == 0xC0000000)
						{
							int trigger = buffer[i+k+m]&0x3fffffff;
						} else {
								if ((buffer[i+k+m]&0x04000000) == 0x04000000)
									{
										int ch = (buffer[i+k+m]>>16) & 0x1F;
										data1[id][ch] = (buffer[i+k+m]) & 0x1FFF;
			                   //       printf("data[%d][ %d ] is %d \n",id, ch, data1[id][ch]); 
									
									} 
								}

			  h101->Fill(data1[4][0]);				             
   	          h102->Fill(data1[4][1]);
              h103->Fill(data1[4][2]);
              h104->Fill(data1[4][3]);
              h105->Fill(data1[4][4]);
              h106->Fill(data1[4][5]);
              h107->Fill(data1[4][6]);
              h108->Fill(data1[4][7]);
              h109->Fill(data1[4][8]);
              h110->Fill(data1[4][9]);
              h111->Fill(data1[4][10]);
              h112->Fill(data1[4][11]);
              h113->Fill(data1[4][12]);
              h114->Fill(data1[4][13]);
              h115->Fill(data1[4][14]);
              h116->Fill(data1[4][15]);

			  h117->Fill(data1[4][16]);				             
   	          h118->Fill(data1[4][17]);
              h119->Fill(data1[4][18]);
              h120->Fill(data1[4][19]);
              h121->Fill(data1[4][20]);
              h122->Fill(data1[4][21]);
              h123->Fill(data1[4][22]);
              h124->Fill(data1[4][23]);
              h125->Fill(data1[4][24]);
              h126->Fill(data1[4][25]);
              h127->Fill(data1[4][26]);
              h128->Fill(data1[4][27]);
              h129->Fill(data1[4][28]);
              h130->Fill(data1[4][29]);
              h131->Fill(data1[4][30]);
              h132->Fill(data1[4][31]);

			  h201->Fill(data1[5][0]);				             
   	          h202->Fill(data1[5][1]);
              h203->Fill(data1[5][2]);
              h204->Fill(data1[5][3]);
              h205->Fill(data1[5][4]);
              h206->Fill(data1[5][5]);
              h207->Fill(data1[5][6]);
              h208->Fill(data1[5][7]);
              h209->Fill(data1[5][8]);
              h210->Fill(data1[5][9]);
              h211->Fill(data1[5][10]);
              h212->Fill(data1[5][11]);
              h213->Fill(data1[5][12]);
              h214->Fill(data1[5][13]);
              h215->Fill(data1[5][14]);
              h216->Fill(data1[5][15]);

			  h217->Fill(data1[5][16]);				             
   	          h218->Fill(data1[5][17]);
              h219->Fill(data1[5][18]);
              h220->Fill(data1[5][19]);
              h221->Fill(data1[5][20]);
              h222->Fill(data1[5][21]);
              h223->Fill(data1[5][22]);
              h224->Fill(data1[5][23]);
              h225->Fill(data1[5][24]);
              h226->Fill(data1[5][25]);
              h227->Fill(data1[5][26]);
              h228->Fill(data1[5][27]);
              h229->Fill(data1[5][28]);
              h230->Fill(data1[5][29]);
              h231->Fill(data1[5][30]);
              h232->Fill(data1[5][31]);

			  h301->Fill(data1[3][0]);				             
//   	          h302->Fill(data1[3][1]);
//              h303->Fill(data1[3][2]);
//              h304->Fill(data1[3][3]);

              for(int n = 0;n<67;n++)
              {
              if(n<32) hits->Fill(strip[n],data1[4][n],1);
              else if(n>31&&n<64) hits->Fill(strip[n],data1[5][n-32],1);
              else hits->Fill(strip[n],data1[3][n-48],1);
			  }

					} //loop ADC data

			k += nrwords;
		//	printf("The cluster No. %d has been decoded\n",k);
			}	

    	if (!(ii % 10)) 
		{
			mfile->Update();       // updates all objects in shared memory
			if (!ii) mfile->ls();  // print contents of mapped file after
     	}                         // first update
     	ii++;
     	gSystem->Sleep(100);
		
		if (h102->GetEntries() >20000)
			{
			h102->Reset();
			h301->Reset();
			hits->Reset();
			}

	} //loop cluster			
			i += size ;
		//	printf("The Event No. %d has been decoded\n",i);	

	}

	} while(count++<100);


	fclose(pFile);
	free(buffer);

	file->Write();
	file->Close();

	return 0;
}








