void CreateRootFile(Int_t runnumber = 1234567, Int_t nevents = 10000)
{
  //  This program creates :
  //    - a one dimensional histogram
  //    - a two dimensional histogram
  //    - a profile histogram
  //
  //  These objects are filled with some random numbers and saved on a file.
  //  If get=1 the macro returns a pointer to the TFile of "hsimple.root"
  //          if this file exists, otherwise it is created.
  //  The file "hsimple.root" is created in $ROOTSYS/tutorials if the caller has
  //  write access to this directory, otherwise the file is created in $PWD
  char filename[100];
  sprintf(filename, "Run_%d_Example.root", runnumber);
  TFile *hfile = new TFile(filename,"RECREATE", "Example file with histograms");
  // Create some histograms, a profile histogram, following our naming convention
  // so we don't interfere with other onlprod modules
  TH1 *hpx = new TH1F("example_px", "This is the px distribution", 100, -4, 4);
  TH2 *hpxpy = new TH2F("example_pxpy", "py vs px", 40, -4, 4, 40, -4, 4);
  TProfile *hprof = new TProfile("example_hprof", "Profile of pz versus px", 100, -4, 4, 0, 20);
  // Fill histograms randomly
  gRandom->SetSeed();
  Float_t px, py, pz;
  for (Int_t i = 0;
       i < nevents;
       i++)
    {
      gRandom->Rannor(px, py);
      pz = px * px + py * py;
      Float_t random = gRandom->Rndm(1);
      hpx->Fill(px);
      hpxpy->Fill(px, py);
      hprof->Fill(px, pz);
    }
  hfile->Write();
  hfile->Close();
  delete hfile;
}
