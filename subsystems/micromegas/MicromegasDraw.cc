#include "MicromegasDraw.h"

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
#include <TLatex.h>
#include <TLegend.h>


#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

MicromegasDraw::MicromegasDraw(const std::string &name)
  : QADraw(name)
{
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  return;
}

MicromegasDraw::~MicromegasDraw()
{
  /* delete db; */
  return;
}

int MicromegasDraw::Draw(const std::string &what)
{
  /* SetsPhenixStyle(); */
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "CLUSTERS")
    {
      iret += DrawClusterInfo();
      idraw++;
    }
  if (what== "ALL" || what == "BCO")
    {
      iret += DrawBCOInfo();
      idraw++;
    }
  if (!idraw)
    {
      std::cout << " Unimplemented Drawing option: " << what << std::endl;
      iret = -1;
    }
  return iret;
}

TH1F* MicromegasDraw::ClusterAverage(TH2F* hist, std::string type) 
{
  int nX = hist->GetNbinsX();  
  int nY = hist->GetNbinsY(); 

  TH1F* graph = new TH1F( Form("avg_%s", type.c_str()), "", nX, hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());

  std::vector<double> xValues, yAverages;

  for (int i = 1; i <= nX; ++i)
    {
      double sum_y = 0; 
      double totalEntries = 0;  
      for (int j = 1; j <= nY; ++j) 
	{
	  double value = hist->GetBinContent(i, j); 
	  double center = hist->GetYaxis()->GetBinCenter(j);  
	  sum_y += value * center;
	  totalEntries += value;   
	}

      double average;
      if (totalEntries > 0){average = sum_y / totalEntries;} else{average = 0;}
      graph->SetBinContent(i, average);
      const char* label = hist->GetXaxis()->GetBinLabel(i);
      graph->GetXaxis()->SetBinLabel(i, label); 
    }

  graph->SetMarkerStyle(8);

  return graph;
}

int MicromegasDraw::BinValues(TH1F* hist)
{
    if (!hist) return 0;
    for (int i = 1; i <= hist->GetNbinsX(); ++i)  
    {
      double x = hist->GetXaxis()->GetBinCenter(i);
      double y = hist->GetBinContent(i) + 0.03 * hist->GetMaximum();
      TLatex* latex = new TLatex();
      latex->SetTextSize(0.05); 
      latex->SetTextAlign(22);
      latex->DrawLatex(x, y, Form("%.5f", hist->GetBinContent(i)));
      delete latex;
      }
    return 0;
}


