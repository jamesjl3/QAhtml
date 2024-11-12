#include "SiliconSeedsDraw.h"

#include <sPhenixStyle.C>

#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>

#include <TCanvas.h>
#include <TColor.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TPad.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

SiliconSeedsDraw::SiliconSeedsDraw(const std::string &name)
    : QADraw(name)
{
    memset(TC, 0, sizeof(TC));
    memset(transparent, 0, sizeof(transparent));
    memset(Pad, 0, sizeof(Pad));
    DBVarInit();
    histprefix = "h_SiliconSeedsQA_";
    return;
}

SiliconSeedsDraw::~SiliconSeedsDraw()
{
    /* delete db; */
    return;
}

int SiliconSeedsDraw::Draw(const std::string &what)
{
    gStyle->SetPaintTextFormat(".3g");
    int iret = 0;
    int idraw = 0;
    if (what == "ALL" || what == "TRACKLET")
    {
        iret += DrawTrackBasicInfo();
        idraw++;
    }
    if (what == "ALL" || what == "TRACKDCA")
    {
        iret += DrawTrackDCAInfo();
        idraw++;
    }
    if (what == "ALL" || what == "TRACKQUALITY")
    {
        iret += DrawTrackQualityInfo();
        idraw++;
    }
    if (what == "ALL" || what == "TRACKCHARGE")
    {
        iret += DrawTrackChargeInfo();
        idraw++;
    }
    if (what == "ALL" || what == "VERTEX")
    {
        iret += DrawVertexInfo();
        idraw++;
    }
    if (what == "ALL" || what == "VERTEXQUALITY")
    {
        iret += DrawVertexQualityInfo();
        idraw++;
    }
    if (!idraw)
    {
        std::cout << " Unimplemented Drawing option: " << what << std::endl;
        iret = -1;
    }
    return iret;
}

int SiliconSeedsDraw::MakeCanvas(const std::string &name, int num)
{
    QADrawClient *cl = QADrawClient::instance();
    int xsize = cl->GetDisplaySizeX();
    int ysize = cl->GetDisplaySizeY();
    // xpos (-1) negative: do not draw menu bar
    TC[num] = new TCanvas(name.c_str(), (boost::format("Silicon Seeds Plots %d") % num).str().c_str(), -1, 0, (int)(xsize / 1.2), (int)(ysize / 1.2));
    gSystem->ProcessEvents();

    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "Thomas and Tanner", 0.05, 0.52, 0.48, 0.97, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "both", 0.53, 0.52, 0.96, 0.97, 0);
    Pad[num][2] = new TPad((boost::format("mypad%d2") % num).str().c_str(), "like", 0.05, 0.02, 0.48, 0.47, 0);
    Pad[num][3] = new TPad((boost::format("mypad%d3") % num).str().c_str(), "Chilis", 0.53, 0.02, 0.96, 0.47, 0);

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

