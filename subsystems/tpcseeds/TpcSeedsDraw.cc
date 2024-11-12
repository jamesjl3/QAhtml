#include "TpcSeedsDraw.h"

#include <sPhenixStyle.C>

#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>

#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TPad.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TF1.h>
#include <TLine.h>

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

TpcSeedsDraw::TpcSeedsDraw(const std::string &name)
    : QADraw(name)
{
    gStyle->SetOptStat(0);
    memset(TC, 0, sizeof(TC));
    memset(transparent, 0, sizeof(transparent));
    memset(Pad, 0, sizeof(Pad));
    DBVarInit();
    histprefix = "h_TpcSeedsQA_";
    return;
}

TpcSeedsDraw::~TpcSeedsDraw()
{
    /* delete db; */
    return;
}

int TpcSeedsDraw::Draw(const std::string &what)
{
    /* SetsPhenixStyle(); */
    int iret = 0;
    int idraw = 0;
    if (what == "ALL" || what == "TRACKLET")
    {
        iret += DrawTrackletInfo();
        idraw++;
    }
    if (what == "ALL" || what == "CLUSTER1")
    {
        iret += DrawClusterInfo1();
        idraw++;
    }
    if (what == "ALL" || what == "CLUSTER2")
    {
        iret += DrawClusterInfo2();
        idraw++;
    }
    if (what == "ALL" || what == "DCA1")
    {
        iret += DrawDCAInfo1();
        idraw++;
    }
    if (what == "ALL" || what == "DCA2")
    {
        iret += DrawDCAInfo2();
        idraw++;
    }
    if (what == "ALL" || what == "VERTEX")
    {
        iret += DrawVertexInfo();
        idraw++;
    }
    if (what == "ALL" || what == "DEDX1")
    {
        iret += DrawdEdxInfo1();
        idraw++;
    }
    if (what == "ALL" || what == "DEDX2")
    {
        iret += DrawdEdxInfo2();
        idraw++;
    }
    if (!idraw)
    {
        std::cout << " Unimplemented Drawing option: " << what << std::endl;
        iret = -1;
    }
    return iret;
}

int TpcSeedsDraw::MakeCanvas(const std::string &name, int num)
{
    std::cout << "Tpc Seeds MakeCanvas() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();
    int xsize = cl->GetDisplaySizeX();
    int ysize = cl->GetDisplaySizeY();
    // xpos (-1) negative: do not draw menu bar
    TC[num] = new TCanvas(name.c_str(), (boost::format("Tpc Seeds Plots %d") % num).str().c_str(), -1, 0, (int)(xsize), (int)(ysize * 2.2));
    TC[num]->SetCanvasSize(xsize, ysize * 2.2);
    gSystem->ProcessEvents();

    int nrow = 4;
    if (num==6) nrow = 5;
    double yoffset = 0.02;
    double ywidth=(1.-yoffset-yoffset)/(double)nrow;
    double x1=0, y1=0, x2=0, y2=0;
    for (int i=0; i<2*nrow; i++)
    {
        if (i%2==0)
        {
          x1=0.5;
          x2=0.95;
        }
        else if (i%2==1)
        {
          x1=0.05;
          x2=0.5;
        }
        y1=0.02+(i/2)*ywidth;
        y2=0.02+(i/2+1)*ywidth;

        Pad[num][2*nrow-1-i] = new TPad(
                                (boost::format("mypad_%1%_%2%") % num % i).str().c_str(),
                                "pad",
                                x1, y1, x2, y2
                                );
    }

    for (int i=0; i<2*nrow; i++) Pad[num][2*nrow-1-i]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
    transparent[num]->SetFillStyle(4000);
    transparent[num]->Draw();

    return 0;
}

