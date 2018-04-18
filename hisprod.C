//----------------------------------------------------------------------------//
//----Histogram producer program which create histogram for online display----// 
//----The raw data with cluster(or something else?) format will be decoded----//
//----Histogram will be filled and mapped to a memeory block -----------------//
//--------------------created on Nov. 14, 2012 -------------------------------//
//------------------------Author: Huagen Xu-----------------------------------//
//----Modified on Nov. 29, 2012, the function to open the data file was ------//
//----changed, by using open instead of fopen---------------------------------//


	#include <TROOT.h>
#include <TApplication.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TMapFile.h>
#include <TSystem.h>
#include <iostream>
#include <TCanvas.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "day_1_online.h"

using namespace std;

int decoding(int f);

int main()
{
		TApplication app("app",0,0);
		TMapFile::SetMapAddress(0xb46a5000);
		mfile = TMapFile::Create("histprod.map","RECREATE",5000000,"Demo of DayOne Online");

/*
		h301 = new TH1F("ADC3_ch1","Si01_HV",8192,0.,8192.);
		h302 = new TH1F("ADC3_ch2","Si02_HV",8192,0.,8192.);
		h303 = new TH1F("ADC3_ch3","Ge01_HV",8192,0.,8192.);
		h304 = new TH1F("ADC3_ch4","Ge02_HV",8192,0.,8192.);

		hits1 = new TH2S("Si01_hits","Hits on strips of Si01",48,0,48,8192,0.,8192.);
		hits2 = new TH2S("Si02_hits","Hits on strips of Si02",64,0,64,8192,0.,8192.);
		hits3 = new TH2S("Ge01_hits","Hits on strips of Ge01",32,0,32,8192,0.,8192.);
		hits4 = new TH2S("Ge02_hits","Hits on strips of Ge02",32,0,32,8192,0.,8192.);
*/
   		h217 = new TH1F("ADC2_ch17","Si_13_rear",nbinsx_madc, xlow_madc, xup_madc);
   		h218 = new TH1F("ADC2_ch18","Si_15_rear",nbinsx_madc, xlow_madc, xup_madc);
   		h219 = new TH1F("ADC2_ch19","Ge_5mm_rear",nbinsx_madc, xlow_madc, xup_madc);
   		h220 = new TH1F("ADC2_ch20","Ge_11mm_rear",nbinsx_madc, xlow_madc, xup_madc);

		Si_13_hits = new TH2S("Si_13_strip","Spectrum on Si_13",49,0.,49., 1024,0.,8192.);
		Si_15_hits = new TH2S("Si_15_Strip","Spectrum on Si_15",65,0.,65., 1024,0.,8192.);
		Ge_5mm_hits = new TH2S("Ge_5mm_Strip","Spectrum on Ge_5mm",33,0.,33., 1024,0.,8192.);
		Ge_11mm_hits = new TH2S("Ge_11mm_Strip","Spectrum on Ge_11mm",33,0.,33., 1024,0.,8192.);


		mfile->Print();		
		
		u_int32_t result;
		char filename[40],filename2[50],froot[10]={'.','r','o','o','t'};
		puts("Type the file name to be decoded:");
		scanf("%s",filename);
		strcpy(filename2, filename);
		strcat(filename2, froot);

		TFile *file = new TFile(filename2, "RECREATE");		

		//open data file for decoding

		int f,res,count = 0;
		f = open(filename,O_RDONLY);
		if(f<0) {
			printf("Open file: %s\n", strerror(errno));
			return 1;
		} else {
			printf("Data file is open!\n");
		}

		do {
			res = decoding(f);
			printf("Decoding status: res = decoding(f) is %d\n",res);
		} while(res==0 && count++<10);

		close(f);
		printf("Close the data file!\n");

		file->Write();
		file->Close();

		return 0;
}


