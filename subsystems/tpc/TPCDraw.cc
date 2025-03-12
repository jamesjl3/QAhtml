#include "TPCDraw.h"

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

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

TPCDraw::TPCDraw(const std::string &name)
  : QADraw(name)
{
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  return;
}

TPCDraw::~TPCDraw()
{
  /* delete db; */
  return;
}

int TPCDraw::Draw(const std::string &what)
{
  /* SetsPhenixStyle(); */
  int iret = 0;
  int idraw = 0;
  if (what == "ALLTPC" || what == "HITS")
  {
    iret += DrawChannelHits();
    idraw++;
  }
  if (what == "ALLTPC" || what == "ADCS")
  {
    iret += DrawChannelADCs();
    idraw++;
  }
  if (what == "ALL" || what == "CLUSTERS")
  {
    iret += DrawClusterInfo();
    idraw++;
  }
  if (what == "ALL" || what == "REGIONS")
  {
    iret += DrawRegionInfo();
    idraw++;
  }
  if (!idraw)
  {
    std::cout << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int TPCDraw::MakeCanvas(const std::string &name, int num)
{
  std::cout << num << std::endl;
  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  // xpos (-1) negative: do not draw menu bar
  TC[num] = new TCanvas(name.c_str(), (boost::format("TPC Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2) , (int) (ysize / 1.2));
  gSystem->ProcessEvents();

  if (num != 12 && num != 18)
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
  else if (num == 12)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "no", 0.05, 0.25, 0.45, 0.75);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "no", 0.5, 0.25, 0.95, 0.75);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
  }
  else if (num == 18)
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

  // this one is used to plot the run number on the canvas
  transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[num]->SetFillStyle(4000);
  transparent[num]->Draw();

  return 0;
}

int TPCDraw::DrawChannelHits()
{
  std::cout << "TPC DrawChannelHits() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  // Loop over all sectors, 4 at a time
  for (int quad = 0; quad < 6; quad ++)
  {
    std::vector<TH1F*> h_channel_hits;
    for (int i = 0; i < 4; i++)
    {
      TH1F *h_channel_hits_sec = dynamic_cast <TH1F *> (cl->getHisto((boost::format("h_channel_hits_sec%i") % (quad*4 + i)).str()));
      h_channel_hits.push_back(h_channel_hits_sec);
    }
    
    // make a canvas for the quad grouping
    if (! gROOT->FindObject((boost::format("chan_hits_%i") % quad).str().c_str()))
    {
      MakeCanvas((boost::format("chan_hits_%i") % quad).str(), quad);
    }
    TC[quad]->Clear("D");
    for (int i = 0; i < 4; i++)
    {
      Pad[quad][i]->cd();
      if (h_channel_hits[i])
      {
        h_channel_hits[i]->SetTitle((boost::format("TPC Sector %i Hits Per Channel") % (quad*4 + i)).str().c_str());
        h_channel_hits[i]->SetXTitle("Channel Number");
        h_channel_hits[i]->SetYTitle("Hits");
        h_channel_hits[i]->DrawCopy();
        gPad->SetRightMargin(0.15);
      }
      else
      {
        // histogram is missing
        return -1;
      }
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
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpc Hits Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[quad]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[quad]->Update();
  } 
  std::cout << "DrawChannelHits Ending" << std::endl;
  return 0;
}

int TPCDraw::DrawChannelADCs()
{
  std::cout << "TPC DrawChannelADCs() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  // Loop over all sectors, 4 at a time
  for (int quad = 0; quad < 6; quad ++)
  {
    std::vector<TH2F*> h_channel_adcs;
    for (int i = 0; i < 4; i++)
    {
      TH2F *h_channel_adcs_sec = dynamic_cast <TH2F *> (cl->getHisto((boost::format("h_channel_ADCs_sec%i") % (quad*4 + i)).str()));
      h_channel_adcs.push_back(h_channel_adcs_sec);
    }
    
    // make a canvas for the quad grouping
    if (! gROOT->FindObject((boost::format("chan_ADCs_%i") % quad).str().c_str()))
    {
      MakeCanvas((boost::format("chan_ADCs_%i") % quad).str(), quad + 6);
    }
    TC[quad + 6]->Clear("D");
    for (int i = 0; i < 4; i++)
    {
      Pad[quad + 6][i]->cd();
      if (h_channel_adcs[i])
      {
        h_channel_adcs[i]->SetTitle((boost::format("TPC Sector %i ADCs Per Channel") % (quad*4 + i)).str().c_str());
        h_channel_adcs[i]->SetXTitle("Channel Number");
        h_channel_adcs[i]->SetYTitle("ADC");
        h_channel_adcs[i]->DrawCopy("COLZ");
        gPad->SetRightMargin(0.15);
      }
      else
      {
        // histogram is missing
        return -1;
      }
    }
    
    /* db->DBcommit(); */

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpc ADC Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[quad + 6]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[quad + 6]->Update();
  }
  std::cout << "DrawChannelADCs Ending" << std::endl;
  return 0;
}

int TPCDraw::DrawClusterInfo()
{
  std::cout << "TPC DrawClusterInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  TH2F *h_clusterssector = dynamic_cast <TH2F *> (cl->getHisto("h_TpcClusterQA_ncluspersector"));
  TH2F *h_totalclusters = dynamic_cast <TH2F *> (cl->getHisto("h_TpcClusterQA_stotal_clusters"));

  if (! gROOT->FindObject("tpc_clus_info"))
  {
    MakeCanvas("tpc_clus_info", 12);
  }
  TC[12]->Clear("D");
  Pad[12][0]->cd();
  if (h_clusterssector)
  {
    h_clusterssector->RebinY(2);
    if (m_isStreaming)
    {
      h_clusterssector->GetYaxis()->SetRangeUser(0,5000);
    }   
    else
    {
      h_clusterssector->GetYaxis()->SetRangeUser(0,1000);
    }
    h_clusterssector->DrawCopy("COLZ");
    gPad->SetRightMargin(0.15); 
  } 
  else
  {
    // histogram is missing
    return -1;
  } 
  Pad[12][1]->cd();
  if (h_totalclusters)
  {
    h_totalclusters->RebinY(2);
    if (m_isStreaming)
    {
      h_totalclusters->GetYaxis()->SetRangeUser(0,150);
    }
    else
    {
      h_totalclusters->GetYaxis()->SetRangeUser(0,100);
    }
    h_totalclusters->GetYaxis()->SetTitle("Clusters per Event");
    h_totalclusters->DrawCopy("COLZ");
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
  runnostream1 << Name() << "_tpc_cluster_info Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[12]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[12]->Update();
 
  std::cout << "DrawClusterInfo Ending" << std::endl;
  return 0;
}

int TPCDraw::DrawRegionInfo()
{
  std::cout << "TPC DrawRegionInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  std::vector<std::string> histNames;
  histNames.push_back("clusedge");
  histNames.push_back("clusoverlap");
  histNames.push_back("rphi_error");
  histNames.push_back("z_error");
  histNames.push_back("zsize");
 
  for (unsigned int k = 0; k < histNames.size(); k++)
  {
    std::vector<TH1F*> h_regions;
    for (int reg = 0; reg < 3; reg++)
    {
      TH1F *h_region = dynamic_cast <TH1F *> (cl->getHisto((boost::format("h_TpcClusterQA_%s_%i") % histNames[k] % reg).str()));
      h_regions.push_back(h_region);
    }

    if (! gROOT->FindObject((boost::format("tpc_regions_%s") % histNames[k]).str().c_str()))
    {
      MakeCanvas((boost::format("tpc_regions_%s") % histNames[k]).str(), 13+k);
    }
    TC[13+k]->Clear("D");
    for (int reg = 0; reg < 3; reg++)
    {
      Pad[13+k][reg]->cd();
      if (h_regions[reg])
      { 
        h_regions[reg]->DrawCopy();
        gPad->SetRightMargin(0.15);

        TLatex *title = new TLatex();
        title->SetTextSize(0.06);
        title->SetNDC();
        if (k == 2)
        {
          title->DrawLatex(0.25, 0.75, (boost::format("Region %i") % (reg+1)).str().c_str());
        }
        else
        {
          title->DrawLatex(0.6, 0.75, (boost::format("Region %i") % (reg+1)).str().c_str());
        }
      }
      else
      {
        // histogram is missing
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
    runnostream1 << Name() << "_tpc_region_info Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[13+k]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[13+k]->Update();
  }
  std::vector<TH1F*> h_regions;
  for (int reg = 0; reg < 3; reg++)
  {
    TH1F *h_region_0 = dynamic_cast <TH1F *> (cl->getHisto((boost::format("h_TpcClusterQA_phisize_side0_%i") % reg).str()));
    TH1F *h_region_1 = dynamic_cast <TH1F *> (cl->getHisto((boost::format("h_TpcClusterQA_phisize_side1_%i") % reg).str()));
    h_regions.push_back(h_region_0);
    h_regions.push_back(h_region_1);
  }

  if (! gROOT->FindObject("tpc_regions_phisize"))
  {
    MakeCanvas("tpc_regions_phisize", 18);
  }
  TC[18]->Clear("D");
  for (int reg = 0; reg < 3; reg++)
  {
    Pad[18][2*reg]->cd();
    if (h_regions[2*reg])
    {
      h_regions[2*reg]->DrawCopy();
      gPad->SetRightMargin(0.15);
        
      TLatex *title = new TLatex();
      title->SetTextSize(0.06);
      title->SetNDC();
      title->DrawLatex(0.35, 0.75, (boost::format("Region %i Side 0") % (reg+1)).str().c_str());
    }
    else
    {
      // histogram is missing
      return -1; 
    }
    Pad[18][2*reg + 1]->cd();
    if (h_regions[2*reg + 1])
    {
      h_regions[2*reg + 1]->DrawCopy();
      gPad->SetRightMargin(0.15);
      
      TLatex *title = new TLatex();
      title->SetTextSize(0.06);
      title->SetNDC();
      title->DrawLatex(0.35, 0.75, (boost::format("Region %i Side 1") % (reg+1)).str().c_str());
    }
    else
    {
      // histogram is missing
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
  runnostream1 << Name() << "_tpc_region_info Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[18]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[18]->Update();
  std::cout << "DrawRegionInfo Ending" << std::endl;

  return 0;
}
 
int TPCDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
  {
    return iret;
  }

  QADrawClient *cl = QADrawClient::instance();
  std::string pngfile;

  // Register the 1st canvas png file to the menu and produces the png file.
  if (what == "ALLTPC" || what == "HITS")
  {
    for (int quad = 0; quad < 6; quad++)
    {
      pngfile = cl->htmlRegisterPage(*this, (boost::format("TPC_Hits_s%i-%i") % (4*quad) % (4*quad + 3)).str(), (boost::format("%i") % (quad + 1)).str(), "png");
      cl->CanvasToPng(TC[quad], pngfile);
    }
  }
  if (what == "ALLTPC" || what == "ADCS")
  {
    for (int quad = 0; quad < 6; quad++)
    {
      pngfile = cl->htmlRegisterPage(*this, (boost::format("TPC_ADCs_s%i-%i") % (4*quad) % (4*quad + 3)).str(), (boost::format("%i") % (quad + 7)).str(), "png");
      cl->CanvasToPng(TC[quad + 6], pngfile);
    }
  }
  if (what == "ALL" || what == "CLUSTERS")
  {
    pngfile = cl->htmlRegisterPage(*this, "tpc_clus_info", "13", "png");
    cl->CanvasToPng(TC[12], pngfile);
  }
  if (what == "ALL" || what == "REGIONS")
  {
    std::vector<std::string> histNames;
    histNames.push_back("clusedge");
    histNames.push_back("clusoverlap");
    histNames.push_back("rphi_error");
    histNames.push_back("z_error");
    histNames.push_back("zsize");
    histNames.push_back("phisize");
    for (int quad = 0; quad < 6; quad++)
    {
      pngfile = cl->htmlRegisterPage(*this, (boost::format("TPC_Regions_%s") % histNames[quad]).str(), (boost::format("%i") % (quad + 14)).str(), "png");
      cl->CanvasToPng(TC[quad + 13], pngfile);
    }
  }
  return 0;
}

int TPCDraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}