int TpcSeedsDraw::DrawTrackletInfo()
{
    std::cout << "Tpc Seeds DrawTrackletInfo() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH1F *h_ntrack1d = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("nrecotracks1d")));
    TH1F *h_ntrack1d_pos = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("nrecotracks1d_pos")));
    TH1F *h_ntrack1d_neg = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("nrecotracks1d_neg")));
    TH1F *h_ntrack1d_ptg1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("nrecotracks1d_ptg1")));
    TH1F *h_ntrack1d_ptg1_pos = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("nrecotracks1d_ptg1_pos")));
    TH1F *h_ntrack1d_ptg1_neg = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("nrecotracks1d_ptg1_neg")));
    TH1F *h_pt = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("pt")));
    TH1F *h_pt_pos = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("pt_pos")));
    TH1F *h_pt_neg = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("pt_neg")));
    TH2F *h_ntrack_pos = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("nrecotracks_pos")));
    TH2F *h_ntrack_neg = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("nrecotracks_neg")));
    //TH1F *h_trackcrossing_pos = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("trackcrossing_pos")));
    //TH1F *h_trackcrossing_neg = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("trackcrossing_neg")));
    TH1F *h_ntrack_isfromvtx_pos = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntrack_isfromvtx_pos")));
    TH1F *h_ntrack_isfromvtx_neg = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntrack_isfromvtx_neg")));

    const int index_page = 0;

    if (!gROOT->FindObject("track_info"))
    {
        MakeCanvas("track_info", index_page);
    }
    TC[index_page]->Clear("D");

    Pad[index_page][0]->cd();
    if (h_ntrack1d && h_ntrack1d_pos && h_ntrack1d_neg)
    {
        h_ntrack1d->SetTitle("Number of tpc tracks");
        h_ntrack1d->SetXTitle("Number of tpc tracks");
        h_ntrack1d->SetYTitle("Entries");
        h_ntrack1d->SetMarkerColor(kBlack);
        h_ntrack1d->SetLineColor(kBlack);
        auto ymax = h_ntrack1d->GetMaximum();
        auto ymax_pos = h_ntrack1d_pos->GetMaximum();
        auto ymax_neg = h_ntrack1d_neg->GetMaximum();
        h_ntrack1d->SetMaximum(1.2*std::max({ymax, ymax_pos, ymax_neg}));
        h_ntrack1d->DrawCopy("");
        h_ntrack1d_pos->SetMarkerColor(kRed);
        h_ntrack1d_pos->SetLineColor(kRed);
        h_ntrack1d_pos->DrawCopy("same");
        h_ntrack1d_neg->SetMarkerColor(kBlue);
        h_ntrack1d_neg->SetLineColor(kBlue);
        h_ntrack1d_neg->DrawCopy("same");
        auto legend = new TLegend(0.55, 0.7, 0.83, 0.9);
        legend->AddEntry(h_ntrack1d, "Positive + Negative", "pl");
        legend->AddEntry(h_ntrack1d_pos, "Positive charged", "pl");
        legend->AddEntry(h_ntrack1d_neg, "Negative charged", "pl");
        legend->Draw();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][1]->cd();
    Pad[index_page][1]->SetLogy(1);
    if (h_ntrack1d_ptg1 && h_ntrack1d_ptg1_pos && h_ntrack1d_ptg1_neg)
    {
        h_ntrack1d_ptg1->SetTitle("Number of tpc tracks with p_{T}>1GeV");
        h_ntrack1d_ptg1->SetXTitle("Number of tpc tracks");
        h_ntrack1d_ptg1->SetYTitle("Entries");
        h_ntrack1d_ptg1->SetMarkerColor(kBlack);
        h_ntrack1d_ptg1->SetLineColor(kBlack);
        auto ymax = h_ntrack1d_ptg1->GetMaximum();
        auto ymax_pos = h_ntrack1d_ptg1_pos->GetMaximum();
        auto ymax_neg = h_ntrack1d_ptg1_neg->GetMaximum();
        h_ntrack1d_ptg1->SetMaximum(1.2*std::max({ymax, ymax_pos, ymax_neg}));
        h_ntrack1d_ptg1->DrawCopy("");
        h_ntrack1d_ptg1_pos->SetMarkerColor(kRed);
        h_ntrack1d_ptg1_pos->SetLineColor(kRed);
        h_ntrack1d_ptg1_pos->DrawCopy("same");
        h_ntrack1d_ptg1_neg->SetMarkerColor(kBlue);
        h_ntrack1d_ptg1_neg->SetLineColor(kBlue);
        h_ntrack1d_ptg1_neg->DrawCopy("same");
        auto legend = new TLegend(0.55, 0.7, 0.83, 0.9);
        legend->AddEntry(h_ntrack1d_ptg1, "Positive + Negative", "pl");
        legend->AddEntry(h_ntrack1d_ptg1_pos, "Positive charged", "pl");
        legend->AddEntry(h_ntrack1d_ptg1_neg, "Negative charged", "pl");
        legend->Draw();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][2]->cd();
    Pad[index_page][2]->SetLogy(1);
    if (h_pt && h_pt_pos && h_pt_neg)
    {
        h_pt->SetTitle("p_{T} distribution");
        h_pt->SetXTitle("p_{T} [GeV]");
        h_pt->SetYTitle("Entries");
        h_pt->SetMarkerColor(kBlack);
        h_pt->SetLineColor(kBlack);
        auto ymax = h_pt->GetMaximum();
        auto ymax_pos = h_pt_pos->GetMaximum();
        auto ymax_neg = h_pt_neg->GetMaximum();
        h_pt->SetMaximum(1.2*std::max({ymax, ymax_pos, ymax_neg}));
        h_pt->DrawCopy("");
        h_pt_pos->SetMarkerColor(kRed);
        h_pt_pos->SetLineColor(kRed);
        h_pt_pos->DrawCopy("same");
        h_pt_neg->SetMarkerColor(kBlue);
        h_pt_neg->SetLineColor(kBlue);
        h_pt_neg->DrawCopy("same");
        auto legend = new TLegend(0.55, 0.7, 0.83, 0.9);
        legend->AddEntry(h_pt, "Positive + Negative", "pl");
        legend->AddEntry(h_pt_pos, "Positive charged", "pl");
        legend->AddEntry(h_pt_neg, "Negative charged", "pl");
        legend->Draw();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

/*
    Pad[index_page][3]->cd();
    if (h_trackcrossing_pos && h_trackcrossing_neg)
    {
        h_trackcrossing_pos->SetXTitle("Track crossing");
        h_trackcrossing_pos->SetYTitle("Entries");
        h_trackcrossing_pos->SetMarkerColor(kRed);
        h_trackcrossing_pos->SetLineColor(kRed);
        auto ymax_pos = h_trackcrossing_pos->GetMaximum();
        auto ymax_neg = h_trackcrossing_neg->GetMaximum();
        h_trackcrossing_pos->SetMaximum(1.2*std::max({ymax_pos, ymax_neg}));
        h_trackcrossing_pos->DrawCopy();
        h_trackcrossing_neg->SetMarkerColor(kBlue);
        h_trackcrossing_neg->SetLineColor(kBlue);
        h_trackcrossing_neg->DrawCopy("same");
        auto legend = new TLegend(0.55, 0.7, 0.83, 0.9);
        legend->AddEntry(h_trackcrossing_pos, "Positive charged", "pl");
        legend->AddEntry(h_trackcrossing_neg, "Negative charged", "pl");
        legend->Draw();
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }
*/

    Pad[index_page][4]->cd();
    if (h_ntrack_pos)
    {
        h_ntrack_pos->SetXTitle("#eta");
        h_ntrack_pos->SetYTitle("#phi [rad]");
        h_ntrack_pos->SetZTitle("Number of positive tracks with p_{T}>1GeV");
        h_ntrack_pos->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][5]->cd();
    if (h_ntrack_neg)
    {
        h_ntrack_neg->SetXTitle("#eta");
        h_ntrack_neg->SetYTitle("#phi [rad]");
        h_ntrack_neg->SetZTitle("Number of negative tracks with p_{T}>1GeV");
        h_ntrack_neg->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][6]->cd();
    if (h_ntrack_isfromvtx_pos)
    {
        h_ntrack_isfromvtx_pos->SetTitle("Is positive track from a vertex");
        h_ntrack_isfromvtx_pos->SetXTitle("Is positive track associated to a vertex");
        h_ntrack_isfromvtx_pos->SetYTitle("Normalized Entries");
        h_ntrack_isfromvtx_pos->Scale(1. / h_ntrack_isfromvtx_pos->Integral());
        h_ntrack_isfromvtx_pos->SetMarkerSize(1.5);
        h_ntrack_isfromvtx_pos->DrawCopy("histtext0");
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][7]->cd();
    if (h_ntrack_isfromvtx_neg)
    {
        h_ntrack_isfromvtx_neg->SetTitle("Is negative track from a vertex");
        h_ntrack_isfromvtx_neg->SetXTitle("Is negative track associated to a vertex");
        h_ntrack_isfromvtx_neg->SetYTitle("Normalized Entries");
        h_ntrack_isfromvtx_neg->Scale(1. / h_ntrack_isfromvtx_neg->Integral());
        h_ntrack_isfromvtx_neg->SetMarkerSize(1.5);
        h_ntrack_isfromvtx_neg->DrawCopy("histtext0");
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.03);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpcseeds Track Info Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[index_page]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[index_page]->Update();

    std::cout << "DrawTrackletInfo Ending" << std::endl;
    return 0;
}