int SiliconSeedsDraw::DrawTrackBasicInfo()
{
    std::cout << "Silicon Seeds DrawTrackBasicInfo() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH1F *h_ntrack1d = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("nrecotracks1d")));
    TH2F *h_ntrack = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("nrecotracks")));
    TH2F *h_nmaps_nintt = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("nmaps_nintt")));
    TProfile2D *h_avgnclus_eta_phi = dynamic_cast<TProfile2D *>(cl->getHisto(histprefix + std::string("avgnclus_eta_phi")));

    if (!gROOT->FindObject("trackbasic_info"))
    {
        MakeCanvas("trackbasic_info", 0);
    }
    TC[0]->Clear("D");

    Pad[0][0]->cd();
    if (h_ntrack1d)
    {
        h_ntrack1d->SetTitle("Number of silicon-only tracks");
        h_ntrack1d->SetXTitle("Number of silicon-only tracks");
        h_ntrack1d->SetYTitle("Entries");
        h_ntrack1d->DrawCopy();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[0][1]->cd();
    if (h_ntrack)
    {
        h_ntrack->SetXTitle("#eta");
        h_ntrack->SetYTitle("#phi [rad]");
        h_ntrack->SetZTitle("Number of tracks");
        h_ntrack->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[0][2]->cd();
    if (h_nmaps_nintt)
    {
        h_nmaps_nintt->SetXTitle("Number of MVTX clusters");
        h_nmaps_nintt->SetYTitle("Number of INTT clusters");
        h_nmaps_nintt->SetZTitle("Fraction");
        h_nmaps_nintt->SetMarkerSize(2.0);
        h_nmaps_nintt->Scale(1. / h_nmaps_nintt->Integral());
        h_nmaps_nintt->DrawCopy("colztext45");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[0][3]->cd();
    if (h_avgnclus_eta_phi)
    {
        h_avgnclus_eta_phi->SetXTitle("#eta");
        h_avgnclus_eta_phi->SetYTitle("#phi [rad]");
        h_avgnclus_eta_phi->SetZTitle("Average number of clusters per track");
        h_avgnclus_eta_phi->DrawCopy("colz");
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
    runnostream1 << Name() << "_siliconseeds Track Info Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[0]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[0]->Update();

    std::cout << "DrawTrackBasicInfo Ending" << std::endl;
    return 0;
}

int SiliconSeedsDraw::DrawTrackDCAInfo()
{
    std::cout << "Silicon Seeds DrawTrackDCAInfo() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH2F *h_dcaxyorigin_phi = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcaxyorigin_phi")));
    TH2F *h_dcazorigin_phi = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcazorigin_phi")));
    TH2F *h_dcaxyvtx_phi = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcaxyvtx_phi")));
    TH2F *h_dcazvtx_phi = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("dcazvtx_phi")));

    if (!gROOT->FindObject("trackdca_info"))
    {
        MakeCanvas("trackdca_info", 1);
    }
    TC[1]->Clear("D");

    Pad[1][0]->cd();
    if (h_dcaxyorigin_phi)
    {
        h_dcaxyorigin_phi->SetXTitle("#phi [rad]");
        h_dcaxyorigin_phi->SetYTitle("DCA_{xy} wrt origin [cm]");
        h_dcaxyorigin_phi->SetZTitle("Entries");
        h_dcaxyorigin_phi->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[1][1]->cd();
    if (h_dcaxyvtx_phi)
    {
        h_dcaxyvtx_phi->SetXTitle("#phi [rad]");
        h_dcaxyvtx_phi->SetYTitle("DCA_{xy} wrt vertex [cm]");
        h_dcaxyvtx_phi->SetZTitle("Entries");
        h_dcaxyvtx_phi->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[1][2]->cd();
    if (h_dcazorigin_phi)
    {
        h_dcazorigin_phi->SetXTitle("#phi [rad]");
        h_dcazorigin_phi->SetYTitle("DCA_{z} wrt origin [cm]");
        h_dcazorigin_phi->SetZTitle("Entries");
        h_dcazorigin_phi->DrawCopy("colz");
        gPad->SetRightMargin(0.17);
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[1][3]->cd();
    if (h_dcazvtx_phi)
    {
        h_dcazvtx_phi->SetXTitle("#phi [rad]");
        h_dcazvtx_phi->SetYTitle("DCA_{z} wrt vertex [cm]");
        h_dcazvtx_phi->SetZTitle("Entries");
        h_dcazvtx_phi->DrawCopy("colz");
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
    runnostream1 << Name() << "_siliconseeds Track Info Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[1]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[1]->Update();

    std::cout << "DrawTrackDCAInfo Ending" << std::endl;
    return 0;
}

int SiliconSeedsDraw::DrawTrackQualityInfo()
{
    std::cout << "Silicon Seeds DrawTrackQualityInfo() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH1F *h_trackcrossing = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("trackcrossing")));
    TH1F *h_trackchi2ndf = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("trackchi2ndf")));
    TH1F *h_ntrack_isfromvtx = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntrack_isfromvtx")));

    if (!gROOT->FindObject("trackquality_info"))
    {
        MakeCanvas("trackquality_info", 2);
    }
    TC[2]->Clear("D");

    Pad[2][0]->cd();
    if (h_trackcrossing)
    {
        h_trackcrossing->SetXTitle("Track crossing");
        h_trackcrossing->SetYTitle("Entries");
        h_trackcrossing->DrawCopy();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[2][1]->cd();
    if (h_trackchi2ndf)
    {
        gPad->SetLogy();
        h_trackchi2ndf->SetXTitle("Track #chi2/ndof");
        h_trackchi2ndf->SetYTitle("Entries");
        h_trackchi2ndf->DrawCopy();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[2][2]->cd();
    if (h_ntrack_isfromvtx)
    {
        h_ntrack_isfromvtx->SetTitle("Is track from a vertex");
        h_ntrack_isfromvtx->SetXTitle("Is track associated to a vertex");
        h_ntrack_isfromvtx->SetYTitle("Fraction");
        h_ntrack_isfromvtx->Scale(1. / h_ntrack_isfromvtx->Integral());
        h_ntrack_isfromvtx->GetYaxis()->SetRangeUser(0, 1.0);
        h_ntrack_isfromvtx->SetMarkerSize(2.5);
        h_ntrack_isfromvtx->GetXaxis()->SetNdivisions(3);
        h_ntrack_isfromvtx->DrawCopy("histtext0");
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
    runnostream1 << Name() << "_siliconseeds Track Info Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[2]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[2]->Update();

    std::cout << "DrawTrackQualityInfo Ending" << std::endl;
    return 0;
}

int SiliconSeedsDraw::DrawTrackChargeInfo()
{
    std::cout << "Silicon Seeds DrawTrackChargeInfo() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH1F *h_trackpt_inclusive = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("trackpt")));
    TH1F *h_trackpt_pos = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("trackpt_pos")));
    TH1F *h_trackpt_neg = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("trackpt_neg")));
    TH1F *h_ntrack_IsPosCharge = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntrack_IsPosCharge")));

    if (!gROOT->FindObject("trackcharge_info"))
    {
        MakeCanvas("trackcharge_info", 3);
    }

    TC[3]->Clear("D");
    Pad[3][0]->cd();
    if (h_trackpt_inclusive && h_trackpt_pos && h_trackpt_neg)
    {
        gPad->SetLogy();
        h_trackpt_inclusive->SetXTitle("Track p_{T} [GeV]");
        h_trackpt_inclusive->SetYTitle("Entries");
        h_trackpt_inclusive->SetLineColor(kBlack);
        h_trackpt_inclusive->DrawCopy();
        h_trackpt_pos->SetMarkerColor(kRed);
        h_trackpt_pos->SetLineColor(kRed);
        h_trackpt_pos->DrawCopy("same");
        h_trackpt_neg->SetMarkerColor(kBlue);
        h_trackpt_neg->SetLineColor(kBlue);
        h_trackpt_neg->DrawCopy("same");
        auto legend = new TLegend(0.45, 0.7, 0.8, 0.9);
        legend->AddEntry(h_trackpt_inclusive, "Inclusive", "pl");
        legend->AddEntry(h_trackpt_pos, "Positive charged", "pl");
        legend->AddEntry(h_trackpt_neg, "Negative charged", "pl");
        legend->Draw();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[3][1]->cd();
    if (h_ntrack_IsPosCharge)
    {
        h_ntrack_IsPosCharge->SetXTitle("Number of tracks with positive charge");
        h_ntrack_IsPosCharge->SetYTitle("Fraction");
        h_ntrack_IsPosCharge->Scale(1. / h_ntrack_IsPosCharge->Integral());
        h_ntrack_IsPosCharge->GetYaxis()->SetRangeUser(0, 1.0);
        h_ntrack_IsPosCharge->SetMarkerSize(2.5);
        h_ntrack_IsPosCharge->GetXaxis()->SetNdivisions(3);
        h_ntrack_IsPosCharge->DrawCopy("histtext0");
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
    runnostream1 << Name() << "_siliconseeds Track Info Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[3]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[3]->Update();

    std::cout << "DrawTrackQualityInfo Ending" << std::endl;
    return 0;
}

int SiliconSeedsDraw::DrawVertexInfo()
{
    std::cout << "Silicon Seeds DrawVertexInfo() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH1F *h_nvertex = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("nrecovertices")));
    TH2F *h_vx_vy = dynamic_cast<TH2F *>(cl->getHisto(histprefix + std::string("vx_vy")));
    TH1F *h_vz = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("vz")));
    TH1F *h_vt = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("vt")));

    if (!gROOT->FindObject("vertex_info"))
    {
        MakeCanvas("vertex_info", 4);
    }
    TC[4]->Clear("D");

    Pad[4][0]->cd();
    if (h_nvertex)
    {
        gPad->SetLogy();
        h_nvertex->SetXTitle("Number of vertices");
        h_nvertex->SetYTitle("Entries");
        h_nvertex->DrawCopy();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[4][1]->cd();
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

    Pad[4][2]->cd();
    if (h_vz)
    {
        h_vz->SetXTitle("Vertex z [cm]");
        h_vz->SetYTitle("Entries");
        h_vz->DrawCopy();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[4][3]->cd();
    if (h_vt)
    {
        h_vt->SetXTitle("Vertex t [ns]");
        h_vt->SetYTitle("Entries");
        h_vt->DrawCopy("colz");
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
    runnostream1 << Name() << "_siliconseeds Vertex Info Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[4]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[4]->Update();

    std::cout << "DrawVertexInfo Ending" << std::endl;
    return 0;
}

int SiliconSeedsDraw::DrawVertexQualityInfo()
{
    std::cout << "Silicon Seeds DrawVertexQualityInfo() Beginning" << std::endl;
    QADrawClient *cl = QADrawClient::instance();

    TH1F *h_vcrossing = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("vertexcrossing")));
    TH1F *h_vchi2dof = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("vertexchi2dof")));
    TH1F *h_ntrackpervertex = dynamic_cast<TH1F *>(cl->getHisto(histprefix + std::string("ntrackspervertex")));

    if (!gROOT->FindObject("vertexquality_info"))
    {
        MakeCanvas("vertexquality_info", 5);
    }
    TC[5]->Clear("D");

    Pad[5][0]->cd();
    if (h_vcrossing)
    {
        h_vcrossing->SetXTitle("Vertex crossing ");
        h_vcrossing->SetYTitle("Entries");
        h_vcrossing->DrawCopy();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[5][1]->cd();
    if (h_vchi2dof)
    {
        h_vchi2dof->SetXTitle("Vertex #chi2/ndof");
        h_vchi2dof->SetYTitle("Entries");
        h_vchi2dof->DrawCopy();
    }
    else
    {
        // histogram is missing
        return -1;
    }

    Pad[5][2]->cd();
    if (h_ntrackpervertex)
    {
        gPad->SetLogy();
        h_ntrackpervertex->SetXTitle("Number of tracks per vertex");
        h_ntrackpervertex->SetYTitle("Entries");
        h_ntrackpervertex->DrawCopy();
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
    runnostream1 << Name() << "_siliconseeds Vertex Info Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[5]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[5]->Update();

    std::cout << "DrawVertexQualityInfo Ending" << std::endl;
    return 0;
}

int SiliconSeedsDraw::MakeHtml(const std::string &what)
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
        pngfile = cl->htmlRegisterPage(*this, "trackbasic_info", "1", "png");
        cl->CanvasToPng(TC[0], pngfile);
    }

    if (what == "ALL" || what == "TRACKDCA")
    {
        pngfile = cl->htmlRegisterPage(*this, "trackdca_info", "2", "png");
        cl->CanvasToPng(TC[1], pngfile);
    }

    if (what == "ALL" || what == "TRACKQUALITY")
    {
        pngfile = cl->htmlRegisterPage(*this, "trackquality_info", "3", "png");
        cl->CanvasToPng(TC[2], pngfile);
    }

    if (what == "ALL" || what == "TRACKCHARGE")
    {
        pngfile = cl->htmlRegisterPage(*this, "trackcharge_info", "4", "png");
        cl->CanvasToPng(TC[3], pngfile);
    }

    if (what == "ALL" || what == "VERTEX")
    {
        pngfile = cl->htmlRegisterPage(*this, "vertex_info", "5", "png");
        cl->CanvasToPng(TC[4], pngfile);
    }

    if (what == "ALL" || what == "VERTEXQUALITY")
    {
        pngfile = cl->htmlRegisterPage(*this, "vertexquality_info", "6", "png");
        cl->CanvasToPng(TC[5], pngfile);
    }

    pngfile = cl->htmlRegisterPage(*this, "SiliconSeeds_Summary", "7", "png");
    cl->CanvasToPng(siseedsSummary, pngfile);

    return 0;
}

int SiliconSeedsDraw::DBVarInit()
{
    /* db = new QADrawDB(this); */
    /* db->DBInit(); */
    return 0;
}

void SiliconSeedsDraw::SetSiSeedsSummary(int runnumber, TCanvas *c)
{
    siseedsSummary = c;
    siseedsSummary->cd();
    // add the run number title
    // QADrawClient *cl = QADrawClient::instance();
    TPad *tr = new TPad("transparent_siseeds", "", 0, 0, 1, 1);
    tr->SetFillStyle(4000);
    tr->Draw();
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream;
    std::string runstring;
    runnostream << Name() << "_SiliconSeeds_summary Run " << runnumber;
    runstring = runnostream.str();
    tr->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());
    siseedsSummary->Update();
}
