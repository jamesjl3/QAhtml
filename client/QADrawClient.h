#ifndef QA_CLIENT_QADRAWCLIENT_H
#define QA_CLIENT_QADRAWCLIENT_H

#include <fun4all/Fun4AllBase.h>
#include <map>
#include <set>
#include <string>
#include <vector>

class ClientHistoList;
class QADraw;
class QAHtml;
class QARunDBodbc;
class TCanvas;
class TH1;
class TNamed;
class TStyle;

class QADrawClient : public Fun4AllBase
{
 public:
  static QADrawClient *instance();
  ~QADrawClient() override;
  void Print(const std::string &what = "ALL") const override;
  TNamed *getHisto(const std::string &hname);
  void registerHisto(const char *hname, const char *subsys);

  void registerDrawer(QADraw *Drawer);
  int ReadHistogramsFromFile(const std::string &filename);
  int GetHistoList(std::set<std::string> &histolist);
  void updateHistoMap(const std::string &hname, TNamed *h1d);
  int Draw(const std::string &who = "ALL", const std::string &what = "ALL");
  int MakeHtml(const std::string &who = "ALL", const std::string &what = "ALL");

  std::string htmlRegisterPage(const QADraw &drawer,
                               const std::string &path,
                               const std::string &basefilename,
                               const std::string &ext);

  void htmlAddMenu(const QADraw &drawer, const std::string &path,
                   const std::string &relfilename);

  void htmlNamer(const QADraw &drawer, const std::string &basefilename,
                 const std::string &ext, std::string &fullfilename,
                 std::string &filename);

  void VerbosityAll(const int v);

  int RunNumber() const { return runnumber; }
  void RunNumber(const int runno);
  std::string RunTime();
  std::string build() const { return m_build; }
  void build(const std::string build) { m_build = build;}

  void SetDisplaySizeX(const int xsize) { display_sizex = xsize; }
  void SetDisplaySizeY(const int ysize) { display_sizey = ysize; }
  int GetDisplaySizeX() { return display_sizex; }
  int GetDisplaySizeY() { return display_sizey; }
  int CanvasToPng(TCanvas *canvas, std::string const &filename);
  int HistoToPng(TH1 *histo, std::string const &pngfilename, const std::string &drawopt = "", const int statopt = 11);

  int SaveLogFile(const QADraw &drawer);
  int ExtractRunNumber(const std::string &fname);
  std::string ExtractBuild(const std::string &fname);

  std::string RunType();
  int EventsInRun();
  time_t BeginRunUnixTime();
  time_t EndRunUnixTime();

 private:
  QADrawClient();
  int DoSomething(const std::string &who, const std::string &what, const std::string &opt);
  void InitAll();
  std::vector<std::string> tokenize(const std::string &str, char delimiter);
  static QADrawClient *__instance;
  QAHtml *fHtml{nullptr};
  QARunDBodbc *rdb{nullptr};
  int runnumber{0};
  std::string m_build = "";
  int display_sizex{0};
  int display_sizey{0};
  TStyle *defaultStyle{nullptr};
  std::map<std::string, ClientHistoList *> Histo;
  std::map<std::string, QADraw *> DrawerList;
};

#endif /* QA_CLIENT_QADRAWCLIENT_H */