int TpcSeedsDraw::DrawClusterInfo1()
{
    std::cout << "Tpc Seeds DrawClusterInfo1() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH1F *h_ntpc_pos = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntpc_pos")));
    TH1F *h_ntpc_neg = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntpc_neg")));
    TH1F *h_ntpot_pos = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntpot_pos")));
    TH1F *h_ntpot_neg = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntpot_neg")));
    TProfile2D *h_avgnclus_eta_phi_pos = dynamic_cast<TProfile2D *>(cl->getHisto(histprefix + std::string("avgnclus_eta_phi_pos")));
    TProfile2D *h_avgnclus_eta_phi_neg = dynamic_cast<TProfile2D *>(cl->getHisto(histprefix + std::string("avgnclus_eta_phi_neg")));
    TH1F *h_clusphisize1frac_side0_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_side0_0")));
    TH1F *h_clusphisize1frac_side0_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_side0_1")));
    TH1F *h_clusphisize1frac_side0_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_side0_2")));
    TH1F *h_clusphisize1frac_side1_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_side1_0")));
    TH1F *h_clusphisize1frac_side1_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_side1_1")));
    TH1F *h_clusphisize1frac_side1_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_side1_2")));
    TH1F *h_clusphisize1pT_side0_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1pT_side0_0")));
    TH1F *h_clusphisize1pT_side0_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1pT_side0_1")));
    TH1F *h_clusphisize1pT_side0_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1pT_side0_2")));
    TH1F *h_clusphisize1pT_side1_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1pT_side1_0")));
    TH1F *h_clusphisize1pT_side1_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1pT_side1_1")));
    TH1F *h_clusphisize1pT_side1_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1pT_side1_2")));
    TH1F *h_clusphisizegeq1pT_side0_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisizegeq1pT_side0_0")));
    TH1F *h_clusphisizegeq1pT_side0_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisizegeq1pT_side0_1")));
    TH1F *h_clusphisizegeq1pT_side0_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisizegeq1pT_side0_2")));
    TH1F *h_clusphisizegeq1pT_side1_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisizegeq1pT_side1_0")));
    TH1F *h_clusphisizegeq1pT_side1_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisizegeq1pT_side1_1")));
    TH1F *h_clusphisizegeq1pT_side1_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisizegeq1pT_side1_2")));

    const int index_page = 1;

    if (!gROOT->FindObject("cluster_info1"))
    {
        MakeCanvas("cluster_info1", index_page);
    }
    TC[index_page]->Clear("D");

    Pad[index_page][0]->cd();
    if (h_ntpc_pos && h_ntpc_neg)
    {
        h_ntpc_pos->SetTitle("Number of TPC clusters for track with p_{T}>1GeV");
        h_ntpc_pos->SetXTitle("Number of TPC clusters");
        h_ntpc_pos->SetYTitle("Entries");
        h_ntpc_pos->SetMarkerColor(kRed);
        h_ntpc_pos->SetLineColor(kRed);
        auto ymax_pos = h_ntpc_pos->GetMaximum();
        auto ymax_neg = h_ntpc_neg->GetMaximum();
        h_ntpc_pos->SetMaximum(1.2*std::max({ymax_pos, ymax_neg}));
        h_ntpc_pos->DrawCopy("");
        h_ntpc_neg->SetMarkerColor(kBlue);
        h_ntpc_neg->SetLineColor(kBlue);
        h_ntpc_neg->DrawCopy("same");
        auto legend = new TLegend(0.55, 0.7, 0.83, 0.9);
        legend->AddEntry(h_ntpc_pos, "Positive charged", "pl");
        legend->AddEntry(h_ntpc_neg, "Negative charged", "pl");
        legend->Draw();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][1]->cd();
    if (h_ntpot_pos && h_ntpot_neg)
    {
        h_ntpot_pos->SetTitle("Number of TPOT clusters for track with p_{T}>1GeV");
        h_ntpot_pos->SetXTitle("Number of TPOT clusters");
        h_ntpot_pos->SetYTitle("Entries");
        h_ntpot_pos->SetMarkerColor(kRed);
        h_ntpot_pos->SetLineColor(kRed);
        auto ymax_pos = h_ntpot_pos->GetMaximum();
        auto ymax_neg = h_ntpot_neg->GetMaximum();
        h_ntpot_pos->SetMaximum(1.2*std::max({ymax_pos, ymax_neg}));
        h_ntpot_pos->DrawCopy("");
        h_ntpot_neg->SetMarkerColor(kBlue);
        h_ntpot_neg->SetLineColor(kBlue);
        h_ntpot_neg->DrawCopy("same");
        auto legend = new TLegend(0.55, 0.7, 0.83, 0.9);
        legend->AddEntry(h_ntpc_pos, "Positive charged", "pl");
        legend->AddEntry(h_ntpc_neg, "Negative charged", "pl");
        legend->Draw();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][2]->cd();
    if (h_avgnclus_eta_phi_pos)
    {
        h_avgnclus_eta_phi_pos->SetXTitle("#eta");
        h_avgnclus_eta_phi_pos->SetYTitle("#phi [rad]");
        h_avgnclus_eta_phi_pos->SetZTitle("Average number of clusters per positive track with p_{T}>1GeV");
        h_avgnclus_eta_phi_pos->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][3]->cd();
    if (h_avgnclus_eta_phi_neg)
    {
        h_avgnclus_eta_phi_neg->SetXTitle("#eta");
        h_avgnclus_eta_phi_neg->SetYTitle("#phi [rad]");
        h_avgnclus_eta_phi_neg->SetZTitle("Average number of clusters per negative track with p_{T}>1GeV");
        h_avgnclus_eta_phi_neg->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][4]->cd();
    if (h_clusphisize1pT_side0_0 && h_clusphisize1pT_side0_1 && h_clusphisize1pT_side0_2
     && h_clusphisize1pT_side1_0 && h_clusphisize1pT_side1_1 && h_clusphisize1pT_side1_2)
    {
        h_clusphisize1pT_side0_0->SetTitle("p_{T} distribution with TPC Cluster Phi Size == 1");
        h_clusphisize1pT_side0_0->SetXTitle("p_{T} [GeV]");
        h_clusphisize1pT_side0_0->SetYTitle("Entries");
        h_clusphisize1pT_side0_0->SetMarkerColor(kRed);
        h_clusphisize1pT_side0_0->SetLineColor(kRed);
        auto ymax_side0_0 = h_clusphisize1pT_side0_0->GetMaximum();
        auto ymax_side0_1 = h_clusphisize1pT_side0_1->GetMaximum();
        auto ymax_side0_2 = h_clusphisize1pT_side0_2->GetMaximum();
        auto ymax_side1_0 = h_clusphisize1pT_side1_0->GetMaximum();
        auto ymax_side1_1 = h_clusphisize1pT_side1_1->GetMaximum();
        auto ymax_side1_2 = h_clusphisize1pT_side1_2->GetMaximum();
        h_clusphisize1pT_side0_0->SetMaximum(1.2*std::max({ymax_side0_0,ymax_side0_1,ymax_side0_2,ymax_side1_0,ymax_side1_1,ymax_side1_2}));
        h_clusphisize1pT_side0_0->DrawCopy();
        h_clusphisize1pT_side0_1->SetMarkerColor(kBlue);
        h_clusphisize1pT_side0_1->SetLineColor(kBlue);
        h_clusphisize1pT_side0_1->DrawCopy("same");
        h_clusphisize1pT_side0_2->SetMarkerColor(kGreen);
        h_clusphisize1pT_side0_2->SetLineColor(kGreen);
        h_clusphisize1pT_side0_2->DrawCopy("same");
        h_clusphisize1pT_side1_0->SetMarkerColor(kViolet);
        h_clusphisize1pT_side1_0->SetLineColor(kViolet);
        h_clusphisize1pT_side1_0->DrawCopy("same");
        h_clusphisize1pT_side1_1->SetMarkerColor(kOrange);
        h_clusphisize1pT_side1_1->SetLineColor(kOrange);
        h_clusphisize1pT_side1_1->DrawCopy("same");
        h_clusphisize1pT_side1_2->SetMarkerColor(kBlack);
        h_clusphisize1pT_side1_2->SetLineColor(kBlack);
        h_clusphisize1pT_side1_2->DrawCopy("same");
        auto legend = new TLegend(0.55, 0.5, 0.83, 0.9);
        legend->AddEntry(h_clusphisize1pT_side0_0, "south in", "pl");
        legend->AddEntry(h_clusphisize1pT_side0_1, "south mid", "pl");
        legend->AddEntry(h_clusphisize1pT_side0_2, "south out", "pl");
        legend->AddEntry(h_clusphisize1pT_side1_0, "north in", "pl");
        legend->AddEntry(h_clusphisize1pT_side1_1, "north mid", "pl");
        legend->AddEntry(h_clusphisize1pT_side1_2, "north out", "pl");
        legend->Draw();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][5]->cd();
    if (h_clusphisizegeq1pT_side0_0 && h_clusphisizegeq1pT_side0_1 && h_clusphisizegeq1pT_side0_2
     && h_clusphisizegeq1pT_side1_0 && h_clusphisizegeq1pT_side1_1 && h_clusphisizegeq1pT_side1_2)
    {
        h_clusphisizegeq1pT_side0_0->SetTitle("p_{T} distribution with TPC Cluster Phi Size >= 1");
        h_clusphisizegeq1pT_side0_0->SetXTitle("p_{T} [GeV]");
        h_clusphisizegeq1pT_side0_0->SetYTitle("Entries");
        h_clusphisizegeq1pT_side0_0->SetMarkerColor(kRed);
        h_clusphisizegeq1pT_side0_0->SetLineColor(kRed);
        auto ymax_side0_0 = h_clusphisizegeq1pT_side0_0->GetMaximum();
        auto ymax_side0_1 = h_clusphisizegeq1pT_side0_1->GetMaximum();
        auto ymax_side0_2 = h_clusphisizegeq1pT_side0_2->GetMaximum();
        auto ymax_side1_0 = h_clusphisizegeq1pT_side1_0->GetMaximum();
        auto ymax_side1_1 = h_clusphisizegeq1pT_side1_1->GetMaximum();
        auto ymax_side1_2 = h_clusphisizegeq1pT_side1_2->GetMaximum();
        h_clusphisizegeq1pT_side0_0->SetMaximum(1.2*std::max({ymax_side0_0,ymax_side0_1,ymax_side0_2,ymax_side1_0,ymax_side1_1,ymax_side1_2}));
        h_clusphisizegeq1pT_side0_0->DrawCopy();
        h_clusphisizegeq1pT_side0_1->SetMarkerColor(kBlue);
        h_clusphisizegeq1pT_side0_1->SetLineColor(kBlue);
        h_clusphisizegeq1pT_side0_1->DrawCopy("same");
        h_clusphisizegeq1pT_side0_2->SetMarkerColor(kGreen);
        h_clusphisizegeq1pT_side0_2->SetLineColor(kGreen);
        h_clusphisizegeq1pT_side0_2->DrawCopy("same");
        h_clusphisizegeq1pT_side1_0->SetMarkerColor(kViolet);
        h_clusphisizegeq1pT_side1_0->SetLineColor(kViolet);
        h_clusphisizegeq1pT_side1_0->DrawCopy("same");
        h_clusphisizegeq1pT_side1_1->SetMarkerColor(kOrange);
        h_clusphisizegeq1pT_side1_1->SetLineColor(kOrange);
        h_clusphisizegeq1pT_side1_1->DrawCopy("same");
        h_clusphisizegeq1pT_side1_2->SetMarkerColor(kBlack);
        h_clusphisizegeq1pT_side1_2->SetLineColor(kBlack);
        h_clusphisizegeq1pT_side1_2->DrawCopy("same");
        auto legend = new TLegend(0.55, 0.5, 0.83, 0.9);
        legend->AddEntry(h_clusphisizegeq1pT_side0_0, "south in", "pl");
        legend->AddEntry(h_clusphisizegeq1pT_side0_1, "south mid", "pl");
        legend->AddEntry(h_clusphisizegeq1pT_side0_2, "south out", "pl");
        legend->AddEntry(h_clusphisizegeq1pT_side1_0, "north in", "pl");
        legend->AddEntry(h_clusphisizegeq1pT_side1_1, "north mid", "pl");
        legend->AddEntry(h_clusphisizegeq1pT_side1_2, "north out", "pl");
        legend->Draw();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][6]->cd();
    if (h_clusphisize1frac_side0_0 && h_clusphisize1frac_side0_1 && h_clusphisize1frac_side0_2
     && h_clusphisize1frac_side1_0 && h_clusphisize1frac_side1_1 && h_clusphisize1frac_side1_2)
    {
        h_clusphisize1frac_side0_0->SetTitle("Fraction of TPC Cluster Phi Size == 1");
        h_clusphisize1frac_side0_0->SetXTitle("Fraction of TPC Cluster Phi Size == 1");
        h_clusphisize1frac_side0_0->SetYTitle("Entries");
        h_clusphisize1frac_side0_0->SetMarkerColor(kRed);
        h_clusphisize1frac_side0_0->SetLineColor(kRed);
        auto ymax_side0_0 = h_clusphisize1frac_side0_0->GetMaximum();
        auto ymax_side0_1 = h_clusphisize1frac_side0_1->GetMaximum();
        auto ymax_side0_2 = h_clusphisize1frac_side0_2->GetMaximum();
        auto ymax_side1_0 = h_clusphisize1frac_side1_0->GetMaximum();
        auto ymax_side1_1 = h_clusphisize1frac_side1_1->GetMaximum();
        auto ymax_side1_2 = h_clusphisize1frac_side1_2->GetMaximum();
        h_clusphisize1frac_side0_0->SetMaximum(1.2*std::max({ymax_side0_0,ymax_side0_1,ymax_side0_2,ymax_side1_0,ymax_side1_1,ymax_side1_2}));
        h_clusphisize1frac_side0_0->DrawCopy();
        h_clusphisize1frac_side0_1->SetMarkerColor(kBlue);
        h_clusphisize1frac_side0_1->SetLineColor(kBlue);
        h_clusphisize1frac_side0_1->DrawCopy("same");
        h_clusphisize1frac_side0_2->SetMarkerColor(kGreen);
        h_clusphisize1frac_side0_2->SetLineColor(kGreen);
        h_clusphisize1frac_side0_2->DrawCopy("same");
        h_clusphisize1frac_side1_0->SetMarkerColor(kViolet);
        h_clusphisize1frac_side1_0->SetLineColor(kViolet);
        h_clusphisize1frac_side1_0->DrawCopy("same");
        h_clusphisize1frac_side1_1->SetMarkerColor(kOrange);
        h_clusphisize1frac_side1_1->SetLineColor(kOrange);
        h_clusphisize1frac_side1_1->DrawCopy("same");
        h_clusphisize1frac_side1_2->SetMarkerColor(kBlack);
        h_clusphisize1frac_side1_2->SetLineColor(kBlack);
        h_clusphisize1frac_side1_2->DrawCopy("same");
        auto legend = new TLegend(0.55, 0.5, 0.83, 0.9);
        legend->AddEntry(h_clusphisize1frac_side0_0, "south in", "pl");
        legend->AddEntry(h_clusphisize1frac_side0_1, "south mid", "pl");
        legend->AddEntry(h_clusphisize1frac_side0_2, "south out", "pl");
        legend->AddEntry(h_clusphisize1frac_side1_0, "north in", "pl");
        legend->AddEntry(h_clusphisize1frac_side1_1, "north mid", "pl");
        legend->AddEntry(h_clusphisize1frac_side1_2, "north out", "pl");
        legend->Draw();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.03);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpcseeds Cluster Info Page1 Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[index_page]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[index_page]->Update();

    std::cout << "DrawClusterInfo1 Ending" << std::endl;
    return 0;
}

