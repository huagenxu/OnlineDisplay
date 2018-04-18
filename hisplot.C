//-----histogram plot program which is for online display ----------//
//-----the histogram loaded from the shared memeory with mapped file---//
//-----with name histprod.map, the histogram will be updated by -------//
//-----certain entries. The loop will be broke by CTRL+C --------------//
//----------Created on Nov. 14 2012 for Day-1 online demo--------------//
//--------------Author Huagen Xu---------------------------------------//

#include <TApplication.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TH1.h>
#include <TH2.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <TMapFile.h>

using namespace std;

/******************************************************************************************************/
int histplot()
{
	gStyle->SetFrameFillColor(10);
	gStyle->SetFrameFillStyle(0);
	gStyle->SetPadBorderMode(1);
	gStyle->SetCanvasBorderMode(1);
	gStyle->SetCanvasColor(10);
	gStyle->SetOptStat();

	//Create 2 new canvas and 4 pads for each
	TCanvas *c1, *c2, *c3, *c4;
	TPad *pad11, *pad12, *pad13, *pad14, *pad21, *pad22, *pad23, *pad24, *pad31, *pad32, *pad33, *pad34, *pad35, *pad36, *pad41, *pad42;
	
	c1 = new TCanvas("c1","Rear side signals of recoil detectors", 50,10, 900, 450);
	c1->Draw();
	c1->Modified();
	c1->Update();
	pad11 = new TPad("pad11", "This is pad11", 0.02,0.52,0.48,0.98,10,0,0);
	pad12 = new TPad("pad12", "This is pad12", 0.52,0.52,0.98,0.98,10,0,0);
	pad13 = new TPad("pad13", "This is pad13", 0.02,0.02,0.48,0.48,10,0,0);
	pad14 = new TPad("pad14", "This is pad14", 0.52,0.02,0.98,0.48,10,0,0);
	pad11->Draw();
	pad12->Draw();
	pad13->Draw();
	pad14->Draw();

	c1->Draw();
	c1->Modified();
	c1->Update();

	c2 = new TCanvas("c2","Hits on recoil detectors", 50,510, 900, 450);
	c2->Draw();
	c2->Modified();
	c2->Update();
	pad21 = new TPad("pad21", "This is pad21", 0.02,0.52,0.48,0.98,10,0,0);
	pad22 = new TPad("pad22", "This is pad22", 0.52,0.52,0.98,0.98,10,0,0);
	pad23 = new TPad("pad23", "This is pad23", 0.02,0.02,0.48,0.48,10,0,0);
	pad24 = new TPad("pad24", "This is pad24", 0.52,0.02,0.98,0.48,10,0,0);
	pad21->Draw();
	pad22->Draw();
	pad23->Draw();
	pad24->Draw();

	c2->Draw();
	c2->Modified();
	c2->Update();
/*
	c3 = new TCanvas("c3","Scintillator counts", 950,10, 400, 400);
	c3->Draw();
	c3->Modified();
	c3->Update();
	pad31 = new TPad("pad31", "This is pad31", 0.02,0.68,0.48,0.98,10,0,0);
	pad32 = new TPad("pad32", "This is pad32", 0.52,0.68,0.98,0.98,10,0,0);
	pad33 = new TPad("pad33", "This is pad33", 0.02,0.36,0.48,0.66,10,0,0);
	pad34 = new TPad("pad34", "This is pad34", 0.52,0.36,0.98,0.66,10,0,0);
	pad35 = new TPad("pad35", "This is pad35", 0.02,0.02,0.34,0.66,10,0,0);
	pad36 = new TPad("pad36", "This is pad36", 0.52,0.02,0.34,0.66,10,0,0);
	pad31->Draw();
	pad32->Draw();
	pad33->Draw();
	pad34->Draw();
	pad35->Draw();
	pad36->Draw();

	c3->Draw();
	c3->Modified();
	c3->Update();
*/
	c4 = new TCanvas("c4","Histogram of Scintillators", 1050,50, 600, 600);
	c4->Draw();
	c4->Modified();
	c4->Update();
	pad41 = new TPad("pad41", "This is pad41", 0.02,0.52,0.98,0.98,10,0,0);
	pad42 = new TPad("pad42", "This is pad42", 0.02,0.02,0.98,0.48,10,0,0);
//	pad43 = new TPad("pad43", "This is pad43", 0.02,0.02,0.48,0.48,10,0,0);
//	pad44 = new TPad("pad44", "This is pad44", 0.52,0.02,0.98,0.48,10,0,0);
	pad41->Draw();
	pad42->Draw();
//	pad43->Draw();
//	pad44->Draw();

	c4->Draw();
	c4->Modified();
	c4->Update();


//cout<<"In hisplot(): check point 1 before TApplication app!"<<endl;
	TApplication app("app",0,0);
//cout<<"In hisplot(): check point 2 after TApplication app"<<endl;
	TMapFile *mfile = 0;
	//TString path = "/home/huage/code/cluster_decoding/DayOneOnline";
	mfile = TMapFile::Create("histprod.map");
//cout<<"In hisplot(): check point 3 after TMapFile::Create(hitprod.map)"<<endl;

	mfile->Print();
	mfile->ls();

	TH1F *h217 = 0;
	TH1F *h218 = 0;
	TH1F *h219 = 0;
	TH1F *h220 = 0;

	TH2S *Ge_5mm_hits = 0;
	TH2S *Ge_11mm_hits = 0;
	TH2S *Si_13_hits = 0;
	TH2S *Si_15_hits = 0;
/*
	TH1F *h701 = 0;
	TH1F *h702 = 0;
	TH1F *h703 = 0;
	TH1F *h705 = 0;
	TH1F *h707 = 0;
	TH1F *h709 = 0;
*/
	TH2S *Scint_qhits = 0;
	TH2S *Scint_thits = 0;

	//loop displaying the histograms, Once the producer stops this script will break out of the loop

	Double_t oldentries = 0;
	while(1) 
	{
		h217 = (TH1F*) mfile->Get("ADC2_ch17",h217);
		h218 = (TH1F*) mfile->Get("ADC2_ch18",h218);
		h219 = (TH1F*) mfile->Get("ADC2_ch19",h219);
		h220 = (TH1F*) mfile->Get("ADC2_ch20",h220);

		Si_13_hits = (TH2S*) mfile->Get("Si_12_strips",Si_13_hits);
		Si_15_hits = (TH2S*) mfile->Get("Si_15_strips",Si_15_hits);
		Ge_5mm_hits = (TH2S*) mfile->Get("Ge_5mm_strips",Ge_5mm_hits);
		Ge_11mm_hits = (TH2S*) mfile->Get("Ge_11mm_strips",Ge_11mm_hits);
/*
		h701 = (TH1F*) mfile->Get("QDC1_ch1",h701);
		h702 = (TH1F*) mfile->Get("QDC1_ch2",h702);
		h703 = (TH1F*) mfile->Get("QDC1_ch3",h703);
		h705 = (TH1F*) mfile->Get("QDC1_ch5",h705);
		h707 = (TH1F*) mfile->Get("QDC1_ch7",h707);
		h709 = (TH1F*) mfile->Get("QDC1_ch9",h709);
*/
		Scint_qhits = (TH2S*) mfile->Get("ScintillatorsQ",Scint_qhits);
		Scint_thits = (TH2S*) mfile->Get("ScintillatorsT",Scint_thits);

		cout<<" --- - ---"<<endl;
		//cout<<"h217->GetEntries :"<<h217->GetEntries()<<endl;
		
		if(h217->GetEntries() == oldentries) 
		{
			gSystem ->Sleep(500);
			cout<<"--- + ----"<<endl;
		}
		oldentries = h217->GetEntries();
		cout<<"Entries: "<<oldentries<<endl;
//cout<<"In hisplot(): check point 4 before pad1 update"<<endl;
		pad11->cd(); gPad->SetLogy(); h217->SetXTitle("ADC channel");//h217->GetXaxis()->SetRangeUser(3200.,3600.); 
h217->Draw();
		pad13->cd(); gPad->SetLogy(); h218->SetXTitle("ADC channel");//h218->GetXaxis()->SetRangeUser(2950,3350); 
h218->Draw();
		pad12->cd(); gPad->SetLogy();h219->SetXTitle("ADC channel"); //h219->GetXaxis()->SetRangeUser(3700,4100);
h219->Draw();
	 	pad14->cd(); gPad->SetLogy(); h220->SetXTitle("ADC channel"); //h220->GetXaxis()->SetRangeUser(3900,4300);
h220->Draw();
		c1->Modified();
		c1->Update();
//cout<<"In hisplot(): check point 5 after pad1 update"<<endl;

		pad21->cd(); gPad->SetLogz(); Si_13_hits->SetXTitle("Signal channel"); Si_13_hits->SetYTitle("ADC channel");
Si_13_hits->Draw("colz");
		pad23->cd(); gPad->SetLogz(); Si_15_hits->SetXTitle("Signal channel"); Si_15_hits->SetYTitle("ADC channel");
 Si_15_hits->Draw("colz");
		pad22->cd(); gPad->SetLogz(); Ge_5mm_hits->SetXTitle("Signal channel"); Ge_5mm_hits->SetYTitle("ADC channel");
 Ge_5mm_hits->Draw("colz");
		pad24->cd(); gPad->SetLogz(); Ge_11mm_hits->SetXTitle("Signal channel"); Ge_11mm_hits->SetYTitle("ADC channel");
 Ge_11mm_hits->Draw("colz");

		c2->Modified();
		c2->Update();
//cout<<"In hisplot(): check point 6 after pad2 update"<<endl;
	
/*		pad31->cd(); gPad->SetLogy(); 
		h701->SetXTitle("QDC channel"); 
		h701->Draw();

		pad33->cd(); gPad->SetLogy(); 
		h702->SetXTitle("QDC channel");
		h702->Draw();

		pad32->cd(); gPad->SetLogy();
		h703->SetXTitle("QDC channel"); 
		h703->Draw();

	 	pad34->cd(); gPad->SetLogy(); 
		h705->SetXTitle("QDC channel"); 
		h705->Draw();

		pad35->cd(); gPad->SetLogy();
		h707->SetXTitle("QDC channel");
		h707->Draw();

	 	pad36->cd(); gPad->SetLogy(); 
		h709->SetXTitle("QDC channel"); 
		h709->Draw();

		c3->Modified();
		c3->Update();
*/
		pad41->cd(); gPad->SetLogz(); Scint_qhits->SetXTitle("Signal channel"); Scint_qhits->SetYTitle("QDC channel");
Scint_qhits->Draw("colz");
		pad42->cd(); gPad->SetLogz(); Scint_thits->SetXTitle("Signal channel"); Scint_thits->SetYTitle("TDC channel");
Scint_thits->Draw("colz");
		//pad42->cd(); gPad->SetLogz(); Ge_5mm_hits->SetXTitle("Signal channel"); Ge_5mm_hits->SetYTitle("ADC channel");
// Ge_5mm_hits->Draw("colz");
		//pad44->cd(); gPad->SetLogz(); Ge_11mm_hits->SetXTitle("Signal channel"); Ge_11mm_hits->SetYTitle("ADC channel");
// Ge_11mm_hits->Draw("colz");

		c4->Modified();
		c4->Update();

		gSystem->Sleep(50);
		if(gSystem->ProcessEvents()) break; //Giving the interrup flag

	}

	return 0;

}

/*****************************************************************************************************************************/
int main()
{
	TApplication app("app",0, 0);
	histplot();
	app.Run();
	return 0;
}




















