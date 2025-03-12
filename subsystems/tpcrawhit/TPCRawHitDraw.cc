#include "TPCRawHitDraw.h"

#include <sPhenixStyle.C>

#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>

#include <tpc/TpcMap.h>

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
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TEllipse.h>
#include <TRandom3.h>
#include <THStack.h>

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

TPCRawHitDraw::TPCRawHitDraw(const std::string &name)
  : QADraw(name)
{
  gStyle->SetOptStat(0);
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  histprefix = "h_TpcRawHitQA_"; 
  return;
}

TPCRawHitDraw::~TPCRawHitDraw()
{
  /* delete db; */
  return;
}

int TPCRawHitDraw::Draw(const std::string &what)
{
  /* SetsPhenixStyle(); */
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "SECTOR")
  {
    iret += DrawSectorInfo();
    idraw++;
  }
  if (what == "ALL" || what == "ONLMON")
  {
    iret += DrawOnlMon();
    idraw++;
  }
  if (!idraw)
  {
    std::cout << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int TPCRawHitDraw::MakeCanvas(const std::string &name, int num)
{
  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  // xpos (-1) negative: do not draw menu bar
  TC[num] = new TCanvas(name.c_str(), (boost::format("TPC Raw Hit Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2) , (int) (ysize / 1.2));
  gSystem->ProcessEvents();

  if (num < 24)
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
  else
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "no", 0.05, 0.25, 0.45, 0.75);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "no", 0.5, 0.25, 0.95, 0.75);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
  }
  // this one is used to plot the run number on the canvas
  transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[num]->SetFillStyle(4000);
  transparent[num]->Draw();

  return 0;
}

int TPCRawHitDraw::DrawSectorInfo()
{
  std::cout << "TPCRawHit DrawSectorInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  std::vector<TH1*> h_hits_secs;
  std::vector<TH2*> h_hits_secs_fees;
  std::vector<TH1*> h_hits_secs_laser;
  std::vector<TH2*> h_hits_secs_fees_laser;

  for (int s = 0; s < 24; s++)
  {
    TH1* h = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("nhits_sec") + std::to_string(s)));
    TH2* h2 = dynamic_cast <TH2 *> (cl->getHisto(histprefix + std::string("nhits_sec") + std::to_string(s) + std::string("_fees")));

    h_hits_secs.push_back(h); 
    h_hits_secs_fees.push_back(h2); 
    
    h = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("nhits_sec") + std::to_string(s) + "_laser"));
    h2 = dynamic_cast <TH2 *> (cl->getHisto(histprefix + std::string("nhits_sec") + std::to_string(s) + std::string("_fees_laser")));
    
    h_hits_secs_laser.push_back(h); 
    h_hits_secs_fees_laser.push_back(h2); 
  }

  for (int q = 0; q < 4; q++)
  {
    if (! gROOT->FindObject((boost::format("sec_rawhits_%i") % q).str().c_str()))
    {
      MakeCanvas((boost::format("sec_rawhits_%i") % q).str(), q);
    }
    TC[q]->Clear("D");

    for (int i = 0; i < 6; i++)
    {
      Pad[q][i]->cd();
      if (h_hits_secs[q*6 + i])
      {
        h_hits_secs[q*6 + i]->GetXaxis()->SetNdivisions(5);
        h_hits_secs[q*6 + i]->DrawCopy("HIST");
        gPad->SetRightMargin(0.15);

        TLatex *title = new TLatex();
        title->SetTextSize(0.06);
        title->SetNDC();
        title->DrawLatex(0.6, 0.75, (boost::format("Sector %i") % (q*6 + i)).str().c_str());
      }
      else
      {
        //histogram is missing
        return -1;
      }
    }
  
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_nhits Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[q]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[q]->Update(); 
  }
  
  for (int q = 0; q < 4; q++)
  {
    if (! gROOT->FindObject((boost::format("sec_fee_rawhits_%i") % q).str().c_str()))
    {
      MakeCanvas((boost::format("sec_fee_rawhits_%i") % q).str(), q + 4);
    }
    TC[q + 4]->Clear("D");

    for (int i = 0; i < 6; i++)
    {
      Pad[q + 4][i]->cd();
      if (h_hits_secs_fees[q*6 + i])
      {
        h_hits_secs_fees[q*6 + i]->GetXaxis()->SetTitle((boost::format("FEE (Sector %i)") % (q*6+i)).str().c_str());
        h_hits_secs_fees[q*6 + i]->DrawCopy("COLZ");
        gPad->SetRightMargin(0.15);
        gPad->SetLeftMargin(0.2);
      }
      else
      {
        //histogram is missing
        return -1;
      }
    }
  
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_nhits_fees Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[q + 4]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[q + 4]->Update(); 
  }
  
  for (int q = 0; q < 4; q++)
  {
    if (! gROOT->FindObject((boost::format("sec_laser_rawhits_%i") % q).str().c_str()))
    {
      MakeCanvas((boost::format("sec_laser_rawhits_%i") % q).str(), q+8);
    }
    TC[q+8]->Clear("D");

    for (int i = 0; i < 6; i++)
    {
      Pad[q+8][i]->cd();
      if (h_hits_secs_laser[q*6 + i])
      {
        h_hits_secs_laser[q*6 + i]->GetXaxis()->SetNdivisions(5);
        h_hits_secs_laser[q*6 + i]->DrawCopy("HIST");
        gPad->SetRightMargin(0.15);

        TLatex *title = new TLatex();
        title->SetTextSize(0.06);
        title->SetNDC();
        title->DrawLatex(0.6, 0.75, (boost::format("Sector %i") % (q*6 + i)).str().c_str());
      }
      else
      {
        //histogram is missing
        return -1;
      }
    }
  
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_nhits_laser Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[q+8]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[q+8]->Update(); 
  }
  
  for (int q = 0; q < 4; q++)
  {
    if (! gROOT->FindObject((boost::format("sec_fee_laser_rawhits_%i") % q).str().c_str()))
    {
      MakeCanvas((boost::format("sec_fee_laser_rawhits_%i") % q).str(), q + 12);
    }
    TC[q + 12]->Clear("D");

    for (int i = 0; i < 6; i++)
    {
      Pad[q + 12][i]->cd();
      if (h_hits_secs_fees_laser[q*6 + i])
      {
        h_hits_secs_fees_laser[q*6 + i]->GetXaxis()->SetTitle((boost::format("FEE (Sector %i)") % (q*6+i)).str().c_str());
        h_hits_secs_fees_laser[q*6 + i]->DrawCopy("COLZ");
        gPad->SetRightMargin(0.15);
        gPad->SetLeftMargin(0.2);
      }
      else
      {
        //histogram is missing
        return -1;
      }
    }
  
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_nhits_fees_laser Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[q + 12]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[q + 12]->Update(); 
  }

  std::cout << "DrawSectorInfo Ending" << std::endl;
  return 0;
}
 
