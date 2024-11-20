# run3_analyzer

TO INSTALL:

Clone the repository to your space:\
`git clone --recursive https://github.com/vkhristenko/test-analysis`\
Create and go into build folder:\
`cd test-analysis && mkdir build && cd build`
(OPTIONAL) git clone --recurse-submodules https://github.com/uiowahep/Analysis\
Compile the code:
`cmake -DCMAKE_PREFIX_PATH="/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.28.10/x86_64-centos8-gcc85-opt;/cvmfs/sft.cern.ch/lcg/releases/LCG_103/Boost/1.81.0/x86_64-centos9-gcc11-opt/" ..`
4. make
 - (REMINDER) If any edits are made… you must “make” once more


CHANGES:

1. std::string const NTUPLEMAKER_NAME = “ntuplemaker_H2DiMuonMaker”;\
   --> std::string const NTUPLEMAKER_NAME = “”;
2. TH1D *h = new TH1D(“pileup”, “pileup”, 50, 0, 50);\
   --> TH1D *h = new TH1D(“pileup”, “pileup”, 50, 0, 50);
3. SET_BRANCH_BOOL_ARRAY(Muon_isGlobal);\
   --> // SET_BRNACH_BOOL_ARRAY(Muon_isGlobal);
4. SET_BRANCH_UCHAR_ARRAY(Muon_miniIsoId);\
   --> // SET_BRANCH_UCHAR_ARRAY(Muon_miniIsoId);
5. mu._isGlobal = Muon_isGlobal[i]\
   --> // mu._isGlobal = Muon_isGlobal[i]
6. Comment out “streamer._chain->SetBranchAddress...”
7. if (!(aux->hasHLTFired[0] || aux->_hasHLTFired[1]))\
   continue;\
   --> // if (!(aux->hasHLTFired[0] || aux->_hasHLTFired[1]))\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;// continue;
8. if (!passMuons(muons1.at(im), muons2.at(im)))\
   continue;\
   --> // if (!passMuons(muons1.at(im), muons2.at(im)))\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;// continue;
