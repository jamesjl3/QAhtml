#include "BCODraw.h"

#include <sPhenixStyle.C>

#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>

#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TPaveText.h>
#include <TLatex.h>

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

BCODraw::BCODraw(const std::string &name)
  : QADraw(name)
{
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  return;
}

BCODraw::~BCODraw()
{
  /* delete db; */
  return;
}

int BCODraw::Draw(const std::string &what)
{
  /* SetsPhenixStyle(); */
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "MVTX")
  {
    iret += DrawMVTX();
    idraw++;
  }
  if (what == "ALL" || what == "INTT")
  {
    iret += DrawINTT();
    idraw++;
  }
  if (what == "ALL" || what == "TPC")
  {
    iret += DrawTPC();
    idraw++;
  }
  if (what == "ALL" || what == "TPOT")
  {
    iret += DrawTPOT();
    idraw++;
  }
  if (!idraw)
  {
    std::cout << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int BCODraw::MakeCanvas(const std::string &name, int num)
{
  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  // xpos (-1) negative: do not draw menu bar
  TC[num] = new TCanvas(name.c_str(), (boost::format("BCO Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2) , (int) (ysize / 1.2));
  gSystem->ProcessEvents();

  if (num == 0 || num == 2)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.25, 0.45, 0.75, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.5, 0.25, 0.95, 0.75, 0);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw(); 
  }
  else if (num == 4 || num == 5)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.2, 0.95, 0.8, 0);

    Pad[num][0]->Draw();
  }
  else if (num == 1)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.52, 0.32, 0.97, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.37, 0.52, 0.64, 0.97, 0);
    Pad[num][2] = new TPad((boost::format("mypad%d2") % num).str().c_str(), "name", 0.69, 0.52, 0.96, 0.97, 0);
    Pad[num][3] = new TPad((boost::format("mypad%d3") % num).str().c_str(), "here", 0.05, 0.02, 0.32, 0.47, 0);
    Pad[num][4] = new TPad((boost::format("mypad%d4") % num).str().c_str(), "hi", 0.37, 0.02, 0.64, 0.47, 0);
    Pad[num][5] = new TPad((boost::format("mypad%d5") % num).str().c_str(), "hello", 0.69, 0.02, 0.96, 0.47, 0);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
    Pad[num][2]->Draw();
    Pad[num][3]->Draw();
    Pad[num][4]->Draw();
    Pad[num][5]->Draw();
  }
  else if (num == 3)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.52, 0.22, 0.97, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.28, 0.52, 0.45, 0.97, 0);
    Pad[num][2] = new TPad((boost::format("mypad%d2") % num).str().c_str(), "name", 0.55, 0.52, 0.72, 0.97, 0);
    Pad[num][3] = new TPad((boost::format("mypad%d3") % num).str().c_str(), "here", 0.78, 0.52, 0.95, 0.97, 0);
    Pad[num][4] = new TPad((boost::format("mypad%d4") % num).str().c_str(), "hi", 0.05, 0.02, 0.22, 0.47, 0);
    Pad[num][5] = new TPad((boost::format("mypad%d5") % num).str().c_str(), "hello", 0.28, 0.02, 0.45, 0.47, 0);
    Pad[num][6] = new TPad((boost::format("mypad%d6") % num).str().c_str(), "howdy", 0.55, 0.02, 0.72, 0.47, 0);
    Pad[num][7] = new TPad((boost::format("mypad%d7") % num).str().c_str(), "yo", 0.78, 0.02, 0.95, 0.47, 0);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
    Pad[num][2]->Draw();
    Pad[num][3]->Draw();
    Pad[num][4]->Draw();
    Pad[num][5]->Draw();
    Pad[num][6]->Draw();
    Pad[num][7]->Draw();
  }
  
  // this one is used to plot the run number on the canvas
  transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[num]->SetFillStyle(4000);
  transparent[num]->Draw();

  return 0;
}

