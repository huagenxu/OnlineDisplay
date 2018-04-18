
#include <TH1.h>
#include <TROOT.h>
#include <TTree.h>
#include <TMapFile.h>

typedef unsigned int u_int32_t;
typedef unsigned long u_long32_t;
//typedef unsigned int ems_u32;

u_int32_t *buffer;
int decoding (u_int32_t);
u_int32_t data1[6][34];



//   TH2S *hits = new TH2S("HitsOnStrip","Spectrum on each strip",70,0.,70., 8192,0.,8192.);
   double mean[67];
   int hit[67]={0,0,0,0,0};
   double strip[67]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
                     20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,
                     37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,
                     54,55,56,57,58,59,60,61,62,63,64,65,66,67};


   TH1F *h101 = new TH1F("ADC1_ch1","Strip1",8192,0.,8192.);
   TH1F *h102 = new TH1F("ADC1_ch2","Strip2",8192,0.,8192.);
   TH1F *h103 = new TH1F("ADC1_ch3","Strip3",8192,0.,8192.);
   TH1F *h104 = new TH1F("ADC1_ch4","Strip4",8192,0.,8192.);
   TH1F *h105 = new TH1F("ADC1_ch5","Strip5",8192,0.,8192.);
   TH1F *h106 = new TH1F("ADC1_ch6","Strip6",8192,0.,8192.);
   TH1F *h107 = new TH1F("ADC1_ch7","Strip7",8192,0.,8192.);
   TH1F *h108 = new TH1F("ADC1_ch8","Strip8",8192,0.,8192.);

   TH1F *h109 = new TH1F("ADC1_ch9","Strip9",8192,0.,8192.);
   TH1F *h110 = new TH1F("ADC1_ch10","Strip10",8192,0.,8192.);
   TH1F *h111 = new TH1F("ADC1_ch11","Strip11",8192,0.,8192.);
   TH1F *h112 = new TH1F("ADC1_ch12","Strip12",8192,0.,8192.);
   TH1F *h113 = new TH1F("ADC1_ch13","Strip13",8192,0.,8192.);
   TH1F *h114 = new TH1F("ADC1_ch14","Strip14",8192,0.,8192.);
   TH1F *h115 = new TH1F("ADC1_ch15","Strip15",8192,0.,8192.);
   TH1F *h116 = new TH1F("ADC1_ch16","Strip16",8192,0.,8192.);

   TH1F *h117 = new TH1F("ADC1_ch17","Strip17",8192,0.,8192.);
   TH1F *h118 = new TH1F("ADC1_ch18","Strip18",8192,0.,8192.);
   TH1F *h119 = new TH1F("ADC1_ch19","Strip19",8192,0.,8192.);
   TH1F *h120 = new TH1F("ADC1_ch20","Strip20",8192,0.,8192.);
   TH1F *h121 = new TH1F("ADC1_ch21","Strip21",8192,0.,8192.);
   TH1F *h122 = new TH1F("ADC1_ch22","Strip22",8192,0.,8192.);
   TH1F *h123 = new TH1F("ADC1_ch23","Strip23",8192,0.,8192.);
   TH1F *h124 = new TH1F("ADC1_ch24","Strip24",8192,0.,8192.);

   TH1F *h125 = new TH1F("ADC1_ch25","Strip25",8192,0.,8192.);
   TH1F *h126 = new TH1F("ADC1_ch26","Strip26",8192,0.,8192.);
   TH1F *h127 = new TH1F("ADC1_ch27","Strip27",8192,0.,8192.);
   TH1F *h128 = new TH1F("ADC1_ch28","Strip28",8192,0.,8192.);
   TH1F *h129 = new TH1F("ADC1_ch29","Strip29",8192,0.,8192.);
   TH1F *h130 = new TH1F("ADC1_ch30","Strip30",8192,0.,8192.);

   TH1F *h131 = new TH1F("ADC1_ch31","Strip31",8192,0.,8192.);
   TH1F *h132 = new TH1F("ADC1_ch32","Strip32",8192,0.,8192.);


   TH1F *h201 = new TH1F("ADC2_ch1","Strip33",8192,0.,8192.);
   TH1F *h202 = new TH1F("ADC2_ch2","Strip34",8192,0.,8192.);
   TH1F *h203 = new TH1F("ADC2_ch3","Strip35",8192,0.,8192.);
   TH1F *h204 = new TH1F("ADC2_ch4","Strip36",8192,0.,8192.);
   TH1F *h205 = new TH1F("ADC2_ch5","Strip37",8192,0.,8192.);
   TH1F *h206 = new TH1F("ADC2_ch6","Strip38",8192,0.,8192.);
   TH1F *h207 = new TH1F("ADC2_ch7","Strip39",8192,0.,8192.);
   TH1F *h208 = new TH1F("ADC2_ch8","Strip40",8192,0.,8192.);

   TH1F *h209 = new TH1F("ADC2_ch9","Strip41",8192,0.,8192.);
   TH1F *h210 = new TH1F("ADC2_ch10","Strip42",8192,0.,8192.);
   TH1F *h211 = new TH1F("ADC2_ch11","Strip43",8192,0.,8192.);
   TH1F *h212 = new TH1F("ADC2_ch12","Strip44",8192,0.,8192.);
   TH1F *h213 = new TH1F("ADC2_ch13","Strip45",8192,0.,8192.);
   TH1F *h214 = new TH1F("ADC2_ch14","Strip46",8192,0.,8192.);
   TH1F *h215 = new TH1F("ADC2_ch15","Strip47",8192,0.,8192.);
   TH1F *h216 = new TH1F("ADC2_ch16","Strip48",8192,0.,8192.);

   TH1F *h217 = new TH1F("ADC2_ch17","Strip49",8192,0.,8192.);
   TH1F *h218 = new TH1F("ADC2_ch18","Strip50",8192,0.,8192.);
   TH1F *h219 = new TH1F("ADC2_ch19","Strip51",8192,0.,8192.);
   TH1F *h220 = new TH1F("ADC2_ch20","Strip52",8192,0.,8192.);
   TH1F *h221 = new TH1F("ADC2_ch21","Strip53",8192,0.,8192.);
   TH1F *h222 = new TH1F("ADC2_ch22","Strip54",8192,0.,8192.);
   TH1F *h223 = new TH1F("ADC2_ch23","Strip55",8192,0.,8192.);
   TH1F *h224 = new TH1F("ADC2_ch24","Strip56",8192,0.,8192.);

   TH1F *h225 = new TH1F("ADC2_ch25","Strip57",8192,0.,8192.);
   TH1F *h226 = new TH1F("ADC2_ch26","Strip58",8192,0.,8192.);
   TH1F *h227 = new TH1F("ADC2_ch27","Strip59",8192,0.,8192.);
   TH1F *h228 = new TH1F("ADC2_ch28","Strip60",8192,0.,8192.);
   TH1F *h229 = new TH1F("ADC2_ch29","Strip61",8192,0.,8192.);
   TH1F *h230 = new TH1F("ADC2_ch30","Strip62",8192,0.,8192.);
   TH1F *h231 = new TH1F("ADC2_ch31","Strip63",8192,0.,8192.);
   TH1F *h232 = new TH1F("ADC2_ch32","Strip64",8192,0.,8192.);

//   TH1F *h301 = new TH1F("ADC3_ch1","HV",8192,0.,8192.);
//   TH1F *h302 = new TH1F("ADC3_ch2","",8192,0.,8192.);
//   TH1F *h303 = new TH1F("ADC3_ch3","",8192,0.,8192.);
//   TH1F *h304 = new TH1F("ADC3_ch4","",8192,0.,8192.);





















