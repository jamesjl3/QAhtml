#include "CaloDraw.h"
#include "CaloGoodRunChecker.h"

#include <sPhenixStyle.C>

#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>

#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile2D.h>
#include <TLatex.h>
#include <TString.h>
#include <TPad.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TGaxis.h>

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

CaloDraw::CaloDraw(const std::string &name)
  : QADraw(name)
{
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  histprefix = "h_CaloValid_";
  return;
}

CaloDraw::~CaloDraw()
{
  /* delete db; */
  return;
}

int CaloDraw::Draw(const std::string &what)
{
  /* SetsPhenixStyle(); 
  gStyle->SetTitleSize(gStyle->GetTitleSize("X")*2.0, "X");
  gStyle->SetTitleSize(gStyle->GetTitleSize("Y")*2.0, "Y");
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetTitleOffset(0.85, "XY");
  gStyle->SetLabelSize(gStyle->GetLabelSize("X")*1.5, "X");
  gStyle->SetLabelSize(gStyle->GetLabelSize("Y")*1.5, "Y");
  gStyle->SetLabelSize(gStyle->GetLabelSize("Z")*1.5, "Z");
  TGaxis::SetMaxDigits(4);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptStat(10);
  gROOT->ForceStyle();
*/
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "CEMC")
  {
    iret += DrawCemc();
    idraw++;
    /* std::cout << "iret = " << iret << ", idraw = " << idraw << std::endl; */
  }
  if (what == "ALL" || what == "IHCAL")
  {
    iret += DrawIhcal();
    idraw++;
    /* std::cout << "iret = " << iret << ", idraw = " << idraw << std::endl; */
  }
  if (what == "ALL" || what == "OHCAL")
  {
    iret += DrawOhcal();
    idraw++;
    /* std::cout << "iret = " << iret << ", idraw = " << idraw << std::endl; */
  }
  if (what == "ALL" || what == "CORR")
  {
    iret += DrawCorr();
    idraw++;
    /* std::cout << "iret = " << iret << ", idraw = " << idraw << std::endl; */
  }
  if (what == "ALL" || what == "ZDC")
  {
    iret += DrawZdcMbd();
    idraw++;
    /* std::cout << "iret = " << iret << ", idraw = " << idraw << std::endl; */
  }
  if (!idraw)
  {
    std::cout << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int CaloDraw::MakeCanvas(const std::string &name, int num)
{
  /* QADrawClient *cl = QADrawClient::instance(); */
  /* int xsize = cl->GetDisplaySizeX(); */
  /* int ysize = cl->GetDisplaySizeY(); */
  /* TC[num] = new TCanvas(name.c_str(), (boost::format("Calo Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2), (int) (ysize / 1.2)); */
  // xpos (-1) negative: do not draw menu bar
  TC[num] = new TCanvas(name.c_str(), (boost::format("Calo Plots %d") % num).str().c_str(), -1, 0, canvas_xsize, canvas_ysize);
  TC[num]->UseCurrentStyle();
  gSystem->ProcessEvents();

  if (num==1 || num==11 || num==12 || num==13)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.25, 0.45, 0.75, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.5, 0.25, 0.95, 0.75, 0);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
  }
  else if (num==7)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "", 0.05, 0.05, 0.95, 0.95, 0);
    Pad[num][0]->Draw();
  }
  else if (num==5)
  {
    TC[num]->Divide(3, 2, 0.025, 0.025);
    Pad[num][0] = (TPad*)TC[num]->GetPad(1);
    Pad[num][1] = (TPad*)TC[num]->GetPad(2);
    Pad[num][2] = (TPad*)TC[num]->GetPad(3);
    Pad[num][3] = (TPad*)TC[num]->GetPad(4);
    Pad[num][4] = (TPad*)TC[num]->GetPad(5);
    Pad[num][5] = (TPad*)TC[num]->GetPad(6);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
    Pad[num][2]->Draw();
    Pad[num][3]->Draw();
    Pad[num][4]->Draw();
    Pad[num][5]->Draw();
  }
  else
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.52, 0.45, 0.97, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.5, 0.52, 0.95, 0.97, 0);
    Pad[num][2] = new TPad((boost::format("mypad%d2") % num).str().c_str(), "name", 0.05, 0.02, 0.45, 0.47, 0);
    Pad[num][3] = new TPad((boost::format("mypad%d3") % num).str().c_str(), "here", 0.5, 0.02, 0.95, 0.47, 0);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
    Pad[num][2]->Draw();
    Pad[num][3]->Draw();
  }

  // this one is used to plot the run number on the canvas
  transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[num]->SetFillStyle(4000);
  transparent[num]->Draw();

  return 0;
}