int MicromegasDraw::MakeCanvas(const std::string &name, int num)
{
  QADrawClient *cl = QADrawClient::instance();
  gStyle->SetOptTitle(1);
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  TC[num] = new TCanvas(name.c_str(), (boost::format("Micromegas Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2) , (int) (ysize / 1.2));
  gSystem->ProcessEvents();

  Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.52, 0.45, 0.97, 0);
  Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.5, 0.52, 0.95, 0.97, 0);
  Pad[num][2] = new TPad((boost::format("mypad%d2") % num).str().c_str(), "name", 0.05, 0.02, 0.45, 0.47, 0);
  Pad[num][3] = new TPad((boost::format("mypad%d3") % num).str().c_str(), "here", 0.5, 0.02, 0.95, 0.47, 0);
 
  for (int i=0; i<4; i++)
    {
      Pad[num][i]->SetTopMargin(0.15); 
      Pad[num][i]->SetBottomMargin(0.15);
      Pad[num][i]->SetRightMargin(0.15);
      Pad[num][i]->SetLeftMargin(0.15);
    }
   
  Pad[num][0]->Draw();
  Pad[num][1]->Draw();
  Pad[num][2]->Draw();
  Pad[num][3]->Draw();
  
  // this one is used to plot the run number on the canvas
  transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[num]->SetFillStyle(4000);
  transparent[num]->Draw();

  return 0;
}

int MicromegasDraw::DrawClusterInfo()
{
  QADrawClient *cl = QADrawClient::instance();

  TH1F* h_cluster_count_ref = dynamic_cast<TH1F*>(cl->getHisto("h_MicromegasClusterQA_clustercount_ref"));
  TH1F* h_cluster_count_found = dynamic_cast<TH1F*>(cl->getHisto("h_MicromegasClusterQA_clustercount_found"));
  TH2F* h_cluster_multiplicity_raw = dynamic_cast<TH2F*>(cl->getHisto("h_MicromegasClusterQA_cluster_multiplicity"));
  TH2F* h_cluster_size_raw = dynamic_cast<TH2F*>(cl->getHisto("h_MicromegasClusterQA_cluster_size"));
  TH2F* h_cluster_charge_raw = dynamic_cast<TH2F*>(cl->getHisto("h_MicromegasClusterQA_cluster_charge"));

  if (!h_cluster_count_ref || !h_cluster_count_found || !h_cluster_multiplicity_raw || !h_cluster_size_raw || !h_cluster_charge_raw)
  {
    std::cerr << "Error: One or more histograms could not be retrieved." << std::endl;
    return -1;
  }

  TH1F *efficiency = (TH1F*)h_cluster_count_found->Clone("efficiency");
  efficiency->Divide(h_cluster_count_ref);

  TH1F* h_cluster_multiplicity = ClusterAverage(h_cluster_multiplicity_raw, "mult");
  TH1F* h_cluster_size = ClusterAverage(h_cluster_size_raw, "size");
  TH1F* h_cluster_charge = ClusterAverage(h_cluster_charge_raw, "charge");

  if (!TC[0])
  {
    MakeCanvas("ClusterQA", 0);
  }

  TC[0]->cd();
  TC[0]->Clear("D");

  Pad[0][1]->cd();
  h_cluster_multiplicity->SetTitle("Cluster Multiplicity");
  h_cluster_multiplicity->GetXaxis()->SetTitle("Chamber");
  h_cluster_multiplicity->GetYaxis()->SetTitle("Multiplicity");
  h_cluster_multiplicity->SetMinimum(0);
  h_cluster_multiplicity->SetMaximum(10);
  h_cluster_multiplicity->DrawCopy("P");

  Pad[0][2]->cd();
  h_cluster_size->SetTitle("Cluster Size");
  h_cluster_size->GetXaxis()->SetTitle("Chamber");
  h_cluster_size->GetYaxis()->SetTitle("Size");
  h_cluster_size->SetMinimum(0);
  h_cluster_size->SetMaximum(8);
  h_cluster_size->DrawCopy("P");

  Pad[0][0]->cd();
  h_cluster_charge->SetTitle("Cluster Charge");
  h_cluster_charge->GetXaxis()->SetTitle("Chamber");
  h_cluster_charge->GetYaxis()->SetTitle("Charge");
  h_cluster_charge->SetMinimum(0);
  h_cluster_charge->SetMaximum(1000);
  h_cluster_charge->DrawCopy("P");
  
  Pad[0][3]->cd();
  efficiency->SetMinimum(0);
  efficiency->SetMaximum(1);
  efficiency->SetTitle("Efficiency Estimate by Chamber");
  efficiency->GetXaxis()->SetTitle("Chamber");
  efficiency->GetYaxis()->SetTitle("Efficiency");
  efficiency->DrawCopy();

  TC[0]->Update();

  return 0;
}

int MicromegasDraw::DrawBCOInfo()
{
  QADrawClient *cl = QADrawClient::instance();
  TH1F* h_waveform_bco_dropped = dynamic_cast<TH1F*>(cl->getHisto("h_MicromegasBCOQA_waveform_count_dropped_bco"));
  TH1F* h_waveform_pool_dropped = dynamic_cast<TH1F*>(cl->getHisto("h_MicromegasBCOQA_waveform_count_dropped_pool"));
  TH1F* h_waveform_total = dynamic_cast<TH1F*>(cl->getHisto("h_MicromegasBCOQA_waveform_count_total"));
  TH1F* h_gl1_raw = dynamic_cast<TH1F*>(cl->getHisto("h_MicromegasBCOQA_packet_stat"));

  if (!h_waveform_bco_dropped || !h_waveform_pool_dropped || !h_waveform_total || !h_gl1_raw)
    {
      std::cerr << "Error: One or more histograms could not be retrieved." << std::endl;
      return -1;
    }

  TH1F *h_drop= new TH1F("h_drop", "Drop Rate", 3, 0, 3);
  
  h_drop->GetXaxis()->SetBinLabel(1, "5001");
  h_drop->GetXaxis()->SetBinLabel(2, "5002");
  h_drop->GetXaxis()->SetBinLabel(3, "all");
  h_drop->GetXaxis()->SetTitle("Packet");
  h_drop->GetYaxis()->SetTitle("Waveform Drop Rate");
  h_drop->SetTitle("Fraction of Dropped Waveforms by packet");

  h_drop->SetBinContent(1, ( h_waveform_bco_dropped->GetBinContent(1)+ h_waveform_pool_dropped->GetBinContent(1))/h_waveform_total->GetBinContent(1));
  h_drop->SetBinContent(2, ( h_waveform_bco_dropped->GetBinContent(2)+ h_waveform_pool_dropped->GetBinContent(2))/h_waveform_total->GetBinContent(2)); 
  h_drop->SetBinContent(3, ( h_waveform_bco_dropped->GetBinContent(1)+ h_waveform_pool_dropped->GetBinContent(1)+h_waveform_bco_dropped->GetBinContent(2)+ h_waveform_pool_dropped->GetBinContent(2))/(h_waveform_total->GetBinContent(1)+h_waveform_total->GetBinContent(2)) ); 

  TH1F *h_gl1= new TH1F("h_gl1", "Match Rate", 3, 0, 3);
  h_gl1->GetXaxis()->SetBinLabel(1, "5001");
  h_gl1->GetXaxis()->SetBinLabel(2, "5002");
  h_gl1->GetXaxis()->SetBinLabel(3, "all");

  h_gl1->SetBinContent(3,h_gl1_raw->GetBinContent(4)/h_gl1_raw->GetBinContent(1));
  h_gl1->SetBinContent(2,h_gl1_raw->GetBinContent(3)/h_gl1_raw->GetBinContent(1));
  h_gl1->SetBinContent(1,h_gl1_raw->GetBinContent(2)/h_gl1_raw->GetBinContent(1));
  
  if (!TC[1])
  {
    MakeCanvas("BCO_QA", 1);
  }

  TC[1]->cd();
  TC[1]->Clear("D");

  Pad[1][0]->cd();
  h_drop->SetMinimum(0);
  h_drop->SetMaximum(1.6);
  h_drop->SetFillColor(42); 
  h_drop->SetFillStyle(3002);
  h_drop->DrawCopy();

  TLegend* legend_drop = new TLegend(0.56, 0.6, 0.85, 0.84);
  legend_drop->SetHeader("Values", "C");
  legend_drop->SetTextSize(0.045);
  legend_drop->SetBorderSize(0);
  legend_drop->SetFillStyle(0);

  for (int i = 1; i <= h_drop->GetNbinsX(); ++i)  
  {
    legend_drop->AddEntry((TObject*)0, Form("%s: %.4f", h_drop->GetXaxis()->GetBinLabel(i), h_drop->GetBinContent(i)), "");
  }
  legend_drop->Draw();
  gPad->Update(); 

  Pad[1][1]->cd();
  h_gl1->GetXaxis()->SetTitle("Packet");
  h_gl1->GetYaxis()->SetTitle("Match Rate");
  h_gl1->SetTitle("Matching Tagger Rate by packet");
  h_gl1->SetFillColor(42); 
  h_gl1->SetFillStyle(3002);
  h_gl1->SetMinimum(0);
  h_gl1->SetMaximum(1.6);
  h_gl1->DrawCopy();

  TLegend* legend_gl1 = new TLegend(0.65, 0.6, 0.85, 0.84);
  legend_gl1->SetHeader("Values", "C");
  legend_gl1->SetTextSize(0.045);
  legend_gl1->SetBorderSize(0);
  legend_gl1->SetFillStyle(0);

  for (int i = 1; i <= h_gl1->GetNbinsX(); ++i)  
  {
    legend_gl1->AddEntry((TObject*)0, Form("%s: %.4f", h_gl1->GetXaxis()->GetBinLabel(i), h_gl1->GetBinContent(i)), "");
  }
  legend_gl1->Draw();
  gPad->Update(); 

  TC[1]->Update();

  return 0;
}

int MicromegasDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
  {
    return iret;
  }

  QADrawClient *cl = QADrawClient::instance();
  std::string pngfile;

  // Register the canvas png file to the menu and produces the png file.
  
  if (what == "ALL" || what == "CLUSTERS")
    {
      pngfile = cl->htmlRegisterPage(*this, "cluster_info", "1", "png");
      cl->CanvasToPng(TC[0], pngfile);
    }
  if (what == "ALL" || what == "BCO")
    {
      pngfile = cl->htmlRegisterPage(*this, "bco_info", "2", "png");
      cl->CanvasToPng(TC[1], pngfile);

    }

  return 0;
}

int MicromegasDraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}