int TpcSeedsDraw::DrawClusterInfo2()
{
    std::cout << "Tpc Seeds DrawClusterInfo2() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH2F *h_clusphisize1frac_pt_side0_0 = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_pt_side0_0")));
    TH2F *h_clusphisize1frac_pt_side0_1 = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_pt_side0_1")));
    TH2F *h_clusphisize1frac_pt_side0_2 = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_pt_side0_2")));
    TH2F *h_clusphisize1frac_pt_side1_0 = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_pt_side1_0")));
    TH2F *h_clusphisize1frac_pt_side1_1 = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_pt_side1_1")));
    TH2F *h_clusphisize1frac_pt_side1_2 = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_pt_side1_2")));

    TH1F *h_clusphisize1frac_mean_numerator_side0_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_numerator_side0_0")));
    TH1F *h_clusphisize1frac_mean_numerator_side0_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_numerator_side0_1")));
    TH1F *h_clusphisize1frac_mean_numerator_side0_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_numerator_side0_2")));
    TH1F *h_clusphisize1frac_mean_numerator_side1_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_numerator_side1_0")));
    TH1F *h_clusphisize1frac_mean_numerator_side1_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_numerator_side1_1")));
    TH1F *h_clusphisize1frac_mean_numerator_side1_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_numerator_side1_2")));

    TH1F *h_clusphisize1frac_mean_denominator_side0_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_denominator_side0_0")));
    TH1F *h_clusphisize1frac_mean_denominator_side0_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_denominator_side0_1")));
    TH1F *h_clusphisize1frac_mean_denominator_side0_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_denominator_side0_2")));
    TH1F *h_clusphisize1frac_mean_denominator_side1_0 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_denominator_side1_0")));
    TH1F *h_clusphisize1frac_mean_denominator_side1_1 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_denominator_side1_1")));
    TH1F *h_clusphisize1frac_mean_denominator_side1_2 = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("clusphisize1frac_mean_denominator_side1_2")));

    const int index_page = 2;

    if (!gROOT->FindObject("cluster_info2"))
    {
        MakeCanvas("cluster_info2", index_page);
    }
    TC[index_page]->Clear("D");

    Pad[index_page][0]->cd();
    if (h_clusphisize1frac_pt_side0_0 && h_clusphisize1frac_mean_numerator_side0_0 && h_clusphisize1frac_mean_denominator_side0_0)
    {
        h_clusphisize1frac_pt_side0_0->SetXTitle("p_{T} [GeV]");
        h_clusphisize1frac_pt_side0_0->SetYTitle("Fraction of TPC Cluster Phi Size == 1");
        h_clusphisize1frac_pt_side0_0->SetZTitle("Entries");
        h_clusphisize1frac_pt_side0_0->SetTitle("TPC south in");
        h_clusphisize1frac_pt_side0_0->DrawCopy("colz");

        TText *text1 = new TText(1.10, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_0->GetBinContent(1) / h_clusphisize1frac_mean_denominator_side0_0->GetBinContent(1)));
        TText *text2 = new TText(1.65, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_0->GetBinContent(2) / h_clusphisize1frac_mean_denominator_side0_0->GetBinContent(2)));
        TText *text3 = new TText(2.20, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_0->GetBinContent(3) / h_clusphisize1frac_mean_denominator_side0_0->GetBinContent(3)));
        TText *text4 = new TText(2.75, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_0->GetBinContent(4) / h_clusphisize1frac_mean_denominator_side0_0->GetBinContent(4)));
        text1->SetTextSize(0.06);
        text2->SetTextSize(0.06);
        text3->SetTextSize(0.06);
        text4->SetTextSize(0.06);
        text1->Draw();
        text2->Draw();
        text3->Draw();
        text4->Draw();

        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][2]->cd();
    if (h_clusphisize1frac_pt_side0_1 && h_clusphisize1frac_mean_numerator_side0_1 && h_clusphisize1frac_mean_denominator_side0_1)
    {
        h_clusphisize1frac_pt_side0_1->SetXTitle("p_{T} [GeV]");
        h_clusphisize1frac_pt_side0_1->SetYTitle("Fraction of TPC Cluster Phi Size == 1");
        h_clusphisize1frac_pt_side0_1->SetZTitle("Entries");
        h_clusphisize1frac_pt_side0_1->SetTitle("TPC south mid");
        h_clusphisize1frac_pt_side0_1->DrawCopy("colz");

        TText *text1 = new TText(1.10, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_1->GetBinContent(1) / h_clusphisize1frac_mean_denominator_side0_1->GetBinContent(1)));
        TText *text2 = new TText(1.65, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_1->GetBinContent(2) / h_clusphisize1frac_mean_denominator_side0_1->GetBinContent(2)));
        TText *text3 = new TText(2.20, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_1->GetBinContent(3) / h_clusphisize1frac_mean_denominator_side0_1->GetBinContent(3)));
        TText *text4 = new TText(2.75, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_1->GetBinContent(4) / h_clusphisize1frac_mean_denominator_side0_1->GetBinContent(4)));
        text1->SetTextSize(0.06);
        text2->SetTextSize(0.06);
        text3->SetTextSize(0.06);
        text4->SetTextSize(0.06);
        text1->Draw();
        text2->Draw();
        text3->Draw();
        text4->Draw();

        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][4]->cd();
    if (h_clusphisize1frac_pt_side0_2 && h_clusphisize1frac_mean_numerator_side0_2 && h_clusphisize1frac_mean_denominator_side0_2)
    {
        h_clusphisize1frac_pt_side0_2->SetXTitle("p_{T} [GeV]");
        h_clusphisize1frac_pt_side0_2->SetYTitle("Fraction of TPC Cluster Phi Size == 1");
        h_clusphisize1frac_pt_side0_2->SetZTitle("Entries");
        h_clusphisize1frac_pt_side0_2->SetTitle("TPC south out");
        h_clusphisize1frac_pt_side0_2->DrawCopy("colz");

        TText *text1 = new TText(1.10, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_2->GetBinContent(1) / h_clusphisize1frac_mean_denominator_side0_2->GetBinContent(1)));
        TText *text2 = new TText(1.65, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_2->GetBinContent(2) / h_clusphisize1frac_mean_denominator_side0_2->GetBinContent(2)));
        TText *text3 = new TText(2.20, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_2->GetBinContent(3) / h_clusphisize1frac_mean_denominator_side0_2->GetBinContent(3)));
        TText *text4 = new TText(2.75, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side0_2->GetBinContent(4) / h_clusphisize1frac_mean_denominator_side0_2->GetBinContent(4)));
        text1->SetTextSize(0.06);
        text2->SetTextSize(0.06);
        text3->SetTextSize(0.06);
        text4->SetTextSize(0.06);
        text1->Draw();
        text2->Draw();
        text3->Draw();
        text4->Draw();

        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][1]->cd();
    if (h_clusphisize1frac_pt_side1_0 && h_clusphisize1frac_mean_numerator_side1_0 && h_clusphisize1frac_mean_denominator_side1_0)
    {
        h_clusphisize1frac_pt_side1_0->SetXTitle("p_{T} [GeV]");
        h_clusphisize1frac_pt_side1_0->SetYTitle("Fraction of TPC Cluster Phi Size == 1");
        h_clusphisize1frac_pt_side1_0->SetZTitle("Entries");
        h_clusphisize1frac_pt_side1_0->SetTitle("TPC north in");
        h_clusphisize1frac_pt_side1_0->DrawCopy("colz");

        TText *text1 = new TText(1.10, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_0->GetBinContent(1) / h_clusphisize1frac_mean_denominator_side1_0->GetBinContent(1)));
        TText *text2 = new TText(1.65, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_0->GetBinContent(2) / h_clusphisize1frac_mean_denominator_side1_0->GetBinContent(2)));
        TText *text3 = new TText(2.20, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_0->GetBinContent(3) / h_clusphisize1frac_mean_denominator_side1_0->GetBinContent(3)));
        TText *text4 = new TText(2.75, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_0->GetBinContent(4) / h_clusphisize1frac_mean_denominator_side1_0->GetBinContent(4)));
        text1->SetTextSize(0.06);
        text2->SetTextSize(0.06);
        text3->SetTextSize(0.06);
        text4->SetTextSize(0.06);
        text1->Draw();
        text2->Draw();
        text3->Draw();
        text4->Draw();

        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][3]->cd();
    if (h_clusphisize1frac_pt_side1_1 && h_clusphisize1frac_mean_numerator_side1_1 && h_clusphisize1frac_mean_denominator_side1_1)
    {
        h_clusphisize1frac_pt_side1_1->SetXTitle("p_{T} [GeV]");
        h_clusphisize1frac_pt_side1_1->SetYTitle("Fraction of TPC Cluster Phi Size == 1");
        h_clusphisize1frac_pt_side1_1->SetZTitle("Entries");
        h_clusphisize1frac_pt_side1_1->SetTitle("TPC north mid");
        h_clusphisize1frac_pt_side1_1->DrawCopy("colz");

        TText *text1 = new TText(1.10, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_1->GetBinContent(1) / h_clusphisize1frac_mean_denominator_side1_1->GetBinContent(1)));
        TText *text2 = new TText(1.65, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_1->GetBinContent(2) / h_clusphisize1frac_mean_denominator_side1_1->GetBinContent(2)));
        TText *text3 = new TText(2.20, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_1->GetBinContent(3) / h_clusphisize1frac_mean_denominator_side1_1->GetBinContent(3)));
        TText *text4 = new TText(2.75, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_1->GetBinContent(4) / h_clusphisize1frac_mean_denominator_side1_1->GetBinContent(4)));
        text1->SetTextSize(0.06);
        text2->SetTextSize(0.06);
        text3->SetTextSize(0.06);
        text4->SetTextSize(0.06);
        text1->Draw();
        text2->Draw();
        text3->Draw();
        text4->Draw();

        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][5]->cd();
    if (h_clusphisize1frac_pt_side1_2 && h_clusphisize1frac_mean_numerator_side1_2 && h_clusphisize1frac_mean_denominator_side1_2)
    {
        h_clusphisize1frac_pt_side1_2->SetXTitle("p_{T} [GeV]");
        h_clusphisize1frac_pt_side1_2->SetYTitle("Fraction of TPC Cluster Phi Size == 1");
        h_clusphisize1frac_pt_side1_2->SetZTitle("Entries");
        h_clusphisize1frac_pt_side1_2->SetTitle("TPC north out");
        h_clusphisize1frac_pt_side1_2->DrawCopy("colz");

        TText *text1 = new TText(1.10, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_2->GetBinContent(1) / h_clusphisize1frac_mean_denominator_side1_2->GetBinContent(1)));
        TText *text2 = new TText(1.65, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_2->GetBinContent(2) / h_clusphisize1frac_mean_denominator_side1_2->GetBinContent(2)));
        TText *text3 = new TText(2.20, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_2->GetBinContent(3) / h_clusphisize1frac_mean_denominator_side1_2->GetBinContent(3)));
        TText *text4 = new TText(2.75, 0.90, Form("%.3f",h_clusphisize1frac_mean_numerator_side1_2->GetBinContent(4) / h_clusphisize1frac_mean_denominator_side1_2->GetBinContent(4)));
        text1->SetTextSize(0.06);
        text2->SetTextSize(0.06);
        text3->SetTextSize(0.06);
        text4->SetTextSize(0.06);
        text1->Draw();
        text2->Draw();
        text3->Draw();
        text4->Draw();

        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.03);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpcseeds Cluster Info Page2 Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[index_page]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[index_page]->Update();

    std::cout << "DrawClusterInfo2 Ending" << std::endl;
    return 0;
}