int CaloDraw::DrawCemc()
{
  QADrawClient *cl = QADrawClient::instance();

  TH2 *cemc_e_chi2 = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("cemc_e_chi2")));
  TH2F *cemc_etaphi = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("cemc_etaphi")));
  TH2 *cemc_etaphi_time = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("cemc_etaphi_time")));
  TH1F *emcal_proj = (TH1F *) proj(cemc_etaphi)->Clone("h_emcal_proj");
  TH1 *invMass = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("InvMass")));
  TH2 *etaphi_clus = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("etaphi_clus")));
  TProfile2D *h_CaloValid_cemc_etaphi_pedRMS = dynamic_cast<TProfile2D *>(cl->getHisto(histprefix + std::string("cemc_etaphi_pedRMS")));
  TProfile2D *h_CaloValid_cemc_etaphi_ZSpedRMS = dynamic_cast<TProfile2D *>(cl->getHisto(histprefix + std::string("cemc_etaphi_ZSpedRMS")));
  TH2 *cemc_hotmap = nullptr;
  if (calo_checker) cemc_hotmap = calo_checker->cemc_hcdmap;
  /* TH1 *cemc_etaphi_wQA = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("cemc_etaphi_wQA"))); */
  
  // canvas 1
  if (!gROOT->FindObject("cemc1"))
  {
    MakeCanvas("cemc1", 0);
  }
  /* TC[0]->Clear("D"); */
  Pad[0][0]->cd();
  if (cemc_e_chi2)
  {
    cemc_e_chi2->SetTitle("EMCal #chi^{2} vs Energy");
    cemc_e_chi2->SetXTitle("Tower E (GeV) EMCal");
    cemc_e_chi2->SetYTitle("Tower #chi^{2} EMCal");
    cemc_e_chi2->GetXaxis()->SetNdivisions(505);
    cemc_e_chi2->GetXaxis()->SetRangeUser(-1, 15);
    cemc_e_chi2->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetLogy();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[0][1]->cd();
  if (cemc_etaphi)
  {
    cemc_etaphi->SetTitle("EMCal Occupancy");
    cemc_etaphi->SetXTitle("#it{#eta}_{i} EMCal");
    cemc_etaphi->SetYTitle("#it{#phi}_{i} EMCal");
    cemc_etaphi->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }
  Pad[0][2]->cd();
  if (cemc_etaphi_time)
  {
    cemc_etaphi_time->SetTitle("EMCal Mean Signal Peak Time");
    cemc_etaphi_time->SetXTitle("#it{#eta}_{i} EMCal");
    cemc_etaphi_time->SetYTitle("#it{#phi}_{i} EMCal");
    cemc_etaphi_time->GetXaxis()->SetNdivisions(505);
    cemc_etaphi_time->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  Pad[0][3]->cd();
  if (emcal_proj)
  {
    emcal_proj->SetTitle("EMCal #eta Projection");
    emcal_proj->SetXTitle("#it{#eta}_{i} EMCal");
    /* emcal_proj->GetXaxis()->SetNdivisions(505); */
    emcal_proj->SetYTitle("N^{twr}(E_{T} > 1 GeV)");
    emcal_proj->DrawCopy("HIST");
    gPad->UseCurrentStyle();
  }

  // canvas 2
  if (!gROOT->FindObject("cemc2"))
  {
    MakeCanvas("cemc2", 1);
  }
  /* TC[1]->Clear("D"); */
  Pad[1][0]->cd();
  if (invMass)
  {
    invMass->SetTitle("EMCal Diphoton Invariant Mass");
    invMass->SetXTitle("M_{#gamma #gamma} (GeV)");
    invMass->SetYTitle("Counts");
    invMass->DrawCopy();
    gPad->UseCurrentStyle();
  }
  Pad[1][1]->cd();
  if (etaphi_clus)
  {
    etaphi_clus->SetTitle("EMCal Clusters");
    etaphi_clus->SetXTitle("#it{#eta}_{i} EMCal Clusters");
    etaphi_clus->SetYTitle("#it{#phi}_{i} EMCal Clusters");
    etaphi_clus->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }

  // do "summary" canvas before tower masking canvas, so CemcGoodRun gets called first
  // canvas 4
  /* if (!gROOT->FindObject("cemc4")) */
  /* { */
  /*   MakeCanvas("cemc4", 7); */
  /* } */
  /* /1* TC[7]->Clear("D"); *1/ */
  /* Pad[7][0]->cd(); */
  /* bool isgoodrun = CemcGoodRun(); */
  /* // n_events, hot/cold/dead_towers, cemc_time_* and vtxz_* are now filled */

  // canvas 3
  if (!gROOT->FindObject("cemc3"))
  {
    MakeCanvas("cemc3", 6);
  }
  /* TC[6]->Clear("D"); */
  Pad[6][0]->cd();
  if (cemc_hotmap)
  {
    cemc_hotmap->SetTitle("EMCal Hot Tower Mask");
    cemc_hotmap->SetXTitle("#it{#eta}_{i}");
    cemc_hotmap->SetYTitle("#it{#phi}_{i}");
    // change to a discrete color palette
    int palette[4] = {kWhite, kGray+2, kRed, kBlue};
    cemc_hotmap->GetZaxis()->SetRangeUser(-0.5,3.5);
    cemc_hotmap->DrawCopy("COLZ");
    gStyle->SetPalette(4, palette);
    gPad->UseCurrentStyle();
    gPad->Update();
    gPad->SetRightMargin(0.15);
  }
  Pad[6][1]->cd();
  gStyle->SetPalette(kBird);
  TH2 *h_hitmask = nullptr;
  int nonzero_towers = 0;
  // Make masked tower histogram
  if (cemc_etaphi && cemc_hotmap)
  {
    h_hitmask = (TH2*)cemc_etaphi->Clone("h_hitmask");
    int nbins = cemc_hotmap->GetNcells();
    for (int i=0; i<=nbins; i++)
    {
      if (cemc_hotmap->GetBinContent(i) != 0)
      {
	h_hitmask->SetBinContent(i, 0);
	nonzero_towers++;
      }
    }
    h_hitmask->SetTitle("EMCal Tower Hits w/ Masking");
    h_hitmask->SetXTitle("#it{#eta}_{i}");
    h_hitmask->SetYTitle("#it{#phi}_{i}");
    h_hitmask->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  Pad[6][2]->cd();
  myText(0.25, 0.80, kBlack, "Hot Tower Mask Legend:");
  myText(0.25, 0.75, kBlack, "0/empty = good tower");
  myText(0.25, 0.70, kGray+2, "1 = dead tower");
  myText(0.25, 0.65, kRed, "2 = hot tower");
  myText(0.25, 0.60, kBlue, "3 = cold tower");
  int dead_towers = 999999; int hot_towers = 999999; int cold_towers = 999999;
  if (calo_checker)
  {
    dead_towers = calo_checker->cemc_dead_towers;
    hot_towers = calo_checker->cemc_hot_towers;
    cold_towers = calo_checker->cemc_cold_towers;
  }
  myText(0.75, 0.70, kBlack, Form("This run: %d dead, %d hot, %d cold", dead_towers, hot_towers, cold_towers), 0.06);
  myText(0.75, 0.62, kBlack, "Expected: 128 dead, 0 hot, 0 cold", 0.06);
  Pad[6][3]->cd();
  TH1F *emcal_proj_masked = nullptr;
  if (h_hitmask) emcal_proj_masked = (TH1F *) proj(h_hitmask)->Clone("h_emcal_proj_masked");
  if (emcal_proj_masked)
  {
    emcal_proj_masked->SetTitle("EMCal #eta Projection w/ Masking");
    emcal_proj_masked->SetXTitle("#it{#eta}_{i} EMCal");
    /* emcal_proj_masked->GetXaxis()->SetNdivisions(505); */
    emcal_proj_masked->SetYTitle("N^{twr}(E_{T} > 1 GeV)");
    emcal_proj_masked->DrawCopy("HIST");
    gPad->UseCurrentStyle();
  }
  if (!gROOT->FindObject("cemc4"))
  {
    MakeCanvas("cemc4", 11);
  }
  Pad[11][0]->cd();
  if (h_CaloValid_cemc_etaphi_pedRMS)
  {
    h_CaloValid_cemc_etaphi_pedRMS->SetTitle("CaloValid EMCal Eta-Phi Ped RMS");
    h_CaloValid_cemc_etaphi_pedRMS->SetXTitle("#it{#eta}_{i} EMCal");
    h_CaloValid_cemc_etaphi_pedRMS->SetYTitle("#it{#phi}_{i} EMCal");
    h_CaloValid_cemc_etaphi_pedRMS->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  Pad[11][1]->cd();
  if (h_CaloValid_cemc_etaphi_ZSpedRMS)
  {
    h_CaloValid_cemc_etaphi_ZSpedRMS->SetTitle("CaloValid EMCal Eta-Phi ZSped RMS");
    h_CaloValid_cemc_etaphi_ZSpedRMS->SetXTitle("#it{#eta}_{i} EMCal");
    h_CaloValid_cemc_etaphi_ZSpedRMS->SetYTitle("#it{#phi}_{i} EMCal");
    h_CaloValid_cemc_etaphi_ZSpedRMS->Draw("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }

  // remove this plot 
  /*
  TH2 *ohcal_etaphi = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("ohcal_etaphi")));
  TH1 *ohcal_proj = (TH1F *) proj(ohcal_etaphi)->Clone("h_ohcal_proj");
  TH2 *ihcal_etaphi = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("ihcal_etaphi")));
  TH1 *ihcal_proj = (TH1 *) proj(ihcal_etaphi)->Clone("h_ihcal_proj");
  TH1 *h_fb_ratio_emcal = FBratio(emcal_proj);
  TH1 *h_fb_ratio_ohcal = FBratio(ohcal_proj);
  TH1 *h_fb_ratio_ihcal = FBratio(ihcal_proj);
  Pad[0][6]->cd();
  h_fb_ratio_emcal->Draw("ex0");
  h_fb_ratio_emcal->SetTitle("Calo North-South Ratio");
  h_fb_ratio_emcal->SetYTitle("N^{twr}(#eta_{i})/N^{twr}(#eta_{N-i})");
  h_fb_ratio_emcal->SetXTitle("#eta_{i}");
  h_fb_ratio_emcal->GetYaxis()->SetRangeUser(0.1, 2);
  h_fb_ratio_ohcal->Draw("ex0 same");
  h_fb_ratio_ohcal->SetLineColor(kBlue);
  h_fb_ratio_ohcal->SetMarkerColor(kBlue);
  h_fb_ratio_ohcal->SetMarkerStyle(22);
  h_fb_ratio_ihcal->Draw("ex0 same");
  h_fb_ratio_ihcal->SetLineColor(kRed);
  h_fb_ratio_ihcal->SetMarkerColor(kRed);
  h_fb_ratio_ihcal->SetMarkerStyle(33);
  gPad->UseCurrentStyle();

  myText(0.52, 0.20, 1, "EMCal");
  myText(0.67, 0.20, kBlue, "oHCal");
  myText(0.82, 0.20, kRed, "iHCal");
  */

  /* db->DBcommit(); */

  /*
  // retrieve variables from previous runs
  vector<QADrawDBVar> history;
  time_t current = cl->BeginRunUnixTime();
  // go back 24 hours
  time_t back =   current - 24*3600;
  db->GetVar(back,current,"meanpx",history);
  DrawGraph(Pad[3],history,back,current);
  */
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream1, runnostream2, runnostream3, runnostream4, runnostream5;
  std::string runstring1, runstring2, runstring3, runstring4, runstring5;
  runnostream1 << Name() << "_cemc_towers Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  runnostream2 << Name() << "_cemc_clusters Run " << cl->RunNumber() << ", build " << cl->build();
  runstring2 = runnostream2.str();
  runnostream3 << Name() << "_cemc_tower_masking Run " << cl->RunNumber() << ", build " << cl->build();
  runstring3 = runnostream3.str();
  /* runnostream4 << Name() << "_cemc_summary Run " << cl->RunNumber() << ", build " << cl->build(); */
  /* runstring4 = runnostream4.str(); */
  runnostream5 << Name() << "_cemc_pedestal_RMS_ Run " << cl->RunNumber() << ", build " << cl->build();
  runstring5 = runnostream5.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring2.c_str());
  transparent[6]->cd();
  PrintRun.DrawText(0.5, 1., runstring3.c_str());
  transparent[11]->cd();
  PrintRun.DrawText(0.5, 1., runstring5.c_str());
  /* transparent[7]->cd(); */
  /* PrintRun.DrawText(0.5, 1., runstring4.c_str()); */

  TC[0]->Update();
  TC[1]->Update();
  TC[6]->Update();
  TC[11]->Update();
  /* TC[7]->Update(); */
  return 0;
}

int CaloDraw::DrawIhcal()
{
  QADrawClient *cl = QADrawClient::instance();

  TH2 *ihcal_e_chi2 = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("ihcal_e_chi2")));
  TH2F *ihcal_etaphi = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("ihcal_etaphi")));
  TH2 *ihcal_etaphi_time = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("ihcal_etaphi_time")));
  TH1F *ihcal_proj = (TH1F *) proj(ihcal_etaphi)->Clone("h_ihcal_proj");
  TProfile2D *h_CaloValid_ihcal_etaphi_pedRMS = dynamic_cast<TProfile2D *>(cl->getHisto(histprefix + std::string("ihcal_etaphi_pedRMS")));
  TProfile2D *h_CaloValid_ihcal_etaphi_ZSpedRMS = dynamic_cast<TProfile2D *>(cl->getHisto(histprefix + std::string("ihcal_etaphi_ZSpedRMS")));
  TH2 *ihcal_hotmap = nullptr;
  if (calo_checker) ihcal_hotmap = calo_checker->ihcal_hcdmap;
  /* TH1 *ihcal_etaphi_wQA = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("ihcal_etaphi_wQA"))); */


  // canvas 1

  if (!gROOT->FindObject("ihcal1"))
  {
    MakeCanvas("ihcal1", 2);
  }
  /* TC[2]->Clear("D"); */
  Pad[2][0]->cd();
  if (ihcal_e_chi2)
  {
    ihcal_e_chi2->SetTitle("iHCal #chi^{2} vs Energy");
    ihcal_e_chi2->SetXTitle("Tower E (GeV) iHCal");
    ihcal_e_chi2->SetYTitle("Tower #chi^{2} iHCal");
    ihcal_e_chi2->GetXaxis()->SetNdivisions(505);
    ihcal_e_chi2->GetXaxis()->SetRangeUser(-1, 15);
    ihcal_e_chi2->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetLogy();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[2][1]->cd();
  if (ihcal_etaphi)
  {
    ihcal_etaphi->SetTitle("iHCal Occupancy");
    ihcal_etaphi->SetXTitle("#it{#eta}_{i} iHCal");
    ihcal_etaphi->SetYTitle("#it{#phi}_{i} iHCal");
    ihcal_etaphi->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  Pad[2][2]->cd();
  if (ihcal_etaphi_time)
  {
    ihcal_etaphi_time->SetTitle("iHCal Mean Signal Peak Time");
    ihcal_etaphi_time->SetXTitle("#it{#eta}_{i} iHCal");
    ihcal_etaphi_time->SetYTitle("#it{#phi}_{i} iHCal");
    ihcal_etaphi_time->GetXaxis()->SetNdivisions(505);
    ihcal_etaphi_time->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  Pad[2][3]->cd();
  if (ihcal_proj)
  {
    ihcal_proj->SetTitle("iHCal #eta Projection");
    ihcal_proj->SetXTitle("#it{#eta}_{i} iHCal");
    ihcal_proj->SetYTitle("N^{twr}(E_{T} > 1 GeV)");
    ihcal_proj->DrawCopy("HIST");
    gPad->UseCurrentStyle();
  }

  // Canvas 2
  // do "summary" canvas before tower masking canvas, so ihcalGoodRun gets called first
  //  if (!gROOT->FindObject("ihcal3")) 
  //{ 
  //MakeCanvas("ihcal3", 7); 
  //} 
  /* /1* TC[7]->Clear("D"); *1/ */
  //Pad[7][0]->cd(); 
  //bool isgoodrun = ihcalGoodRun(); 
  //n_events, hot/cold/dead_towers, ihcal_time_* and vtxz_* are now filled 
  /* db->DBcommit(); */

  // Canvas 3

  if (!gROOT->FindObject("ihcal2"))
    {
      MakeCanvas("ihcal2", 8);
    }
  /* TC[8]->Clear("D"); */
  Pad[8][0]->cd();
  if (ihcal_hotmap)
    {
      ihcal_hotmap->SetTitle("IHcal Hot Tower Mask");
      ihcal_hotmap->SetXTitle("#it{#eta}_{i}");
      ihcal_hotmap->SetYTitle("#it{#phi}_{i}");
      // change to a discrete color palette
      int palette[4] = {kWhite, kGray+2, kRed, kBlue};
      ihcal_hotmap->GetZaxis()->SetRangeUser(-0.5,3.5);
      ihcal_hotmap->DrawCopy("COLZ");
      gStyle->SetPalette(4, palette);
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  Pad[8][1]->cd();
  gStyle->SetPalette(kBird);
  TH2 *h_hitmask = nullptr;
  int nonzero_towers = 0;
  // Make masked tower histogram
  if (ihcal_etaphi && ihcal_hotmap)
    {
      h_hitmask = (TH2*)ihcal_etaphi->Clone("h_hitmask");
      int nbins = ihcal_hotmap->GetNcells();
      for (int i=0; i<=nbins; i++)
	{
	  if (ihcal_hotmap->GetBinContent(i) != 0)
	    {
	      h_hitmask->SetBinContent(i, 0);
	      nonzero_towers++;
	    }
	}
      h_hitmask->SetTitle("IHcal Tower Hits w/ Masking");
      h_hitmask->SetXTitle("#it{#eta}_{i}");
      h_hitmask->SetYTitle("#it{#phi}_{i}");
      h_hitmask->DrawCopy("COLZ");
      gPad->UseCurrentStyle();
      gPad->SetRightMargin(0.15);
    }
  Pad[8][2]->cd();
  myText(0.25, 0.80, kBlack, "Hot Tower Mask Legend:");
  myText(0.25, 0.75, kBlack, "0/empty = good tower");
  myText(0.25, 0.70, kGray+2, "1 = dead tower");
  myText(0.25, 0.65, kRed, "2 = hot tower");
  myText(0.25, 0.60, kBlue, "3 = cold tower");
  int dead_towers = 999999; int hot_towers = 999999; int cold_towers = 999999;
  if (calo_checker)
    {
      dead_towers = calo_checker->ihcal_dead_towers;
      hot_towers = calo_checker->ihcal_hot_towers;
      cold_towers = calo_checker->ihcal_cold_towers;
    }
  myText(0.75, 0.70, kBlack, Form("This run: %d dead, %d hot, %d cold", dead_towers, hot_towers, cold_towers), 0.06);
  myText(0.75, 0.62, kBlack, "Expected: 0 dead, 0 hot, 0 cold", 0.06);
  Pad[8][3]->cd();
  TH1F *ihcal_proj_masked = nullptr;
  if (h_hitmask) ihcal_proj_masked = (TH1F *) proj(h_hitmask)->Clone("h_ihcal_proj_masked");
  if (ihcal_proj_masked)
    {
      ihcal_proj_masked->SetTitle("IHCal #eta Projection w/ Masking");
      ihcal_proj_masked->SetXTitle("#it{#eta}_{i} EMCal");
      ihcal_proj_masked->SetYTitle("N^{twr}(E_{T} > 1 GeV)");
      ihcal_proj_masked->DrawCopy("HIST");
      gPad->UseCurrentStyle();
    }

  if (!gROOT->FindObject("ihcal4"))
  {
    MakeCanvas("ihcal4", 12);
  }
  Pad[12][0]->cd();
  if (h_CaloValid_ihcal_etaphi_pedRMS)
  {
    h_CaloValid_ihcal_etaphi_pedRMS->SetTitle("CaloValid IHCal Eta-Phi Ped RMS");
    h_CaloValid_ihcal_etaphi_pedRMS->SetXTitle("#it{#eta}_{i} IHCal");
    h_CaloValid_ihcal_etaphi_pedRMS->SetYTitle("#it{#phi}_{i} IHCal");
    h_CaloValid_ihcal_etaphi_pedRMS->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  Pad[12][1]->cd();
  if (h_CaloValid_ihcal_etaphi_ZSpedRMS)
  {
    h_CaloValid_ihcal_etaphi_ZSpedRMS->SetTitle("CaloValid IHCal Eta-Phi ZSped RMS");
    h_CaloValid_ihcal_etaphi_ZSpedRMS->SetXTitle("#it{#eta}_{i} IHCal");
    h_CaloValid_ihcal_etaphi_ZSpedRMS->SetYTitle("#it{#phi}_{i} IHCal");
    h_CaloValid_ihcal_etaphi_ZSpedRMS->Draw("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream1,runnostream2, runnostream3, runnostream4;
  std::string runstring1, runstring2, runstring3, runstring4;
  runnostream1 << Name() << "_ihcal Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  runnostream2 << Name() << "_ihcal_tower_masking Run " << cl->RunNumber() << ", build " << cl->build();
  runstring2 = runnostream2.str();
  // runnostream3 << Name() << "_ihcal_summary Run " << cl->RunNumber() << ", build " << cl->build(); 
  //runstring3 = runnostream3.str(); 
  runnostream4 << Name() << "_ihcal_pedestal_RMS_ Run " << cl->RunNumber() << ", build " << cl->build();
  runstring4 = runnostream4.str();
  transparent[2]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());
  //transparent[7]->cd();
  //PrintRun.DrawText(0.5, 1., runstring3.c_str());
  transparent[8]->cd();
  PrintRun.DrawText(0.5, 1., runstring2.c_str());
  transparent[12]->cd();
  PrintRun.DrawText(0.5, 1., runstring4.c_str());

  TC[2]->Update();
  //TC[7]->Update();
  TC[8]->Update();
  TC[12]->Update();
  return 0;
}


int CaloDraw::DrawOhcal()
{
  QADrawClient *cl = QADrawClient::instance();

  TH2 *ohcal_e_chi2 = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("ohcal_e_chi2")));
  TH2F *ohcal_etaphi = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("ohcal_etaphi")));
  TH2 *ohcal_etaphi_time = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("ohcal_etaphi_time")));
  TProfile2D *h_CaloValid_ohcal_etaphi_pedRMS = dynamic_cast<TProfile2D *>(cl->getHisto(histprefix + std::string("ohcal_etaphi_pedRMS")));
  TProfile2D *h_CaloValid_ohcal_etaphi_ZSpedRMS = dynamic_cast<TProfile2D *>(cl->getHisto(histprefix + std::string("ohcal_etaphi_ZSpedRMS")));
  TH1F *ohcal_proj = (TH1F *) proj(ohcal_etaphi)->Clone("h_ohcal_proj");
  TH2 *ohcal_hotmap = nullptr;
  if (calo_checker) ohcal_hotmap = calo_checker->ohcal_hcdmap;
  /* TH1 *ohcal_etaphi_wQA = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("ohcal_etaphi_wQA"))); */

  // canvas 1
  if (!gROOT->FindObject("ohcal"))
  {
    MakeCanvas("ohcal", 3);
  }
  /* TC[3]->Clear("D"); */
  Pad[3][0]->cd();
  if (ohcal_e_chi2)
  {
    ohcal_e_chi2->SetTitle("oHCal #chi^{2} vs Energy");
    ohcal_e_chi2->SetXTitle("Tower E (GeV) oHCal");
    ohcal_e_chi2->SetYTitle("Tower #chi^{2} oHCal");
    ohcal_e_chi2->GetXaxis()->SetNdivisions(505);
    ohcal_e_chi2->GetXaxis()->SetRangeUser(-1, 15);
    ohcal_e_chi2->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetLogy();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[3][1]->cd();
  if (ohcal_etaphi)
  {
    ohcal_etaphi->SetTitle("oHCal Occupancy");
    ohcal_etaphi->SetXTitle("#it{#eta}_{i} oHCal");
    ohcal_etaphi->SetYTitle("#it{#phi}_{i} oHCal");
    ohcal_etaphi->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  Pad[3][2]->cd();
  if (ohcal_etaphi_time)
  {
    ohcal_etaphi_time->SetTitle("oHCal Mean Signal Peak Time");
    ohcal_etaphi_time->SetXTitle("#it{#eta}_{i} oHCal");
    ohcal_etaphi_time->SetYTitle("#it{#phi}_{i} oHCal");
    ohcal_etaphi_time->GetXaxis()->SetNdivisions(505);
    ohcal_etaphi_time->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  Pad[3][3]->cd();
  if (ohcal_proj)
  {
    ohcal_proj->SetTitle("oHCal #eta Projection");
    ohcal_proj->SetXTitle("#it{#eta}_{i} oHCal");
    ohcal_proj->SetYTitle("N^{twr}(E_{T} > 1 GeV)");
    ohcal_proj->DrawCopy("HIST");
    gPad->UseCurrentStyle();
  }
  /* db->DBcommit(); */

  // Canvas 2                                                                                                                                                                                             
  // do "summary" canvas before tower masking canvas, so ohcalGoodRun gets called first                                                                                                                   
  //if (!gROOT->FindObject("ohcal3"))
  //{
  //  MakeCanvas("ohcal3", 9);
  //}
  /* /1* TC[9]->Clear("D"); *1/ */
  //Pad[9][0]->cd();
  //bool isgoodrun = ohcalGoodRun();
  //n_events, hot/cold/dead_towers, ohcal_time_* and vtxz_* are now filled
    /* db->DBcommit(); */

    // Canvas 3                                                                                                                                                                                           
    if (!gROOT->FindObject("ohcal2"))
      {
	MakeCanvas("ohcal2", 10);
      }
  /* TC[10]->Clear("D"); */
  Pad[10][0]->cd();
  if (ohcal_hotmap)
    {
      ohcal_hotmap->SetTitle("OHcal Hot Tower Mask");
      ohcal_hotmap->SetXTitle("#it{#eta}_{i}");
      ohcal_hotmap->SetYTitle("#it{#phi}_{i}");
      // change to a discrete color palette                                                                                                                                                                
      int palette[4] = {kWhite, kGray+2, kRed, kBlue};
      ohcal_hotmap->GetZaxis()->SetRangeUser(-0.5,3.5);
      ohcal_hotmap->DrawCopy("COLZ");
      gStyle->SetPalette(4, palette);
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  Pad[10][1]->cd();
  gStyle->SetPalette(kBird);
  TH2 *h_hitmask = nullptr;
  int nonzero_towers = 0;
  // Make masked tower histogram                                                                                                                                                                           
  if (ohcal_etaphi && ohcal_hotmap)
    {
      h_hitmask = (TH2*)ohcal_etaphi->Clone("h_hitmask");
      int nbins = ohcal_hotmap->GetNcells();
      for (int i=0; i<=nbins; i++)
        {
          if (ohcal_hotmap->GetBinContent(i) != 0)
            {
              h_hitmask->SetBinContent(i, 0);
              nonzero_towers++;
            }
        }
      h_hitmask->SetTitle("OHcal Tower Hits w/ Masking");
      h_hitmask->SetXTitle("#it{#eta}_{i}");
      h_hitmask->SetYTitle("#it{#phi}_{i}");
      h_hitmask->DrawCopy("COLZ");
      gPad->UseCurrentStyle();
      gPad->SetRightMargin(0.15);
    }
  Pad[10][2]->cd();
  myText(0.25, 0.80, kBlack, "Hot Tower Mask Legend:");
  myText(0.25, 0.75, kBlack, "0/empty = good tower");
  myText(0.25, 0.70, kGray+2, "1 = dead tower");
  myText(0.25, 0.65, kRed, "2 = hot tower");
  myText(0.25, 0.60, kBlue, "3 = cold tower");
  int dead_towers = 999999; int hot_towers = 999999; int cold_towers = 999999;
  if (calo_checker)
    {
      dead_towers = calo_checker->ohcal_dead_towers;
      hot_towers = calo_checker->ohcal_hot_towers;
      cold_towers = calo_checker->ohcal_cold_towers;
    }
  myText(0.75, 0.70, kBlack, Form("This run: %d dead, %d hot, %d cold", dead_towers, hot_towers, cold_towers), 0.06);
  myText(0.75, 0.62, kBlack, "Expected: 0 dead, 0 hot, 0 cold", 0.06);
  Pad[10][3]->cd();
  TH1F *ohcal_proj_masked = nullptr;
  if (h_hitmask) ohcal_proj_masked = (TH1F *) proj(h_hitmask)->Clone("h_ohcal_proj_masked");
  if (ohcal_proj_masked)
    {
      ohcal_proj_masked->SetTitle("OHCal #eta Projection w/ Masking");
      ohcal_proj_masked->SetXTitle("#it{#eta}_{i} EMCal");
      ohcal_proj_masked->SetYTitle("N^{twr}(E_{T} > 1 GeV)");
      ohcal_proj_masked->DrawCopy("HIST");
      gPad->UseCurrentStyle();
    }
  if (!gROOT->FindObject("ohcal4"))
  {
    MakeCanvas("ohcal4", 13);
  }
  Pad[13][0]->cd();
  if (h_CaloValid_ohcal_etaphi_pedRMS)
  {
    h_CaloValid_ohcal_etaphi_pedRMS->SetTitle("CaloValid OHCal Eta-Phi Ped RMS");
    h_CaloValid_ohcal_etaphi_pedRMS->SetXTitle("#it{#eta}_{i} OHCal");
    h_CaloValid_ohcal_etaphi_pedRMS->SetYTitle("#it{#phi}_{i} OHCal");
    h_CaloValid_ohcal_etaphi_pedRMS->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  Pad[13][1]->cd();
  if (h_CaloValid_ohcal_etaphi_ZSpedRMS)
  {
    h_CaloValid_ohcal_etaphi_ZSpedRMS->SetTitle("CaloValid OHCal Eta-Phi ZSped RMS");
    h_CaloValid_ohcal_etaphi_ZSpedRMS->SetXTitle("#it{#eta}_{i} OHCal");
    h_CaloValid_ohcal_etaphi_ZSpedRMS->SetYTitle("#it{#phi}_{i} OHCal");
    h_CaloValid_ohcal_etaphi_ZSpedRMS->Draw("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream1,runnostream2, runnostream3, runnostream4;
  std::string runstring1, runstring2, runstring3, runstring4;
  runnostream1 << Name() << "_ohcal Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  runnostream2 << Name() << "_ohcal_tower_masking Run " << cl->RunNumber() << ", build " << cl->build();
  runstring2 = runnostream2.str();
  //runnostream3 << Name() << "_ohcal_summary Run " << cl->RunNumber() << ", build " << cl->build();
  //runstring3 = runnostream3.str();
  runnostream4 << Name() << "_ohcal_pedestal_RMS_ Run " << cl->RunNumber() << ", build " << cl->build();
  runstring4 = runnostream4.str();
  transparent[3]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());
  //transparent[9]->cd();
  //PrintRun.DrawText(0.5, 1., runstring3.c_str());
  transparent[10]->cd();
  PrintRun.DrawText(0.5, 1., runstring2.c_str());
  transparent[13]->cd();
  PrintRun.DrawText(0.5, 1., runstring4.c_str());

  TC[3]->Update();
  //TC[9]->Update();
  TC[10]->Update();
  TC[13]->Update();
  return 0;
}

int CaloDraw::DrawZdcMbd()
{
  QADrawClient *cl = QADrawClient::instance();

  TH1 *zdc_Northcalib = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("zdcNorthcalib")));
  TH1 *zdc_Southcalib = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("zdcSouthcalib")));
  TH1 *vtx_z = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("vtx_z_raw")));

  // canvas 1
  if (!gROOT->FindObject("zdc&mbd"))
  {
    MakeCanvas("zdc&mbd", 4);
  }
  /* TC[3]->Clear("D"); */
  Pad[4][0]->cd();
  if (zdc_Northcalib && zdc_Southcalib)
  {
    zdc_Northcalib->SetLineColor(kBlue);
      


    zdc_Northcalib->GetXaxis()->SetRangeUser(0.0, 12000);
    zdc_Northcalib->SetTitle("ZDC Total Energy");
    zdc_Northcalib->SetXTitle("#Sigma #it{E}^{ZDC Side}");
    zdc_Northcalib->SetYTitle("Events");
    zdc_Northcalib->GetXaxis()->SetNdivisions(505);
    zdc_Northcalib->DrawCopy();
    gPad->UseCurrentStyle();

    zdc_Southcalib->SetLineColor(kRed);
    zdc_Southcalib->DrawCopy("same");
    gPad->SetLogy();

    myText(0.75, 0.80, kBlue, "North");
    myText(0.65, 0.80, kRed, "South");
  }
  else
  {
    // histogram is missing
    /* return -1; */
  }
  Pad[4][1]->cd();
  if (zdc_Northcalib && zdc_Southcalib)
  {
    zdc_Northcalib->Draw();
    zdc_Northcalib->SetLineColor(kBlue);
    zdc_Northcalib->GetXaxis()->SetRangeUser(10, 300);
    zdc_Northcalib->SetTitle("ZDC Total Energy");
    zdc_Northcalib->SetXTitle("#Sigma #it{E}^{ZDC Side}");
    zdc_Northcalib->SetYTitle("Events");
    gPad->UseCurrentStyle();

    TGraph *gr_1n = new TGraph();
    gr_1n->SetPoint(0, 100, 0);
    gr_1n->SetPoint(1, 100, 1e7);
    gr_1n->SetLineStyle(7);
    gr_1n->Draw("l");

    zdc_Southcalib->Draw("same");
    zdc_Southcalib->SetLineColor(kRed);
    gPad->SetLogy();

    myText(0.75, 0.80, kBlue, "North");
    myText(0.65, 0.80, kRed, "South");
  }
  Pad[4][2]->cd();
  if (vtx_z)
  {
    vtx_z->SetTitle("MBD Vertex z");
    vtx_z->SetXTitle("MBD Vtx #it{z} (cm)");
    vtx_z->SetYTitle("Counts");
    vtx_z->DrawCopy("");
    gPad->UseCurrentStyle();
  }

  /* db->DBcommit(); */

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  runnostream << Name() << "_zdc&mbd Run " << cl->RunNumber() << ", build " << cl->build();
  runstring = runnostream.str();
  transparent[4]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());

  TC[4]->Update();
  return 0;
}

