/// ------ Histogramm consumer for hfsepprod.map file: cons_3.C ------ ///
/// --- borodina@ikp246:~/ROOT/ProdConsEx/SepPrRootCons/serprod3.C --- ///
/// ------------------ Last modification 17.07.2008 ------------------ ///
//

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
#include <TStyle.h>

using namespace std;

int cons()
{

	//gROOT->Reset();
	gStyle->SetFrameFillColor(10);
	gStyle->SetFrameFillStyle(0);
	gStyle->SetPadBorderMode(1);
 //  gStyle->SetPadColor(0);
	gStyle->SetCanvasBorderMode(1);
	gStyle->SetCanvasColor(10);
	gStyle->SetOptStat();
	
	// Create a new canvas and 3 pads
	TCanvas *c1;
	TPad *pad1, *pad2, *pad3;

	c1 = new TCanvas("c1","Demo of Day_one Online",200,10,700,780);
	c1->Draw();
	c1->Modified();
	c1->Update();
	pad1 = new TPad("pad1","This is pad1",0.02,0.52,0.98,0.98,10,0,0);
	pad2 = new TPad("pad2","This is pad2",0.02,0.02,0.48,0.48,10,0,0);
	pad3 = new TPad("pad3","This is pad3",0.52,0.02,0.98,0.48,10,0,0);
	pad1->Draw();// pad1->SetFillStyle(0);
	pad2->Draw();// pad2->SetFillStyle(0);
	pad3->Draw();// pad3->SetFillStyle(0);
	c1->Draw();
	c1->Modified();
	c1->Update();

	TApplication app("app",0,0);
	TMapFile* mfile = 0;
	mfile = TMapFile::Create("hfsepprod.map");
  
	mfile->Print();
	mfile->ls();

	TH1F	*h102 = 0;
	TH1F	*h301 = 0;
	TH2S	*hits = 0;

	// Loop displaying the histograms. Once the producer stops this
	// script will break out of the loop.
	Double_t oldentries = 0;
	while (1) {
		h102    = (TH1F *) mfile->Get("ADC1_ch2", h102);
		h301    = (TH1F *) mfile->Get("ADC3_ch1", h301);
		hits    = (TH2S *) mfile->Get("HitsOnStrip", hits);
		cout << " --- - --- " << endl;
		if (h301->GetEntries() == oldentries) {
			gSystem->Sleep(1000);
			cout << " --- + --- " << endl;
     		// continue;
		}
		oldentries = h301->GetEntries();
		cout <<" Entries: " << oldentries << endl;

		pad2->cd();
		h102->Draw();
		pad3->cd();
		h301->Draw();
		pad1->cd();
		gPad->SetLogz();
		hits->Draw("colz");
		c1->Modified();
		c1->Update();

		gSystem->Sleep(1000);
		if (gSystem->ProcessEvents())
			break;
	}

	return 0;
}

int main(){
	TApplication app("app",0,0);
	cons();
	app.Run();
	return 0;
}