int TpcSeedsDraw::DrawDCAInfo1()
{
    std::cout << "Tpc Seeds DrawDCAInfo1() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH2F *h_dcaxyorigin_phi_north_pos = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcaxyorigin_phi_north_pos")));
    TH2F *h_dcaxyorigin_phi_north_neg = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcaxyorigin_phi_north_neg")));
    TH2F *h_dcaxyorigin_phi_south_pos = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcaxyorigin_phi_south_pos")));
    TH2F *h_dcaxyorigin_phi_south_neg = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcaxyorigin_phi_south_neg")));
    TH2F *h_dcazorigin_phi_pos = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcazorigin_phi_pos")));
    TH2F *h_dcazorigin_phi_neg = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcazorigin_phi_neg")));

    const int index_page = 3;

    if (!gROOT->FindObject("dca_info1"))
    {
        MakeCanvas("dca_info1", index_page);
    }
    TC[index_page]->Clear("D");

    Pad[index_page][0]->cd();
    if (h_dcaxyorigin_phi_north_pos)
    {
        h_dcaxyorigin_phi_north_pos->SetXTitle("#phi [rad]");
        h_dcaxyorigin_phi_north_pos->SetYTitle("Positive north track DCA_{xy} wrt origin [cm]");
        h_dcaxyorigin_phi_north_pos->SetZTitle("Entries");
        h_dcaxyorigin_phi_north_pos->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][1]->cd();
    if (h_dcaxyorigin_phi_north_neg)
    {
        h_dcaxyorigin_phi_north_neg->SetXTitle("#phi [rad]");
        h_dcaxyorigin_phi_north_neg->SetYTitle("Negative north track DCA_{xy} wrt origin [cm]");
        h_dcaxyorigin_phi_north_neg->SetZTitle("Entries");
        h_dcaxyorigin_phi_north_neg->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][2]->cd();
    if (h_dcaxyorigin_phi_south_pos)
    {
        h_dcaxyorigin_phi_south_pos->SetXTitle("#phi [rad]");
        h_dcaxyorigin_phi_south_pos->SetYTitle("Positive south track DCA_{xy} wrt origin [cm]");
        h_dcaxyorigin_phi_south_pos->SetZTitle("Entries");
        h_dcaxyorigin_phi_south_pos->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][3]->cd();
    if (h_dcaxyorigin_phi_south_neg)
    {
        h_dcaxyorigin_phi_south_neg->SetXTitle("#phi [rad]");
        h_dcaxyorigin_phi_south_neg->SetYTitle("Negative south track DCA_{xy} wrt origin [cm]");
        h_dcaxyorigin_phi_south_neg->SetZTitle("Entries");
        h_dcaxyorigin_phi_south_neg->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][4]->cd();
    if (h_dcazorigin_phi_pos)
    {
        h_dcazorigin_phi_pos->SetXTitle("#phi [rad]");
        h_dcazorigin_phi_pos->SetYTitle("Positive track DCA_{z} wrt origin [cm]");
        h_dcazorigin_phi_pos->SetZTitle("Entries");
        h_dcazorigin_phi_pos->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][5]->cd();
    if (h_dcazorigin_phi_neg)
    {
        h_dcazorigin_phi_neg->SetXTitle("#phi [rad]");
        h_dcazorigin_phi_neg->SetYTitle("Negative track DCA_{z} wrt origin [cm]");
        h_dcazorigin_phi_neg->SetZTitle("Entries");
        h_dcazorigin_phi_neg->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.03);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpcseeds DCA Info Page1 Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[index_page]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[index_page]->Update();

    std::cout << "DrawDCAInfo1 Ending" << std::endl;
    return 0;
}