int CaloDraw::DrawCorr()
{
  QADrawClient *cl = QADrawClient::instance();

  TH2 *emcal_mbd = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("emcal_mbd_correlation")));
  TH2 *emcal_hcal = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("emcal_hcal_correlation")));
  TH2 *zdc_emcal = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("zdc_emcal_correlation")));
  TH2 *ihcal_mbd = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("ihcal_mbd_correlation")));
  TH2 *ohcal_mbd = dynamic_cast<TH2 *>(cl->getHisto(histprefix + std::string("ohcal_mbd_correlation")));

  // canvas 1
  if (!gROOT->FindObject("corr"))
  {
    MakeCanvas("corr", 5);
  }
  /* TC[4]->Clear("D"); */
  Pad[5][0]->cd();
  if (emcal_mbd)
  {
    emcal_mbd->SetTitle("EMCal MBD Correlation");
    emcal_mbd->SetXTitle("#Sigma #it{E}^{EMCal} [Arb]");
    emcal_mbd->SetYTitle("#Sigma #it{E}^{MBD} [Arb]");
    emcal_mbd->GetXaxis()->SetNdivisions(505);
    emcal_mbd->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[5][1]->cd();
  if (emcal_hcal)
  {
    emcal_hcal->SetTitle("EMCal HCal Correlation");
    emcal_hcal->SetXTitle("#Sigma #it{E}^{EMCal} [Arb]");
    emcal_hcal->SetYTitle("#Sigma #it{E}^{HCal} [Arb]");
    emcal_hcal->GetXaxis()->SetNdivisions(505);
    emcal_hcal->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }
  Pad[5][2]->cd();
  if (zdc_emcal)
  {
    zdc_emcal->SetTitle("ZDC EMCal Correlation");
    zdc_emcal->SetXTitle("#Sigma #it{E}^{EMCal} [Arb]");
    zdc_emcal->SetYTitle("#Sigma #it{E}^{ZDC} [Arb]");
    zdc_emcal->GetXaxis()->SetNdivisions(505);
    zdc_emcal->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }
  Pad[5][3]->cd();
  if (ihcal_mbd)
  {
    ihcal_mbd->SetTitle("iHCal MBD Correlation");
    ihcal_mbd->SetXTitle("#Sigma #it{E}^{iHCal} [Arb]");
    ihcal_mbd->SetYTitle("#Sigma #it{E}^{MBD} [Arb]");
    ihcal_mbd->GetXaxis()->SetNdivisions(505);
    ihcal_mbd->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }
  Pad[5][4]->cd();
  if (ohcal_mbd)
  {
    ohcal_mbd->SetTitle("oHCal MBD Correlation");
    ohcal_mbd->SetXTitle("#Sigma #it{E}^{oHCal} [Arb]");
    ohcal_mbd->SetYTitle("#Sigma #it{E}^{MBD} [Arb]");
    ohcal_mbd->GetXaxis()->SetNdivisions(505);
    ohcal_mbd->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }

  /* db->DBcommit(); */

  /*
  // retrieve variables from previous runs
  vector<QADrawDBVar> history;
  time_t current = cl->BeginRunUnixTime();
  // go back 24 hours
  time_t back =   current - 24*3600;
  db->GetVar(back,current,"meanpx",history);
  DrawGraph(Pad[3],history,back,current);
  */
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream1;//, runnostream2;
  std::string runstring1;//, runstring2;
  runnostream1 << Name() << "_correlation Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  /* runnostream2 << Name() << "_corr_2 Run " << cl->RunNumber() << ", build " << cl->build(); */
  /* runstring2 = runnostream2.str(); */
  transparent[5]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());
  /* transparent[5]->cd(); */
  /* PrintRun.DrawText(0.5, 1., runstring2.c_str()); */

  TC[5]->Update();
  /* TC[6]->Update(); */
  return 0;
}

int CaloDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret)  // on error no html output please
  {
    return iret;
  }

  QADrawClient *cl = QADrawClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  std::string pngfile = cl->htmlRegisterPage(*this, "EMCal/Towers", "cemc1", "png");
  cl->CanvasToPng(TC[0], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "EMCal/Clusters", "cemc2", "png");
  cl->CanvasToPng(TC[1], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "EMCal/Masking", "cemc3", "png");
  cl->CanvasToPng(TC[6], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "EMCal/Pedestal", "cemc5", "png");
  cl->CanvasToPng(TC[11], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "EMCal/Summary", "cemc4", "png");
  cl->CanvasToPng(cemcSummary, pngfile);
  /* cl->CanvasToPng(TC[7], pngfile); */
  pngfile = cl->htmlRegisterPage(*this, "iHCal/Run", "ihcal1", "png");
  cl->CanvasToPng(TC[2], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "iHCal/Masking", "ihcal2", "png");
  cl->CanvasToPng(TC[8], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "iHCal/Summary", "ihcal3", "png");
  cl->CanvasToPng(ihcalSummary, pngfile);
  pngfile = cl->htmlRegisterPage(*this, "iHCal/Pedestal", "ihcal4", "png");
  cl->CanvasToPng(TC[12], pngfile);
 /* cl->CanvasToPng(TC[7], pngfile); */
  pngfile = cl->htmlRegisterPage(*this, "oHCal/Run", "ohcal", "png");
  cl->CanvasToPng(TC[3], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "oHCal/Masking", "ohcal2", "png");
  cl->CanvasToPng(TC[10], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "oHCal/Summary", "ohcal3", "png");
  cl->CanvasToPng(ohcalSummary, pngfile);
  pngfile = cl->htmlRegisterPage(*this, "oHCal/Pedestal", "ohcal4", "png");
  cl->CanvasToPng(TC[13], pngfile);
 /* cl->CanvasToPng(TC[9], pngfile); */
  pngfile = cl->htmlRegisterPage(*this, "ZDC&MBD", "zdc&mbd", "png");
  cl->CanvasToPng(TC[4], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "Correlations", "correlations", "png");
  cl->CanvasToPng(TC[5], pngfile);
  /* pngfile = cl->htmlRegisterPage(*this, "CORR", "6", "png"); */
  /* cl->CanvasToPng(TC[5], pngfile); */
  /* pngfile = cl->htmlRegisterPage(*this, "ZDC&MBD", "7", "png"); */
  /* cl->CanvasToPng(TC[6], pngfile); */

  return 0;
}

int CaloDraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}

void CaloDraw::SetCemcSummary(TCanvas* c)
{
  cemcSummary = c;
  /*
  cemcSummary->cd();
  // add the run number title
  QADrawClient *cl = QADrawClient::instance();
  TPad* tr = new TPad("transparent_cemc", "", 0, 0, 1, 1);
  tr->SetFillStyle(4000);
  tr->Draw();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  runnostream << Name() << "_cemc_summary Run " << cl->RunNumber() << ", build " << cl->build();
  runstring = runnostream.str();
  tr->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  cemcSummary->Update();
  */
}

void CaloDraw::SetihcalSummary(TCanvas* c)
{
  ihcalSummary = c;
  /*
  ihcalSummary->cd();
  
  // Add the run number title
  QADrawClient *cl = QADrawClient::instance();
  TPad* tr = new TPad("transparent_ihcal", "", 0, 0, 1, 1);
  tr->SetFillStyle(4000);
  tr->Draw();  
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  runnostream << Name()<< "_ihcal_summary Run " << cl->RunNumber() << ", build " << cl->build();
  runstring = runnostream.str();
  tr->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  ihcalSummary->Update();
  */
}

void CaloDraw::SetohcalSummary(TCanvas* c)
{
  ohcalSummary = c;
  /*
  ohcalSummary->cd();
  
  // Add the run number title
  QADrawClient *cl = QADrawClient::instance();
  TPad* tr = new TPad("transparent_ohcal", "", 0, 0, 1, 1);
  tr->SetFillStyle(4000);
  tr->Draw();  
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  runnostream << Name() << "_ohcal_summary Run " << cl->RunNumber() << ", build " << cl->build();
  runstring = runnostream.str();
  tr->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  ohcalSummary->Update();
  */
}

