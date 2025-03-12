#include <qahtml/OnlProdClient.h>
#include <qahtml/ExampleDraw.h>

R__LOAD_LIBRARY(libonlprodexample.so)

void draw_example(const char *rootfile) {

  OnlProdClient *cl = OnlProdClient::instance();
  OnlProdDraw *ex = new ExampleDraw();
  cl->registerDrawer(ex);

  cl->ReadHistogramsFromFile(rootfile);
  cl->Verbosity(1);
  cl->MakeHtml();
  delete cl;
  gSystem->Exit(0);
  return ;
}
