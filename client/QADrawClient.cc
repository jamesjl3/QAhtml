#include "QADrawClient.h"
#include "ClientHistoList.h"
#include "QADraw.h"
#include "QAHtml.h"
#include "QARunDBodbc.h"

#include <TCanvas.h>
#include <TClass.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGFrame.h>
#include <TH1.h>
#include <TImage.h>
#include <TIterator.h>
#include <TNamed.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

QADrawClient *QADrawClient::__instance = nullptr;

QADrawClient *QADrawClient::instance()
{
  if (__instance)
  {
    return __instance;
  }
  __instance = new QADrawClient();
  return __instance;
}

QADrawClient::QADrawClient()
  : Fun4AllBase("QADRAWCLIENT")
{
  for (int i = 0; i < 15; i++)
  {
    gSystem->IgnoreSignal((ESignals) i);
  }
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  defaultStyle = gStyle;
  InitAll();
}

void QADrawClient::InitAll()
{
  if (!gClient)
  {
    utsname ThisNode{};
    uname(&ThisNode);
    std::cout << "virtual framebuffer not running on " << ThisNode.nodename
              << ", check if process /usr/X11R6/bin/Xvfb is alive" << std::endl;
    exit(1);
  }

  fHtml = new QAHtml(getenv("QA_HTMLDIR"));
  TGFrame *rootWin = (TGFrame *) gClient->GetRoot();
  display_sizex = rootWin->GetDefaultWidth();
  display_sizey = rootWin->GetDefaultHeight();
  return;
}

QADrawClient::~QADrawClient()
{
  delete fHtml;
  delete rdb;
  while (Histo.begin() != Histo.end())
  {
    delete Histo.begin()->second;
    Histo.erase(Histo.begin());
  }
  return;
}

void QADrawClient::registerHisto(const char *hname, const char *subsys)
{
  std::map<std::string, ClientHistoList *>::const_iterator histoiter = Histo.find(hname);
  if (histoiter != Histo.end())
  {
#ifdef DEBUG
    std::cout << "deleting histogram " << hname << " at " << Histo[hname] << std::endl;
#endif

    histoiter->second->SubSystem(subsys);
  }
  else
  {
    ClientHistoList *newhisto = new ClientHistoList();
    newhisto->SubSystem(subsys);
    Histo[hname] = newhisto;
#ifdef DEBUG

    std::cout << "new histogram " << hname << " at " << Histo[hname] << std::endl;
#endif
  }
  return;
}

void QADrawClient::registerDrawer(QADraw *Drawer)
{
  std::map<std::string, QADraw *>::iterator iter = DrawerList.find(Drawer->Name());
  if (iter != DrawerList.end())
  {
    std::cout << "Drawer " << Drawer->Name() << " already registered, I won't overwrite it" << std::endl;
    std::cout << "Use a different name and try again" << std::endl;
  }
  else
  {
    DrawerList[Drawer->Name()] = Drawer;
    Drawer->Init();
  }
  defaultStyle->cd();
  return;
}

int QADrawClient::Draw(const std::string &who, const std::string &what)
{
  return DoSomething(who, what, "DRAW");
}

int QADrawClient::MakeHtml(const std::string &who, const std::string &what)
{
  mode_t old_umask = 0;
  char *onlprod_real_html = getenv("QA_REAL_HTML");
  if (!onlprod_real_html)
  {
    old_umask = umask(S_IWOTH);
    std::cout << "Making html output group writable so others can run tests as well" << std::endl;
  }
  fHtml->runNumber(RunNumber());  // do not forget this !
  fHtml->RunType(RunType());
  int iret = DoSomething(who, what, "HTML");
  if (!onlprod_real_html)
  {
    umask(old_umask);
  }
  return iret;
}