TH1 *CaloDraw::proj(TH2 *h2)
{
  int x = h2->GetXaxis()->GetNbins();
  int y = h2->GetYaxis()->GetNbins();
  TH1 *h = (TH1F *) h2->ProjectionX("temp");
  h->Reset();
  for (int ix = 1; ix < x + 1; ix++)
  {
    float sum = 0;
    float cc = 0;
    for (int iy = 1; iy < y + 1; iy++)
    {
      float bc = h2->GetBinContent(ix, iy);
      if (bc == 0)
      {
        cc += 1;
      }
      sum += bc;
    }
    if (cc == y)
    {
      continue;
    }
    float sum_cor = sum * y / (y - cc);
    h->SetBinContent(ix, sum_cor);
    h->SetBinError(ix, std::sqrt(sum_cor));
  }
  return h;
}

TH1 *CaloDraw::FBratio(TH1 *h)
{
  if (!h)
  {
    return nullptr;
  }
  const int Nbin = h->GetXaxis()->GetNbins();
  TH1 *hfb = new TH1F("temp32", "", Nbin / 2, 0, Nbin / 2.);

  for (int i = 0; i < Nbin / 2; i++)
  {
    int b1 = i + 1;
    int b2 = Nbin - i;
    float ratio = h->GetBinContent(b1) / h->GetBinContent(b2);
    float err = sqrt(pow(h->GetBinError(b1) / h->GetBinContent(b1), 2) + pow(h->GetBinError(b2) / h->GetBinContent(b2), 2)) * pow(ratio, 2);
    hfb->SetBinContent(i, ratio);
    hfb->SetBinError(i, err);
  }
  return hfb;
}

void CaloDraw::myText(double x, double y, int color, const char *text, double tsize)
{
  TLatex l;
  l.SetTextAlign(22);
  l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextColor(color);
  l.DrawLatex(x, y, text);
}

