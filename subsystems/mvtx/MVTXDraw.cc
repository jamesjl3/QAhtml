#include "MVTXDraw.h"

#include <sPhenixStyle.C>

#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

MVTXDraw::MVTXDraw(const std::string &name)
  : QADraw(name)
{
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  histprefix = "h_MvtxClusterQA_"; 
  l = new TLatex();
  c_good = kGreen+3;
  c_ok = kOrange-3;
  c_bad = kRed;
  return;
}

MVTXDraw::~MVTXDraw()
{
  /* delete db; */
  return;
}

int MVTXDraw::Draw(const std::string &what)
{
  /* SetsPhenixStyle(); */
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "CHIP")
  {
    iret += DrawChipInfo();
    idraw++;
  }
  if (what == "ALL" || what == "CLUSTER")
  {
    iret += DrawClusterInfo();
    idraw++;
  }
  if (what == "ALL" || what == "SUMMARY")
  {
    iret += DrawSummaryInfo(); idraw++;
    idraw++;
  }
  if (!idraw)
  {
    std::cout << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int MVTXDraw::MakeCanvas(const std::string &name, int num)
{
  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  // xpos (-1) negative: do not draw menu bar
  TC[num] = new TCanvas(name.c_str(), (boost::format("MVTX Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2) , (int) (ysize / 1.2));
  gSystem->ProcessEvents();

  if (num == 0 || num == 1)
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
  else if (num == 2)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "Yo", 0.05, 0.3, 0.95, 0.97, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "Yo", 0.05, 0.05, 0.48, 0.55, 0);
    Pad[num][2] = new TPad((boost::format("mypad%d2") % num).str().c_str(), "Yo", 0.51, 0.05, 0.98, 0.55, 0);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
    Pad[num][2]->Draw();
  }
  

  // this one is used to plot the run number on the canvas
  transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[num]->SetFillStyle(4000);
  transparent[num]->Draw();

  return 0;
}

int MVTXDraw::DrawChipInfo()
{
  std::cout << "MVTX DrawChipInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  TH1F *h_occupancy = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("chipOccupancy")));
  TH1F *h_clusSize = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterSize")));
  TH1I *h_strobe = dynamic_cast <TH1I *> (cl->getHisto(histprefix + std::string("strobeTiming")));

  if (! gROOT->FindObject("chip_info"))
  {
    MakeCanvas("chip_info", 0);
  }
  TC[0]->Clear("D");
  Pad[0][0]->cd();
  if (h_occupancy)
  {
    h_occupancy->SetTitle("MVTX Chip Occupancy");
    h_occupancy->SetXTitle("Chip Occupancy [%]");
    h_occupancy->SetYTitle("Normalized Entries");
    h_occupancy->GetXaxis()->SetRange(1, h_occupancy->GetNbinsX() + 1); // include overflow bin
    h_occupancy->Scale(1./h_occupancy->Integral());
    h_occupancy->DrawCopy();
    gPad->SetLogy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[0][1]->cd();
  if (h_clusSize)
  {
    h_clusSize->SetTitle("MVTX Cluster Size");
    h_clusSize->SetXTitle("Cluster Size");
    h_clusSize->SetYTitle("Normalized Entries");
    h_clusSize->GetXaxis()->SetRange(1, h_occupancy->GetNbinsX() + 1); // include over flow bin
    h_clusSize->Scale(1./h_clusSize->Integral());
    h_clusSize->DrawCopy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[0][2]->cd();
  if (h_strobe)
  {
    h_strobe->DrawCopy();
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
  runnostream1 << Name() << "_mvtx Info Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[0]->Update();
 
  std::cout << "DrawChipInfo Ending" << std::endl;
  return 0;
}

int MVTXDraw::DrawClusterInfo()
{
  std::cout << "MVTX DrawChipInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  TH1F *h_clusPhi_incl = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterPhi_incl")));
  TH1F *h_clusPhi_l0 = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterPhi_l0")));
  TH1F *h_clusPhi_l1 = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterPhi_l1")));
  TH1F *h_clusPhi_l2 = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterPhi_l2")));
  TH2F *h_clusZ_clusPhi_l0 = dynamic_cast <TH2F *> (cl->getHisto(histprefix + std::string("clusterZ_clusPhi_l0")));
  TH2F *h_clusZ_clusPhi_l1 = dynamic_cast <TH2F *> (cl->getHisto(histprefix + std::string("clusterZ_clusPhi_l1")));
  TH2F *h_clusZ_clusPhi_l2 = dynamic_cast <TH2F *> (cl->getHisto(histprefix + std::string("clusterZ_clusPhi_l2")));

  if (! gROOT->FindObject("cluster_info"))
  {
    MakeCanvas("cluster_info", 1);
  }

  TC[1]->Clear("D");

  Pad[1][0]->cd();
  if (h_clusPhi_incl && h_clusPhi_l0 && h_clusPhi_l1 && h_clusPhi_l2)
  {
    h_clusPhi_incl->SetTitle("MVTX Cluster #phi");
    h_clusPhi_incl->SetXTitle("Cluster #phi wrt origin [rad]");
    h_clusPhi_incl->SetYTitle("Entries");
    h_clusPhi_incl->SetMarkerSize(0.5);
    h_clusPhi_incl->DrawCopy();
    h_clusPhi_l0->SetMarkerSize(0.5);
    h_clusPhi_l0->SetMarkerColor(kRed);
    h_clusPhi_l0->SetLineColor(kRed);
    h_clusPhi_l0->DrawCopy("same");
    h_clusPhi_l1->SetMarkerSize(0.5);
    h_clusPhi_l1->SetMarkerColor(kBlue);
    h_clusPhi_l1->SetLineColor(kBlue);
    h_clusPhi_l1->DrawCopy("same");
    h_clusPhi_l2->SetMarkerSize(0.5);
    h_clusPhi_l2->SetMarkerColor(kGreen+2);
    h_clusPhi_l2->SetLineColor(kGreen+2);
    h_clusPhi_l2->DrawCopy("same");
    auto legend = new TLegend(0.45, 0.7, 0.7, 0.9);
    legend->AddEntry(h_clusPhi_incl, "Inclusive", "pl");
    legend->AddEntry(h_clusPhi_l0, "Layer 0", "pl");
    legend->AddEntry(h_clusPhi_l1, "Layer 1", "pl");
    legend->AddEntry(h_clusPhi_l2, "Layer 2", "pl");
    legend->SetFillStyle(0);
    legend->Draw();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1][1]->cd();
  if (h_clusZ_clusPhi_l0)
  {
    h_clusZ_clusPhi_l0->SetTitle("MVTX Cluster Z vs phi Layer 0");
    h_clusZ_clusPhi_l0->SetXTitle("Cluster (layer 0) Z [cm]");
    h_clusZ_clusPhi_l0->SetYTitle("Cluster (layer 0) #phi wrt origin [rad]");
    h_clusZ_clusPhi_l0->SetZTitle("Entries");
    h_clusZ_clusPhi_l0->DrawCopy("colz");
    gPad->SetRightMargin(0.17);
    gPad->SetLogz();
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1][2]->cd();
  if (h_clusZ_clusPhi_l1)
  {
    h_clusZ_clusPhi_l1->SetTitle("MVTX Cluster Z vs phi Layer 1");
    h_clusZ_clusPhi_l1->SetXTitle("Cluster (layer 1) Z [cm]");
    h_clusZ_clusPhi_l1->SetYTitle("Cluster (layer 1) #phi wrt origin [rad]");
    h_clusZ_clusPhi_l1->SetZTitle("Entries");
    h_clusZ_clusPhi_l1->DrawCopy("colz");
    gPad->SetRightMargin(0.17);
    gPad->SetLogz();
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1][3]->cd();
  if (h_clusZ_clusPhi_l2)
  {
    h_clusZ_clusPhi_l2->SetTitle("MVTX Cluster Z vs phi Layer 2");
    h_clusZ_clusPhi_l2->SetXTitle("Cluster (layer 2) Z [cm]");
    h_clusZ_clusPhi_l2->SetYTitle("Cluster (layer 2) #phi wrt origin [rad]");
    h_clusZ_clusPhi_l2->SetZTitle("Entries");
    h_clusZ_clusPhi_l2->DrawCopy("colz");
    gPad->SetRightMargin(0.17);
    gPad->SetLogz();
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
  runnostream1 << Name() << "_mvtx Info Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[1]->Update();
 
  std::cout << "DrawClusterInfo Ending" << std::endl;
  return 0;
}

int MVTXDraw::DrawSummaryInfo()
{
  std::cout << "MVTX DrawSummaryInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  TH1F *h_clusPhi_incl = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterPhi_incl")));
  TH1F *h_occupancy = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("chipOccupancy")));

  if (! gROOT->FindObject("summary_info"))
  {
    MakeCanvas("summary_info", 2);
  }

  TC[2]->Clear("D");

  // Fit the cluster phi histogram 
  TF1 *f1 = new TF1("f1", "pol0(0)+[1]*cos(x+[2])", -3.14159, 3.14159);
  f1->SetParLimits(0, 0, 1E7);
  f1->SetParLimits(1, 0, 1E7);
  f1->SetParLimits(2, 0, 1E7);
  h_clusPhi_incl->Fit("f1", "R P M N 0 Q");
  float chi2ndf = f1->GetChisquare() / f1->GetNDF();
  float scaledchi2ndf = chi2ndf / f1->GetParameter(0);
  float BoverA = f1->GetParameter(1) / f1->GetParameter(0);
  float avgclus = f1->GetParameter(0);

  h_occupancy->Scale(1. / h_occupancy->Integral(-1, -1));
  h_occupancy->GetXaxis()->SetRange(1, h_occupancy->GetNbinsX() + 1);
  float probOccupancygt0p003 = h_occupancy->Integral(h_occupancy->FindBin(0.3), h_occupancy->GetNbinsX() + 1);

  Pad[2][0]->cd();

  // Draw separator lines
  TLine *sep = new TLine();
  sep->SetLineColor(kBlack);
  sep->SetLineWidth(2);
  sep->SetLineStyle(2);
  sep->DrawLine(gPad->GetUxmin(), 0.85, gPad->GetUxmax(), 0.85);
  sep->DrawLine(gPad->GetUxmin(), 0.58, gPad->GetUxmax(), 0.58);
  sep->DrawLine(gPad->GetUxmin(), 0.41, gPad->GetUxmax(), 0.41);

  l->SetNDC();
  l->SetTextAlign(textalign);
  l->SetTextSize(titletextsize);
  l->SetTextColor(kBlack);
  l->DrawLatex(textposx, 0.9, (boost::format("MVTX Cluster QA Summary - Run %d, build %s") % cl->RunNumber() % cl->build()).str().c_str());
  
  if (scaledchi2ndf < scaledchi2ndf_good)
  {
    l->SetTextColor(kBlack);
    l->SetTextSize(stattextsize);
    l->DrawLatex(textposx, 0.80, (boost::format("Cluster #phi fit A+B#timescos(#phi+C): #chi^{2}/ndf = %.5g, scaled #chi^{2}/ndf = %.5g, B/A = %.5g") % chi2ndf % scaledchi2ndf % BoverA).str().c_str());
    l->SetTextColor(c_good);
    l->DrawLatex(textposx, 0.74, Form("#chi^{2}/ndf GOOD (scaled #chi^{2}/ndf < %.5g); ", scaledchi2ndf_good));
    if (BoverA > bovera_high || BoverA < bovera_low)
    {
      l->SetTextColor(c_bad);
      l->DrawLatex(textposx, 0.68, Form("B/A = %.5g OUT OF RANGE (%.5g, %.5g)", BoverA, bovera_low, bovera_high));
    }
    else
    {
      l->SetTextColor(c_good);
      l->DrawLatex(textposx, 0.68, Form("B/A = %.5g WITHIN RANGE (%.5g, %.5g)", BoverA, bovera_low, bovera_high));
    }
    if (avgclus < avgnclus_theshold)
    {
      l->SetTextColor(c_ok);
      l->DrawLatex(textposx, 0.62, Form("Run length: Short (Average number of clusters per bin %.5g < %.5g)", avgclus, avgnclus_theshold));
    }
    else
    {
      l->SetTextColor(c_good);
      l->DrawLatex(textposx, 0.62, Form("Run length: Normal (Average number of clusters per bin %.5g #geq %.5g)", avgclus, avgnclus_theshold));
    }
  }
  else if (scaledchi2ndf >= scaledchi2ndf_good && scaledchi2ndf < scaledchi2ndf_bad)
  {
    l->SetTextColor(kBlack);
    l->SetTextSize(stattextsize);
    l->DrawLatex(textposx, 0.80, (boost::format("Cluster #phi fit A+B#timescos(#phi+C): #chi^{2}/ndf = %.5g, scaled #chi^{2}/ndf = %.5g, B/A = %.5g") % chi2ndf % scaledchi2ndf % BoverA).str().c_str());
    l->SetTextColor(c_ok);
    l->DrawLatex(textposx, 0.74, Form("#chi^{2}/ndf OK (%g #leq scaled #chi^{2}/ndf < %g)", scaledchi2ndf_good, scaledchi2ndf_bad));
    if (BoverA > bovera_high || BoverA < bovera_low)
    {
      l->SetTextColor(c_bad);
      l->DrawLatex(textposx, 0.68, Form("B/A = %.5g OUT OF RANGE (%.5g, %.5g)", BoverA, bovera_low, bovera_high));
    }
    else
    {
      l->SetTextColor(c_good);
      l->DrawLatex(textposx, 0.68, Form("B/A = %.5g WITHIN RANGE (%.5g, %.5g)", BoverA, bovera_low, bovera_high));
    }
    if (avgclus < avgnclus_theshold)
    {
      l->SetTextColor(c_ok);
      l->DrawLatex(textposx, 0.62, Form("Run length: Short (Average number of clusters per bin %.5g < %.5g)", avgclus, avgnclus_theshold));
    }
    else
    {
      l->SetTextColor(c_good);
      l->DrawLatex(textposx, 0.62, Form("Run length: Normal (Average number of clusters per bin %.5g #geq %.5g)", avgclus, avgnclus_theshold));
    }
  }
  else if (scaledchi2ndf >= scaledchi2ndf_bad)
  {
    l->SetTextColor(kBlack);
    l->SetTextSize(stattextsize);
    l->DrawLatex(textposx, 0.80, (boost::format("Cluster #phi fit A+B#timescos(#phi+C): #chi^{2}/ndf = %.5g, scaled #chi^{2}/ndf = %.5g, B/A = %.5g") % chi2ndf % scaledchi2ndf % BoverA).str().c_str());
    l->SetTextColor(c_bad);
    l->DrawLatex(textposx, 0.74, Form("#chi^{2}/ndf BAD (scaled #chi^{2}/ndf #geq %g)", scaledchi2ndf_bad));
    if (BoverA > bovera_high || BoverA < bovera_low)
    {
      l->SetTextColor(c_bad);
      l->DrawLatex(textposx, 0.68, Form("B/A = %.5g OUT OF RANGE (%.5g, %.5g)", BoverA, bovera_low, bovera_high));
    }
    else
    {
      l->SetTextColor(c_good);
      l->DrawLatex(textposx, 0.68, Form("B/A = %.5g WITHIN RANGE (%.5g, %.5g)", BoverA, bovera_low, bovera_high));
    }
    if (avgclus < avgnclus_theshold)
    {
      l->SetTextColor(c_ok);
      l->DrawLatex(textposx, 0.62, Form("Run length: Short (Average number of clusters per bin %.5g < %.5g)", avgclus, avgnclus_theshold));
    }
    else
    {
      l->SetTextColor(c_good);
      l->DrawLatex(textposx, 0.62, Form("Run length: Normal (Average number of clusters per bin %.5g #geq %.5g)", avgclus, avgnclus_theshold));
    }
  }
  else
  {
    l->SetTextColor(c_bad);
    l->SetTextSize(stattextsize);
    l->DrawLatex(textposx, 0.80, "Cluster #phi fit N/A (Check if cluster QA histograms exist)");
  }

  if (probOccupancygt0p003 < probOccupancygt0p003_95percentile)
  {
    l->SetTextColor(kBlack);
    l->SetTextSize(stattextsize);
    l->DrawLatex(textposx, 0.52, Form("Prob. of chip occupancy #geq 0.3%%: %.4e", probOccupancygt0p003));
    l->SetTextColor(c_good);
    l->DrawLatex(textposx, 0.46, Form("GOOD: Lower than 95%% of analyzed runs (%.4e)", probOccupancygt0p003_95percentile));
  }
  else
  {
    l->SetTextColor(kBlack);
    l->SetTextSize(stattextsize);
    l->DrawLatex(textposx, 0.52, Form("Prob. of chip occupancy #geq 0.3%%: %.4e", probOccupancygt0p003));
    l->SetTextColor(c_bad);
    l->DrawLatex(textposx, 0.46, Form("BAD: Higher than 95%% of analyzed runs (%.4e)", probOccupancygt0p003_95percentile));
  }

  // Draw the cluster phi and the fit
  Pad[2][1]->cd();
  h_clusPhi_incl->SetStats(0);
  h_clusPhi_incl->SetXTitle("Cluster #phi wrt origin [rad]");
  h_clusPhi_incl->SetYTitle("Entries");
  h_clusPhi_incl->SetMarkerSize(0.5);
  h_clusPhi_incl->DrawCopy();
  f1->SetLineColor(kRed);
  f1->Draw("same");
  TLegend *leg = new TLegend(0.2, 0.15, 0.8, 0.35);
  leg->SetTextSize(0.03);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetHeader("Fit function: A+B#timescos(#phi+C)");
  leg->SetNColumns(2);
  leg->AddEntry((TObject *)0, Form("#chi^{2}/ndf=%g", chi2ndf), "");
  leg->AddEntry((TObject *)0, Form("A=%g", f1->GetParameter(0)), "");
  leg->AddEntry((TObject *)0, Form("Scaled #chi^{2}/ndf=%g", scaledchi2ndf), "");
  leg->AddEntry((TObject *)0, Form("B=%g", f1->GetParameter(1)), "");
  leg->AddEntry((TObject *)0, Form("log_{10}(scaled #chi^{2}/ndf)=%g", log10(scaledchi2ndf)), "");
  leg->AddEntry((TObject *)0, Form("B/A=%g", BoverA), "");
  leg->AddEntry((TObject *)0, "", "");
  leg->AddEntry((TObject *)0, Form("C=%g", f1->GetParameter(2)), "");
  leg->Draw();

  // Draw the chip occupancy
  Pad[2][2]->cd();
  h_occupancy->SetStats(0);
  h_occupancy->SetXTitle("Chip Occupancy [%]");
  h_occupancy->SetYTitle("Normalized Entries");
  h_occupancy->DrawCopy();
  gPad->Update();
  gPad->SetLogy();
  // Draw the line to indicate the 95% percentile
  TLine *l95 = new TLine(0.3, gPad->GetUymin(), 0.3, gPad->GetUymax());
  l95->SetLineColor(c_ok);
  l95->SetLineWidth(2);
  l95->SetLineStyle(2);
  l95->Draw();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  std::ostringstream runnostream1;
  std::string runstring1;
  runnostream1 << Name() << "_mvtx Info Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[2]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[2]->Update();

  std::cout << "DrawSummaryInfo Ending" << std::endl;
  return 0;
}
 
int MVTXDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
  {
    return iret;
  }

  QADrawClient *cl = QADrawClient::instance();
  std::string pngfile;

  // Register the 1st canvas png file to the menu and produces the png file.
  if (what == "ALL" || what == "HITS")
  {
    pngfile = cl->htmlRegisterPage(*this, "chip_info", "1", "png");
    cl->CanvasToPng(TC[0], pngfile);
  }
  if (what == "ALL" || what == "CLUSTER")
  {
    pngfile = cl->htmlRegisterPage(*this, "cluster_info", "2", "png");
    cl->CanvasToPng(TC[1], pngfile);
  }
  if (what == "ALL" || what == "SUMMARY")
  {
    pngfile = cl->htmlRegisterPage(*this, "summary_info", "3", "png");
    cl->CanvasToPng(TC[2], pngfile);
  }
  return 0;
}

int MVTXDraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}
