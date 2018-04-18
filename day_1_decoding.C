/*
* day_1_decoding.C
* created on Nov. 5, 2012 by Huagen XU
*
*Program of decoding cluster format  data with C++
*This is an important step to realize online display
*   
*/

#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h> 
#include <stdio.h>
#include <TROOT.h>
#include <TPad.h>
#include <TMapFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TProfile.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
//#include <TTree.h>

#include "day_1_decoding.h"


u_int32_t strips[67],rear[4];

	TTree *tree = new TTree("strips","Energy info of detector");
	tree->Branch("strips", &strips,"strips[67]/i");
	tree->Branch("rear", &rear,"rear[4]/i");

int main()
{	
	FILE *pFile;
	u_long32_t lSize;	
	u_int32_t result;
	char filename[40],filename2[50];
//	printf("the size of the u_int32_t is %d\n",sizeof(u_int32_t));

	printf("Type the file name to be decoded\n");
	scanf("%s",filename);

	char froot[10] = {'.','r','o','o','t'}
	printf("The filename is %s\n",filename);
    strcpy(filename2,filename);
//	printf("The filename2 is %s\n",filename2);
	strcat(filename2,froot);
//	printf("the name of root file is %s\n",strcat(filename2,froot));

	
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
	buffer = (char*) malloc (sizeof(u_int32_t)*lSize);
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

	decoding(result);

	fclose(pFile);
	free(buffer);

	TCanvas *c1 = new TCanvas("Test","test", 800,800, 800, 800);
	c1->Divide(4,4);
	
    c1->cd(1); 	gPad->SetLogy(); h101->Draw();
	c1->cd(2);	gPad->SetLogy(); h102->Draw();
    c1->cd(3); 	gPad->SetLogy(); h103->Draw();
	c1->cd(4);	gPad->SetLogy(); h104->Draw();

    c1->cd(5); 	gPad->SetLogy(); h105->Draw();
	c1->cd(6);	gPad->SetLogy(); h106->Draw();
    c1->cd(7); 	gPad->SetLogy(); h107->Draw();
	c1->cd(8);	gPad->SetLogy(); h108->Draw();

    c1->cd(9); 	gPad->SetLogy(); h109->Draw();
	c1->cd(10);	gPad->SetLogy(); h110->Draw();
    c1->cd(11); gPad->SetLogy(); h111->Draw();
	c1->cd(12);	gPad->SetLogy(); h112->Draw();

    c1->cd(13); gPad->SetLogy(); h113->Draw();
	c1->cd(14);	gPad->SetLogy(); h114->Draw();
    c1->cd(15); gPad->SetLogy(); h115->Draw();
	c1->cd(16); gPad->SetLogy(); h116->Draw();

	TCanvas *c2 = new TCanvas("Hits on each strip","", 600,600,600,600);
	gPad->SetLogz();	
	hits->Draw("colz");

 //       h100->Write();
        h101->Write();
 	h102->Write();
 	h103->Write();
        h104->Write();
        h105->Write();
 	h106->Write();
 	h107->Write();

        h108->Write();
        h109->Write();
 	h110->Write();
 	h111->Write();

        h112->Write();
        h113->Write();
 	h114->Write();
 	h115->Write();
        h116->Write();
        h117->Write();
 	h118->Write();
 	h119->Write();
        h120->Write();
        h121->Write();
 	h122->Write();
 	h123->Write();
        h124->Write();
        h125->Write();
 	h126->Write();
 	h127->Write();

        h128->Write();
        h129->Write();
 	h130->Write();
 	h131->Write();
	h132->Write();


        h201->Write();
 	h202->Write();
 	h203->Write();
        h204->Write();
        h205->Write();
 	h206->Write();
 	h207->Write();

        h208->Write();
        h209->Write();
 	h210->Write();
 	h211->Write();

        h212->Write();
        h213->Write();
 	h214->Write();
 	h215->Write();
        h216->Write();
        h217->Write();
 	h218->Write();
 	h219->Write();
        h220->Write();
        h221->Write();
 	h222->Write();
 	h223->Write();
        h224->Write();
        h225->Write();
 	h226->Write();
 	h227->Write();

        h228->Write();
        h229->Write();
 	h230->Write();
 	h231->Write();
	h232->Write();

        h301->Write();
        h302->Write();
 	h303->Write();
 	h304->Write();

	hits->Write();

	tree->Print();

	file->Write();
	file->Close();

	return 0;

}


int decoding(u_int32_t res) 
{
	printf("in decoding(): decoding process gets started!\n");

//	int res = 0;
	int size = 0;//number of all following works
	const int endiantest = 0x12345678;

	for (int i= 0;i<res&i<100000;i++) 
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
   	          h302->Fill(data1[3][1]);
              h303->Fill(data1[3][2]);
              h304->Fill(data1[3][3]);

			for(int a = 0; a<32; a++)
			{
				strips[a]=data1[4][a];
				strips[a+32]=data1[5][a];
			}
			for(int l=0;l<4;l++) {
				rear[l]=data1[3][l];
			}


			tree->Fill();

					} //loop ADC data

              for(int n = 0;n<67;n++)
              {
              if(n<32) hits->Fill(strip[n],data1[4][n],1);
              else if(n>31&&n<64) hits->Fill(strip[n],data1[5][n-32],1);
              else hits->Fill(strip[n],data1[3][n-48],1);
			 }


			k += nrwords;
		//	printf("The cluster No. %d has been decoded\n",k);

			}
	

	} //loop cluster			
			i += size ;
		//	printf("The Event No. %d has been decoded\n",i);	

	}

	tree->Print();
	
	
	return 0;

}





























