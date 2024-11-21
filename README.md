# run3_analyzer

TO INSTALL:

Clone the repository to your space:\
`git clone https://github.com/kaderci-johannes/run3_analyzer.git`\
\
Create and go into build folder:\
`cd run3_analyzeru&& mkdir build && cd build`\
\
Configure the code:\
`cmake -DCMAKE_PREFIX_PATH="/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.28.10/x86_64-centos8-gcc85-opt;/cvmfs/sft.cern.ch/lcg/releases/LCG_103/Boost/1.81.0/x86_64-centos9-gcc11-opt/" ..`\
\
Compile:\
`make`\
\
REMINDER - If any edits are made into the C++ files, you must “make” once more.

\
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

\
-- will be edited from here --
TO RUN:  

(LOCATION) 

    cd /afs/cern.ch/user/d/dblend/test-analysis/build/src/drivers 

 

(TEST FILE) 

(HELP OPTIONS) 

    ./process_nanoaod_2024_quick_test –help 

 

(EXAMPLE) 

    ./process_nanoaod_2024_quick_test --root-file /eos/user/d/dblend/Smashing_Physics/8AE6D24A-148A-1E4C-A948-4ECF4C03732E.root  

 

(HIGGS ANALYZER) 

 

(EXAMPLE) 

    Full Version 

    ./process_HiggsAnalysis_wCutsExtended_NoPairing_v0 --input test.list --output output.root --isMC yes --genPUMC true --puMC myMCPUfile.root --puDATA PileupHistogram-goldenJSON-13tev-2017-69200ub-99bins.root 

    Simple Version without pileup 

    ./process_HiggsAnalysis_wCutsExtended_NoPairing_v0 --input test.list --output output.root --isMC false