int QADrawClient::DoSomething(const std::string &who, const std::string &what, const std::string &opt)
{
  int i = 0;
  std::map<std::string, QADraw *>::iterator iter;
  if (who != "ALL")
  {
    iter = DrawerList.find(what);
    if (iter != DrawerList.end())
    {
      if (opt == "DRAW")
      {
        iter->second->Draw(what);
      }
      else if (opt == "HTML")
      {
        if (Verbosity() > 0)
        {
          std::cout << " creating html output for "
                    << iter->second->Name() << std::endl;
        }
        if (iter->second->MakeHtml(what))
        {
          std::cout << "subsystem " << iter->second->Name()
                    << " not in root file, skipping" << std::endl;
        }
      }
      defaultStyle->cd();
      return 0;
    }
    else
    {
      std::cout << "Drawer " << who << " not in list" << std::endl;
      Print("DRAWER");
      return -1;
    }
  }
  else
  {
    for (iter = DrawerList.begin(); iter != DrawerList.end(); ++iter)
    {
      if (opt == "DRAW")
      {
        i += iter->second->Draw(what);
      }
      else if (opt == "HTML")
      {
        if (Verbosity() > 0)
        {
          std::cout << " creating html output for "
                    << iter->second->Name() << std::endl;
        }
        gROOT->Reset();
        int iret = iter->second->MakeHtml(what);
        if (iret)
        {
          std::cout << "subsystem " << iter->second->Name()
                    << " not in root file, skipping" << std::endl;
          // delete all canvases (no more piling up of 50 canvases)
          // if run for a single subsystem this leaves the canvas intact
          // for debugging
          TSeqCollection *allCanvases = gROOT->GetListOfCanvases();
          TCanvas *canvas = nullptr;
          while ((canvas = (TCanvas *) allCanvases->First()))
          {
            std::cout << "Deleting Canvas " << canvas->GetName() << std::endl;
            delete canvas;
          }
        }
        i += iret;
      }
      defaultStyle->cd();  // restore default style
    }
  }
  return i;
}

void QADrawClient::updateHistoMap(const std::string &hname, TNamed *h1d)
{
  std::map<std::string, ClientHistoList *>::const_iterator histoiter = Histo.find(hname);
  if (histoiter != Histo.end())
  {
#ifdef DEBUG
    std::cout << "deleting histogram " << hname << " at " << Histo[hname] << std::endl;
#endif

    delete histoiter->second->Histo();  // delete old histogram
    histoiter->second->Histo(h1d);
  }
  else
  {
    ClientHistoList *newhisto = new ClientHistoList();
    newhisto->Histo(h1d);
    Histo[hname] = newhisto;
#ifdef DEBUG

    std::cout << "new histogram " << hname << " at " << Histo[hname] << std::endl;
#endif
  }
  return;
}

TNamed *
QADrawClient::getHisto(const std::string &hname)
{
  std::map<std::string, ClientHistoList *>::const_iterator histoiter = Histo.find(hname);
  if (histoiter != Histo.end())
  {
    return histoiter->second->Histo();
  }
  return nullptr;
}