int TpcSeedsDraw::DrawDCAInfo2()
{
    std::cout << "Tpc Seeds DrawDCAInfo2() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH2F *h_dcaxyvtx_phi_pos = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcaxyvtx_phi_pos")));
    TH2F *h_dcaxyvtx_phi_neg = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcaxyvtx_phi_neg")));
    TH2F *h_dcazvtx_phi_pos = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcazvtx_phi_pos")));
    TH2F *h_dcazvtx_phi_neg = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcazvtx_phi_neg")));

    const int index_page = 4;

    if (!gROOT->FindObject("dca_info2"))
    {
        MakeCanvas("dca_info2", index_page);
    }
    TC[index_page]->Clear("D");

    Pad[index_page][0]->cd();
    if (h_dcaxyvtx_phi_pos)
    {
        h_dcaxyvtx_phi_pos->SetXTitle("#phi [rad]");
        h_dcaxyvtx_phi_pos->SetYTitle("Positive track DCA_{xy} wrt vertex [cm]");
        h_dcaxyvtx_phi_pos->SetZTitle("Entries");
        h_dcaxyvtx_phi_pos->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][1]->cd();
    if (h_dcaxyvtx_phi_neg)
    {
        h_dcaxyvtx_phi_neg->SetXTitle("#phi [rad]");
        h_dcaxyvtx_phi_neg->SetYTitle("Negative track DCA_{xy} wrt vertex [cm]");
        h_dcaxyvtx_phi_neg->SetZTitle("Entries");
        h_dcaxyvtx_phi_neg->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][2]->cd();
    if (h_dcazvtx_phi_pos)
    {
        h_dcazvtx_phi_pos->SetXTitle("#phi [rad]");
        h_dcazvtx_phi_pos->SetYTitle("Positive track DCA_{z} wrt vertex [cm]");
        h_dcazvtx_phi_pos->SetZTitle("Entries");
        h_dcazvtx_phi_pos->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][3]->cd();
    if (h_dcazvtx_phi_neg)
    {
        h_dcazvtx_phi_neg->SetXTitle("#phi [rad]");
        h_dcazvtx_phi_neg->SetYTitle("Negative track DCA_{z} wrt vertex [cm]");
        h_dcazvtx_phi_neg->SetZTitle("Entries");
        h_dcazvtx_phi_neg->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.03);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpcseeds DCA Info Page2 Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[index_page]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[index_page]->Update();

    std::cout << "DrawDCAInfo2 Ending" << std::endl;
    return 0;
}

int TpcSeedsDraw::DrawVertexInfo()
{
    std::cout << "Tpc Seeds DrawVertexInfo() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH1F *h_nvertex = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("nrecovertices")));
    TH2F *h_vx_vy = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("vx_vy")));
    TH1F *h_vz = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("vz")));
    TH1F *h_vt = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("vt")));
    //TH1F *h_vcrossing = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("vertexcrossing")));
    TH1F *h_vchi2dof = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("vertexchi2dof")));
    TH1F *h_ntrackpervertex = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntrackspervertex")));

    const int index_page = 5;

    if (!gROOT->FindObject("vertex_info"))
    {
        MakeCanvas("vertex_info", index_page);
    }
    TC[index_page]->Clear("D");

    Pad[index_page][0]->cd();
    if (h_nvertex)
    {
        h_nvertex->SetXTitle("Number of vertices");
        h_nvertex->SetYTitle("Entries");
        h_nvertex->DrawCopy();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][1]->cd();
    if (h_vx_vy)
    {
        h_vx_vy->SetXTitle("Vertex x [cm]");
        h_vx_vy->SetYTitle("Vertex y [cm]");
        h_vx_vy->SetZTitle("Entries");
        h_vx_vy->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][2]->cd();
    if (h_vz)
    {
        h_vz->SetXTitle("Vertex z [cm]");
        h_vz->SetYTitle("Entries");
        h_vz->DrawCopy();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][3]->cd();
    if (h_vt)
    {
        h_vt->SetXTitle("Vertex t [ns]");
        h_vt->SetYTitle("Entries");
        h_vt->DrawCopy("colz");
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

/*
    Pad[index_page][4]->cd();
    if (h_vcrossing)
    {
        h_vcrossing->SetXTitle("Vertex crossing ");
        h_vcrossing->SetYTitle("Entries");
        h_vcrossing->DrawCopy();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }
*/

    Pad[index_page][4]->cd();
    if (h_vchi2dof)
    {
        h_vchi2dof->SetXTitle("Vertex #chi2/ndof");
        h_vchi2dof->SetYTitle("Entries");
        h_vchi2dof->DrawCopy();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][5]->cd();
    if (h_ntrackpervertex)
    {
        h_ntrackpervertex->SetXTitle("Number of tracks per vertex");
        h_ntrackpervertex->SetYTitle("Entries");
        h_ntrackpervertex->DrawCopy();
        gPad->SetRightMargin(0.15);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.03);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpcseeds Vertex Info Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[index_page]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[index_page]->Update();

    std::cout << "DrawVertexInfo Ending" << std::endl;
    return 0;
}

