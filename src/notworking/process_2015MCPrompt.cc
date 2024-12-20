
//	
#ifdef STANDALONE
#include "Muon.h"
#include "Jet.h"
#include "Vertex.h"
#include "Event.h"
#include "MET.h"
#include "Constants.h"
#include "Streamer.h"
#include "MetaHiggs.h"

//	ROOT headers
#include "TFile.h"
#include "TChain.h"
#include "TString.h"
#include "TMath.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include "LumiReweightingStandAlone.h"

std::string const NTUPLEMAKER_NAME =  "ntuplemaker_H2DiMuonMaker";
std::string const CATEGORY_NAMES[5] = {
	"VBFTight", "ggFTight", "VBFLoose", "01JetTight", "01JetLoose" } ;

#define VBFTight 0
#define ggFTight 1
#define VBFLoose 2
#define JET01Tight 3
#define JET01Loose 4
#define NCATEGORIES 5

std::map<std::string, std::vector<std::pair<int, long long int> > > mapcats;
std::vector<std::pair<int, long long int> >  categories[NCATEGORIES];

using namespace analysis::core;
using namespace analysis::processing;

TH1D *hDiJetMass;
TH1D *hDiJetdeta;
TH1D *hDiMuonpt;
TH1D *hDiMuonMass;
TH1D *hDiMuoneta;
TH1D *hDiMuondphi;
TH1D *hMuonpt;
TH1D *hMuoneta;
TH1D *hMuonphi;

bool passVertex(Vertices* v)
{
	if (v->size()==0)
		return false;
	int n=0;
	for (Vertices::const_iterator it=v->begin();
		it!=v->end() && n<20; ++it)
	{
		if (TMath::Abs(it->_z)<24 &&
			it->_ndf>4)
			return true;
		n++;
	}

	return false;
}

bool passMuon(Muon const& m)
{
	if (m._isGlobal && m._isTracker &&
		m._pt>10 && TMath::Abs(m._eta)<2.4 &&
		m._isTight && (m._trackIsoSumPt/m._pt)<0.1)
		return true;
	return false;
}

bool passMuonHLT(Muon const& m)
{
	if ((m._isHLTMatched[1] || m._isHLTMatched[0]) &&
		m._pt>20 && TMath::Abs(m._eta)<2.4)
		return true;

	return false;
}

bool passMuons(Muon const& m1, Muon const& m2)
{
	if (m1._charge!=m2._charge &&
		passMuon(m1) && passMuon(m2))
		if (passMuonHLT(m1) || passMuonHLT(m2))
			return true;

	return false;
}

float jetMuondR(float jeta,float jphi, float meta, float mphi)
{
	TLorentzVector p4j,p4m;
	p4j.SetPtEtaPhiM(10, jeta, jphi, 0);
	p4m.SetPtEtaPhiM(10, meta, mphi, 0);
	return p4j.DeltaR(p4m);
}