void QADrawClient::Print(const std::string &what) const
{
  if (what == "ALL" || what == "DRAWER")
  {
    // loop over the map and print out the content (name and location in memory)
    std::cout << "--------------------------------------" << std::endl
              << std::endl;
    std::cout << "List of Drawers in QADrawClient:" << std::endl;

    std::map<std::string, QADraw *>::const_iterator hiter;
    for (hiter = DrawerList.begin(); hiter != DrawerList.end(); ++hiter)
    {
      std::cout << hiter->first << " is at " << hiter->second << std::endl;
    }
    std::cout << std::endl;
  }
  if (what == "ALL" || what == "HISTOS")
  {
    // loop over the map and print out the content (name and location in memory)
    std::cout << "--------------------------------------" << std::endl
              << std::endl;
    std::cout << "List of Histograms in QADrawClient:" << std::endl;

    std::map<std::string, ClientHistoList *>::const_iterator hiter;
    for (hiter = Histo.begin(); hiter != Histo.end(); ++hiter)
    {
      std::cout << hiter->first << " Address " << hiter->second->Histo()
                << ", subsystem " << hiter->second->SubSystem() << std::endl;
    }
    std::cout << std::endl;
  }
  return;
}
std::string QADrawClient::ExtractBuild(const std::string& filename)
{
  std::string build = "";
  std::vector<std::string> tokens = tokenize(filename, '_');
  for (auto token : tokens)
    {
      if(token.find("new") != std::string::npos)
	{
	  build = "new";
	}
      else if (token.find("ana") != std::string::npos)
	{
	  build = token; 
	}
    }

  return build;
}
int QADrawClient::ReadHistogramsFromFile(const std::string &filename)
{
  TDirectory *save = gDirectory;  // save current dir (which will be overwritten by the following fileopen)
  TFile *histofile = new TFile(filename.c_str(), "READ");
  if (!histofile)
  {
    std::cout << "Can't open " << filename << std::endl;
    return -1;
  }
  save->cd();
  build(ExtractBuild(filename));
  RunNumber(ExtractRunNumber(filename));
  TIterator *titer = histofile->GetListOfKeys()->MakeIterator();
  TObject *obj;
  TNamed *histo, *histoptr;
  while ((obj = titer->Next()))
  {
    if (Verbosity() > 0)
    {
      std::cout << "TObject at " << obj << std::endl;
      std::cout << obj->GetName() << std::endl;
      std::cout << obj->ClassName() << std::endl;
    }
    histofile->GetObject(obj->GetName(), histoptr);
    if (histoptr)
    {
      histo = (TNamed *) (histoptr->Clone());
      updateHistoMap(histo->GetName(), histo);
      if (Verbosity() > 0)
      {
        std::cout << "HistoName: " << histo->GetName() << std::endl;
        std::cout << "HistoClass: " << histo->ClassName() << std::endl;
      }
    }
  }
  delete histofile;
  delete titer;
  return 0;
}

int QADrawClient::GetHistoList(std::set<std::string> &histolist)
{
  std::map<std::string, ClientHistoList *>::const_iterator iter;
  for (iter = Histo.begin(); iter != Histo.end(); ++iter)
  {
    histolist.insert(iter->first);
  }
  return 0;
}

//_____________________________________________________________________________
void QADrawClient::VerbosityAll(const int v)
{
  Verbosity(v);
  if (fHtml)
  {
    fHtml->verbosity(v);
  }
}

//_____________________________________________________________________________
void QADrawClient::htmlAddMenu(const QADraw &drawer,
                               const std::string &path,
                               const std::string &relfilename)
{
  fHtml->addMenu(drawer.Name(), path, relfilename);
}

//_____________________________________________________________________________
void QADrawClient::htmlNamer(const QADraw &drawer,
                             const std::string &basefilename,
                             const std::string &ext,
                             std::string &fullfilename,
                             std::string &filename)
{
  fHtml->namer(drawer.Name(), basefilename, ext, fullfilename, filename);
}

//_____________________________________________________________________________
std::string
QADrawClient::htmlRegisterPage(const QADraw &drawer,
                               const std::string &path,
                               const std::string &basefilename,
                               const std::string &ext)
{
  return fHtml->registerPage(drawer.Name(), path, basefilename, ext);
}

int QADrawClient::CanvasToPng(TCanvas *canvas, std::string const &pngfilename)
{
  // in order not to collide when running multiple html generators
  // create a unique filename (okay tempnam is not totally safe against
  // multiple procs getting the same name but the local /tmp filesystem should
  // prevent at least multiple machines colliding)
  char *tmpname = tempnam("/tmp", "TC");
  canvas->Print(tmpname, "gif");  // write gif format
  TImage *img = TImage::Open(tmpname);
  img->WriteImage(pngfilename.c_str());
  delete img;
  remove(tmpname);
  // NOLINTNEXTLINE(hicpp-no-malloc)
  free(tmpname);
  return 0;
}