int TpcSeedsDraw::DrawdEdxInfo1()
{
    std::cout << "Tpc Seeds DrawdEdxInfo1() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH2F *h_dedx_pq[10];
    for (int i = 0; i < 10; i++)
    {
      h_dedx_pq[i] = dynamic_cast<TH2F *>(cl->getHisto(std::string(histprefix) + Form("dedx_pq_%d", i)));
    }

    const int index_page = 6;

    if (!gROOT->FindObject("dEdx_info1"))
    {
        MakeCanvas("dEdx_info1", index_page);
    }
    TC[index_page]->Clear("D");

    for (int i = 0; i < 10; i++)
    {
      Pad[index_page][i]->cd();
      if (h_dedx_pq[i])
      {
        h_dedx_pq[i]->SetXTitle("p#timesq");
        h_dedx_pq[i]->SetYTitle("Mean cluster Adc corrected by path length [arb]");
        h_dedx_pq[i]->SetZTitle("Entries");
        h_dedx_pq[i]->DrawCopy("colz");
        gPad->SetRightMargin(0.17);

        int gzmin = -100 + i * 20;
        int gzmax = -100 + ( i + 1 ) * 20;
        TPaveText *pt = new TPaveText(0.2, 0.7, 0.45, 0.9, "brNDC");
        pt->AddText(Form("Cluster gz in [%d,%d] cm", gzmin, gzmax));
        pt->SetTextAlign(12);
        pt->SetTextFont(42);
        pt->SetTextSize(0.05);
        pt->SetFillStyle(0);
        pt->SetBorderSize(0);
        pt->Draw("same");
      }
      else
      {
          // histogram is missing
          return -1;
      }
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.03);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpcseeds dEdx vs p*q in Z bin Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[index_page]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[index_page]->Update();

    std::cout << "DrawdEdxInfo1 Ending" << std::endl;
    return 0;
}