void categorize(Jets* jets, Muon const& mu1, Muon const&  mu2, 
	MET const& met, Event const& event, float puweight=1.)
{
	int run = event._run;
	long long e = event._event;
	std::pair<int, long long int> runevent(run, e);

	TLorentzVector p4m1, p4m2;
	p4m1.SetPtEtaPhiM(mu1._pt, mu1._eta, 
		mu1._phi, PDG_MASS_Mu);
	p4m2.SetPtEtaPhiM(mu2._pt, mu2._eta, 
		mu2._phi, PDG_MASS_Mu);
	TLorentzVector p4dimuon = p4m1 + p4m2;
	if (!(p4dimuon.M()>110 && p4dimuon.M()<160 &&
		mu1._isPF && mu2._isPF))
		return;

	std::vector<TLorentzVector> p4jets;
	for (Jets::const_iterator it=jets->begin(); it!=jets->end(); ++it)
	{
		if (it->_pt>30 && TMath::Abs(it->_eta)<4.7)
		{
			if (!(jetMuondR(it->_eta, it->_phi, mu1._eta, mu1._phi)<0.3) && 
				!(jetMuondR(it->_eta, it->_phi, mu2._eta, mu2._phi)<0.3))
			{
				TLorentzVector p4;
				p4.SetPtEtaPhiM(it->_pt, it->_eta, it->_phi, it->_mass);
				p4jets.push_back(p4);
			}
		}
	}
	if (p4jets.size()>2)
		return;

	bool isPreSelected = false;
	if (p4jets.size()==2)
	{
		TLorentzVector p4lead = p4jets[0]; 
		TLorentzVector p4sub = p4jets[1];
		TLorentzVector dijet = p4lead + p4sub;

		float deta = p4lead.Eta() - p4sub.Eta();
		float dijetmass = dijet.M();
			
		if (p4lead.Pt()>40 && p4sub.Pt()>30 &&
			met._pt<40)
		{
			isPreSelected = true;

			//	fill the histograms
			double dphi = p4m1.DeltaPhi(p4m2);
			hDiJetMass->Fill(dijetmass, puweight);
			hDiJetdeta->Fill(TMath::Abs(deta), puweight);
			hDiMuonpt->Fill(p4dimuon.Pt(), puweight);
			hDiMuonMass->Fill(p4dimuon.M(), puweight);
			hDiMuoneta->Fill(p4dimuon.Eta(), puweight);
			hDiMuondphi->Fill(dphi, puweight);
			hMuonpt->Fill(p4m1.Pt(), puweight);
			hMuonpt->Fill(p4m2.Pt(), puweight);
			hMuoneta->Fill(p4m1.Eta(), puweight);
			hMuoneta->Fill(p4m2.Eta(), puweight);
			hMuonphi->Fill(p4m1.Phi(), puweight);
			hMuonphi->Fill(p4m2.Phi(), puweight);

			//	categorize
			if (dijetmass>650 && TMath::Abs(deta)>3.5)
			{
				categories[VBFTight].push_back(runevent);
				mapcats[CATEGORY_NAMES[VBFTight]].push_back(runevent);
				return;}
			if (dijetmass>250 && p4dimuon.Pt()>50)
			{
				categories[ggFTight].push_back(runevent);
				mapcats[CATEGORY_NAMES[ggFTight]].push_back(runevent);
				return;}
			else
			{
				categories[VBFLoose].push_back(runevent);
				mapcats[CATEGORY_NAMES[VBFLoose]].push_back(runevent);
				return;}
		}
	}
	if (!isPreSelected)
	{
		if (p4dimuon.Pt()>=10)
		{
			categories[JET01Tight].push_back(runevent);
			mapcats[CATEGORY_NAMES[JET01Tight]].push_back(runevent);
			return;
		}
		else
		{
			categories[JET01Loose].push_back(runevent);
			mapcats[CATEGORY_NAMES[JET01Loose]].push_back(runevent);
			return;
		}
	}
	
	return;
}

float sampleinfo(std::string const& inputname)
{
	Streamer s(inputname, NTUPLEMAKER_NAME+"/Meta");
	s.chainup();

	using namespace analysis::dimuon;
	MetaHiggs *meta=NULL;
	s._chain->SetBranchAddress("Meta", &meta);

	long long int numEvents = 0;
	long long int numEventsWeighted = 0;
	for (int i=0; i<s._chain->GetEntries(); i++)
	{
		s._chain->GetEntry(i);
		numEvents+=meta->_nEventsProcessed;
		numEventsWeighted+=meta->_sumEventWeights;
	}
	std::cout 
		<< "#events processed total = " << numEvents << std::endl
		<< "#events weighted total = " << numEventsWeighted << std::endl;

	return numEventsWeighted;
}

