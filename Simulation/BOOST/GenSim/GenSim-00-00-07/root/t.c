int t()
{
  
  for(Int_t i=0;i<339;i)
  {
    TFile* f1 = new TFile("ppdc.root");
    TH1F* h1 = (TH1F*)f1->Get("htemp");
    //set parameters
    Double_t pMag = h1->GetRandom();
    cout<<"pMag: "<<pMag<<endl;
    f1->Close();

    TFile* f2 = new TFile("theta.root");
    TH1F* h2 = (TH1F*)f2->Get("htemp");
    //randomize cos(theta)
    Double_t theta =(Double_t)h2->GetRandom();
    cout<<"theta: "<<theta<<endl;
    G4double costheta = cos(theta);
    f2->Close();

    TFile* f3 = new TFile("phi.root");
    TH1F* h3 = (TH1F*)f3->Get("htemp");
    //randomize phi
    Double_t phi = (Double_t)h3->GetRandom();
    cout<<"phi: "<<phi<<endl;
    f3->Close();
  }
}