int TpcSeedsDraw::DrawdEdxInfo2()
{
    std::cout << "Tpc Seeds DrawdEdxInfo2() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH2F *h_dedx_pq[10];
    for (int i = 0; i < 10; i++)
    {
      h_dedx_pq[i] = dynamic_cast<TH2F *>(cl->getHisto(std::string(histprefix) + Form("dedx_pq_%d", i)));
    }

    TH2F *h_dedx_pca = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dedx_pcaz")));

    const int index_page = 7;

    if (!gROOT->FindObject("dEdx_info2"))
    {
        MakeCanvas("dEdx_info2", index_page);
    }
    TC[index_page]->Clear("D");

    Pad[index_page][0]->cd();
    if (h_dedx_pq[0] && h_dedx_pq[4])
    {
      int xBinMin = h_dedx_pq[0]->GetXaxis()->FindBin(-3.);
      int xBinMax = h_dedx_pq[0]->GetXaxis()->FindBin(0.2);

      TH1D *h_dedx_z0 = h_dedx_pq[0]->ProjectionY("h_dedx_z0", xBinMin, xBinMax);
      TH1D *h_dedx_z4 = h_dedx_pq[4]->ProjectionY("h_dedx_z4", xBinMin, xBinMax);

      h_dedx_z0->GetXaxis()->SetRangeUser(0, 3000);
      h_dedx_z4->GetXaxis()->SetRangeUser(0, 3000);

      h_dedx_z0->Scale(h_dedx_z4->Integral() / h_dedx_z0->Integral());

      float ymax = h_dedx_z0->GetMaximum() > h_dedx_z4->GetMaximum() ? h_dedx_z0->GetMaximum() : h_dedx_z4->GetMaximum();

      h_dedx_z0->SetMaximum(1.1*ymax);
      h_dedx_z0->SetLineColor(kRed);
      h_dedx_z0->Draw("hist");
      h_dedx_z0->GetXaxis()->SetTitle("Mean cluster Adc corrected by path length");
      h_dedx_z0->GetYaxis()->SetTitle("Events");
      h_dedx_z4->SetLineColor(kBlue);
      h_dedx_z4->Draw("hist,same");

      TF1 *landauFit_z0 = new TF1("landauFit_z0", "[2] * TMath::Landau(x,[0],[1])", 0, 3000);
      TF1 *landauFit_z4 = new TF1("landauFit_z4", "[2] * TMath::Landau(x,[0],[1])", 0, 3000);
      landauFit_z0->SetParameters(0.8*h_dedx_z0->GetMean(), 0.5*h_dedx_z0->GetRMS(), h_dedx_z0->Integral());
      landauFit_z4->SetParameters(0.8*h_dedx_z4->GetMean(), 0.5*h_dedx_z4->GetRMS(), h_dedx_z4->Integral());
      landauFit_z0->SetParLimits(0, 0.05*h_dedx_z0->GetMean(), 2*h_dedx_z0->GetMean());
      landauFit_z4->SetParLimits(0, 0.05*h_dedx_z4->GetMean(), 2*h_dedx_z4->GetMean());
      landauFit_z0->SetParLimits(1, 0.1*h_dedx_z0->GetRMS(), 5*h_dedx_z0->GetRMS());
      landauFit_z4->SetParLimits(1, 0.1*h_dedx_z4->GetRMS(), 5*h_dedx_z4->GetRMS());
      landauFit_z0->SetParLimits(2, 0, 2*h_dedx_z0->Integral());
      landauFit_z4->SetParLimits(2, 0, 2*h_dedx_z4->Integral());
      h_dedx_z0->Fit("landauFit_z0");
      h_dedx_z4->Fit("landauFit_z4");

      landauFit_z0->SetLineColor(kRed+1);
      landauFit_z0->SetLineWidth(2);
      landauFit_z4->SetLineColor(kBlue+1);
      landauFit_z4->SetLineWidth(2);

      landauFit_z0->Draw("same");
      landauFit_z4->Draw("same");

      float xpos_z0 = landauFit_z0->GetParameter(0);
      float xpos_z4 = landauFit_z4->GetParameter(0);
      float z0_to_z4_ratio = xpos_z0 / xpos_z4;
      if (isnan(z0_to_z4_ratio))
      {
        xpos_z0 = -1;
        xpos_z4 = -1;
      }

      TPaveText *pt = new TPaveText(0.48, 0.6, 0.9, 0.9, "brNDC");
      pt->AddText("Negative tracks p > 0.2 GeV");
      pt->AddText(Form("Laudau Fit"));
      pt->AddText(Form("Mean #color[2]{HighZ} / #color[4]{LowZ} = %.2f",z0_to_z4_ratio));
      pt->SetTextAlign(12);
      pt->SetTextFont(42);
      pt->SetTextSize(0.05);
      pt->SetFillStyle(0);
      pt->SetBorderSize(0);
      pt->Draw();

      TLegend *legend = new TLegend(0.48, 0.45, 0.9, 0.6);
      legend->AddEntry(h_dedx_z0, "Cluster Z in [-100,-80] cm", "l");
      legend->AddEntry(h_dedx_z4, "Cluster Z in [-20,0] cm", "l");
      legend->SetTextSize(0.04);
      legend->Draw();

      TLine *line_z0 = new TLine(xpos_z0, 0, xpos_z0, 1.1*ymax);
      line_z0->SetLineStyle(2);
      line_z0->SetLineWidth(2);
      line_z0->SetLineColor(kRed);
      line_z0->Draw();

      TLine *line_z4 = new TLine(xpos_z4, 0, xpos_z4, 1.1*ymax);
      line_z4->SetLineStyle(2);
      line_z4->SetLineWidth(2);
      line_z4->SetLineColor(kBlue);
      line_z4->Draw();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][1]->cd();
    if (h_dedx_pq[9] && h_dedx_pq[5])
    {
      int xBinMin = h_dedx_pq[9]->GetXaxis()->FindBin(-3.);
      int xBinMax = h_dedx_pq[9]->GetXaxis()->FindBin(0.2);

      TH1D *h_dedx_z9 = h_dedx_pq[9]->ProjectionY("h_dedx_z9", xBinMin, xBinMax);
      TH1D *h_dedx_z5 = h_dedx_pq[5]->ProjectionY("h_dedx_z5", xBinMin, xBinMax);

      h_dedx_z9->GetXaxis()->SetRangeUser(0, 3000);
      h_dedx_z5->GetXaxis()->SetRangeUser(0, 3000);

      h_dedx_z9->Scale(h_dedx_z5->Integral() / h_dedx_z9->Integral());

      float ymax = h_dedx_z9->GetMaximum() > h_dedx_z5->GetMaximum() ? h_dedx_z9->GetMaximum() : h_dedx_z5->GetMaximum();

      h_dedx_z9->SetMaximum(1.1*ymax);
      h_dedx_z9->SetLineColor(kRed);
      h_dedx_z9->Draw("hist");
      h_dedx_z9->GetXaxis()->SetTitle("Mean cluster Adc corrected by path length");
      h_dedx_z9->GetYaxis()->SetTitle("Events");
      h_dedx_z5->SetLineColor(kBlue);
      h_dedx_z5->Draw("hist,same");

      TF1 *landauFit_z9 = new TF1("landauFit_z9", "[2] * TMath::Landau(x,[0],[1])", 0, 3000);
      TF1 *landauFit_z5 = new TF1("landauFit_z5", "[2] * TMath::Landau(x,[0],[1])", 0, 3000);
      landauFit_z9->SetParameters(0.8*h_dedx_z9->GetMean(), 0.5*h_dedx_z9->GetRMS(), h_dedx_z9->Integral());
      landauFit_z5->SetParameters(0.8*h_dedx_z5->GetMean(), 0.5*h_dedx_z5->GetRMS(), h_dedx_z5->Integral());
      landauFit_z9->SetParLimits(0, 0.05*h_dedx_z9->GetMean(), 2*h_dedx_z9->GetMean());
      landauFit_z5->SetParLimits(0, 0.05*h_dedx_z5->GetMean(), 2*h_dedx_z5->GetMean());
      landauFit_z9->SetParLimits(1, 0.1*h_dedx_z9->GetRMS(), 5*h_dedx_z9->GetRMS());
      landauFit_z5->SetParLimits(1, 0.1*h_dedx_z5->GetRMS(), 5*h_dedx_z5->GetRMS());
      landauFit_z9->SetParLimits(2, 0, 2*h_dedx_z9->Integral());
      landauFit_z5->SetParLimits(2, 0, 2*h_dedx_z5->Integral());
      h_dedx_z9->Fit("landauFit_z9");
      h_dedx_z5->Fit("landauFit_z5");

      landauFit_z9->SetLineColor(kRed+1);
      landauFit_z9->SetLineWidth(2);
      landauFit_z5->SetLineColor(kBlue+1);
      landauFit_z5->SetLineWidth(2);

      landauFit_z9->Draw("same");
      landauFit_z5->Draw("same");

      float xpos_z9 = landauFit_z9->GetParameter(0);
      float xpos_z5 = landauFit_z5->GetParameter(0);
      float z9_to_z5_ratio = xpos_z9 / xpos_z5;
      if (isnan(z9_to_z5_ratio))
      {
        xpos_z9 = -1;
        xpos_z5 = -1;
      }

      TPaveText *pt = new TPaveText(0.48, 0.6, 0.9, 0.9, "brNDC");
      pt->AddText("Negative tracks p > 0.2 GeV");
      pt->AddText(Form("Laudau Fit"));
      pt->AddText(Form("Mean #color[2]{HighZ} / #color[4]{LowZ} = %.2f",z9_to_z5_ratio));
      pt->SetTextAlign(12);
      pt->SetTextFont(42);
      pt->SetTextSize(0.05);
      pt->SetFillStyle(0);
      pt->SetBorderSize(0);
      pt->Draw();

      TLegend *legend = new TLegend(0.48, 0.45, 0.9, 0.6);
      legend->AddEntry(h_dedx_z9, "Cluster Z in [80,100] cm", "l");
      legend->AddEntry(h_dedx_z5, "Cluster Z in [0,20] cm", "l");
      legend->SetTextSize(0.04);
      legend->Draw();

      TLine *line_z9 = new TLine(xpos_z9, 0, xpos_z9, 1.1*ymax);
      line_z9->SetLineStyle(2);
      line_z9->SetLineWidth(2);
      line_z9->SetLineColor(kRed);
      line_z9->Draw();

      TLine *line_z5 = new TLine(xpos_z5, 0, xpos_z5, 1.1*ymax);
      line_z5->SetLineStyle(2);
      line_z5->SetLineWidth(2);
      line_z5->SetLineColor(kBlue);
      line_z5->Draw();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[index_page][2]->cd();
    if (h_dedx_pca)
    {
      h_dedx_pca->SetXTitle("pcaz [cm]");
      h_dedx_pca->SetYTitle("dE/dx [arb]");
      h_dedx_pca->SetZTitle("Entries");
      h_dedx_pca->DrawCopy("colz");
      gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.03);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpcseeds dEdx vs Z Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[index_page]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[index_page]->Update();

    std::cout << "DrawdEdxInfo2 Ending" << std::endl;
    return 0;
}

int TpcSeedsDraw::MakeHtml(const std::string &what)
{
    int iret = Draw(what);
    if (iret) // on error no html output please
    {
        return iret;
    }

    QADrawClient *cl = QADrawClient::instance();
    std::string pngfile;
    
    // Register the 1st canvas png file to the menu and produces the png file.
    if (what == "ALL" || what == "TRACKLET")
    {
        pngfile = cl->htmlRegisterPage(*this, "track_info", "1", "png");
        cl->CanvasToPng(TC[0], pngfile);
    }
    
    if (what == "ALL" || what == "CLUSTER1")
    {
        pngfile = cl->htmlRegisterPage(*this, "cluster_info1", "2", "png");
        cl->CanvasToPng(TC[1], pngfile);
    }
 
    if (what == "ALL" || what == "CLUSTER2")
    {
        pngfile = cl->htmlRegisterPage(*this, "cluster_info2", "3", "png");
        cl->CanvasToPng(TC[2], pngfile);
    }

    if (what == "ALL" || what == "DCA1")
    {
        pngfile = cl->htmlRegisterPage(*this, "dca_info1", "4", "png");
        cl->CanvasToPng(TC[3], pngfile);
    }
 
    if (what == "ALL" || what == "DCA2")
    {
        pngfile = cl->htmlRegisterPage(*this, "dca_info2", "5", "png");
        cl->CanvasToPng(TC[4], pngfile);
    }

    if (what == "ALL" || what == "VERTEX")
    {
        pngfile = cl->htmlRegisterPage(*this, "vertex_info", "6", "png");
        cl->CanvasToPng(TC[5], pngfile);
    }

    if (what == "ALL" || what == "DEDX1")
    {
        pngfile = cl->htmlRegisterPage(*this, "dEdx_info1", "7", "png");
        cl->CanvasToPng(TC[6], pngfile);
    }

    if (what == "ALL" || what == "DEDX2")
    {
        pngfile = cl->htmlRegisterPage(*this, "dEdx_info2", "8", "png");
        cl->CanvasToPng(TC[7], pngfile);
    }

    return 0;
}

int TpcSeedsDraw::DBVarInit()
{
    /* db = new QADrawDB(this); */
    /* db->DBInit(); */
    return 0;
}