int QADrawClient::HistoToPng(TH1 *histo, std::string const &pngfilename, const std::string &drawopt, const int statopt)
{
  TCanvas *cgiCanv = new TCanvas("cgiCanv", "cgiCanv", 200, 200, 650, 500);
  gStyle->SetOptStat(statopt);
  cgiCanv->SetFillColor(0);
  cgiCanv->SetBorderMode(0);
  cgiCanv->SetBorderSize(2);
  cgiCanv->SetFrameFillColor(0);
  cgiCanv->SetFrameBorderMode(0);
  cgiCanv->SetTickx();
  cgiCanv->SetTicky();
  cgiCanv->cd();
  histo->SetMarkerStyle(8);
  histo->SetMarkerSize(0.15);
  histo->Draw(drawopt.c_str());
  // returned char array from tempnam() needs to be free'd after use
  char *tmpname = tempnam("/tmp", "HI");
  cgiCanv->Print(tmpname, "gif");
  TImage *img = TImage::Open(tmpname);
  img->WriteImage(pngfilename.c_str());
  remove(tmpname);
  // NOLINTNEXTLINE(hicpp-no-malloc)
  free(tmpname);
  delete cgiCanv;
  return 0;
}

int QADrawClient::SaveLogFile(const QADraw &drawer)
{
  // sendfile example shamelessly copied from
  // http://www.linuxgazette.com/issue91/tranter.html
  std::ostringstream logfilename, msg;
  const char *logdir = getenv("QA_LOGDIR");
  if (logdir)
  {
    logfilename << logdir << "/";
  }
  int irun = RunNumber();
  logfilename << drawer.Name() << "_" << irun << ".log";
  int src;  /* file descriptor for source file */
  int dest; /* file descriptor for destination file */
  struct stat stat_buf
  {
  };                /* hold information about input file */
  off_t offset = 0; /* byte offset used by sendfile */
                    // NOLINTNEXTLINE(hicpp-vararg)
  src = open(logfilename.str().c_str(), O_RDONLY);
  if (src != -1)
  {
    fstat(src, &stat_buf);
    std::string outfile = htmlRegisterPage(drawer, "Logfile", "log", "txt");
    // NOLINTNEXTLINE(hicpp-vararg)
    dest = open(outfile.c_str(), O_WRONLY | O_CREAT, stat_buf.st_mode);
    sendfile(dest, src, &offset, stat_buf.st_size);
    close(dest);
    close(src);
  }
  return 0;
}

int QADrawClient::ExtractRunNumber(const std::string &filename)
{
  int runno = 0;
  std::vector<std::string> tokens = tokenize(filename, '-');
  auto iter = tokens.rbegin();
  ++iter;
  if (Verbosity() > 0)
  {
    std::cout << "run number string is " << *iter << std::endl;
  }
  try
  {
    runno = std::stoi(*iter);
  }
  catch (const std::invalid_argument &e)
  {
    std::cout << "Problem extracting runnumber from filename "
              << filename << ", tried std::stoi on "
              << *iter << std::endl;
  }
  return runno;
}

void QADrawClient::RunNumber(const int runno)
{
  if (runnumber != runno)
  {
    if (rdb)
    {
      rdb->FillFromdaqDB(runno);
    }
    else
    {
      rdb = new QARunDBodbc(runno);
    }
    runnumber = runno;
  }
}

std::string QADrawClient::RunTime()
{
  if (!rdb)
  {
    rdb = new QARunDBodbc();
  }
  return rdb->RunTime();
}

std::string
QADrawClient::RunType()
{
  if (!rdb)
  {
    rdb = new QARunDBodbc();
  }
  return rdb->RunType();
}

int QADrawClient::EventsInRun()
{
  if (!rdb)
  {
    rdb = new QARunDBodbc();
  }
  return rdb->EventsInRun();
}

time_t
QADrawClient::BeginRunUnixTime()
{
  if (!rdb)
  {
    rdb = new QARunDBodbc();
  }
  return rdb->BeginRunUnixTime();
}

time_t
QADrawClient::EndRunUnixTime()
{
  if (!rdb)
  {
    rdb = new QARunDBodbc();
  }
  return rdb->EndRunUnixTime();
}

std::vector<std::string> QADrawClient::tokenize(const std::string &str, char delimiter)
{
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = str.find(delimiter);

  while (end != std::string::npos)
  {
    tokens.push_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
  }
  tokens.push_back(str.substr(start));

  return tokens;
}