int TPCRawHitDraw::DrawOnlMon()
{
  std::cout << "TPCRawHit DrawOnlMon() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  std::vector<TH1*> h_hits_sample_1;
  std::vector<TH1*> h_hits_sample_2;
  std::vector<TH1*> h_hits_sample_3;
  std::vector<TH1*> h_adc_1;
  std::vector<TH1*> h_adc_2;
  std::vector<TH1*> h_adc_3;

  for (int s = 0; s < 24; s++)
  {
    {
      TH1* h1 = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("nhits_sample_sec") + std::to_string(s) + std::string("_R0")));
      TH1* h2 = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("nhits_sample_sec") + std::to_string(s) + std::string("_R1")));
      TH1* h3 = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("nhits_sample_sec") + std::to_string(s) + std::string("_R2")));
      h_hits_sample_1.push_back(h1);
      h_hits_sample_2.push_back(h2);
      h_hits_sample_3.push_back(h3);
    }
    {
      TH1* h1 = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("adc_sec") + std::to_string(s) + std::string("_R0")));
      TH1* h2 = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("adc_sec") + std::to_string(s) + std::string("_R1")));
      TH1* h3 = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("adc_sec") + std::to_string(s) + std::string("_R2")));
      h_adc_1.push_back(h1);
      h_adc_2.push_back(h2);
      h_adc_3.push_back(h3);
    }
  }
  
  TH1* h_northHits = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("xyPos_North")));
  TH1* h_southHits = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("xyPos_South")));

  for (int q = 0; q < 4; q++)
  {
    if (! gROOT->FindObject((boost::format("sampleDist_%i") % q).str().c_str()))
    {
      MakeCanvas((boost::format("sampleDist_%i") % q).str(), q + 16);
    }
    TC[q + 16]->Clear("D");

    for (int i = 0; i < 6; i++)
    {
      Pad[q + 16][i]->cd();
      if (h_hits_sample_1[q*6 + i] && h_hits_sample_2[q*6 + i] && h_hits_sample_3[q*6 + i])
      {
        /*double maxBinContent = 0;
        for (int b = 1; b <= h_hits_sample_1[q*6 + i]->GetNbinsX(); b++) 
        {
          double binContent = std::max({h_hits_sample_1[q*6 + i]->GetBinContent(b), 
                                        h_hits_sample_2[q*6 + i]->GetBinContent(b), 
                                        h_hits_sample_3[q*6 + i]->GetBinContent(b)});
          if (binContent > maxBinContent) 
          {
            maxBinContent = binContent;
          }
        }

        h_hits_sample_1[q*6 + i]->GetYaxis()->SetRangeUser(0, maxBinContent * 1.1); 
        h_hits_sample_2[q*6 + i]->GetYaxis()->SetRangeUser(0, maxBinContent * 1.1); 
        h_hits_sample_3[q*6 + i]->GetYaxis()->SetRangeUser(0, maxBinContent * 1.1);*/
        
        h_hits_sample_1[q*6 + i]->SetLineColor(kRed);
        h_hits_sample_2[q*6 + i]->SetLineColor(kGreen);
        h_hits_sample_3[q*6 + i]->SetLineColor(kBlue);

        THStack *hs = new THStack("hs", "Stacked Histograms");        
        hs->Add(h_hits_sample_1[q*6 + i]);
        hs->Add(h_hits_sample_2[q*6 + i]);
        hs->Add(h_hits_sample_3[q*6 + i]);

        hs->Draw("nostack HIST");

        hs->GetXaxis()->SetTitle((boost::format("Time Bin [1/20 MHz] (Sector %i)") % (q*6+i)).str().c_str());
        hs->GetYaxis()->SetTitle("Entries");
        hs->SetTitle((boost::format("Sector %i Sample Time Distribution") % (q*6 + i)).str().c_str());

        //h_hits_sample_1[q*6 + i]->SetLineColor(kRed);
        //h_hits_sample_1[q*6 + i]->DrawCopy();
        //h_hits_sample_2[q*6 + i]->SetLineColor(kGreen);
        //h_hits_sample_2[q*6 + i]->DrawCopy("same");
        //h_hits_sample_3[q*6 + i]->SetLineColor(kBlue);
        //h_hits_sample_3[q*6 + i]->DrawCopy("same");

        if (i == 0)
        {
          auto legend = new TLegend(0.7, 0.7, 0.9, 0.9);
          legend->AddEntry(h_hits_sample_1[q*6 + i], "R1", "L");
          legend->AddEntry(h_hits_sample_2[q*6 + i], "R2", "L");
          legend->AddEntry(h_hits_sample_3[q*6 + i], "R3", "L");
          legend->SetFillStyle(0);
          legend->Draw();
        }
        //gPad->SetRightMargin(0.15);
      }
      else
      {
        //histogram is missing
        return -1;
      }
    }
  
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_Sample Time Dist Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[q + 16]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[q + 16]->Update(); 
  }
  
  for (int q = 0; q < 4; q++)
  {
    if (! gROOT->FindObject((boost::format("sec_adc_%i") % q).str().c_str()))
    {
      MakeCanvas((boost::format("sec_adc_%i") % q).str(), q + 20);
    }
    TC[q + 20]->Clear("D");

    for (int i = 0; i < 6; i++)
    {
      Pad[q + 20][i]->cd();
      if (h_adc_1[q*6 + i] && h_adc_2[q*6 + i] && h_adc_3[q*6 + i])
      {
        h_adc_1[q*6 + i]->SetLineColor(kRed);
        //h_adc_1[q*6 + i]->GetXaxis()->SetTitle((boost::format("ADC-pedestal [ADU] (Sector %i)") % (q*6 + i)).str().c_str());
        //h_adc_1[q*6 + i]->DrawCopy("HIST");
        h_adc_2[q*6 + i]->SetLineColor(kGreen);
        //h_adc_2[q*6 + i]->GetXaxis()->SetTitle((boost::format("ADC-pedestal [ADU] (Sector %i)") % (q*6 + i)).str().c_str());
        //h_adc_2[q*6 + i]->DrawCopy("HIST same");
        h_adc_3[q*6 + i]->SetLineColor(kBlue);
        //h_adc_3[q*6 + i]->GetXaxis()->SetTitle((boost::format("ADC-pedestal [ADU] (Sector %i)") % (q*6 + i)).str().c_str());
        //h_adc_3[q*6 + i]->DrawCopy("HIST same");
        //double maxBinContent = 0;
        //for (int b = 1; b <= h_adc_1[q*6 + i]->GetNbinsX(); b++) 
        //{
        //  double binContent = std::max({h_adc_1[q*6 + i]->GetBinContent(b), h_adc_2[q*6 + i]->GetBinContent(b), 
        //                                h_adc_3[q*6 + i]->GetBinContent(b)});
        //  if (binContent > maxBinContent) maxBinContent = binContent;
        //}
        //h_adc_1[q*6 + i]->GetYaxis()->SetRangeUser(0, maxBinContent * 1.1); 
        //h_adc_2[q*6 + i]->GetYaxis()->SetRangeUser(0, maxBinContent * 1.1); 
        //h_adc_3[q*6 + i]->GetYaxis()->SetRangeUser(0, maxBinContent * 1.1); 
        
        THStack *hs = new THStack("hs", "Stacked Histograms");        
        hs->Add(h_adc_1[q*6 + i]);
        hs->Add(h_adc_2[q*6 + i]);
        hs->Add(h_adc_3[q*6 + i]);

        hs->Draw("nostack HIST");

        hs->GetXaxis()->SetTitle((boost::format("ADC-pedestal [ADU] (Sector %i)") % (q*6+i)).str().c_str());
        hs->GetYaxis()->SetTitle("Entries");

        if (i == 0)
        {
          auto legend = new TLegend(0.45, 0.7, 0.7, 0.9);
          legend->AddEntry(h_adc_1[q*6 + i], "R1", "L");
          legend->AddEntry(h_adc_2[q*6 + i], "R2", "L");
          legend->AddEntry(h_adc_3[q*6 + i], "R3", "L");
          legend->SetFillStyle(0);
          legend->Draw();
        }
        gPad->SetLogy();
        //gPad->SetRightMargin(0.15);
      }
      else
      {
        //histogram is missing
        return -1;
      }
    }
  
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_adc Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[q + 20]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[q + 20]->Update(); 
  }

  bool outFEEs[24][26] = {{false}};
  float halfFEEs = 12*26;
  float outN = 0;
  float outS = 0;
  for (int s = 0; s < 24; s++)
  {
    TH2* h2 = dynamic_cast <TH2 *> (cl->getHisto(histprefix + std::string("nhits_sec") + std::to_string(s) + std::string("_fees")));
    TProfile* tp = h2->ProfileX();
    for (int f = 0; f < 26; f++)
    {
      if (tp->GetBinContent(f+1) <= 25)
      {
        outFEEs[s][f] = true;
        if (s < 12) outN++;
        else if (s >= 12) outS++;
      }    
    }
  }
  float fracOutN = outN/halfFEEs;
  float fracOutS = outS/halfFEEs;

  int FEE_R[26]{2, 2, 1, 1, 1, 3, 3, 3, 3, 3, 3, 2, 2, 1, 2, 2, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3};
  int FEE_map[26]{4, 5, 0, 2, 1, 11, 9, 10, 8, 7, 6, 0, 1, 3, 7, 6, 5, 4, 3, 2, 0, 2, 1, 3, 5, 4};
  TpcMap M;
  M.setMapNames("AutoPad-R1-RevA.sch.ChannelMapping.csv", "AutoPad-R2-RevA-Pads.sch.ChannelMapping.csv", "AutoPad-R3-RevA.sch.ChannelMapping.csv");

  double FEE_bounds[24][26][2] = {{{0}}};

  for (int sector = 0; sector < 24; sector++)
  {
    for (int fee = 0; fee < 26; fee++)
    {
      int feeM = FEE_map[fee];
      if (FEE_R[fee] == 2)
      {
        feeM += 6;
      }
      if (FEE_R[fee] == 3)
      {
        feeM += 14;
      }
      double lower = 0;
      double upper = 0;
      double phi = 0;
      for (int channel = 0; channel < 256; channel++)
      {
        if (sector < 12)  // NS
        {
          phi = M.getPhi(feeM, channel) + (sector) *M_PI / 6;
        }
        else if (sector >= 12)  // SS
        {
          phi = M.getPhi(feeM, channel) + (18 - sector) * M_PI / 6;
        }

        if(channel == 0)
        {
          lower = phi;
          upper = phi;
        }
        else
        {
          if (phi < lower) lower = phi;
          else if (phi > upper) upper = phi;
        }
      }
      FEE_bounds[sector][fee][0] = lower;
      FEE_bounds[sector][fee][1] = upper;
    }
  }
  float n1FEEs_N = 0;  
  float n2FEEs_N = 0;  
  float n3FEEs_N = 0;
  float n1FEEs_S = 0;  
  float n2FEEs_S = 0;  
  float n3FEEs_S = 0;
  float fracN1FEE = -1;  
  float fracN2FEE = -1;  
  float fracN3FEE = -1;  
  float fracS1FEE = -1;  
  float fracS2FEE = -1;  
  float fracS3FEE = -1;  
  float totalN = 0;  
  float totalS = 0;  
  TRandom3 randGen;
  for (float nRand = 0; nRand < 100000; nRand++)
  {
    // North Bounds: [-0.25336, 6.01289]
    // South Bounds: [-2.87135, 3.39489]
    double randPhi = randGen.Uniform(-0.25336, 6.01289);
    // NS
    for (int sector = 0; sector < 12; sector++)
    {
      bool r1 = false;
      bool r2 = false;
      bool r3 = false;
      bool feeOn1 = false;
      bool feeOn2 = false;
      bool feeOn3 = false;
      bool foundOne = false;
      for (int fee = 0; fee < 26; fee++)
      {
        int region = FEE_R[fee];
        if ((region == 1 && r1) || (region == 2 && r2) || (region == 3 && r3)) continue;

        if (randPhi >= FEE_bounds[sector][fee][0] && randPhi <= FEE_bounds[sector][fee][1])
        {
          foundOne = true;
          if (region == 1)
          {
            r1 = true;
            if (outFEEs[sector][fee] == false) feeOn1 = true;
  
            continue; 
          }
          else if (region == 2)
          {
            r2 = true;
            if (outFEEs[sector][fee] == false) feeOn2 = true; 
  
            continue; 
          }
          else if (region == 3)
          {
            r3 = true;
            if (outFEEs[sector][fee] == false) feeOn3 = true; 
  
            continue; 
          }
        } 
      }
      if (foundOne)
      {
        if (!(r1 && r2 && r3)) 
        {
          break;
        }
        totalN++;
        if (feeOn1 && feeOn2 && feeOn3) n3FEEs_N++;
        if ((feeOn1 && feeOn2) || (feeOn1 && feeOn3) || (feeOn2 && feeOn3)) n2FEEs_N++;
        if (feeOn1 || feeOn2 || feeOn3) n1FEEs_N++;

        break; 
      }
    }

    randPhi = randGen.Uniform(-2.87135, 3.39489);
    // SS
    for (int sector = 12; sector < 24; sector++)
    {
      bool r1 = false;
      bool r2 = false;
      bool r3 = false;
      bool feeOn1 = false;
      bool feeOn2 = false;
      bool feeOn3 = false;
      bool foundOne = false;
      for (int fee = 0; fee < 26; fee++)
      {
        int region = FEE_R[fee];
        if ((region == 1 && r1) || (region == 2 && r2) || (region == 3 && r3)) continue;

        if (randPhi >= FEE_bounds[sector][fee][0] && randPhi <= FEE_bounds[sector][fee][1])
        {
          foundOne = true;
          if (region == 1)
          {
            r1 = true;
            if (outFEEs[sector][fee] == false) feeOn1 = true;
  
            continue; 
          }
          else if (region == 2)
          {
            r2 = true;
            if (outFEEs[sector][fee] == false) feeOn2 = true; 
  
            continue; 
          }
          else if (region == 3)
          {
            r3 = true;
            if (outFEEs[sector][fee] == false) feeOn3 = true; 
  
            continue; 
          }
        } 
      }
      if (foundOne)
      {
        if (!(r1 && r2 && r3)) 
        {
          break;
        }
        totalS++;
        if (feeOn1 && feeOn2 && feeOn3) n3FEEs_S++;
        if ((feeOn1 && feeOn2) || (feeOn1 && feeOn3) || (feeOn2 && feeOn3)) n2FEEs_S++;
        if (feeOn1 || feeOn2 || feeOn3) n1FEEs_S++;

        break; 
      }
    }
  }  
  fracN1FEE = n1FEEs_N/totalN;    
  fracN2FEE = n2FEEs_N/totalN;    
  fracN3FEE = n3FEEs_N/totalN;    
  fracS1FEE = n1FEEs_S/totalS;    
  fracS2FEE = n2FEEs_S/totalS;    
  fracS3FEE = n3FEEs_S/totalS;    

  std::ostringstream fracnostream1, fracnostream2, fracnostream3, feeOutstream;
  std::string fracstring1, fracstring2, fracstring3, feeOutString;
  fracnostream1 << "Fraction of >= 1 FEE tracks: North = " << fracN1FEE << ", South = " << fracS1FEE << std::endl; 
  fracnostream2 << "Fraction of >= 2 FEE tracks: North = " << fracN2FEE << ", South = " << fracS2FEE << std::endl; 
  fracnostream3 << "Fraction of 3 FEE tracks: North = " << fracN3FEE << ", South = " << fracS3FEE << std::endl;
  feeOutstream << "Fraction of FEEs Out: North = " << fracOutN << ", South = " << fracOutS << std::endl;
  fracstring1 = fracnostream1.str();   
  fracstring2 = fracnostream2.str();   
  fracstring3 = fracnostream3.str();   
  feeOutString = feeOutstream.str();

  if (! gROOT->FindObject("rawhit_xy"))
  {
    MakeCanvas("rawhit_xy", 24);
  }
  TC[24]->Clear("D");
  TLatex *title1 = new TLatex();
  title1->SetTextSize(0.04);
  title1->SetNDC();
  title1->DrawLatex(0.15, 0.9, fracstring1.c_str());
  TLatex *title2 = new TLatex();
  title2->SetTextSize(0.04);
  title2->SetNDC();
  title2->DrawLatex(0.15, 0.85, fracstring2.c_str());
  TLatex *title3 = new TLatex();
  title3->SetTextSize(0.04);
  title3->SetNDC();
  title3->DrawLatex(0.15, 0.8, fracstring3.c_str());
  TLatex *title4 = new TLatex();
  title4->SetTextSize(0.04);
  title4->SetNDC();
  title4->DrawLatex(0.15, 0.75, feeOutString.c_str());
  Pad[24][0]->cd();
  if (h_northHits)
  {
    h_northHits->DrawCopy("COLZ");

    Double_t sec_gap_inner = (2*M_PI - 0.5024*12.0)/12.0;
    Double_t sec_gap_outer = (2*M_PI - 0.5097*12.0)/12.0;
    Double_t sec_gap = (sec_gap_inner + sec_gap_outer)/2.0;
    Double_t sec_phi = (0.5024 + 0.5097)/2.0;
    TLine *lines[12];
    for(int ln=0;ln<12;ln++)
    {
      lines[ln] = new TLine(311.05*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),311.05*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)));
    }
    TEllipse *e1 = new TEllipse(0.0,0.0,311.05,311.05);
    TEllipse *e2 = new TEllipse(0.0,0.0,(402.49+411.53)/2.0,(402.49+411.53)/2.0);
    TEllipse *e3 = new TEllipse(0.0,0.0,(583.67+574.75)/2.0,(583.67+574.75)/2.0);
    TEllipse *e4 = new TEllipse(0.0,0.0,759.11,759.11);
    e1->SetFillStyle(0);
    e2->SetFillStyle(0);
    e3->SetFillStyle(0);
    e4->SetFillStyle(0);

    e1->Draw("same");
    e2->Draw("same");
    e3->Draw("same");
    e4->Draw("same");
    for(int ln2=0;ln2<12;ln2++)
    {
      lines[ln2]->Draw("same"); 
    }
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[24][1]->cd();
  if (h_southHits)
  {
    h_southHits->DrawCopy("COLZ");

    Double_t sec_gap_inner = (2*M_PI - 0.5024*12.0)/12.0;
    Double_t sec_gap_outer = (2*M_PI - 0.5097*12.0)/12.0;
    Double_t sec_gap = (sec_gap_inner + sec_gap_outer)/2.0;
    Double_t sec_phi = (0.5024 + 0.5097)/2.0;
    TLine *lines[12];
    for(int ln=0;ln<12;ln++)
    {
      lines[ln] = new TLine(311.05*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),311.05*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)));
    }
    TEllipse *e1 = new TEllipse(0.0,0.0,311.05,311.05);
    TEllipse *e2 = new TEllipse(0.0,0.0,(402.49+411.53)/2.0,(402.49+411.53)/2.0);
    TEllipse *e3 = new TEllipse(0.0,0.0,(583.67+574.75)/2.0,(583.67+574.75)/2.0);
    TEllipse *e4 = new TEllipse(0.0,0.0,759.11,759.11);
    e1->SetFillStyle(0);
    e2->SetFillStyle(0);
    e3->SetFillStyle(0);
    e4->SetFillStyle(0);

    e1->Draw("same");
    e2->Draw("same");
    e3->Draw("same");
    e4->Draw("same");
    for(int ln2=0;ln2<12;ln2++)
    {
      lines[ln2]->Draw("same"); 
    }
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
  runnostream1 << Name() << "_hitsXY Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[24]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[24]->Update(); 
 
  std::cout << "DrawOnlMon Ending" << std::endl;
  return 0;
}
int TPCRawHitDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
  {
    return iret;
  }

  QADrawClient *cl = QADrawClient::instance();
  std::string pngfile;

  // Register the 1st canvas png file to the menu and produces the png file.
  if (what == "ALL" || what == "SECTOR")
  {
    for (int q = 0; q < 4; q++)
    {
      pngfile = cl->htmlRegisterPage(*this, (boost::format("sec_rawhits_%i") % q).str(), (boost::format("%i") % (q + 1)).str(), "png");
      cl->CanvasToPng(TC[q], pngfile);
    }
    for (int q = 0; q < 4; q++)
    {
      pngfile = cl->htmlRegisterPage(*this, (boost::format("sec_fee_rawhits_%i") % q).str(), (boost::format("%i") % (q + 5)).str(), "png");
      cl->CanvasToPng(TC[q + 4], pngfile);
    }
    for (int q = 0; q < 4; q++)
    {
      pngfile = cl->htmlRegisterPage(*this, (boost::format("sec_laser_rawhits_%i") % q).str(), (boost::format("%i") % (q + 9)).str(), "png");
      cl->CanvasToPng(TC[q + 8], pngfile);
    }
    for (int q = 0; q < 4; q++)
    {
      pngfile = cl->htmlRegisterPage(*this, (boost::format("sec_fee_laser_rawhits_%i") % q).str(), (boost::format("%i") % (q + 13)).str(), "png");
      cl->CanvasToPng(TC[q + 12], pngfile);
    }
  }
  if (what == "ALL" || what == "ONLMON")
  {
    for (int q = 0; q < 4; q++)
    {
      pngfile = cl->htmlRegisterPage(*this, (boost::format("sampleDist_%i") % q).str(), (boost::format("%i") % (q + 17)).str(), "png");
      cl->CanvasToPng(TC[q + 16], pngfile);
    }
    for (int q = 0; q < 4; q++)
    {
      pngfile = cl->htmlRegisterPage(*this, (boost::format("sec_adc_%i") % q).str(), (boost::format("%i") % (q + 21)).str(), "png");
      cl->CanvasToPng(TC[q + 20], pngfile);
    }
    pngfile = cl->htmlRegisterPage(*this, "rawhit_xy", "25", "png");
    cl->CanvasToPng(TC[24], pngfile);
  }
  return 0;
}

int TPCRawHitDraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}