int decoding(int f)
{
       printf("In decoding(): Start decoding!\n");
	// if (!buf)...
		bufsize = 2;
 
	int res=0;
	int size; //number of all following words
	const int endiantest = 0x12345678;
/*	
	off_t pos;
	pos = lseek(f,0, SEEK_CUR);
	//printf("pos=%llu\n", (unsigned long long)pos);
*/

	if((res=read(f,buf,4*2))!=4*2) 
	{
		printf("In decoding(): read size: res=%d, error=%s\n", res, strerror(errno));
		return 1;
	}

	if (buf[1]!=endiantest)
	{
	printf("In decoding(): buffer[1] is not 0x12345678!\n");
	} else 
	{
	size=buf[0];
	printf("size is %d \n", size);
	if(size<8) return 1;
	}
/*
	printf("buf[0] is %d\n",buf[0]);
	printf("buf[1] is 0x%0x\n",buf[1]);
	printf("buf[2] is %d\n",buf[2]);
	printf("buf[3] is %d\n",buf[3]);
	printf("buf[4] is %d\n",buf[4]);	
	printf("buf[5] is %d\n",buf[5]);
	printf("buf[6] is %d\n",buf[6]);
	printf("buf[7] is %d\n",buf[7]);	
   */     	
  // printf("bufsize is %d \n", bufsize);   

		if (size+2>bufsize) 
		{
        buf=(u_int32_t*)realloc(buf,(size+2)*4);
		bufsize=size+2;
		}
     printf("bufsize is %d \n", bufsize);

	 read(f,buf+2,(size-1)*4);

	for(int i=0;i<size+1&&i<4;i++)
	{
	printf("buf[ %d ] is 0x%08x \n",i, buf[i]);	
	}


	if (buf[2]!=0)
	{
		printf("buf[2]!=0 \n");			
		return 0;
	}

    int ii =  0;
	//loop one cluster data with datasize of "size"
	for (int n=0;n<size+1&&n<100000;n++) 
	{
       //    printf("Buf [%d] is 0x%08x \n", n, buf[n]);
           if((buf[n]&0xFF000000) == 0x40000000) 
			{   
				cout<<"buf["<<n<<"] is event header"<<endl;
               // continue;
				int adcres = buf[n]>>12 & 0x7;
		//		printf("the ADC resolution is %d \n",adcres);                
				int nrwords = buf[n]&0xfff;
		//		printf("the following words are %d \n",nrwords);
                int id = (buf[n]>>16)&0xff;
		//		printf("the ADC id is %d \n",id);
             
                for(int i=1;i<=nrwords;i++)
                  { 
					if((buf[n+i]&0xF0000000) ==0xC0000000)
					{ int trigger= buf[n+i]&0x3fffffff;
					  // printf("the trigger number of ADC %d is: %d \n",id, trigger);
					}else 
						{ 
						  if((buf[n+i]&0xf4000000)==0x04000000)
							{// cout<<"the buf["<<n+i<<"] is ADC  data"<<endl;
						  	    int ch= (buf[n+i]>>16) & 0x1F;    //  cout<<" ch = "<<ch<<endl;
		                        data1[id][ch] = (buf[n+i]) & 0x1FFF;   //  cout<<" buf["<<n+i<<"] is "<<buf[ch]<<" "<<endl;
							//	printf("data[%d][ %d ] is 0x%08x \n",id, ch, data1[id][ch]);   
	        			  //    printf("data[%d][ %d ] is %d \n",id, ch, data1[id][ch]); 
							} 
						}
            
/*								h101->Fill(data1[1][0]);
								h102->Fill(data1[1][1]);
								h103->Fill(data1[1][2]);
								h104->Fill(data1[1][3]);
								h105->Fill(data1[1][4]);
								h106->Fill(data1[1][5]);
								h107->Fill(data1[1][6]);
								h108->Fill(data1[1][7]);
								h109->Fill(data1[1][8]);
								h110->Fill(data1[1][9]);
								h111->Fill(data1[1][10]);
								h112->Fill(data1[1][11]);
								h113->Fill(data1[1][12]);
								h114->Fill(data1[1][13]);
								h115->Fill(data1[1][14]);
								h116->Fill(data1[1][15]);

								h117->Fill(data1[1][16]);
								h118->Fill(data1[1][17]);
								h119->Fill(data1[1][18]);
								h120->Fill(data1[1][19]);
								h121->Fill(data1[1][20]);
								h122->Fill(data1[1][21]);
								h123->Fill(data1[1][22]);
								h124->Fill(data1[1][23]);
								h125->Fill(data1[1][24]);
								h126->Fill(data1[1][25]);
								h127->Fill(data1[1][26]);
								h128->Fill(data1[1][27]);
								h129->Fill(data1[1][28]);
								h130->Fill(data1[1][29]);
								h131->Fill(data1[1][30]);
								h132->Fill(data1[1][31]);

								h201->Fill(data1[2][0]);				             
								h202->Fill(data1[2][1]);
								h203->Fill(data1[2][2]);
					            h204->Fill(data1[2][3]);
					            h205->Fill(data1[2][4]);
					            h206->Fill(data1[2][5]);
					            h207->Fill(data1[2][6]);
					            h208->Fill(data1[2][7]);
					            h209->Fill(data1[2][8]);
					            h210->Fill(data1[2][9]);
					            h211->Fill(data1[2][10]);
					            h212->Fill(data1[2][11]);
					            h213->Fill(data1[2][12]);
					            h214->Fill(data1[2][13]);
					            h215->Fill(data1[2][14]);
					            h216->Fill(data1[2][15]);

								h217->Fill(data1[2][16]);				             
						        h218->Fill(data1[2][17]);
					            h219->Fill(data1[2][18]);
					            h220->Fill(data1[2][19]);

					            h221->Fill(data1[2][20]);
					            h222->Fill(data1[2][21]);
					            h223->Fill(data1[2][22]);
					            h224->Fill(data1[2][23]);
					            h225->Fill(data1[2][24]);
					            h226->Fill(data1[2][25]);
					            h227->Fill(data1[2][26]);
					            h228->Fill(data1[2][27]);
					            h229->Fill(data1[2][28]);
					            h230->Fill(data1[2][29]);
					            h231->Fill(data1[2][30]);
					            h232->Fill(data1[2][31]);

								h301->Fill(data1[3][0]);				             
								h302->Fill(data1[3][1]);
            					h303->Fill(data1[3][2]);
            					h304->Fill(data1[3][3]);
           					    h305->Fill(data1[3][4]);
              					h306->Fill(data1[3][5]);
              					h307->Fill(data1[3][6]);
              					h308->Fill(data1[3][7]);
					            h309->Fill(data1[3][8]);
              					h310->Fill(data1[3][9]);
              					h311->Fill(data1[3][10]);
              					h312->Fill(data1[3][11]);
              					h313->Fill(data1[3][12]);
              					h314->Fill(data1[3][13]);
              					h315->Fill(data1[3][14]);
              					h316->Fill(data1[3][15]);

              					h317->Fill(data1[3][16]);
              					h318->Fill(data1[3][17]);
              					h319->Fill(data1[3][18]);
              					h320->Fill(data1[3][19]);
              					h321->Fill(data1[3][20]);
              					h322->Fill(data1[3][21]);
              					h323->Fill(data1[3][22]);
              					h324->Fill(data1[3][23]);
              					h325->Fill(data1[3][24]);
              					h326->Fill(data1[3][25]);
              					h327->Fill(data1[3][26]);
              					h328->Fill(data1[3][27]);
              					h329->Fill(data1[3][28]);
              					h330->Fill(data1[3][29]);
              					h331->Fill(data1[3][30]);
              					h332->Fill(data1[3][31]);

								h401->Fill(data1[4][0]);
								h402->Fill(data1[4][1]);
								h403->Fill(data1[4][2]);
								h404->Fill(data1[4][3]);
								h405->Fill(data1[4][4]);
								h406->Fill(data1[4][5]);
								h407->Fill(data1[4][6]);
								h408->Fill(data1[4][7]);
								h409->Fill(data1[4][8]);
								h410->Fill(data1[4][9]);
								h411->Fill(data1[4][10]);
								h412->Fill(data1[4][11]);
								h413->Fill(data1[4][12]);
								h414->Fill(data1[4][13]);
								h415->Fill(data1[4][14]);
								h416->Fill(data1[4][15]);

								h417->Fill(data1[4][16]);
								h418->Fill(data1[4][17]);
								h419->Fill(data1[4][18]);
								h420->Fill(data1[4][19]);
								h421->Fill(data1[4][20]);
								h422->Fill(data1[4][21]);
								h423->Fill(data1[4][22]);
								h424->Fill(data1[4][23]);
								h425->Fill(data1[4][24]);
								h426->Fill(data1[4][25]);
								h427->Fill(data1[4][26]);
								h428->Fill(data1[4][27]);
								h429->Fill(data1[4][28]);
								h430->Fill(data1[4][29]);
								h431->Fill(data1[4][30]);
								h432->Fill(data1[4][31]);

								h501->Fill(data1[5][0]);				             
								h502->Fill(data1[5][1]);
								h503->Fill(data1[5][2]);
					            h504->Fill(data1[5][3]);
					            h505->Fill(data1[5][4]);
					            h506->Fill(data1[5][5]);
					            h507->Fill(data1[5][6]);
					            h508->Fill(data1[5][7]);
					            h509->Fill(data1[5][8]);
					            h510->Fill(data1[5][9]);
					            h511->Fill(data1[5][10]);
					            h512->Fill(data1[5][11]);
					            h513->Fill(data1[5][12]);
					            h514->Fill(data1[5][13]);
					            h515->Fill(data1[5][14]);
					            h516->Fill(data1[5][15]);

								h517->Fill(data1[5][16]);				             
						        h518->Fill(data1[5][17]);
					            h519->Fill(data1[5][18]);
					            h520->Fill(data1[5][19]);
					            h521->Fill(data1[5][20]);
					            h522->Fill(data1[5][21]);
					            h523->Fill(data1[5][22]);
					            h524->Fill(data1[5][23]);
					            h525->Fill(data1[5][24]);
					            h526->Fill(data1[5][25]);
					            h527->Fill(data1[5][26]);
					            h528->Fill(data1[5][27]);
					            h529->Fill(data1[5][28]);
					            h530->Fill(data1[5][29]);
					            h531->Fill(data1[5][30]);
					            h532->Fill(data1[5][31]);

								h601->Fill(data1[6][0]);				             
								h602->Fill(data1[6][1]);
            					h603->Fill(data1[6][2]);
            					h604->Fill(data1[6][3]);
           					    h605->Fill(data1[6][4]);
              					h606->Fill(data1[6][5]);
              					h607->Fill(data1[6][6]);
              					h608->Fill(data1[6][7]);
					            h609->Fill(data1[6][8]);
              					h610->Fill(data1[6][9]);
              					h611->Fill(data1[6][10]);
              					h612->Fill(data1[6][11]);
              					h613->Fill(data1[6][12]);
              					h614->Fill(data1[6][13]);
              					h615->Fill(data1[6][14]);
              					h616->Fill(data1[6][15]);

              					h617->Fill(data1[6][16]);
              					h618->Fill(data1[6][17]);
              					h619->Fill(data1[6][18]);
              					h620->Fill(data1[6][19]);
              					h621->Fill(data1[6][20]);
              					h622->Fill(data1[6][21]);
              					h623->Fill(data1[6][22]);
              					h624->Fill(data1[6][23]);
              					h625->Fill(data1[6][24]);
              					h626->Fill(data1[6][25]);
              					h627->Fill(data1[6][26]);
              					h628->Fill(data1[6][27]);
              					h629->Fill(data1[6][28]);
              					h630->Fill(data1[6][29]);
              					h631->Fill(data1[6][30]);
              					h632->Fill(data1[6][31]);

*/
								for(int n=0; n<64;n++)
								{

									if(n<32) {
									Ge_5mm_hits->Fill(Ge_5mm_strip[n],data1[5][n],1);
                                	Ge_11mm_hits->Fill(Ge_11mm_strip[n],data1[6][n],1);
									Si_13_hits->Fill(Si_13_strip[n],data1[1][n],1);
									Si_15_hits->Fill(Si_15_strip[n],data1[3][n],1);
									}
									else if(n>31&&n<48) {
													Si_13_hits->Fill(Si_13_strip[n],data1[2][n-32],1);
													Si_15_hits->Fill(Si_15_strip[n],data1[4][n-32],1);
													}
									else Si_15_hits->Fill(Si_15_strip[n],data1[4][n-32],1);

								}
              
/*								h101->Fill(data1[4][0]);
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

								for(int n=0; n<64;n++)
								{
								if(n<32) hits1->Fill(strip[n],data1[4][n],1);
								else if(n>31&&n<64)hits1->Fill(strip[n],data1[5][n-32],1);
								else hits1->Fill(strip[n],data1[3][n-48],1);
								}
*/
				} //loop one ADC data
            n += nrwords;
	//		printf("the n is %d\n",n);

          }  // else continue;    

			if(!(ii%10))
			{
				mfile->Update(); // update all objects in  shared memroy
				if(!ii) mfile->ls();
			}
			ii++;
			gSystem->Sleep(100);

			if(h217->GetEntries()>20000)
				{
				h217->Reset();
				h218->Reset();
				h219->Reset();
				h220->Reset();

				Si_13_hits->Reset();
				Si_15_hits->Reset();
				Ge_5mm_hits->Reset();
				Ge_11mm_hits->Reset();
				}
  
      //  n += nrwords;
      //  cout<<"the Entries of histogram is "<<h0->GetEntries()<< endl;
     } 
    return 0;
}