int BCODraw::DrawMVTX()
{
  std::cout << "BCO DrawMVTX() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  const int nmvtxfelix = 12;
  //const int nmvtxfees = 12;

  auto h_mvtxrefgl1 = dynamic_cast <TH1 *> (cl->getHisto("h_MvtxPoolQA_RefGL1BCO"));
  auto h_mvtxallpackets = dynamic_cast <TH1 *> (cl->getHisto("h_MvtxPoolQA_TagBCOAllFelixs"));
  auto h_mvtxallpacketsallfees = dynamic_cast <TH1 *> (cl->getHisto("h_MvtxPoolQA_TagBCOAllFelixsAllFees"));
    
  if (h_mvtxrefgl1 && h_mvtxallpackets && h_mvtxallpacketsallfees)
  {
    const int nMvtxGL1BCOs = h_mvtxrefgl1->GetEntries();
    //TH1* mvtxTagFelix[nmvtxfelix][nmvtxfees];
    //int nMvtxTagFelix[nmvtxfelix][nmvtxfees];
    //float mvtxTagFelixFrac[nmvtxfelix][nmvtxfees];
    float mvtxTagFelixx[nmvtxfelix];
    float mvtxTagFelixxFrac[nmvtxfelix];
    float mvtxTagAllFelixFrac[nmvtxfelix];
    //float mvtxFelixPacket[nmvtxfees];
    //TGraph *mvtxFelixFracGr[nmvtxfelix];
    for(int i=0; i<nmvtxfelix; i++)
    {
      mvtxTagFelixx[i] = (dynamic_cast <TH1 *> (cl->getHisto((boost::format("h_MvtxPoolQA_TagBCO_felix%i") % i).str())))->GetEntries(); 
      mvtxTagFelixxFrac[i] = mvtxTagFelixx[i] / nMvtxGL1BCOs;

      mvtxTagAllFelixFrac[i] = ((dynamic_cast <TH1 *> (cl->getHisto((boost::format("h_MvtxPoolQA_TagBCOAllFees_Felix%i") % (i)).str())))->GetEntries()) / nMvtxGL1BCOs; 

      // for(int j=0; j<nmvtxfees; j++)
      // {
      //   mvtxTagFelix[i][j] = (dynamic_cast <TH1 *> (cl->getHisto((boost::format("h_MvtxPoolQA_TagBCO_felix%i_fee%i") % i % j).str()))); 
      //   nMvtxTagFelix[i][j] = mvtxTagFelix[i][j]->GetEntries();
      //
      //   mvtxTagFelixFrac[i][j] = (float)nMvtxTagFelix[i][j] / nMvtxGL1BCOs;
      //   mvtxFelixPacket[j] = j;
      // }
      // mvtxFelixFracGr[i] = new TGraph(nmvtxfees, mvtxFelixPacket, mvtxTagFelixFrac[i]);
    }     
    TH1F* mvtxsummaryhisto = new TH1F("mvtxsummaryhisto",";Felix.Endpoint;Fraction Tagged GL1 BCOs",13,0,13);
    for(int i=0; i<nmvtxfelix; i++)
    {
      mvtxsummaryhisto->SetBinContent(i+1, mvtxTagFelixxFrac[i]);
    }
    mvtxsummaryhisto->SetBinContent(13, h_mvtxallpackets->GetEntries() / nMvtxGL1BCOs);

    TH1F* mvtxAllFelixSummary = new TH1F("mvtxfelixsummaryhisto",";Felix.Endpoint; Fraction All FEEs Tagged GL1 BCOs",13,0,13);
    for(int i=0; i<nmvtxfelix; i++)
    {
      mvtxAllFelixSummary->SetBinContent(i+1, mvtxTagAllFelixFrac[i]);
    }
    mvtxAllFelixSummary->SetBinContent(13, h_mvtxallpacketsallfees->GetEntries() / nMvtxGL1BCOs);

    if (! gROOT->FindObject("mvtx_evt_building_1"))
    {
      MakeCanvas("mvtx_evt_building_1", 0);
    }
    TC[0]->Clear("D");
    Pad[0][0]->cd();
    gStyle->SetOptStat(0);
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(1,"0.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(2,"0.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(3,"1.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(4,"1.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(5,"2.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(6,"2.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(7,"3.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(8,"3.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(9,"4.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(10,"4.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(11,"5.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(12,"5.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(13,"All");
    mvtxAllFelixSummary->DrawCopy("hist");
    gPad->SetRightMargin(0.15);

    Pad[0][1]->cd();
    gStyle->SetOptStat(0);
    mvtxsummaryhisto->GetYaxis()->SetRangeUser(0,1);
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(1,"0.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(2,"0.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(3,"1.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(4,"1.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(5,"2.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(6,"2.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(7,"3.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(8,"3.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(9,"4.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(10,"4.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(11,"5.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(12,"5.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(13,"All");
    mvtxsummaryhisto->DrawCopy("hist");
    gPad->SetRightMargin(0.15);
    
    /*
    if (! gROOT->FindObject("mvtx_evt_building_2"))
    {
      MakeCanvas("mvtx_evt_building_2", 1);
    }
    TC[1]->Clear("D");
    for(int i=0; i<nmvtxfelix; i++)
    {
      Pad[1][i]->cd();
      mvtxFelixFracGr[i]->SetTitle((boost::format("Felix %i") % i).str().c_str());
      mvtxFelixFracGr[i]->GetYaxis()->SetRangeUser(0,1);
      mvtxFelixFracGr[i]->GetXaxis()->SetTitle("FEE");
      mvtxFelixFracGr[i]->GetYaxis()->SetTitle("Fraction Tagged GTM BCOs");
      mvtxFelixFracGr[i]->Draw("ap*");
      //myText(0.22,0.4,kBlack,(boost::format("Felix %i") % (i)).str().c_str());
    }
    */
  }
  else
  {
    // histogram is missing
    return -1;
  }
     
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  std::ostringstream runnostream1;
  std::string runstring1;
  runnostream1 << Name() << "_mvtx evt building Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());
  //transparent[1]->cd();
  //PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[0]->Update();
  //TC[1]->Update();
   
  std::cout << "DrawMVTX Ending" << std::endl;
  return 0;
}

int BCODraw::DrawINTT()
{
  std::cout << "BCO DrawINTT() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  const int ninttebdcs = 8;
  const int ninttfees = 14;
  auto h_inttrefgl1 = dynamic_cast <TH1 *> (cl->getHisto("h_InttPoolQA_RefGL1BCO"));
  auto h_inttallpackets = dynamic_cast <TH1 *> (cl->getHisto("h_InttPoolQA_TagBCOAllServers"));

  if (h_inttrefgl1 && h_inttallpackets)
  {
    const int nInttGL1BCOs = h_inttrefgl1->GetEntries(); 
    TH1* inttTagEbdc[ninttebdcs+1];
    TH1* inttTagEbdcFee[ninttebdcs][ninttfees];
    int nInttTaggedBCOs[ninttebdcs+1];
    int nInttTaggedBCOsFee[ninttebdcs][ninttfees];
    float nInttTaggedBCOsServers[ninttebdcs+1];

    for(int i=0; i<ninttebdcs; i++)
    {
      inttTagEbdc[i] = (dynamic_cast <TH1 *> (cl->getHisto((boost::format("h_InttPoolQA_TagBCO_server%i") % (i)).str()))); 
      nInttTaggedBCOs[i] = inttTagEbdc[i]->GetEntries();
      nInttTaggedBCOsServers[i] = ((dynamic_cast <TH1 *> (cl->getHisto((boost::format("h_InttPoolQA_TagBCOAllFees_Server%i") % (i)).str())))->GetEntries()); 
      nInttTaggedBCOsServers[i] /= nInttGL1BCOs;
      for(int j=0; j<ninttfees; j++)
      {
        inttTagEbdcFee[i][j] = (dynamic_cast <TH1 *> (cl->getHisto((boost::format("h_InttPoolQA_TagBCO_server%i_fee%i") % (i) % (j)).str()))); 
        nInttTaggedBCOsFee[i][j] = inttTagEbdcFee[i][j]->GetEntries();
      }
    }

    nInttTaggedBCOs[8] = h_inttallpackets->GetEntries();
    //float inttebdc[ninttebdcs+1];
    float inttebdc_fee[ninttebdcs][ninttfees];
    float inttebdcfrac[ninttebdcs+1];
    float inttebdcfeefrac[ninttebdcs][ninttfees];
    TGraph *inttebdcfeegr[ninttebdcs];
    inttebdcfrac[8] = (float)nInttTaggedBCOs[8] / nInttGL1BCOs;
    //inttebdc[8] = 8;
    for(int i = 0; i< ninttebdcs; i++)
    {
      //inttebdc[i] = i;
      inttebdcfrac[i] = (float)nInttTaggedBCOs[i] / nInttGL1BCOs;

      for(int j=0; j<ninttfees; j++)
      {
        inttebdc_fee[i][j] = j;
        inttebdcfeefrac[i][j] = (float)nInttTaggedBCOsFee[i][j] / nInttGL1BCOs;
      }
      inttebdcfeegr[i] = new TGraph(ninttfees, inttebdc_fee[i], inttebdcfeefrac[i]);
    }

    TH1* histoversion = new TH1F("inttsummary",";;Fraction Tagged GL1 BCOs;", 9,0,9);
    TH1* histoversionfees = new TH1F("inttsummaryfees",";Server; Fraction All FEEs Tagged GL1 BCO",8,0,8);
    
    for(int i=0; i<ninttebdcs+1; i++)
    {
      histoversion->SetBinContent(i+1, inttebdcfrac[i]);
      histoversionfees->SetBinContent(i+1, nInttTaggedBCOsServers[i]);
    }

    histoversion->SetBinContent(9, inttebdcfrac[8]);

    if (! gROOT->FindObject("intt_evt_building_1"))
    {
      MakeCanvas("intt_evt_building_1", 2);
    }
    TC[2]->Clear("D");
    Pad[2][0]->cd();
    gStyle->SetOptStat(0);
    histoversion->GetYaxis()->SetRangeUser(0,1);
    histoversion->GetXaxis()->SetBinLabel(1,"0");
    histoversion->GetXaxis()->SetBinLabel(2,"1");
    histoversion->GetXaxis()->SetBinLabel(3,"2");
    histoversion->GetXaxis()->SetBinLabel(4,"3"); 
    histoversion->GetXaxis()->SetBinLabel(5,"4");
    histoversion->GetXaxis()->SetBinLabel(6,"5");
    histoversion->GetXaxis()->SetBinLabel(7,"6");
    histoversion->GetXaxis()->SetBinLabel(8,"7");
    histoversion->GetXaxis()->SetBinLabel(9,"All");

    histoversion->GetXaxis()->SetTitle("Server");
    histoversion->DrawCopy("hist");
    //myText(0.22,0.5,kBlack,"#bf{#it{sPHENIX}} internal");
    //mText(0.22,0.42,kBlack,"44681, 200 events");
    gPad->SetRightMargin(0.15);

    Pad[2][1]->cd();
    gStyle->SetOptStat(0);
    histoversionfees->GetYaxis()->SetRangeUser(0,1);
    histoversionfees->GetXaxis()->SetBinLabel(1,"0");
    histoversionfees->GetXaxis()->SetBinLabel(2,"1");
    histoversionfees->GetXaxis()->SetBinLabel(3,"2");
    histoversionfees->GetXaxis()->SetBinLabel(4,"3");
    histoversionfees->GetXaxis()->SetBinLabel(5,"4");
    histoversionfees->GetXaxis()->SetBinLabel(6,"5");
    histoversionfees->GetXaxis()->SetBinLabel(7,"6");
    histoversionfees->GetXaxis()->SetBinLabel(8,"7");
    histoversionfees->DrawCopy("hist");
    gPad->SetRightMargin(0.15);
    
    if (! gROOT->FindObject("intt_evt_building_2"))
    {
      MakeCanvas("intt_evt_building_2", 3);
    }
    TC[3]->Clear("D");
    for(int i=0; i<ninttebdcs; i++)
    {
      Pad[3][i]->cd();
      inttebdcfeegr[i]->SetTitle((boost::format("Server %i") % i).str().c_str());
      inttebdcfeegr[i]->GetXaxis()->SetTitle("FEE ID");
      inttebdcfeegr[i]->GetYaxis()->SetTitle("Fraction Tagged GL1BCOs");
      inttebdcfeegr[i]->GetYaxis()->SetRangeUser(0,1);
      inttebdcfeegr[i]->Draw("ap");
      //myText(0.2,0.25,kBlack,(boost::format("Server %i") % (i)).str().c_str(),0.07);
    }
  }
  else
  {
    // histogram is missing
    return -1;
  }
     
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  std::ostringstream runnostream1;
  std::string runstring1;
  runnostream1 << Name() << "_intt evt building Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[2]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());
  transparent[3]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[2]->Update();
  TC[3]->Update();
   
  std::cout << "DrawINTT Ending" << std::endl;
  return 0;
}

int BCODraw::DrawTPC()
{
  std::cout << "BCO DrawTPC() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  const int ntpcpackets = 2;
  const int ntpcebdcs = 24;

  auto h_tpcrefgl1 = dynamic_cast <TH1 *> (cl->getHisto("h_TpcPoolQA_RefGL1BCO"));
  auto h_tpctagall = dynamic_cast <TH1 *> (cl->getHisto("h_TpcPoolQA_TagBCOAllPackets"));
    
  if (h_tpcrefgl1 && h_tpctagall)
  {
    const int nTpcGL1BCOs = h_tpcrefgl1->GetEntries();
    TH1* tpcTagEbdc[ntpcebdcs][ntpcpackets];
    int nTpcTagEbdc[ntpcebdcs][ntpcpackets];
    float tpcTagEbdcFrac[ntpcebdcs][ntpcpackets];
    //float tpcEbdcPacket[ntpcpackets];
    //TGraph *tpcEbdcFracGr[ntpcebdcs];
    for(int i=0; i<ntpcebdcs; i++)
    {
      for(int j=0; j<ntpcpackets; j++)
        {
          tpcTagEbdc[i][j] = (dynamic_cast <TH1 *> (cl->getHisto((boost::format("h_TpcPoolQA_TagBCO_ebdc%i_packet%i") % (i) % (j)).str()))); 
          nTpcTagEbdc[i][j] = tpcTagEbdc[i][j]->GetEntries();
          tpcTagEbdcFrac[i][j] = (float)nTpcTagEbdc[i][j] / nTpcGL1BCOs;
          //tpcEbdcPacket[j] = j;
        }
      //tpcEbdcFracGr[i] = new TGraph(ntpcpackets, tpcEbdcPacket, tpcTagEbdcFrac[i]);
    }

    TH1 *tpchistosummary = new TH1F("tpchistosummary",";Packet ID;Fraction Tagged GL1 BCOs", 49,0,49);
    for(int i=0; i<48; i++)
    {
      int ebdc = i / 2;
      tpchistosummary->SetBinContent(i+1, tpcTagEbdcFrac[ebdc][i%2]);
    }
    tpchistosummary->SetBinContent(49, h_tpctagall->GetEntries() / nTpcGL1BCOs);
    
    if (! gROOT->FindObject("tpc_evt_building_1"))
    {
      MakeCanvas("tpc_evt_building_1", 4);
    }
    TC[4]->Clear("D");
    Pad[4][0]->cd();
    gStyle->SetOptStat(0);
    tpchistosummary->GetYaxis()->SetRangeUser(0,1);
    tpchistosummary->GetXaxis()->SetBinLabel(1, "4000");
    tpchistosummary->GetXaxis()->SetBinLabel(2, "4001");
    tpchistosummary->GetXaxis()->SetBinLabel(3, "4010");
    tpchistosummary->GetXaxis()->SetBinLabel(4, "4011");
    tpchistosummary->GetXaxis()->SetBinLabel(5, "4020");
    tpchistosummary->GetXaxis()->SetBinLabel(6, "4021");
    tpchistosummary->GetXaxis()->SetBinLabel(7, "4030");
    tpchistosummary->GetXaxis()->SetBinLabel(8, "4031");
    tpchistosummary->GetXaxis()->SetBinLabel(9, "4040");
    tpchistosummary->GetXaxis()->SetBinLabel(10, "4041");
    tpchistosummary->GetXaxis()->SetBinLabel(11, "4050");
    tpchistosummary->GetXaxis()->SetBinLabel(12, "4051");
    tpchistosummary->GetXaxis()->SetBinLabel(13, "4060");
    tpchistosummary->GetXaxis()->SetBinLabel(14, "4061");
    tpchistosummary->GetXaxis()->SetBinLabel(15, "4070");
    tpchistosummary->GetXaxis()->SetBinLabel(16, "4071");
    tpchistosummary->GetXaxis()->SetBinLabel(17, "4080");
    tpchistosummary->GetXaxis()->SetBinLabel(18, "4081");
    tpchistosummary->GetXaxis()->SetBinLabel(19, "4090");
    tpchistosummary->GetXaxis()->SetBinLabel(20, "4091");
    tpchistosummary->GetXaxis()->SetBinLabel(21, "4100");
    tpchistosummary->GetXaxis()->SetBinLabel(22, "4101");
    tpchistosummary->GetXaxis()->SetBinLabel(23, "4110");
    tpchistosummary->GetXaxis()->SetBinLabel(24, "4111");
    tpchistosummary->GetXaxis()->SetBinLabel(25, "4120");
    tpchistosummary->GetXaxis()->SetBinLabel(26, "4121");
    tpchistosummary->GetXaxis()->SetBinLabel(27, "4130");
    tpchistosummary->GetXaxis()->SetBinLabel(28, "4131");
    tpchistosummary->GetXaxis()->SetBinLabel(29, "4140");
    tpchistosummary->GetXaxis()->SetBinLabel(30, "4141");
    tpchistosummary->GetXaxis()->SetBinLabel(31, "4150");
    tpchistosummary->GetXaxis()->SetBinLabel(32, "4151");
    tpchistosummary->GetXaxis()->SetBinLabel(33, "4160");
    tpchistosummary->GetXaxis()->SetBinLabel(34, "4161");
    tpchistosummary->GetXaxis()->SetBinLabel(35, "4170");
    tpchistosummary->GetXaxis()->SetBinLabel(36, "4171");
    tpchistosummary->GetXaxis()->SetBinLabel(37, "4180");
    tpchistosummary->GetXaxis()->SetBinLabel(38, "4181");
    tpchistosummary->GetXaxis()->SetBinLabel(39, "4190");
    tpchistosummary->GetXaxis()->SetBinLabel(40, "4191");
    tpchistosummary->GetXaxis()->SetBinLabel(41, "4200");
    tpchistosummary->GetXaxis()->SetBinLabel(42, "4201");
    tpchistosummary->GetXaxis()->SetBinLabel(43, "4210");
    tpchistosummary->GetXaxis()->SetBinLabel(44, "4211");
    tpchistosummary->GetXaxis()->SetBinLabel(45, "4220");
    tpchistosummary->GetXaxis()->SetBinLabel(46, "4221");
    tpchistosummary->GetXaxis()->SetBinLabel(47, "4230");
    tpchistosummary->GetXaxis()->SetBinLabel(48, "4231");
    tpchistosummary->GetXaxis()->SetBinLabel(49, "All");
    tpchistosummary->GetXaxis()->SetLabelSize(0.03);
    tpchistosummary->GetXaxis()->LabelsOption("v");
    tpchistosummary->DrawCopy("hist");
    gPad->SetRightMargin(0.15);  
  }
  else
  {
    // histogram is missing
    return -1;
  }
     
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  std::ostringstream runnostream1;
  std::string runstring1;
  runnostream1 << Name() << "_tpc evt building Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[4]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[4]->Update();
   
  std::cout << "DrawTPC Ending" << std::endl;
  return 0;
}

int BCODraw::DrawTPOT()
{
  std::cout << "BCO DrawTPOT() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  auto h_tpotstat = dynamic_cast <TH1 *> (cl->getHisto("h_MicromegasBCOQA_packet_stat"));
    
  if (h_tpotstat)
  {
    auto h_copy = new TH1F(
      "h_MicromegasBCOQA_packet_stat_copy",
      "Matching BCO count per packet",
      h_tpotstat->GetNbinsX(), h_tpotstat->GetXaxis()->GetXmin(), h_tpotstat->GetXaxis()->GetXmax() );

    h_copy->GetXaxis()->SetTitle(h_tpotstat->GetXaxis()->GetTitle() );
    for( int i = 0; i < h_tpotstat->GetNbinsX(); ++i )
    { 
      h_copy->GetXaxis()->SetBinLabel( i+1, h_tpotstat->GetXaxis()->GetBinLabel(i+1) ); 
    }

    // normalize
    auto norm = h_tpotstat->GetBinContent(1);

    for( int i = 0; i < 3; ++i )
    { 
      h_copy->SetBinContent( i+1, h_tpotstat->GetBinContent(i+1)/norm ); 
    }

    h_copy->SetBinContent( 4, h_tpotstat->GetBinContent(4)/norm );

    if (! gROOT->FindObject("tpot_evt_building_1"))
    {
      MakeCanvas("tpot_evt_building_1", 5);
    }
    TC[5]->Clear("D");
    Pad[5][0]->cd();
    gStyle->SetOptStat(0);
    h_copy->SetMinimum(0.8);
    h_copy->GetYaxis()->SetTitle("GL1 trigger fraction" );
    h_copy->SetFillStyle(1001);
    h_copy->SetFillColor(kYellow);
    h_copy->SetStats(false);
    h_copy->GetYaxis()->SetTitleOffset(1.4);
    h_copy->DrawCopy("hist");

    // add information
    auto text = new TPaveText(0.2, 0.2, 0.5, 0.4, "NDC" );
    text->SetFillColor(0);
    text->SetFillStyle(0);
    text->SetBorderSize(0);
    text->SetTextAlign(11);
    text->AddText( (boost::format( "Runnumber: %i") % cl->RunNumber() ).str().c_str() );
    text->AddText( (boost::format("build: %s") % cl->build()).str().c_str() );
    text->AddText( (boost::format( "Triggers: %.3g") % double(norm) ).str().c_str() );
    text->Draw();
    gPad->SetLeftMargin(0.15);     
  }
  else
  {
    // histogram is missing
    return -1;
  }
     
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  std::ostringstream runnostream1;
  std::string runstring1;
  runnostream1 << Name() << "_tpot evt building Run " << cl->RunNumber();
  runstring1 = runnostream1.str();
  transparent[5]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());
  
  TC[5]->Update();
   
  std::cout << "DrawTPOT Ending" << std::endl;
  return 0;
}
 
int BCODraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
  {
    return iret;
  }

  QADrawClient *cl = QADrawClient::instance();
  std::string pngfile;

  // Register the canvas png file to the menu and produces the png file.
  if (what == "ALL" || what == "MVTX")
  {
    pngfile = cl->htmlRegisterPage(*this, "mvtx_evt_building_1", "1", "png");
    cl->CanvasToPng(TC[0], pngfile);
    pngfile = cl->htmlRegisterPage(*this, "mvtx_evt_building_2", "2", "png");
    cl->CanvasToPng(TC[1], pngfile);
  }
  if (what == "ALL" || what == "INTT")
  {
    pngfile = cl->htmlRegisterPage(*this, "intt_evt_building_1", "3", "png");
    cl->CanvasToPng(TC[2], pngfile);
    pngfile = cl->htmlRegisterPage(*this, "intt_evt_building_2", "4", "png");
    cl->CanvasToPng(TC[3], pngfile);
  }
  if (what == "ALL" || what == "TPC")
  {
    pngfile = cl->htmlRegisterPage(*this, "tpc_evt_building_1", "5", "png");
    cl->CanvasToPng(TC[4], pngfile);
  }
  if (what == "ALL" || what == "TPOT")
  {
    pngfile = cl->htmlRegisterPage(*this, "tpot_evt_building_1", "6", "png");
    cl->CanvasToPng(TC[5], pngfile);
  }
  return 0;
}

int BCODraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}