void ntuple_process(std::string const& inputname, 
	std::string const& outname, std::string const& pumcname,
	std::string const &pudataname)
{
	long long int numEventsWeighted = sampleinfo(inputname);

	TFile *outroot = new TFile(outname.c_str(), "recreate");
	hDiJetMass = new TH1D("DiJetMass", "DiJetMass", 20, 0, 1000);
	hDiJetdeta = new TH1D("DiJetdeta", "DiJetdeta", 14, 0, 7);
	hDiMuonpt = new TH1D("DiMuonpt", "DiMuonpt", 20, 0, 200);
	hDiMuonMass = new TH1D("DiMuonMass", "DiMuonMass", 50, 110, 160);
	hDiMuoneta = new TH1D("DiMuoneta", "DiMuoneta", 50, -2.5, 2.5);
	hDiMuondphi = new TH1D("DiMuondphi", "DiMoundphi", 18, -3.6, 3.6);
	hMuonpt = new TH1D("Muonpt", "Muonpt", 50, 0, 100);
	hMuoneta = new TH1D("Muoneta", "Muoneta", 50, -2.5, 2.5);
	hMuonphi = new TH1D("Muonphi", "Muonphi", 36, -3.6, 3.6);

	Streamer streamer(inputname, NTUPLEMAKER_NAME+"/Events");
	streamer.chainup();

	Muons *muons1=NULL;
	Muons *muons2=NULL;
	Jets *jets=NULL;
	Vertices *vertices=NULL;
	Event *event=NULL;
	EventAuxiliary *aux=NULL;
	MET *met=NULL;
	streamer._chain->SetBranchAddress("Muons1", &muons1);
	streamer._chain->SetBranchAddress("Muons2", &muons2);
	streamer._chain->SetBranchAddress("Jets", &jets);
	streamer._chain->SetBranchAddress("Vertices", &vertices);
	streamer._chain->SetBranchAddress("Event", &event);
	streamer._chain->SetBranchAddress("EventAuxiliary", &aux);
	streamer._chain->SetBranchAddress("MET", &met);

	TString mc_pileupfile = pumcname.c_str();
	TString data_pileupfile = pudataname.c_str();
	reweight::LumiReWeighting *weighter = new reweight::LumiReWeighting(
		mc_pileupfile.Data(), data_pileupfile.Data(), "pileup", "pileup");

	uint32_t numEntries = streamer._chain->GetEntries();
	for (uint32_t i=0; i<numEntries; i++)
	{
		streamer._chain->GetEntry(i);
		if (i%1000==0)
			std::cout << "### Event " << i << " / " << numEntries
				<< std::endl;

		float puweight = weighter->weight(aux->_nPU)*aux->_genWeight;

		//
		//	Selections
		//
		if (!passVertex(vertices))
			continue;
		if (!(aux->_hasHLTFired[0] || aux->_hasHLTFired[1]))
			continue;
		for (uint32_t im=0; im<muons1->size(); im++)
		{
			if (!passMuons(muons1->at(im), muons2->at(im)))
				continue;
			categorize(jets, muons1->at(im), muons2->at(im), *met, *event,
				puweight);
		}
	}

	ofstream o("categories.txt");
	for (uint32_t i=0; i<NCATEGORIES; i++)
	{
		std::cout << "CATEGORY: " << CATEGORY_NAMES[i] << "  #EVENTS: " 
			<< categories[i].size() << std::endl;
		o << CATEGORY_NAMES[i] << std::endl;
		for (uint32_t ie=0; ie<categories[i].size(); ie++)
			o << categories[i][ie].first << "  " << 
				categories[i][ie].second << std::endl;
	}

	outroot->Write();
	outroot->Close();

	return;
}

int main(int argc, char** argv)
{
	if (argc<2 || argc>5)
	{
		std::cout << "Usage:" << std::endl
			<< "./synchronize <input file pathname> <out file pathname> <PU file MC pathname> <PU file DATA pathname>" << std::endl;
		return 1;
	}

	for (int i=0; i<NCATEGORIES; i++)
	{
		std::cout << CATEGORY_NAMES[i] << std::endl;
		mapcats[CATEGORY_NAMES[i]] = std::vector<std::pair<int, long long int>>();
	}

	ntuple_process(argv[1], argv[2], argv[3], argv[4]);
	return 0;
}

#endif
