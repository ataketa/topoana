#include "../include/topoana.h"
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include <ctime>
#include "TTreeFormula.h"
#include <sstream>

void topoana::getRslt()
{
  unsigned int Nasterisks=89;
  for(unsigned int i=0;i<Nasterisks;i++) cout<<"*";
  cout<<endl<<endl;

  TChain * chn=new TChain(m_ttrNm.c_str());
  vector<unsigned long> vNEtr;
  vNEtr.clear();
  for(unsigned int i=0;i<m_nmsOfIptRootFls.size();i++)
    {
      chn->Add(m_nmsOfIptRootFls[i].c_str());
      if(m_oneOptRootFlByOneIptRootFl==true) vNEtr.push_back(chn->GetEntries());
    }
  unsigned long nEtr=chn->GetEntries();
  cout<<"There are "<<nEtr<<" entries in total in the input root files."<<endl<<endl;

  // The following statement instead of the one next to it ought to be used along with the Ntuple Tool MCGenKinematics with the option [200].
  const unsigned int NpsMax=200;
  // const unsigned int NpsMax=chn->GetMaximum(m_tbrNmOfNps.c_str());
  int Nps,Pid[NpsMax],Midx[NpsMax];
  double Npsd,Pidd[NpsMax],Midxd[NpsMax];
  int Icandi;
  bool isTheEvtPrcsd;
  vector<int> *pVPid=0, *pVMidx=0;
  if(m_strgTpOfRawIptTopoDat=="AOI"||m_strgTpOfRawIptTopoDat=="MSI") chn->SetBranchAddress(m_tbrNmOfNps.c_str(), &Nps);
  else if(m_strgTpOfRawIptTopoDat=="MSD") chn->SetBranchAddress(m_tbrNmOfNps.c_str(), &Npsd);
  if(m_strgTpOfRawIptTopoDat=="AOI")
    {
      chn->SetBranchAddress(m_tbrNmOfPid.c_str(), &Pid);
      chn->SetBranchAddress(m_tbrNmOfMidx.c_str(), &Midx);
    }
  else if(m_strgTpOfRawIptTopoDat=="VOI")
    {
      chn->SetBranchAddress(m_tbrNmOfPid.c_str(), &pVPid);
      chn->SetBranchAddress(m_tbrNmOfMidx.c_str(), &pVMidx);
    }
  else
    {
      const unsigned int NpsMaxTmp=chn->GetMaximum(m_tbrNmOfNps.c_str());
      char strI[10]; string specifierPid,specifierMidx;
      for(unsigned int i=0;i<NpsMaxTmp;i++)
        {
          sprintf(strI, "_%d", i);
          specifierPid=m_tbrNmOfPid+strI;
          if(m_strgTpOfRawIptTopoDat=="MSD") chn->SetBranchAddress(specifierPid.c_str(), &Pidd[i]);
          else chn->SetBranchAddress(specifierPid.c_str(), &Pid[i]);
          specifierMidx=m_tbrNmOfMidx+strI;
          if(m_strgTpOfRawIptTopoDat=="MSD") chn->SetBranchAddress(specifierMidx.c_str(), &Midxd[i]);
          else chn->SetBranchAddress(specifierMidx.c_str(), &Midx[i]);
        }
    }
  if(m_avoidOverCounting==true) chn->SetBranchAddress(m_tbrNmOfIcandi.c_str(), &Icandi);

  bool openANewOptRootFl=true;
  unsigned int iOptRootFls=0;
  string NmOfOptRootFl;
  TFile * fl;
  TTree * tr;
  bool closeTheOptRootFl1;
  bool closeTheOptRootFl2;
  bool closeTheOptRootFl3;
  const unsigned long nTotBytesMaxOfASngOptRootFl=(unsigned long) 3*1024*1024*1024; // The size of a single output root file in memory is 3G.

  int iDcyTr,iCcDcyTr,iDcyIFSts,iCcDcyIFSts;

  const unsigned int nMax=200;

  const unsigned int sAtdbPid=m_vPid_compDcyBrP.size()>0?m_vPid_compDcyBrP.size():1;
  int iCcPDcyBr[sAtdbPid],nPDcyBr[sAtdbPid],nCcPDcyBr[sAtdbPid],nAllPDcyBr[sAtdbPid];
  int iDcyBrP[sAtdbPid][nMax],iCcDcyBrP[sAtdbPid][nMax],iDcyBrCcP[sAtdbPid][nMax];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vPid_compDcyBrP.size();i++) iCcPDcyBr[i]=m_vICcCompDcyBrP[i];

  const unsigned int sAtcdbPid=m_vPid_compCascDcyBrP.size()>0?m_vPid_compCascDcyBrP.size():1;
  int iCcPCascDcyBr[sAtcdbPid],nPCascDcyBr[sAtcdbPid],nCcPCascDcyBr[sAtcdbPid],nAllPCascDcyBr[sAtcdbPid];
  int iCascDcyBrP[sAtcdbPid][nMax],iCcCascDcyBrP[sAtcdbPid][nMax],iCascDcyBrCcP[sAtcdbPid][nMax];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vPid_compCascDcyBrP.size();i++) iCcPCascDcyBr[i]=m_vICcCompCascDcyBrP[i];

  const unsigned int sAtdfstPid=m_vPid_compDcyFStP.size()>0?m_vPid_compDcyFStP.size():1;
  int iCcPDcyFSt[sAtdfstPid],nPDcyFSt[sAtdfstPid],nCcPDcyFSt[sAtdfstPid],nAllPDcyFSt[sAtdfstPid];
  int iDcyFStP[sAtdfstPid][nMax],iCcDcyFStP[sAtdfstPid][nMax],iDcyFStCcP[sAtdfstPid][nMax];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vPid_compDcyFStP.size();i++) iCcPDcyFSt[i]=m_vICcCompDcyFStP[i];

  const unsigned int sAtpbPid=m_vPid_compProdBrP.size()>0?m_vPid_compProdBrP.size():1;
  int iCcPProdBr[sAtpbPid],nPProdBr[sAtpbPid],nCcPProdBr[sAtpbPid],nAllPProdBr[sAtpbPid];
  int iProdBrP[sAtpbPid][nMax],iCcProdBrP[sAtpbPid][nMax],iProdBrCcP[sAtpbPid][nMax];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vPid_compProdBrP.size();i++) iCcPProdBr[i]=m_vICcCompProdBrP[i];

  const unsigned int sAtmPid=m_vPid_compMP.size()>0?m_vPid_compMP.size():1;
  int iCcPM[sAtmPid],nPM[sAtmPid],nCcPM[sAtmPid],nAllPM[sAtmPid];
  int MpidP[sAtmPid][nMax],iCcMP[sAtmPid][nMax],MpidCcP[sAtmPid][nMax];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vPid_compMP.size();i++) iCcPM[i]=m_vICcCompMP[i];

  const unsigned int sVAtecIncDcyBr=m_vCompIncDcyBr.size()>0?m_vCompIncDcyBr.size():1;
  int iCcIncDcyBr[sVAtecIncDcyBr],nIncDcyBr[sVAtecIncDcyBr],nCcIncDcyBr[sVAtecIncDcyBr],nAllIncDcyBr[sVAtecIncDcyBr];
  int iDcyBrIncDcyBr[sVAtecIncDcyBr][nMax],iCcDcyBrIncDcyBr[sVAtecIncDcyBr][nMax],iDcyBrCcIncDcyBr[sVAtecIncDcyBr][nMax];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vCompIncDcyBr.size();i++) iCcIncDcyBr[i]=m_vICcCompIncDcyBr[i];

  const unsigned int sVAtecIRADcyBr=m_vCompIRADcyBr.size()>0?m_vCompIRADcyBr.size():1;
  int iCcIRADcyBr[sVAtecIRADcyBr],nIRADcyBr[sVAtecIRADcyBr],nCcIRADcyBr[sVAtecIRADcyBr],nAllIRADcyBr[sVAtecIRADcyBr];
  int iDcyBrIRADcyBr[sVAtecIRADcyBr][nMax],iCcDcyBrIRADcyBr[sVAtecIRADcyBr][nMax],iDcyBrCcIRADcyBr[sVAtecIRADcyBr][nMax];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vCompIRADcyBr.size();i++) iCcIRADcyBr[i]=m_vICcCompIRADcyBr[i];

  char nmSigDcyTr[200]; int iSigDcyTr,iCcSigDcyTr,iSigDcyIFSts_tr,iCcSigDcyIFSts_tr;
  char nmSigDcyIFSts[200]; int iSigDcyIFSts,iCcSigDcyIFSts;

  const unsigned int sSigP=m_vPid_sigP.size()>0?m_vPid_sigP.size():1;
  int iCcSigP[sSigP],nSigP[sSigP],nCcSigP[sSigP],nAllSigP[sSigP];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vPid_sigP.size();i++) iCcSigP[i]=m_vICcSigP[i];

  const unsigned int sSigDcyBr=m_vSigDcyBr.size()>0?m_vSigDcyBr.size():1;
  int iCcSigDcyBr[sSigDcyBr],nSigDcyBr[sSigDcyBr],nCcSigDcyBr[sSigDcyBr],nAllSigDcyBr[sSigDcyBr];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vSigDcyBr.size();i++) iCcSigDcyBr[i]=m_vICcSigDcyBr[i];

  const unsigned int sSigIncDcyBr=m_vSigIncDcyBr.size()>0?m_vSigIncDcyBr.size():1;
  int iCcSigIncDcyBr[sSigIncDcyBr],nSigIncDcyBr[sSigIncDcyBr],nCcSigIncDcyBr[sSigIncDcyBr],nAllSigIncDcyBr[sSigIncDcyBr];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vSigIncDcyBr.size();i++) iCcSigIncDcyBr[i]=m_vICcSigIncDcyBr[i];

  const unsigned int sSigCascDcyBr=m_vSigCascDcyBr.size()>0?m_vSigCascDcyBr.size():1;
  int iCcSigCascDcyBr[sSigCascDcyBr],nSigCascDcyBr[sSigCascDcyBr],nCcSigCascDcyBr[sSigCascDcyBr],nAllSigCascDcyBr[sSigCascDcyBr];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vSigCascDcyBr.size();i++) iCcSigCascDcyBr[i]=m_vICcSigCascDcyBr[i];

  const unsigned int sSigIncCascDcyBr=m_vSigIncCascDcyBr.size()>0?m_vSigIncCascDcyBr.size():1;
  int iCcSigIncCascDcyBr[sSigIncCascDcyBr],nSigIncCascDcyBr[sSigIncCascDcyBr],nCcSigIncCascDcyBr[sSigIncCascDcyBr],nAllSigIncCascDcyBr[sSigIncCascDcyBr];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vSigIncCascDcyBr.size();i++) iCcSigIncCascDcyBr[i]=m_vICcSigIncCascDcyBr[i];

  const unsigned int sSigIRADcyBr=m_vSigIRADcyBr.size()>0?m_vSigIRADcyBr.size():1;
  int iCcSigIRADcyBr[sSigIRADcyBr],nSigIRADcyBr[sSigIRADcyBr],nCcSigIRADcyBr[sSigIRADcyBr],nAllSigIRADcyBr[sSigIRADcyBr];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vSigIRADcyBr.size();i++) iCcSigIRADcyBr[i]=m_vICcSigIRADcyBr[i];

  const unsigned int sSigIncOrIRACascDcyBr=m_vSigIncOrIRACascDcyBr.size()>0?m_vSigIncOrIRACascDcyBr.size():1;
  int iCcSigIncOrIRACascDcyBr[sSigIncOrIRACascDcyBr],nSigIncOrIRACascDcyBr[sSigIncOrIRACascDcyBr],nCcSigIncOrIRACascDcyBr[sSigIncOrIRACascDcyBr],nAllSigIncOrIRACascDcyBr[sSigIncOrIRACascDcyBr];
  if(m_ccSwitch==true) for(unsigned int i=0;i<m_vSigIncOrIRACascDcyBr.size();i++) iCcSigIncOrIRACascDcyBr[i]=m_vICcSigIncOrIRACascDcyBr[i];

  TTreeFormula * trfml;
  unsigned int nInsts=1;
  if(!m_cut.empty())
    {
      trfml=new TTreeFormula("trfml",m_cut.c_str(),chn);
      chn->SetNotify(trfml); // This statement is indispensible if more than one root file is added to the object of the TChain class.
    }
  unsigned long nEtrThroughTheCut=0;

  vector<int> vPid, vCcPid;
  vector<int> vMidx, vCcMidx;
  vector< list<int> > dcyTr, ccDcyTr;
  vector<int> vIdxOfHead, vCcIdxOfHead;
  vector<int> vMidxOfHead, vCcMidxOfHead;
  string strDcyTr, strCcDcyTr;
  list<int> dcyIFSts,ccDcyIFSts;
  string strDcyIFSts, strCcDcyIFSts;
  string strCascDcyBrP, strCascDcyBrCcP;
  string strDcyFStP, strDcyFStCcP;

  unsigned long nEtrToBePrcsd=nEtr<m_nEtrMax?nEtr:m_nEtrMax;
  if(m_anaTasksForSigIds!="C")
    {
      unsigned long nEtrForTiming=5000;
      clock_t starttime=clock();
      for(unsigned int i=0;i<nEtrToBePrcsd;i++)
        {
          if(openANewOptRootFl==true)
            {
              openANewOptRootFl=false;
              if(m_oneOptRootFlByOneIptRootFl==true)
                {
                  NmOfOptRootFl=m_nmsOfIptRootFls[iOptRootFls].replace(m_nmsOfIptRootFls[iOptRootFls].rfind(".root"),5,"_ta.root");
                }
              else
                {
                  if(iOptRootFls==0)
                    {
                      NmOfOptRootFl=m_mainNmOfOptFls+".root";
                    }
                  else
                    {
                      ostringstream oss;
                      oss<<iOptRootFls;
                      string strIOptRootFls=oss.str();
                      NmOfOptRootFl=m_mainNmOfOptFls+"_"+strIOptRootFls+".root";
                    }
                }
              fl=new TFile(NmOfOptRootFl.c_str(),"recreate");
              if(!fl)
                {
                  cerr<<"Error: Can't create the output root file \""<<NmOfOptRootFl<<"\"!"<<endl;
                  cerr<<"Infor: Please check it."<<endl;
                  exit(-1);
                }

              tr=chn->CloneTree(0);

              if(m_compAnaOfDcyTrs==true)
                {
                  tr->Branch("iDcyTr", &iDcyTr, "iDcyTr/I");
                  if(m_ccSwitch==true) tr->Branch("iCcDcyTr", &iCcDcyTr, "iCcDcyTr/I");
                }
              if(m_compAnaOfDcyIFSts==true)
                {
                  tr->Branch("iDcyIFSts", &iDcyIFSts, "iDcyIFSts/I");
                  if(m_ccSwitch==true) tr->Branch("iCcDcyIFSts", &iCcDcyIFSts, "iCcDcyIFSts/I");
                }
              if(m_vPid_compDcyBrP.size()>0)
                {
                  createBrs(m_vPid_compDcyBrP.size(), "PDcyBr", "P", "iDcyBr", "iCcDcyBr", "iDcyBrCc", m_vNm_compDcyBrP, iCcPDcyBr, tr, nMax, nPDcyBr, iDcyBrP[0], iCcDcyBrP[0], nCcPDcyBr, iDcyBrCcP[0], nAllPDcyBr);
                }
              if(m_vPid_compCascDcyBrP.size()>0)
                {
                  createBrs(m_vPid_compCascDcyBrP.size(), "PCascDcyBr", "P", "iCascDcyBr", "iCcCascDcyBr", "iCascDcyBrCc", m_vNm_compCascDcyBrP, iCcPCascDcyBr, tr, nMax, nPCascDcyBr, iCascDcyBrP[0], iCcCascDcyBrP[0], nCcPCascDcyBr, iCascDcyBrCcP[0], nAllPCascDcyBr);
                }
              if(m_vPid_compDcyFStP.size()>0)
                {
                  createBrs(m_vPid_compDcyFStP.size(), "PDcyFSt", "P", "iDcyFSt", "iCcDcyFSt", "iDcyFStCc", m_vNm_compDcyFStP, iCcPDcyFSt, tr, nMax, nPDcyFSt, iDcyFStP[0], iCcDcyFStP[0], nCcPDcyFSt, iDcyFStCcP[0], nAllPDcyFSt);
                }
              if(m_vPid_compProdBrP.size()>0)
                {
                  createBrs(m_vPid_compProdBrP.size(), "PProdBr", "P", "iProdBr", "iCcProdBr", "iProdBrCc", m_vNm_compProdBrP, iCcPProdBr, tr, nMax, nPProdBr, iProdBrP[0], iCcProdBrP[0], nCcPProdBr, iProdBrCcP[0], nAllPProdBr);
                }
              if(m_vPid_compMP.size()>0)
                {
                  createBrs(m_vPid_compMP.size(), "PMoth", "P", "PDGMoth", "iCcMoth", "PDGMothCc", m_vNm_compMP, iCcPM, tr, nMax, nPM, MpidP[0], iCcMP[0], nCcPM, MpidCcP[0], nAllPM);
                }
              if(m_vCompIncDcyBr.size()>0)
                {
                  createBrs(m_vCompIncDcyBr.size(), "IncDcyBr", "IncDcyBr", "iDcyBr", "iCcDcyBr", "iDcyBrCc", m_vNm_compIncDcyBr, iCcIncDcyBr, tr, nMax, nIncDcyBr, iDcyBrIncDcyBr[0], iCcDcyBrIncDcyBr[0], nCcIncDcyBr, iDcyBrCcIncDcyBr[0], nAllIncDcyBr);
                }
              if(m_vCompIRADcyBr.size()>0)
                {
                  createBrs(m_vCompIRADcyBr.size(), "IRADcyBr", "IRADcyBr", "iDcyBr", "iCcDcyBr", "iDcyBrCc", m_vNm_compIRADcyBr, iCcIRADcyBr, tr, nMax, nIRADcyBr, iDcyBrIRADcyBr[0], iCcDcyBrIRADcyBr[0], nCcIRADcyBr, iDcyBrCcIRADcyBr[0], nAllIRADcyBr);
                }
              if(m_vSigDcyTr.size()>0)
                {
                  tr->Branch("nameSigDcyTr", &nmSigDcyTr, "nameSigDcyTr/C");
                  tr->Branch("iSigDcyTr", &iSigDcyTr, "iSigDcyTr/I");
                  if(m_sigDcyIFSts_tr==true) tr->Branch("iSigDcyIFSts_tr", &iSigDcyIFSts_tr, "iSigDcyIFSts_tr/I");
                  if(m_ccSwitch==true)
                    {
                      tr->Branch("iCcSigDcyTr", &iCcSigDcyTr, "iCcSigDcyTr/I");
                      if(m_sigDcyIFSts_tr==true) tr->Branch("iCcSigDcyIFSts_tr", &iCcSigDcyIFSts_tr, "iCcSigDcyIFSts_tr/I");
                    }
                }
              if(m_vSigDcyIFSts.size()>0)
                {
                  tr->Branch("nameSigDcyIFSts", &nmSigDcyIFSts, "nameSigDcyIFSts/C");
                  tr->Branch("iSigDcyIFSts", &iSigDcyIFSts, "iSigDcyIFSts/I");
                  if(m_ccSwitch==true)
                    {
                      tr->Branch("iCcSigDcyIFSts", &iCcSigDcyIFSts, "iCcSigDcyIFSts/I");
                    }
                }
              if(m_vPid_sigP.size()>0)
                {
                  createBrs(m_vPid_sigP.size(), "SigP", m_vNm_sigP, iCcSigP, tr, nSigP, nCcSigP, nAllSigP);
                }
              if(m_vSigDcyBr.size()>0)
                {
                  createBrs(m_vSigDcyBr.size(), "SigDcyBr", m_vNm_sigDcyBr, iCcSigDcyBr, tr, nSigDcyBr, nCcSigDcyBr, nAllSigDcyBr);
                }
              if(m_vSigIncDcyBr.size()>0)
                {
                  createBrs(m_vSigIncDcyBr.size(), "SigIncDcyBr", m_vNm_sigIncDcyBr, iCcSigIncDcyBr, tr, nSigIncDcyBr, nCcSigIncDcyBr, nAllSigIncDcyBr);
                }
              if(m_vSigCascDcyBr.size()>0)
                {
                  createBrs(m_vSigCascDcyBr.size(), "SigCascDcyBr", m_vNm_sigCascDcyBr, iCcSigCascDcyBr, tr, nSigCascDcyBr, nCcSigCascDcyBr, nAllSigCascDcyBr);
                }
              if(m_vSigIncCascDcyBr.size()>0)
                {
                  createBrs(m_vSigIncCascDcyBr.size(), "SigIncCascDcyBr", m_vNm_sigIncCascDcyBr, iCcSigIncCascDcyBr, tr, nSigIncCascDcyBr, nCcSigIncCascDcyBr, nAllSigIncCascDcyBr);
                }
              if(m_vSigIRADcyBr.size()>0)
                {
                  createBrs(m_vSigIRADcyBr.size(), "SigIRADcyBr", m_vNm_sigIRADcyBr, iCcSigIRADcyBr, tr, nSigIRADcyBr, nCcSigIRADcyBr, nAllSigIRADcyBr);
                }
              if(m_vSigIncOrIRACascDcyBr.size()>0)
                {
                  createBrs(m_vSigIncOrIRACascDcyBr.size(), "SigIncOrIRACascDcyBr", m_vNm_sigIncOrIRACascDcyBr, iCcSigIncOrIRACascDcyBr, tr, nSigIncOrIRACascDcyBr, nCcSigIncOrIRACascDcyBr, nAllSigIncOrIRACascDcyBr);
                }
            }

          chn->GetEntry(i);

          if(m_avoidOverCounting==true) 
            {
              if(Icandi==0) isTheEvtPrcsd=false;
            }
          
          if(!m_cut.empty())
            { 
              // The following four statements are used to handle the cases where array variables are used in the cut.
              nInsts=trfml->GetNdata(); // This statement is indispensable if multiple instances have to be evaluated by the object of the TTreeFormula class.
              bool passTheCut;
              if(m_cutMethod==true)
                {
                  passTheCut=false; 
                  for(unsigned int j=0;j<nInsts;j++) if(trfml->EvalInstance(j)) {passTheCut=true; break;}
                }
              else
                {
                  passTheCut=true; 
                  for(unsigned int j=0;j<nInsts;j++) if(!(trfml->EvalInstance(j))) {passTheCut=false; break;}
                }
              if(!passTheCut)
                {
                  if((i+1>=nEtrForTiming)&&(((i+1)%nEtrForTiming==0)||((i+1)==nEtrToBePrcsd)))
                    {
                      if((i+1)==nEtrForTiming) cout<<"Number of entries processed\tNumber of seconds elapsed"<<endl<<endl;
                      cout<<setiosflags(ios::right)<<setw(14)<<i+1<<"\t\t\t"<<setiosflags(ios::fixed)<<setprecision(2)<<setw(14)<<(clock()-starttime)/((double) CLOCKS_PER_SEC)<<resetiosflags(ios::adjustfield)<<endl;
                      if((i+1)==nEtrToBePrcsd) cout<<endl;
                    }

                  if(m_oneOptRootFlByOneIptRootFl==true) closeTheOptRootFl2=((i+1)==vNEtr[iOptRootFls]);
                  else closeTheOptRootFl2=((m_nEtrMaxInASngOptRootFl!=ULONG_MAX)&&((i+1)%m_nEtrMaxInASngOptRootFl==0));
                  closeTheOptRootFl3=((i+1)==nEtrToBePrcsd);
                  if(closeTheOptRootFl2||closeTheOptRootFl3)
                    {
                      fl->Write();
                      delete tr; // Pay attention to that replacing the "delete tr" by "tr->Delete()" will result in a problem of "*** Break *** segmentation violation".
                      fl->Close();
                      delete fl;
                      if(m_rmIptTBrs==true) rmIptBrs(NmOfOptRootFl);
                      if(m_useArrayTBrsOpt==false) flatArrayBrs(NmOfOptRootFl);
                      openANewOptRootFl=true;
                      iOptRootFls++;
                    }

                  continue;
                }   
              else
                {
                  nEtrThroughTheCut++;
                }
            }
    
          if(m_avoidOverCounting==true)
            {
              if(isTheEvtPrcsd==true)
                {
                  tr->Fill();

                  if((i+1>=nEtrForTiming)&&(((i+1)%nEtrForTiming==0)||((i+1)==nEtrToBePrcsd)))
                    {
                      if((i+1)==nEtrForTiming) cout<<"Number of entries processed\tNumber of seconds elapsed"<<endl<<endl;
                      cout<<setiosflags(ios::right)<<setw(14)<<i+1<<"\t\t\t"<<setiosflags(ios::fixed)<<setprecision(2)<<setw(14)<<(clock()-starttime)/((double) CLOCKS_PER_SEC)<<resetiosflags(ios::adjustfield)<<endl;
                      if((i+1)==nEtrToBePrcsd) cout<<endl;
                    }

                  if(m_oneOptRootFlByOneIptRootFl==true)
                    {
                      closeTheOptRootFl1=((i+1)==vNEtr[iOptRootFls]);
                      closeTheOptRootFl2=false;
                    }
                  else
                    {
                      closeTheOptRootFl1=((m_nEtrMaxInASngOptRootFl==ULONG_MAX)&&(((unsigned long) tr->GetTotBytes())>nTotBytesMaxOfASngOptRootFl));
                      closeTheOptRootFl2=((m_nEtrMaxInASngOptRootFl!=ULONG_MAX)&&((i+1)%m_nEtrMaxInASngOptRootFl==0));
                    }
                  closeTheOptRootFl3=((i+1)==nEtrToBePrcsd);
                  if(closeTheOptRootFl1||closeTheOptRootFl2||closeTheOptRootFl3)
                    {
                      fl->Write();
                      delete tr; // Pay attention to that replacing the "delete tr" by "tr->Delete()" will result in a problem of "*** Break *** segmentation violation".
                      fl->Close();
                      delete fl;
                      if(m_rmIptTBrs==true) rmIptBrs(NmOfOptRootFl);
                      if(m_useArrayTBrsOpt==false) flatArrayBrs(NmOfOptRootFl);
                      openANewOptRootFl=true;
                      iOptRootFls++;
                    }

                  continue;
                }
            }
             
          if(m_strgTpOfRawIptTopoDat=="MSD") reviseIptQts(Npsd,Pidd,Midxd,Nps,Pid,Midx);

          vPid.clear();
          vMidx.clear();
          if(m_strgTpOfRawIptTopoDat=="AOI"||m_strgTpOfRawIptTopoDat=="MSI"||m_strgTpOfRawIptTopoDat=="MSD")
            {
              for(int j=0;j<Nps;j++)
                {
                  vPid.push_back(Pid[j]);
                  vMidx.push_back(Midx[j]);
                  //cout<<j<<"\t"<<Pid[j]<<"\t"<<Midx[j]<<endl; 
                }
            }
          else
            {
              for(unsigned int j=0;j<pVPid->size();j++)
                {
                  vPid.push_back(pVPid->at(j));
                  vMidx.push_back(pVMidx->at(j));
                  //cout<<j<<"\t"<<Pid[j]<<"\t"<<Midx[j]<<endl;
                }
            }
          //cout<<endl;
          /*for(unsigned int j=0;j<vPid.size();j++)
            {
              cout<<j<<"\t"<<vPid[j]<<"\t"<<vMidx[j]<<endl;
            }
          cout<<endl;*/
          sortPs(vPid,vMidx);
          dcyTr.clear();
          vIdxOfHead.clear();
          vMidxOfHead.clear();
          getDcyTr(vPid,vMidx,dcyTr,&vIdxOfHead,&vMidxOfHead);
          dcyIFSts.clear();
          getDcyIFSts(vPid,vMidx,dcyIFSts);
          if(m_ccSwitch==true)
            { 
              vCcPid.clear(); 
              vCcMidx.clear(); 
              for(unsigned int j=0;j<vPid.size();j++) vCcPid.push_back(getCcPid(vPid[j]));
              vCcMidx=vMidx;
              sortPs(vCcPid,vCcMidx);
              ccDcyTr.clear();
              vCcIdxOfHead.clear();
              vCcMidxOfHead.clear();
              getDcyTr(vCcPid,vCcMidx,ccDcyTr,&vCcIdxOfHead,&vCcMidxOfHead);
              ccDcyIFSts.clear();
              getDcyIFSts(vCcPid,vCcMidx,ccDcyIFSts);
            }

         if(m_compAnaOfDcyTrs==true)
           {
             getStrFromVli(dcyTr,strDcyTr);
             if(m_ccSwitch==true) getStrFromVli(ccDcyTr,strCcDcyTr);

             iDcyTr=-1; // If the variable is still equal to -1 after the following loop, then the decay tree is a new decay tree.
             // To get a higher search efficiency, the following code segment is replaced by the next one, which is featured by the use of "unordered_map".
             /*for(unsigned int j=0;j<m_vDcyTr.size();j++)
               {
                 if(dcyTr==m_vDcyTr[j])
                   {
                     iDcyTr=j;
                     iDcyIFSts=m_iDcyTrIDcyIFStsMap[iDcyTr];
                     if(m_ccSwitch==true)
                       {
                         iCcDcyTr=m_vICcDcyTr[j];
                         iCcDcyIFSts=m_iDcyTrICcDcyIFStsMap[iDcyTr];
                       }
                     break;
                   }
                 else if(m_ccSwitch==true&&m_vICcDcyTr[j]!=0&&dcyTr==m_vCcDcyTr[j])
                   {
                     iDcyTr=j;
                     iDcyIFSts=m_iDcyTrIDcyIFStsMap[iDcyTr];
                     iCcDcyTr=-1;
                     iCcDcyIFSts=-1*m_iDcyTrICcDcyIFStsMap[iDcyTr];
                     break;
                   }
               }*/
             if(m_uomDcyTr.find(strDcyTr)!=m_uomDcyTr.end())
               { 
                 iDcyTr=m_uomDcyTr[strDcyTr];
                 iDcyIFSts=m_iDcyTrIDcyIFStsMap[iDcyTr];
                 if(m_ccSwitch==true)
                   { 
                     iCcDcyTr=m_vICcDcyTr[iDcyTr];
                     iCcDcyIFSts=m_iDcyTrICcDcyIFStsMap[iDcyTr];
                   }
               }
             else if(m_ccSwitch==true&&m_uomCcDcyTr.find(strDcyTr)!=m_uomCcDcyTr.end())
               { 
                 iDcyTr=m_uomCcDcyTr[strDcyTr];
                 iDcyIFSts=m_iDcyTrIDcyIFStsMap[iDcyTr];
                 iCcDcyTr=-1;
                 iCcDcyIFSts=-1*m_iDcyTrICcDcyIFStsMap[iDcyTr];
               }
             if(iDcyTr==-1)
               {
                 iDcyTr=m_vDcyTr.size();
                 m_vDcyTr.push_back(dcyTr);
                 m_vIDcyTr.push_back(iDcyTr);
                 m_vNDcyTr.push_back(1);
                 m_uomDcyTr[strDcyTr]=iDcyTr;
                 if(m_ccSwitch==true)
                   {
                     if(ccDcyTr==dcyTr) iCcDcyTr=0;
                     else iCcDcyTr=1;
                     m_vCcDcyTr.push_back(ccDcyTr);
                     m_vICcDcyTr.push_back(iCcDcyTr);
                     m_vNCcDcyTr.push_back(0);
                     m_uomCcDcyTr[strCcDcyTr]=iDcyTr;
                   }

                 if(m_compAnaOfDcyIFSts==true)
                   {
                     getStrFromLi(dcyIFSts,strDcyIFSts);
                     if(m_ccSwitch==true) getStrFromLi(ccDcyIFSts,strCcDcyIFSts);

                     iDcyIFSts=-1; // If the variable is still equal to -1 after the following loop, then the decay initial-final states is a new decay initial-final states.
                     // To get a higher search efficiency, the following code segment is replaced by the next one, which is featured by the use of "unordered_map".
                     /*for(unsigned int j=0;j<m_vDcyIFSts.size();j++)
                       {
                         if(dcyIFSts==m_vDcyIFSts[j])
                           {
                             iDcyIFSts=j;
                             if(m_ccSwitch==true) iCcDcyIFSts=m_vICcDcyIFSts[j];
                             break;
                           }
                         else if(m_ccSwitch==true&&m_vICcDcyIFSts[j]!=0&&dcyIFSts==m_vCcDcyIFSts[j])
                           {
                             iDcyIFSts=j;
                             iCcDcyIFSts=-1;
                             break;
                           }              
                       }*/
                     if(m_uomDcyIFSts.find(strDcyIFSts)!=m_uomDcyIFSts.end())
                       {
                         iDcyIFSts=m_uomDcyIFSts[strDcyIFSts];
                         if(m_ccSwitch==true) iCcDcyIFSts=m_vICcDcyIFSts[iDcyIFSts];
                       }
                     else if(m_ccSwitch==true&&m_uomCcDcyIFSts.find(strDcyIFSts)!=m_uomCcDcyIFSts.end())
                       {
                         iDcyIFSts=m_uomCcDcyIFSts[strDcyIFSts];
                         iCcDcyIFSts=-1;
                       }
                     if(iDcyIFSts==-1)
                       {
                         iDcyIFSts=m_vDcyIFSts.size();
                         m_vDcyIFSts.push_back(dcyIFSts);
                         m_vIDcyIFSts.push_back(iDcyIFSts);
                         m_vNDcyIFSts.push_back(1);
                         m_uomDcyIFSts[strDcyIFSts]=iDcyIFSts;
                         if(m_ccSwitch==true)
                           {
                             if(ccDcyIFSts==dcyIFSts) iCcDcyIFSts=0;
                             else iCcDcyIFSts=1;
                             m_vCcDcyIFSts.push_back(ccDcyIFSts);
                             m_vICcDcyIFSts.push_back(iCcDcyIFSts);
                             m_vNCcDcyIFSts.push_back(0);
                             m_uomCcDcyIFSts[strCcDcyIFSts]=iDcyIFSts;
                           } 
                       }
                     else
                       {
                         if(m_ccSwitch==true)
                           {
                             if(iCcDcyIFSts==-1) m_vNCcDcyIFSts[iDcyIFSts]++;
                             else m_vNDcyIFSts[iDcyIFSts]++;
                           }
                         else m_vNDcyIFSts[iDcyIFSts]++;             
                       }
                     m_iDcyTrIDcyIFStsMap[iDcyTr]=iDcyIFSts;
                     if(m_ccSwitch==true) m_iDcyTrICcDcyIFStsMap[iDcyTr]=iCcDcyIFSts;
                   }
                 else
                   {
                     m_vDcyIFSts.push_back(dcyIFSts);
                   }
               }
             else
               {
                 if(m_ccSwitch==true)
                   {
                     if(iCcDcyTr==-1) m_vNCcDcyTr[iDcyTr]++;
                     else m_vNDcyTr[iDcyTr]++;
                     if(m_compAnaOfDcyIFSts==true)
                       {
                         if(iCcDcyIFSts==-1) m_vNCcDcyIFSts[iDcyIFSts]++;
                         else m_vNDcyIFSts[iDcyIFSts]++;
                       }
                   }
                 else
                   {
                     m_vNDcyTr[iDcyTr]++;
                     if(m_compAnaOfDcyIFSts==true) m_vNDcyIFSts[iDcyIFSts]++;
                   }
               }
            }
          else if(m_compAnaOfDcyIFSts==true)
            {
              getStrFromLi(dcyIFSts,strDcyIFSts);
              if(m_ccSwitch==true) getStrFromLi(ccDcyIFSts,strCcDcyIFSts);

              iDcyIFSts=-1; // If the variable is still equal to -1 after the following loop, then the decay initial-final states is a new decay initial-final states.
              // To get a higher search efficiency, the following code segment is replaced by the next one, which is featured by the use of "unordered_map".
              /*for(unsigned int j=0;j<m_vDcyIFSts.size();j++)
                {
                  if(dcyIFSts==m_vDcyIFSts[j])
                    {
                      iDcyIFSts=j;
                      if(m_ccSwitch==true) iCcDcyIFSts=m_vICcDcyIFSts[j];
                      break;
                    }
                  else if(m_ccSwitch==true&&m_vICcDcyIFSts[j]!=0&&dcyIFSts==m_vCcDcyIFSts[j])
                    {
                      iDcyIFSts=j;
                      iCcDcyIFSts=-1;
                      break;
                    }              
                }*/
              if(m_uomDcyIFSts.find(strDcyIFSts)!=m_uomDcyIFSts.end())
                {
                  iDcyIFSts=m_uomDcyIFSts[strDcyIFSts];
                  if(m_ccSwitch==true) iCcDcyIFSts=m_vICcDcyIFSts[iDcyIFSts];
                }
              else if(m_ccSwitch==true&&m_uomCcDcyIFSts.find(strDcyIFSts)!=m_uomCcDcyIFSts.end())
                {
                  iDcyIFSts=m_uomCcDcyIFSts[strDcyIFSts];
                  iCcDcyIFSts=-1;
                }
              if(iDcyIFSts==-1)
                {
                  iDcyIFSts=m_vDcyIFSts.size();
                  m_vDcyIFSts.push_back(dcyIFSts);
                  m_vIDcyIFSts.push_back(iDcyIFSts);
                  m_vNDcyIFSts.push_back(1);
                  m_uomDcyIFSts[strDcyIFSts]=iDcyIFSts;
                  if(m_ccSwitch==true)
                    {
                      if(ccDcyIFSts==dcyIFSts) iCcDcyIFSts=0;
                      else iCcDcyIFSts=1;
                      m_vCcDcyIFSts.push_back(ccDcyIFSts);
                      m_vICcDcyIFSts.push_back(iCcDcyIFSts);
                      m_vNCcDcyIFSts.push_back(0);
                      m_uomCcDcyIFSts[strCcDcyIFSts]=iDcyIFSts;
                    } 
                }
              else
                {
                  if(m_ccSwitch==true)
                    {
                      if(iCcDcyIFSts==-1) m_vNCcDcyIFSts[iDcyIFSts]++;
                      else m_vNDcyIFSts[iDcyIFSts]++;
                    }
                  else m_vNDcyIFSts[iDcyIFSts]++;             
                }
            }

          if(m_vPid_compDcyBrP.size()>0)
            {
              // Pay attention to that dcyBrCcP equate to ccDcyBrP for self-charge-conjugate particles.
              list<int> dcyBrP, dcyBrCcP;
              dcyBrP.clear(); dcyBrCcP.clear();
              for(unsigned int j=0;j<m_vPid_compDcyBrP.size();j++)
                {
                  nPDcyBr[j]=0;
                  if(m_ccSwitch==true&&m_vICcCompDcyBrP[j]!=0)
                    {
                      nCcPDcyBr[j]=0;
                      nAllPDcyBr[j]=0;
                    }
                }     
              for(unsigned int j=0;j<dcyTr.size();j++)
                {
                  list<int>::iterator liit=dcyTr[j].begin();
                  for(unsigned int k=0;k<m_vPid_compDcyBrP.size();k++)
                    {
                      if((*liit)==m_vPid_compDcyBrP[k])
                        {
                          dcyBrP=dcyTr[j];
                          int _iDcyBrP=-1; // If the variable is still equal to -1 after the following loop, then the decay branch of the particle is a new decay branch of the particle.
                          int _iCcDcyBrP=-9999;
                          for(unsigned int l=0;l<m_vVDcyBrP[k].size();l++)
                            {
                              if(dcyBrP==m_vVDcyBrP[k][l])
                                {
                                  _iDcyBrP=l;
                                  if(m_ccSwitch==true&&m_vICcCompDcyBrP[k]==0) _iCcDcyBrP=m_vVIDcyBrCcP[k][l];
                                  break;
                                }
                              else if(m_ccSwitch==true&&m_vICcCompDcyBrP[k]==0&&m_vVIDcyBrCcP[k][l]!=0&&dcyBrP==m_vVDcyBrCcP[k][l])
                                {
                                  _iDcyBrP=l;
                                  _iCcDcyBrP=-1;
                                  break;
                                }
                            }
                          if(_iDcyBrP==-1)
                            {
                              _iDcyBrP=m_vVDcyBrP[k].size();
                              iDcyBrP[k][(unsigned int) (nPDcyBr[k])]=_iDcyBrP;
                              m_vVDcyBrP[k].push_back(dcyBrP);
                              m_vVIDcyBrP[k].push_back(_iDcyBrP);
                              m_vVNDcyBrP[k].push_back(1);
                              if(m_ccSwitch==true)
                                {
                                  dcyBrCcP.clear();
                                  list<int>::iterator liit=dcyBrP.begin();
                                  for(liit++;liit!=dcyBrP.end();liit++) dcyBrCcP.push_back(getCcPid((*liit)));
                                  sortByPidAndPchrg(dcyBrCcP); 
                                  liit=dcyBrP.begin();
                                  if(m_vICcCompDcyBrP[k]!=0)
                                    {
                                      dcyBrCcP.push_front(getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                                      m_vVDcyBrCcP[k].push_back(dcyBrCcP);
                                      m_vVIDcyBrCcP[k].push_back(_iDcyBrP);
                                      m_vVNDcyBrCcP[k].push_back(0);
                                    }    
                                  else
                                    {
                                      dcyBrCcP.push_front((*liit)); // Here, (*liit) is equal to getCcPid((*liit)).
                                      if(dcyBrCcP==dcyBrP) _iCcDcyBrP=0;
                                      else _iCcDcyBrP=1;
                                      iCcDcyBrP[k][(unsigned int) (nPDcyBr[k])]=_iCcDcyBrP;
                                      m_vVDcyBrCcP[k].push_back(dcyBrCcP);
                                      m_vVIDcyBrCcP[k].push_back(_iCcDcyBrP);
                                      m_vVNDcyBrCcP[k].push_back(0);
                                    }
                                }
                            }
                          else
                            {
                              iDcyBrP[k][(unsigned int) (nPDcyBr[k])]=_iDcyBrP;
                              if(m_ccSwitch==true&&m_vICcCompDcyBrP[k]==0)
                                {
                                  iCcDcyBrP[k][(unsigned int) (nPDcyBr[k])]=_iCcDcyBrP;
                                  if(_iCcDcyBrP==-1) m_vVNDcyBrCcP[k][_iDcyBrP]++;
                                  else m_vVNDcyBrP[k][_iDcyBrP]++;
                                }
                              else
                                {
                                  m_vVNDcyBrP[k][_iDcyBrP]++;
                                }
                            }
                          nPDcyBr[k]++;
                          break;
                        } // Here, "&&m_vICcCompDcyBrP[k]!=0" in the following condition can be removed.
                      else if(m_ccSwitch==true&&m_vICcCompDcyBrP[k]!=0&&(*liit)==m_vPid_ccCompDcyBrP[k])
                        {
                          dcyBrCcP=dcyTr[j];
                          int _iDcyBrP=-1; // If the variable is still equal to -1 after the following loop, then the decay branch of the particle is a new decay branch of the particle.
                          for(unsigned int l=0;l<m_vVDcyBrCcP[k].size();l++)
                            {
                              if(dcyBrCcP==m_vVDcyBrCcP[k][l])
                                {
                                  _iDcyBrP=l;
                                  break;
                                }
                            }
                          if(_iDcyBrP==-1)
                            {
                              _iDcyBrP=m_vVDcyBrP[k].size();
                              iDcyBrCcP[k][(unsigned int) (nCcPDcyBr[k])]=_iDcyBrP;
                              m_vVDcyBrCcP[k].push_back(dcyBrCcP);
                              m_vVIDcyBrCcP[k].push_back(_iDcyBrP);
                              m_vVNDcyBrCcP[k].push_back(1);

                              dcyBrP.clear();
                              list<int>::iterator liit=dcyBrCcP.begin();
                              for(liit++;liit!=dcyBrCcP.end();liit++) dcyBrP.push_back(getCcPid((*liit)));
                              sortByPidAndPchrg(dcyBrP);
                              liit=dcyBrCcP.begin();
                              dcyBrP.push_front(getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                              m_vVDcyBrP[k].push_back(dcyBrP);
                              m_vVIDcyBrP[k].push_back(_iDcyBrP);
                              m_vVNDcyBrP[k].push_back(0);
                            }
                          else
                            {
                              iDcyBrCcP[k][(unsigned int) (nCcPDcyBr[k])]=_iDcyBrP;
                              m_vVNDcyBrCcP[k][_iDcyBrP]++;
                            }
                          nCcPDcyBr[k]++;
                          break;                                    
                        }
                    }
                }
              if(m_ccSwitch==true) for(unsigned int j=0;j<m_vPid_compDcyBrP.size();j++) if(m_vICcCompDcyBrP[j]!=0) nAllPDcyBr[j]=nPDcyBr[j]+nCcPDcyBr[j];
            }

          if(m_vPid_compCascDcyBrP.size()>0)
            {
              // Pay attention to that cascDcyBrCcP equate to ccCascDcyBrP for self-charge-conjugate intermediate-resonance-allowed decays.
              vector< list<int> > cascDcyBrP, cascDcyBrCcP;
              cascDcyBrP.clear(); cascDcyBrCcP.clear();
              for(unsigned int j=0;j<m_vPid_compCascDcyBrP.size();j++)
                {
                  nPCascDcyBr[j]=0;
                  if(m_ccSwitch==true&&m_vICcCompCascDcyBrP[j]!=0)
                    {
                      nCcPCascDcyBr[j]=0;
                      nAllPCascDcyBr[j]=0;
                    }
                }     
              vector< vector< list<int> > > vCascDcyBrP,vCascDcyBrCcP;
              for(unsigned int j=0;j<m_vPid_compCascDcyBrP.size();j++)
                {
                  vCascDcyBrP.clear();
                  getVCascDcyBrP(vCascDcyBrP, dcyTr, vIdxOfHead, vMidxOfHead, m_vPid_compCascDcyBrP[j], m_vHCascDcyBrMax[j]);
                  if(vCascDcyBrP.size()>0)
                    {
                      for(unsigned int k=0;k<vCascDcyBrP.size();k++)
                        {
                          cascDcyBrP=vCascDcyBrP[k];
                          getStrFromVli(cascDcyBrP,strCascDcyBrP);
                          int _iCascDcyBrP=-1; // If the variable is still equal to -1 after the following loop, then the cascade decay branch of the particle is a new cascade decay branch of the particle.
                          int _iCcCascDcyBrP=-9999;
                          // To get a higher search efficiency, the following code segment is replaced by the next one, which is featured by the use of "unordered_map".
                          /*for(unsigned int l=0;l<m_vVCascDcyBrP[j].size();l++)
                            {
                              if(cascDcyBrP==m_vVCascDcyBrP[j][l])
                                {
                                  _iCascDcyBrP=l;
                                  if(m_ccSwitch==true&&m_vICcCompCascDcyBrP[j]==0) _iCcCascDcyBrP=m_vVICascDcyBrCcP[j][l];
                                  break;
                                }
                              else if(m_ccSwitch==true&&m_vICcCompCascDcyBrP[j]==0&&m_vVICascDcyBrCcP[j][l]!=0&&cascDcyBrP==m_vVCascDcyBrCcP[j][l])
                                {
                                  _iCascDcyBrP=l;
                                  _iCcCascDcyBrP=-1;
                                  break;
                                }
                            }*/
                          if(m_vUomCascDcyBrP[j].find(strCascDcyBrP)!=m_vUomCascDcyBrP[j].end())
                            {
                              _iCascDcyBrP=m_vUomCascDcyBrP[j][strCascDcyBrP];
                              if(m_ccSwitch==true&&m_vICcCompCascDcyBrP[j]==0) _iCcCascDcyBrP=m_vVICascDcyBrCcP[j][_iCascDcyBrP];
                            }
                          else if(m_ccSwitch==true&&m_vICcCompCascDcyBrP[j]==0&&m_vUomCascDcyBrCcP[j].find(strCascDcyBrP)!=m_vUomCascDcyBrCcP[j].end())
                            {
                              _iCascDcyBrP=m_vUomCascDcyBrCcP[j][strCascDcyBrP];
                              _iCcCascDcyBrP=-1;
                            }
                          if(_iCascDcyBrP==-1)
                            {
                              _iCascDcyBrP=m_vVCascDcyBrP[j].size();
                              iCascDcyBrP[j][(unsigned int) (nPCascDcyBr[j])]=_iCascDcyBrP;
                              m_vVCascDcyBrP[j].push_back(cascDcyBrP);
                              m_vVICascDcyBrP[j].push_back(_iCascDcyBrP);
                              m_vVNCascDcyBrP[j].push_back(1);
                              m_vUomCascDcyBrP[j][strCascDcyBrP]=_iCascDcyBrP;
                              if(m_ccSwitch==true)
                                {
                                  cascDcyBrCcP.clear();
                                  list<int> dcyBrCcP;
                                  list<int>::iterator liit;
                                  for(unsigned int l=0;l<cascDcyBrP.size();l++)
                                    {
                                      dcyBrCcP.clear();
                                      liit=cascDcyBrP[l].begin();
                                      for(liit++;liit!=cascDcyBrP[l].end();liit++) dcyBrCcP.push_back(getCcPid((*liit)));
                                      sortByPidAndPchrg(dcyBrCcP);
                                      liit=cascDcyBrP[l].begin();
                                      dcyBrCcP.push_front(getCcPid((*liit)));
                                      cascDcyBrCcP.push_back(dcyBrCcP);
                                    }
                                  getStrFromVli(cascDcyBrCcP,strCascDcyBrCcP);
                                  if(m_vICcCompCascDcyBrP[j]!=0)
                                    {
                                      m_vVCascDcyBrCcP[j].push_back(cascDcyBrCcP);
                                      m_vVICascDcyBrCcP[j].push_back(_iCascDcyBrP);
                                      m_vVNCascDcyBrCcP[j].push_back(0);
                                      m_vUomCascDcyBrCcP[j][strCascDcyBrCcP]=_iCascDcyBrP;
                                    }
                                  else
                                    {
                                      if(cascDcyBrCcP==cascDcyBrP) _iCcCascDcyBrP=0;
                                      else _iCcCascDcyBrP=1;
                                      iCcCascDcyBrP[j][(unsigned int) (nPCascDcyBr[j])]=_iCcCascDcyBrP;
                                      m_vVCascDcyBrCcP[j].push_back(cascDcyBrCcP);
                                      m_vVICascDcyBrCcP[j].push_back(_iCcCascDcyBrP);
                                      m_vVNCascDcyBrCcP[j].push_back(0);
                                      m_vUomCascDcyBrCcP[j][strCascDcyBrCcP]=_iCascDcyBrP;
                                    }
                                }
                            }
                          else
                            {
                              iCascDcyBrP[j][(unsigned int) (nPCascDcyBr[j])]=_iCascDcyBrP;
                              if(m_ccSwitch==true&&m_vICcCompCascDcyBrP[j]==0)
                                {
                                  iCcCascDcyBrP[j][(unsigned int) (nPCascDcyBr[j])]=_iCcCascDcyBrP;
                                  if(_iCcCascDcyBrP==-1) m_vVNCascDcyBrCcP[j][_iCascDcyBrP]++;
                                  else m_vVNCascDcyBrP[j][_iCascDcyBrP]++;
                                }
                              else
                                {
                                  m_vVNCascDcyBrP[j][_iCascDcyBrP]++;
                                }
                            }
                          nPCascDcyBr[j]++;
                        }
                    }
                  vCascDcyBrCcP.clear();
                  // Here, "&&m_vICcCompCascDcyBrP[k]!=0" in the following condition can not be removed. Besides, "if" here can not be replaced by "else if".
                  if(m_ccSwitch==true&&m_vICcCompCascDcyBrP[j]!=0)
                    {
                      getVCascDcyBrP(vCascDcyBrCcP, dcyTr, vIdxOfHead, vMidxOfHead, m_vPid_ccCompCascDcyBrP[j], m_vHCascDcyBrMax[j]);
                      for(unsigned int k=0;k<vCascDcyBrCcP.size();k++)
                        {
                          cascDcyBrCcP=vCascDcyBrCcP[k];
                          getStrFromVli(cascDcyBrCcP,strCascDcyBrCcP);
                          int _iCascDcyBrP=-1; // If the variable is still equal to -1 after the following loop, then the cascade decay branch of the charge conjugate particle is a new cascade decay branch of the charge conjugate particle.
                          // To get a higher search efficiency, the following code segment is replaced by the next one, which is featured by the use of "unordered_map".
                          /*for(unsigned int l=0;l<m_vVCascDcyBrCcP[j].size();l++)
                            {
                              if(cascDcyBrCcP==m_vVCascDcyBrCcP[j][l])
                                {
                                  _iCascDcyBrP=l;
                                  break;
                                }
                            }*/
                          if(m_vUomCascDcyBrCcP[j].find(strCascDcyBrCcP)!=m_vUomCascDcyBrCcP[j].end()) _iCascDcyBrP=m_vUomCascDcyBrCcP[j][strCascDcyBrCcP];
                          if(_iCascDcyBrP==-1)
                            {
                              _iCascDcyBrP=m_vVCascDcyBrP[j].size();
                              iCascDcyBrCcP[j][(unsigned int) (nCcPCascDcyBr[j])]=_iCascDcyBrP;
                              m_vVCascDcyBrCcP[j].push_back(cascDcyBrCcP);
                              m_vVICascDcyBrCcP[j].push_back(_iCascDcyBrP);
                              m_vVNCascDcyBrCcP[j].push_back(1);
                              m_vUomCascDcyBrCcP[j][strCascDcyBrCcP]=_iCascDcyBrP;

                              cascDcyBrP.clear();
                              list<int> dcyBrP;
                              list<int>::iterator liit;
                              for(unsigned int l=0;l<cascDcyBrCcP.size();l++)
                                {
                                  dcyBrP.clear();
                                  liit=cascDcyBrCcP[l].begin();
                                  for(liit++;liit!=cascDcyBrCcP[l].end();liit++) dcyBrP.push_back(getCcPid((*liit)));
                                  sortByPidAndPchrg(dcyBrP);
                                  liit=cascDcyBrCcP[l].begin();
                                  dcyBrP.push_front(getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                                  cascDcyBrP.push_back(dcyBrP);
                                }
                              getStrFromVli(cascDcyBrP,strCascDcyBrP);
                              m_vVCascDcyBrP[j].push_back(cascDcyBrP);
                              m_vVICascDcyBrP[j].push_back(_iCascDcyBrP);
                              m_vVNCascDcyBrP[j].push_back(0);
                              m_vUomCascDcyBrP[j][strCascDcyBrP]=_iCascDcyBrP;
                            }
                          else
                            {
                              iCascDcyBrCcP[j][(unsigned int) (nCcPCascDcyBr[j])]=_iCascDcyBrP;
                              m_vVNCascDcyBrCcP[j][_iCascDcyBrP]++;
                            }
                          nCcPCascDcyBr[j]++;
                        }
                      if(m_ccSwitch==true&&m_vICcCompCascDcyBrP[j]!=0) nAllPCascDcyBr[j]=nPCascDcyBr[j]+nCcPCascDcyBr[j];
                    }             
                }               
            }

          if(m_vPid_compDcyFStP.size()>0)
            {
              // Pay attention to that dcyFStCcP equate to ccDcyFStP for self-charge-conjugate intermediate-resonance-allowed decays.
              list<int> dcyFStP, dcyFStCcP;
              dcyFStP.clear(); dcyFStCcP.clear();
              for(unsigned int j=0;j<m_vPid_compDcyFStP.size();j++)
                {
                  nPDcyFSt[j]=0;
                  if(m_ccSwitch==true&&m_vICcCompDcyFStP[j]!=0)
                    {
                      nCcPDcyFSt[j]=0;
                      nAllPDcyFSt[j]=0;
                    }
                }     
              vector< list<int> > vDcyFStP,vDcyFStCcP;
              for(unsigned int j=0;j<m_vPid_compDcyFStP.size();j++)
                {
                  vDcyFStP.clear();
                  getVDcyFStP(vDcyFStP, vPid, vMidx, m_vPid_compDcyFStP[j], m_vNDcyFStP[j]);
                  if(vDcyFStP.size()>0)
                    {
                      for(unsigned int k=0;k<vDcyFStP.size();k++)
                        {
                          dcyFStP=vDcyFStP[k];
                          getStrFromLi(dcyFStP,strDcyFStP);
                          int _iDcyFStP=-1; // If the variable is still equal to -1 after the following loop, then the decay branch of the intermediate-resonance-allowed decay branch is a new exclusive decay branch of the intermediate-resonance-allowed decay branch.
                          int _iCcDcyFStP=-9999;
                          // To get a higher search efficiency, the following code segment is replaced by the next one, which is featured by the use of "unordered_map".
                          /*for(unsigned int l=0;l<m_vVDcyFStP[j].size();l++)
                            {
                              if(dcyFStP==m_vVDcyFStP[j][l])
                                {
                                  _iDcyFStP=l;
                                  if(m_ccSwitch==true&&m_vICcCompDcyFStP[j]==0) _iCcDcyFStP=m_vVIDcyFStCcP[j][l];
                                  break;
                                }
                              else if(m_ccSwitch==true&&m_vICcCompDcyFStP[j]==0&&m_vVIDcyFStCcP[j][l]!=0&&dcyFStP==m_vVDcyFStCcP[j][l])
                                {
                                  _iDcyFStP=l;
                                  _iCcDcyFStP=-1;
                                  break;
                                }
                            }*/
                          if(m_vUomDcyFStP[j].find(strDcyFStP)!=m_vUomDcyFStP[j].end())
                            {
                              _iDcyFStP=m_vUomDcyFStP[j][strDcyFStP];
                              if(m_ccSwitch==true&&m_vICcCompDcyFStP[j]==0) _iCcDcyFStP=m_vVIDcyFStCcP[j][_iDcyFStP];
                            }
                          else if(m_ccSwitch==true&&m_vICcCompDcyFStP[j]==0&&m_vUomDcyFStCcP[j].find(strDcyFStP)!=m_vUomDcyFStCcP[j].end())
                            {
                              _iDcyFStP=m_vUomDcyFStCcP[j][strDcyFStP];
                              _iCcDcyFStP=-1;
                            }
                          if(_iDcyFStP==-1)
                            {
                              _iDcyFStP=m_vVDcyFStP[j].size();
                              iDcyFStP[j][(unsigned int) (nPDcyFSt[j])]=_iDcyFStP;
                              m_vVDcyFStP[j].push_back(dcyFStP);
                              m_vVIDcyFStP[j].push_back(_iDcyFStP);
                              m_vVNDcyFStP[j].push_back(1);
                              m_vUomDcyFStP[j][strDcyFStP]=_iDcyFStP;
                              if(m_ccSwitch==true)
                                {
                                  dcyFStCcP.clear();
                                  list<int>::iterator liit=dcyFStP.begin();
                                  for(liit++;liit!=dcyFStP.end();liit++) dcyFStCcP.push_back(getCcPid((*liit)));
                                  sortByPidAndPchrg(dcyFStCcP);
                                  liit=dcyFStP.begin();
                                  dcyFStCcP.push_front(getCcPid((*liit)));
                                  getStrFromLi(dcyFStCcP,strDcyFStCcP);
                                  if(m_vICcCompDcyFStP[j]!=0)
                                    {
                                      m_vVDcyFStCcP[j].push_back(dcyFStCcP);
                                      m_vVIDcyFStCcP[j].push_back(_iDcyFStP);
                                      m_vVNDcyFStCcP[j].push_back(0);
                                      m_vUomDcyFStCcP[j][strDcyFStCcP]=_iDcyFStP;
                                    }    
                                  else
                                    {
                                      if(dcyFStCcP==dcyFStP) _iCcDcyFStP=0;
                                      else _iCcDcyFStP=1;
                                      iCcDcyFStP[j][(unsigned int) (nPDcyFSt[j])]=_iCcDcyFStP;
                                      m_vVDcyFStCcP[j].push_back(dcyFStCcP);
                                      m_vVIDcyFStCcP[j].push_back(_iCcDcyFStP);
                                      m_vVNDcyFStCcP[j].push_back(0);
                                      m_vUomDcyFStCcP[j][strDcyFStCcP]=_iDcyFStP;
                                    }
                                }
                            }
                          else
                            {
                              iDcyFStP[j][(unsigned int) (nPDcyFSt[j])]=_iDcyFStP;
                              if(m_ccSwitch==true&&m_vICcCompDcyFStP[j]==0)
                                {
                                  iCcDcyFStP[j][(unsigned int) (nPDcyFSt[j])]=_iCcDcyFStP;
                                  if(_iCcDcyFStP==-1) m_vVNDcyFStCcP[j][_iDcyFStP]++;
                                  else m_vVNDcyFStP[j][_iDcyFStP]++;
                                }
                              else
                                {
                                  m_vVNDcyFStP[j][_iDcyFStP]++;
                                }
                            }
                          nPDcyFSt[j]++;
                        }
                    }
                  vDcyFStCcP.clear();
                  // Here, "&&m_vICcCompDcyFStP[k]!=0" in the following condition can not be removed. Besides, "if" here can not be replaced by "else if".
                  if(m_ccSwitch==true&&m_vICcCompDcyFStP[j]!=0)
                    {
                      getVDcyFStP(vDcyFStCcP, vPid, vMidx, m_vPid_ccCompDcyFStP[j], m_vNDcyFStP[j]);
                      for(unsigned int k=0;k<vDcyFStCcP.size();k++)
                        {
                          dcyFStCcP=vDcyFStCcP[k];
                          getStrFromLi(dcyFStCcP,strDcyFStCcP);
                          int _iDcyFStP=-1; // If the variable is still equal to -1 after the following loop, then the decay branch of the intermediate-resonance-allowed decay branch is a new exclusive decay branch of the intermediate-resonance-allowed decay branch.
                          // To get a higher search efficiency, the following code segment is replaced by the next one, which is featured by the use of "unordered_map".
                          /*for(unsigned int l=0;l<m_vVDcyFStCcP[j].size();l++)
                            {
                              if(dcyFStCcP==m_vVDcyFStCcP[j][l])
                                {
                                  _iDcyFStP=l;
                                  break;
                                }
                            }*/
                          if(m_vUomDcyFStCcP[j].find(strDcyFStCcP)!=m_vUomDcyFStCcP[j].end()) _iDcyFStP=m_vUomDcyFStCcP[j][strDcyFStCcP];
                          if(_iDcyFStP==-1)
                            {
                              _iDcyFStP=m_vVDcyFStP[j].size();
                              iDcyFStCcP[j][(unsigned int) (nCcPDcyFSt[j])]=_iDcyFStP;
                              m_vVDcyFStCcP[j].push_back(dcyFStCcP);
                              m_vVIDcyFStCcP[j].push_back(_iDcyFStP);
                              m_vVNDcyFStCcP[j].push_back(1);
                              m_vUomDcyFStCcP[j][strDcyFStCcP]=_iDcyFStP;

                              dcyFStP.clear();
                              list<int>::iterator liit=dcyFStCcP.begin();
                              for(liit++;liit!=dcyFStCcP.end();liit++) dcyFStP.push_back(getCcPid((*liit)));
                              sortByPidAndPchrg(dcyFStP);
                              liit=dcyFStCcP.begin();
                              dcyFStP.push_front(getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                              getStrFromLi(dcyFStP,strDcyFStP);
                              m_vVDcyFStP[j].push_back(dcyFStP);
                              m_vVIDcyFStP[j].push_back(_iDcyFStP);
                              m_vVNDcyFStP[j].push_back(0);
                              m_vUomDcyFStP[j][strDcyFStP]=_iDcyFStP;
                            }
                          else
                            {
                              iDcyFStCcP[j][(unsigned int) (nCcPDcyFSt[j])]=_iDcyFStP;
                              m_vVNDcyFStCcP[j][_iDcyFStP]++;
                            }
                          nCcPDcyFSt[j]++;
                        }
                      if(m_ccSwitch==true&&m_vICcCompDcyFStP[j]!=0) nAllPDcyFSt[j]=nPDcyFSt[j]+nCcPDcyFSt[j];
                    }             
                }               
            }

          if(m_vPid_compProdBrP.size()>0)
            {
              // Pay attention to that prodBrCcP equate to ccProdBrP for self-charge-conjugate particles.
              list<int> prodBrP, prodBrCcP;
              prodBrP.clear(); prodBrCcP.clear();
              for(unsigned int j=0;j<m_vPid_compProdBrP.size();j++)
                {
                  nPProdBr[j]=0;
                  if(m_ccSwitch==true&&m_vICcCompProdBrP[j]!=0)
                    {
                      nCcPProdBr[j]=0;
                      nAllPProdBr[j]=0;
                    }
                }     
              for(unsigned int j=0;j<vPid.size();j++)
                {
                  for(unsigned int k=0;k<m_vPid_compProdBrP.size();k++)
                    {
                      if(vPid[j]==m_vPid_compProdBrP[k])
                        {
                          unsigned int mj=UINT_MAX;
                          for(unsigned int l=0;l<vIdxOfHead.size();l++)
                            {
                              // Here, "||(vIdxOfHead[l]==-1&&((unsigned int) vMidx[j])==j)" is added specially for particles that could be generated directly from e+ and e- and at least with one sister.
                              if((vIdxOfHead[l]==vMidx[j]&&((unsigned int) vMidx[j])!=j)||(vIdxOfHead[l]==-1&&((unsigned int) vMidx[j])==j))
                                {
                                  mj=l;
                                  break;
                                }
                            }
                          if(mj!=UINT_MAX) prodBrP=dcyTr[mj];
                          else
                            {
                              // The else statement is added here to handle the particles that could be generated directly from e+ and e- and with no sisters. (||(vIdxOfHead[l]==vMidx[j]&&((unsigned int) vMidx[j])==j))
                              prodBrP.clear();
                              prodBrP.push_back(-11);
                              prodBrP.push_back(11);
                              prodBrP.push_back(vPid[j]);
                            }
                          int _iProdBrP=-1; // If the variable is still equal to -1 after the following loop, then the production branch of the particle is a new production branch of the particle.
                          int _iCcProdBrP=-9999;
                          for(unsigned int l=0;l<m_vVProdBrP[k].size();l++)
                            {
                              if(prodBrP==m_vVProdBrP[k][l])
                                {
                                  _iProdBrP=l;
                                  if(m_ccSwitch==true&&m_vICcCompProdBrP[k]==0) _iCcProdBrP=m_vVIProdBrCcP[k][l];
                                  break;
                                }
                              else if(m_ccSwitch==true&&m_vICcCompProdBrP[k]==0&&m_vVIProdBrCcP[k][l]!=0&&prodBrP==m_vVProdBrCcP[k][l])
                                {
                                  _iProdBrP=l;
                                  _iCcProdBrP=-1;
                                  break;
                                }
                            }
                          if(_iProdBrP==-1)
                            {
                              _iProdBrP=m_vVProdBrP[k].size();
                              iProdBrP[k][(unsigned int) (nPProdBr[k])]=_iProdBrP;
                              m_vVProdBrP[k].push_back(prodBrP);
                              m_vVIProdBrP[k].push_back(_iProdBrP);
                              m_vVNProdBrP[k].push_back(1);
                              if(m_ccSwitch==true)
                                {
                                  prodBrCcP.clear();
                                  list<int>::iterator liit=prodBrP.begin();
                                  for(liit++;liit!=prodBrP.end();liit++) prodBrCcP.push_back(getCcPid((*liit)));
                                  sortByPidAndPchrg(prodBrCcP); 
                                  liit=prodBrP.begin();
                                  if(m_vICcCompProdBrP[k]!=0)
                                    {
                                      prodBrCcP.push_front(getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                                      m_vVProdBrCcP[k].push_back(prodBrCcP);
                                      m_vVIProdBrCcP[k].push_back(_iProdBrP);
                                      m_vVNProdBrCcP[k].push_back(0);
                                    }    
                                  else
                                    {
                                      prodBrCcP.push_front((*liit)); // Here, (*liit) is equal to getCcPid((*liit)).
                                      if(prodBrCcP==prodBrP) _iCcProdBrP=0;
                                      else _iCcProdBrP=1;
                                      iCcProdBrP[k][(unsigned int) (nPProdBr[k])]=_iCcProdBrP;
                                      m_vVProdBrCcP[k].push_back(prodBrCcP);
                                      m_vVIProdBrCcP[k].push_back(_iCcProdBrP);
                                      m_vVNProdBrCcP[k].push_back(0);
                                    }
                                }
                            }
                          else
                            {
                              iProdBrP[k][(unsigned int) (nPProdBr[k])]=_iProdBrP;
                              if(m_ccSwitch==true&&m_vICcCompProdBrP[k]==0)
                                {
                                  iCcProdBrP[k][(unsigned int) (nPProdBr[k])]=_iCcProdBrP;
                                  if(_iCcProdBrP==-1) m_vVNProdBrCcP[k][_iProdBrP]++;
                                  else m_vVNProdBrP[k][_iProdBrP]++;
                                }
                              else
                                {
                                  m_vVNProdBrP[k][_iProdBrP]++;
                                }
                            }
                          nPProdBr[k]++;
                          break;
                        } // Here, "&&m_vICcCompProdBrP[k]!=0" in the following condition can be removed.
                      else if(m_ccSwitch==true&&m_vICcCompProdBrP[k]!=0&&vPid[j]==m_vPid_ccCompProdBrP[k])
                        {
                          unsigned int mj=UINT_MAX;
                          for(unsigned int l=0;l<vIdxOfHead.size();l++)
                            {
                              // Here, "||(vIdxOfHead[l]==-1&&((unsigned int) vMidx[j])==j)" is added specially for particles that could be generated directly from e+ and e- and at least with one sister.
                              if((vIdxOfHead[l]==vMidx[j]&&((unsigned int) vMidx[j])!=j)||(vIdxOfHead[l]==-1&&((unsigned int) vMidx[j])==j))
                                {
                                  mj=l;
                                  break;
                                }
                            }
                          if(mj!=UINT_MAX) prodBrCcP=dcyTr[mj];
                          else
                            {
                              // The else statement is added here to handle the particles that could be generated directly from e+ and e- and with no sisters. (||(vIdxOfHead[l]==vMidx[j]&&((unsigned int) vMidx[j])==j))
                              prodBrP.clear();
                              prodBrP.push_back(-11);
                              prodBrP.push_back(11);
                              prodBrP.push_back(vPid[j]);
                            }
                          int _iProdBrP=-1; // If the variable is still equal to -1 after the following loop, then the production branch of the particle is a new production branch of the particle.
                          for(unsigned int l=0;l<m_vVProdBrCcP[k].size();l++)
                            {
                              if(prodBrCcP==m_vVProdBrCcP[k][l])
                                {
                                  _iProdBrP=l;
                                  break;
                                }
                            }
                          if(_iProdBrP==-1)
                            {
                              _iProdBrP=m_vVProdBrP[k].size();
                              iProdBrCcP[k][(unsigned int) (nCcPProdBr[k])]=_iProdBrP;
                              m_vVProdBrCcP[k].push_back(prodBrCcP);
                              m_vVIProdBrCcP[k].push_back(_iProdBrP);
                              m_vVNProdBrCcP[k].push_back(1);

                              prodBrP.clear();
                              list<int>::iterator liit=prodBrCcP.begin();
                              for(liit++;liit!=prodBrCcP.end();liit++) prodBrP.push_back(getCcPid((*liit)));
                              sortByPidAndPchrg(prodBrP);
                              liit=prodBrCcP.begin();
                              prodBrP.push_front(getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                              m_vVProdBrP[k].push_back(prodBrP);
                              m_vVIProdBrP[k].push_back(_iProdBrP);
                              m_vVNProdBrP[k].push_back(0);
                            }
                          else
                            {
                              iProdBrCcP[k][(unsigned int) (nCcPProdBr[k])]=_iProdBrP;
                              m_vVNProdBrCcP[k][_iProdBrP]++;
                            }
                          nCcPProdBr[k]++;
                          break;                                    
                        }
                    }
                }
              if(m_ccSwitch==true) for(unsigned int j=0;j<m_vPid_compProdBrP.size();j++) if(m_vICcCompProdBrP[j]!=0) nAllPProdBr[j]=nPProdBr[j]+nCcPProdBr[j];
            }

          if(m_vPid_compMP.size()>0)
            {
              int mpid,mpidCcP;
              for(unsigned int j=0;j<m_vPid_compMP.size();j++)
                {
                  nPM[j]=0;
                  if(m_ccSwitch==true&&m_vICcCompMP[j]!=0)
                    {
                      nCcPM[j]=0;
                      nAllPM[j]=0;
                    }
                }     
              for(unsigned int j=0;j<vPid.size();j++)
                {
                  for(unsigned int k=0;k<m_vPid_compMP.size();k++)
                    {
                      if(vPid[j]==m_vPid_compMP[k])
                        {
                          if(((unsigned int) vMidx[j])!=j) mpid=vPid[vMidx[j]];
                          else mpid=m_pidOfISt;
                          int _iMP=-1; // If the variable is still equal to -1 after the following loop, then the mother of the particle is a new mother of the particle.
                          int _iCcMP=-9999;
                          for(unsigned int l=0;l<m_vVMpidP[k].size();l++)
                            {
                              if(mpid==m_vVMpidP[k][l])
                                {
                                  _iMP=l;
                                  if(m_ccSwitch==true&&m_vICcCompMP[k]==0) _iCcMP=m_vVIMCcP[k][l];
                                  break;
                                }
                              else if(m_ccSwitch==true&&m_vICcCompMP[k]==0&&m_vVIMCcP[k][l]!=0&&mpid==m_vVMpidCcP[k][l])
                                {
                                  _iMP=l;
                                  _iCcMP=-1;
                                  break;
                                }
                            }
                          if(_iMP==-1)
                            {
                              _iMP=m_vVMpidP[k].size();
                              MpidP[k][(unsigned int) (nPM[k])]=mpid;
                              m_vVMpidP[k].push_back(mpid);
                              m_vVIMP[k].push_back(_iMP);
                              m_vVNMP[k].push_back(1);
                              if(m_ccSwitch==true)
                                {
                                  mpidCcP=getCcPid(mpid);
                                  if(m_vICcCompMP[k]!=0)
                                    {
                                      m_vVMpidCcP[k].push_back(mpidCcP);
                                      m_vVIMCcP[k].push_back(_iMP);
                                      m_vVNMCcP[k].push_back(0);
                                    }    
                                  else
                                    {
                                      if(mpidCcP==mpid) _iCcMP=0;
                                      else _iCcMP=1;
                                      iCcMP[k][(unsigned int) (nPM[k])]=_iCcMP;
                                      m_vVMpidCcP[k].push_back(mpidCcP);
                                      m_vVIMCcP[k].push_back(_iCcMP);
                                      m_vVNMCcP[k].push_back(0);
                                    }
                                }
                            }
                          else
                            {
                              MpidP[k][(unsigned int) (nPM[k])]=mpid;
                              if(m_ccSwitch==true&&m_vICcCompMP[k]==0)
                                {
                                  iCcMP[k][(unsigned int) (nPM[k])]=_iCcMP;
                                  if(_iCcMP==-1) m_vVNMCcP[k][_iMP]++;
                                  else m_vVNMP[k][_iMP]++;
                                }
                              else
                                {
                                  m_vVNMP[k][_iMP]++;
                                }
                            }
                          nPM[k]++;
                          break;
                        } // Here, "&&m_vICcCompMP[k]!=0" in the following condition can be removed.
                      else if(m_ccSwitch==true&&m_vICcCompMP[k]!=0&&vPid[j]==m_vPid_ccCompMP[k])
                        {
                          if(((unsigned int) vMidx[j])!=j) mpidCcP=vPid[vMidx[j]];
                          else mpidCcP=m_pidOfISt;
                          int _iMP=-1; // If the variable is still equal to -1 after the following loop, then the mother of the particle is a new mother of the particle.
                          for(unsigned int l=0;l<m_vVMpidCcP[k].size();l++)
                            {
                              if(mpidCcP==m_vVMpidCcP[k][l])
                                {
                                  _iMP=l;
                                  break;
                                }
                            }
                          if(_iMP==-1)
                            {
                              _iMP=m_vVMpidP[k].size();
                              MpidCcP[k][(unsigned int) (nCcPM[k])]=mpidCcP;
                              m_vVMpidCcP[k].push_back(mpidCcP);
                              m_vVIMCcP[k].push_back(_iMP);
                              m_vVNMCcP[k].push_back(1);

                              mpid=getCcPid(mpidCcP);

                              m_vVMpidP[k].push_back(mpid);
                              m_vVIMP[k].push_back(_iMP);
                              m_vVNMP[k].push_back(0);
                            }
                          else
                            {
                              MpidCcP[k][(unsigned int) (nCcPM[k])]=mpidCcP;
                              m_vVNMCcP[k][_iMP]++;
                            }
                          nCcPM[k]++;
                          break;                                    
                        }
                    }
                }
              if(m_ccSwitch==true) for(unsigned int j=0;j<m_vPid_compMP.size();j++) if(m_vICcCompMP[j]!=0) nAllPM[j]=nPM[j]+nCcPM[j];
            }

          if(m_vCompIncDcyBr.size()>0)
            {
              // Pay attention to that dcyBrCcIncDcyBr equate to ccDcyBrIncDcyBr for self-charge-conjugate inclusive decays.
              list<int> dcyBrIncDcyBr, dcyBrCcIncDcyBr;
              dcyBrIncDcyBr.clear(); dcyBrCcIncDcyBr.clear();
              for(unsigned int j=0;j<m_vCompIncDcyBr.size();j++)
                {
                  nIncDcyBr[j]=0;
                  if(m_ccSwitch==true&&m_vICcCompIncDcyBr[j]!=0)
                    {
                      nCcIncDcyBr[j]=0;
                      nAllIncDcyBr[j]=0;
                    }
                }     
              for(unsigned int j=0;j<dcyTr.size();j++)
                {
                  for(unsigned int k=0;k<m_vCompIncDcyBr.size();k++)
                    {
                      if(isLiaMatchedWithLib(m_vCompIncDcyBr[k],dcyTr[j]))
                        {
                          dcyBrIncDcyBr=dcyTr[j];
                          int _iDcyBrIncDcyBr=-1; // If the variable is still equal to -1 after the following loop, then the decay branch of the inclusive decay branch is a new exclusive decay branch of the inclusive decay branch.
                          int _iCcDcyBrIncDcyBr=-9999;
                          for(unsigned int l=0;l<m_vVDcyBrIncDcyBr[k].size();l++)
                            {
                              if(dcyBrIncDcyBr==m_vVDcyBrIncDcyBr[k][l])
                                {
                                  _iDcyBrIncDcyBr=l;
                                  if(m_ccSwitch==true&&m_vICcCompIncDcyBr[k]==0) _iCcDcyBrIncDcyBr=m_vVIDcyBrCcIncDcyBr[k][l];
                                  break;
                                }
                              else if(m_ccSwitch==true&&m_vICcCompIncDcyBr[k]==0&&m_vVIDcyBrCcIncDcyBr[k][l]!=0&&dcyBrIncDcyBr==m_vVDcyBrCcIncDcyBr[k][l])
                                {
                                  _iDcyBrIncDcyBr=l;
                                  _iCcDcyBrIncDcyBr=-1;
                                  break;
                                }
                            }
                          if(_iDcyBrIncDcyBr==-1)
                            {
                              _iDcyBrIncDcyBr=m_vVDcyBrIncDcyBr[k].size();
                              iDcyBrIncDcyBr[k][(unsigned int) (nIncDcyBr[k])]=_iDcyBrIncDcyBr;
                              m_vVDcyBrIncDcyBr[k].push_back(dcyBrIncDcyBr);
                              m_vVIDcyBrIncDcyBr[k].push_back(_iDcyBrIncDcyBr);
                              m_vVNDcyBrIncDcyBr[k].push_back(1);
                              if(m_ccSwitch==true)
                                {
                                  dcyBrCcIncDcyBr.clear();
                                  list<int>::iterator liit=dcyBrIncDcyBr.begin();
                                  if((*liit)==-11) liit++;
                                  for(liit++;liit!=dcyBrIncDcyBr.end();liit++) dcyBrCcIncDcyBr.push_back(getCcPid((*liit)));
                                  sortByPidAndPchrg(dcyBrCcIncDcyBr); 
                                  liit=dcyBrIncDcyBr.begin();
                                  if(m_vICcCompIncDcyBr[k]!=0)
                                    {
                                      if((*liit)!=-11)
                                        {
                                          dcyBrCcIncDcyBr.push_front(getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                                        }
                                      else
                                        {
                                          dcyBrCcIncDcyBr.push_front(11);
                                          dcyBrCcIncDcyBr.push_front(-11);
                                        }
                                      m_vVDcyBrCcIncDcyBr[k].push_back(dcyBrCcIncDcyBr);
                                      m_vVIDcyBrCcIncDcyBr[k].push_back(_iDcyBrIncDcyBr);
                                      m_vVNDcyBrCcIncDcyBr[k].push_back(0);
                                    }    
                                  else
                                    {
                                      if((*liit)!=-11)
                                        {
                                          dcyBrCcIncDcyBr.push_front((*liit)); // Here, (*liit) is equal to getCcPid((*liit)).
                                        }
                                      else
                                        {
                                          dcyBrCcIncDcyBr.push_front(11);
                                          dcyBrCcIncDcyBr.push_front(-11);
                                        }
                                      if(dcyBrCcIncDcyBr==dcyBrIncDcyBr) _iCcDcyBrIncDcyBr=0;
                                      else _iCcDcyBrIncDcyBr=1;
                                      iCcDcyBrIncDcyBr[k][(unsigned int) (nIncDcyBr[k])]=_iCcDcyBrIncDcyBr;
                                      m_vVDcyBrCcIncDcyBr[k].push_back(dcyBrCcIncDcyBr);
                                      m_vVIDcyBrCcIncDcyBr[k].push_back(_iCcDcyBrIncDcyBr);
                                      m_vVNDcyBrCcIncDcyBr[k].push_back(0);
                                    }
                                }
                            }
                          else
                            {
                              iDcyBrIncDcyBr[k][(unsigned int) (nIncDcyBr[k])]=_iDcyBrIncDcyBr;
                              if(m_ccSwitch==true&&m_vICcCompIncDcyBr[k]==0)
                                {
                                  iCcDcyBrIncDcyBr[k][(unsigned int) (nIncDcyBr[k])]=_iCcDcyBrIncDcyBr;
                                  if(_iCcDcyBrIncDcyBr==-1) m_vVNDcyBrCcIncDcyBr[k][_iDcyBrIncDcyBr]++;
                                  else m_vVNDcyBrIncDcyBr[k][_iDcyBrIncDcyBr]++;
                                }
                              else
                                {
                                  m_vVNDcyBrIncDcyBr[k][_iDcyBrIncDcyBr]++;
                                }
                            }
                          nIncDcyBr[k]++;
                        } // Here, "&&m_vICcCompIncDcyBr[k]!=0" in the following condition can be removed.
                      else if(m_ccSwitch==true&&m_vICcCompIncDcyBr[k]!=0&&isLiaMatchedWithLib(m_vCompCcIncDcyBr[k],dcyTr[j]))
                        {
                          dcyBrCcIncDcyBr=dcyTr[j];
                          int _iDcyBrIncDcyBr=-1; // If the variable is still equal to -1 after the following loop, then the decay branch of the inclusive decay branch is a new exclusive decay branch of the inclusive decay branch.
                          for(unsigned int l=0;l<m_vVDcyBrCcIncDcyBr[k].size();l++)
                            {
                              if(dcyBrCcIncDcyBr==m_vVDcyBrCcIncDcyBr[k][l])
                                {
                                  _iDcyBrIncDcyBr=l;
                                  break;
                                }
                            }
                          if(_iDcyBrIncDcyBr==-1)
                            {
                              _iDcyBrIncDcyBr=m_vVDcyBrIncDcyBr[k].size();
                              iDcyBrCcIncDcyBr[k][(unsigned int) (nCcIncDcyBr[k])]=_iDcyBrIncDcyBr;
                              m_vVDcyBrCcIncDcyBr[k].push_back(dcyBrCcIncDcyBr);
                              m_vVIDcyBrCcIncDcyBr[k].push_back(_iDcyBrIncDcyBr);
                              m_vVNDcyBrCcIncDcyBr[k].push_back(1);

                              dcyBrIncDcyBr.clear();
                              list<int>::iterator liit=dcyBrCcIncDcyBr.begin();
                              if((*liit)==-11) liit++;
                              for(liit++;liit!=dcyBrCcIncDcyBr.end();liit++) dcyBrIncDcyBr.push_back(getCcPid((*liit)));
                              sortByPidAndPchrg(dcyBrIncDcyBr);
                              liit=dcyBrCcIncDcyBr.begin();
                              if((*liit)!=-11)
                                {
                                  dcyBrIncDcyBr.push_front(getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                                }
                              else
                                {
                                  dcyBrIncDcyBr.push_front(11);
                                  dcyBrIncDcyBr.push_front(-11);
                                }
                              m_vVDcyBrIncDcyBr[k].push_back(dcyBrIncDcyBr);
                              m_vVIDcyBrIncDcyBr[k].push_back(_iDcyBrIncDcyBr);
                              m_vVNDcyBrIncDcyBr[k].push_back(0);
                            }
                          else
                            {
                              iDcyBrCcIncDcyBr[k][(unsigned int) (nCcIncDcyBr[k])]=_iDcyBrIncDcyBr;
                              m_vVNDcyBrCcIncDcyBr[k][_iDcyBrIncDcyBr]++;
                            }
                          nCcIncDcyBr[k]++;
                        }
                    }
                }
              if(m_ccSwitch==true) for(unsigned int j=0;j<m_vCompIncDcyBr.size();j++) if(m_vICcCompIncDcyBr[j]!=0) nAllIncDcyBr[j]=nIncDcyBr[j]+nCcIncDcyBr[j];
            }

          if(m_vCompIRADcyBr.size()>0)
            {
              // Pay attention to that dcyBrCcIRADcyBr equate to ccDcyBrIRADcyBr for self-charge-conjugate intermediate-resonance-allowed decays.
              vector< list<int> > dcyBrIRADcyBr, dcyBrCcIRADcyBr;
              dcyBrIRADcyBr.clear(); dcyBrCcIRADcyBr.clear();
              for(unsigned int j=0;j<m_vCompIRADcyBr.size();j++)
                {
                  nIRADcyBr[j]=0;
                  if(m_ccSwitch==true&&m_vICcCompIRADcyBr[j]!=0)
                    {
                      nCcIRADcyBr[j]=0;
                      nAllIRADcyBr[j]=0;
                    }
                }     
              vector< vector< list<int> > > vDcyBrIRADcyBr,vDcyBrCcIRADcyBr;
              for(unsigned int j=0;j<m_vCompIRADcyBr.size();j++)
                {
                  vDcyBrIRADcyBr.clear();
                  if(countIRADcyBr(vPid,vMidx,m_vCompIRADcyBr[j],false,&vDcyBrIRADcyBr)>0)
                    {
                      for(unsigned int k=0;k<vDcyBrIRADcyBr.size();k++)
                        {
                          dcyBrIRADcyBr=vDcyBrIRADcyBr[k];
                          int _iDcyBrIRADcyBr=-1; // If the variable is still equal to -1 after the following loop, then the decay branch of the intermediate-resonance-allowed decay branch is a new exclusive decay branch of the intermediate-resonance-allowed decay branch.
                          int _iCcDcyBrIRADcyBr=-9999;
                          for(unsigned int l=0;l<m_vVDcyBrIRADcyBr[j].size();l++)
                            {
                              if(dcyBrIRADcyBr==m_vVDcyBrIRADcyBr[j][l])
                                {
                                  _iDcyBrIRADcyBr=l;
                                  if(m_ccSwitch==true&&m_vICcCompIRADcyBr[j]==0) _iCcDcyBrIRADcyBr=m_vVIDcyBrCcIRADcyBr[j][l];
                                  break;
                                }
                              else if(m_ccSwitch==true&&m_vICcCompIRADcyBr[j]==0&&m_vVIDcyBrCcIRADcyBr[j][l]!=0&&dcyBrIRADcyBr==m_vVDcyBrCcIRADcyBr[j][l])
                                {
                                  _iDcyBrIRADcyBr=l;
                                  _iCcDcyBrIRADcyBr=-1;
                                  break;
                                }
                            }
                          if(_iDcyBrIRADcyBr==-1)
                            {
                              _iDcyBrIRADcyBr=m_vVDcyBrIRADcyBr[j].size();
                              iDcyBrIRADcyBr[j][(unsigned int) (nIRADcyBr[j])]=_iDcyBrIRADcyBr;
                              m_vVDcyBrIRADcyBr[j].push_back(dcyBrIRADcyBr);
                              m_vVIDcyBrIRADcyBr[j].push_back(_iDcyBrIRADcyBr);
                              m_vVNDcyBrIRADcyBr[j].push_back(1);
                              if(m_ccSwitch==true)
                                {
                                  dcyBrCcIRADcyBr.clear();
                                  list<int> subDcyBrCcIRADcyBr;
                                  list<int>::iterator liit;
                                  for(unsigned int l=0;l<dcyBrIRADcyBr.size();l++)
                                    {
                                      subDcyBrCcIRADcyBr.clear();
                                      liit=dcyBrIRADcyBr[l].begin();
                                      if(l==0&&(*liit)==-11) liit++;
                                      for(liit++;liit!=dcyBrIRADcyBr[l].end();liit++) subDcyBrCcIRADcyBr.push_back(getCcPid((*liit)));
                                      sortByPidAndPchrg(subDcyBrCcIRADcyBr);
                                      liit=dcyBrIRADcyBr[l].begin();
                                      if(l==0&&(*liit)==-11)
                                        {
                                          subDcyBrCcIRADcyBr.push_front(11);
                                          subDcyBrCcIRADcyBr.push_front(-11);
                                        }
                                      else
                                        {
                                          subDcyBrCcIRADcyBr.push_front(getCcPid((*liit)));
                                        }
                                      dcyBrCcIRADcyBr.push_back(subDcyBrCcIRADcyBr);
                                    }
                                  if(m_vICcCompIRADcyBr[j]!=0)
                                    {
                                      m_vVDcyBrCcIRADcyBr[j].push_back(dcyBrCcIRADcyBr);
                                      m_vVIDcyBrCcIRADcyBr[j].push_back(_iDcyBrIRADcyBr);
                                      m_vVNDcyBrCcIRADcyBr[j].push_back(0);
                                    }    
                                  else
                                    {
                                      if(dcyBrCcIRADcyBr==dcyBrIRADcyBr) _iCcDcyBrIRADcyBr=0;
                                      else _iCcDcyBrIRADcyBr=1;
                                      iCcDcyBrIRADcyBr[j][(unsigned int) (nIRADcyBr[j])]=_iCcDcyBrIRADcyBr;
                                      m_vVDcyBrCcIRADcyBr[j].push_back(dcyBrCcIRADcyBr);
                                      m_vVIDcyBrCcIRADcyBr[j].push_back(_iCcDcyBrIRADcyBr);
                                      m_vVNDcyBrCcIRADcyBr[j].push_back(0);
                                    }
                                }
                            }
                          else
                            {
                              iDcyBrIRADcyBr[j][(unsigned int) (nIRADcyBr[j])]=_iDcyBrIRADcyBr;
                              if(m_ccSwitch==true&&m_vICcCompIRADcyBr[j]==0)
                                {
                                  iCcDcyBrIRADcyBr[j][(unsigned int) (nIRADcyBr[j])]=_iCcDcyBrIRADcyBr;
                                  if(_iCcDcyBrIRADcyBr==-1) m_vVNDcyBrCcIRADcyBr[j][_iDcyBrIRADcyBr]++;
                                  else m_vVNDcyBrIRADcyBr[j][_iDcyBrIRADcyBr]++;
                                }
                              else
                                {
                                  m_vVNDcyBrIRADcyBr[j][_iDcyBrIRADcyBr]++;
                                }
                            }
                          nIRADcyBr[j]++;
                        }
                    }
                  vDcyBrCcIRADcyBr.clear();
                  // Here, "&&m_vICcCompIRADcyBr[k]!=0" in the following condition can not be removed. Besides, "if" here can not be replaced by "else if".
                  if(m_ccSwitch==true&&m_vICcCompIRADcyBr[j]!=0&&countIRADcyBr(vPid,vMidx,m_vCompCcIRADcyBr[j],false,&vDcyBrCcIRADcyBr)>0)
                    {
                      for(unsigned int k=0;k<vDcyBrCcIRADcyBr.size();k++)
                        {
                          dcyBrCcIRADcyBr=vDcyBrCcIRADcyBr[k];
                          int _iDcyBrIRADcyBr=-1; // If the variable is still equal to -1 after the following loop, then the decay branch of the intermediate-resonance-allowed decay branch is a new exclusive decay branch of the intermediate-resonance-allowed decay branch.
                          for(unsigned int l=0;l<m_vVDcyBrCcIRADcyBr[j].size();l++)
                            {
                              if(dcyBrCcIRADcyBr==m_vVDcyBrCcIRADcyBr[j][l])
                                {
                                  _iDcyBrIRADcyBr=l;
                                  break;
                                }
                            }
                          if(_iDcyBrIRADcyBr==-1)
                            {
                              _iDcyBrIRADcyBr=m_vVDcyBrIRADcyBr[j].size();
                              iDcyBrCcIRADcyBr[j][(unsigned int) (nCcIRADcyBr[j])]=_iDcyBrIRADcyBr;
                              m_vVDcyBrCcIRADcyBr[j].push_back(dcyBrCcIRADcyBr);
                              m_vVIDcyBrCcIRADcyBr[j].push_back(_iDcyBrIRADcyBr);
                              m_vVNDcyBrCcIRADcyBr[j].push_back(1);

                              dcyBrIRADcyBr.clear();
                              list<int> subDcyBrIRADcyBr;
                              list<int>::iterator liit;
                              for(unsigned int l=0;l<dcyBrCcIRADcyBr.size();l++)
                                {
                                  subDcyBrIRADcyBr.clear();
                                  liit=dcyBrCcIRADcyBr[l].begin();
                                  if(l==0&&(*liit)==-11) liit++;
                                  for(liit++;liit!=dcyBrCcIRADcyBr[l].end();liit++) subDcyBrIRADcyBr.push_back(getCcPid((*liit)));
                                  sortByPidAndPchrg(subDcyBrIRADcyBr);
                                  liit=dcyBrCcIRADcyBr[l].begin();
                                  if(l==0&&(*liit)==-11)
                                    {
                                      subDcyBrIRADcyBr.push_front(11);
                                      subDcyBrIRADcyBr.push_front(-11);
                                    }
                                  else
                                    {
                                      subDcyBrIRADcyBr.push_front(getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                                    }
                                  dcyBrIRADcyBr.push_back(subDcyBrIRADcyBr);
                                }
                              m_vVDcyBrIRADcyBr[j].push_back(dcyBrIRADcyBr);
                              m_vVIDcyBrIRADcyBr[j].push_back(_iDcyBrIRADcyBr);
                              m_vVNDcyBrIRADcyBr[j].push_back(0);
                            }
                          else
                            {
                              iDcyBrCcIRADcyBr[j][(unsigned int) (nCcIRADcyBr[j])]=_iDcyBrIRADcyBr;
                              m_vVNDcyBrCcIRADcyBr[j][_iDcyBrIRADcyBr]++;
                            }
                          nCcIRADcyBr[j]++;
                        }
                      if(m_ccSwitch==true&&m_vICcCompIRADcyBr[j]!=0) nAllIRADcyBr[j]=nIRADcyBr[j]+nCcIRADcyBr[j];
                    }             
                }               
            }

          if(m_vSigDcyTr.size()>0)
            {
              strcpy(nmSigDcyTr,"");
              iSigDcyTr=-1;
              iSigDcyIFSts_tr=-1;
              if(m_ccSwitch==true)
                {
                  iCcSigDcyTr=-9999;
                  iCcSigDcyIFSts_tr=-9999;
                }
              for(unsigned int j=0;j<m_vSigDcyTr.size();j++)
                {
                  
                  if(dcyTr==m_vSigDcyTr[j])
                    {
                      iSigDcyTr=j;
                      strcpy(nmSigDcyTr,m_vNm_sigDcyTr[iSigDcyTr].c_str());
                      iSigDcyIFSts_tr=m_iSigDcyTrISigDcyIFSts_trMap[iSigDcyTr]; 
                      if(m_anaTasksForSigIds=="TC") m_vNSigDcyTr[iSigDcyTr]++;
                      if(m_anaTasksForSigIds=="TC") m_vNSigDcyIFSts_tr[iSigDcyIFSts_tr]++;
                      if(m_ccSwitch==true)
                        {
                          if(m_vNSigDcyTr[iSigDcyTr]+m_vNCcSigDcyTr[iSigDcyTr]==1) m_iSigDcyTrIDcyTrMap[iSigDcyTr]=iDcyTr;
                          if(m_vNSigDcyIFSts_tr[iSigDcyIFSts_tr]+m_vNCcSigDcyIFSts_tr[iSigDcyIFSts_tr]==1) m_iSigDcyIFSts_trIDcyIFStsMap[iSigDcyIFSts_tr]=iDcyIFSts;
                          iCcSigDcyTr=m_vICcSigDcyTr[j];
                          iCcSigDcyIFSts_tr=m_iSigDcyTrICcSigDcyIFSts_trMap[iSigDcyTr];
                          if(m_vNSigDcyTr[iSigDcyTr]+m_vNCcSigDcyTr[iSigDcyTr]==1) m_iSigDcyTrICcDcyTrMap[iSigDcyTr]=iCcDcyTr;
                          if(m_vNSigDcyIFSts_tr[iSigDcyIFSts_tr]+m_vNCcSigDcyIFSts_tr[iSigDcyIFSts_tr]==1) m_iSigDcyIFSts_trICcDcyIFStsMap[iSigDcyIFSts_tr]=iCcDcyIFSts;
                        }
                      else
                        {
                          if(m_vNSigDcyTr[iSigDcyTr]==1) m_iSigDcyTrIDcyTrMap[iSigDcyTr]=iDcyTr;
                          if(m_vNSigDcyIFSts_tr[iSigDcyIFSts_tr]==1) m_iSigDcyIFSts_trIDcyIFStsMap[iSigDcyIFSts_tr]=iDcyIFSts;
                        }
                      break;
                    }
                  else if(m_ccSwitch==true&&m_vICcSigDcyTr[j]!=0&&dcyTr==m_vCcSigDcyTr[j])
                    {
                      iSigDcyTr=j;
                      strcpy(nmSigDcyTr,m_vNm_sigDcyTr[iSigDcyTr].c_str());
                      iSigDcyIFSts_tr=m_iSigDcyTrISigDcyIFSts_trMap[iSigDcyTr];
                      if(m_anaTasksForSigIds=="TC") m_vNCcSigDcyTr[iSigDcyTr]++;
                      if(m_anaTasksForSigIds=="TC") m_vNCcSigDcyIFSts_tr[iSigDcyIFSts_tr]++;
                      if(m_vNSigDcyTr[iSigDcyTr]+m_vNCcSigDcyTr[iSigDcyTr]==1) m_iSigDcyTrIDcyTrMap[iSigDcyTr]=iDcyTr;
                      if(m_vNSigDcyIFSts_tr[iSigDcyIFSts_tr]+m_vNCcSigDcyIFSts_tr[iSigDcyIFSts_tr]==1) m_iSigDcyIFSts_trIDcyIFStsMap[iSigDcyIFSts_tr]=iDcyIFSts;
                      iCcSigDcyTr=-1;
                      iCcSigDcyIFSts_tr=-1*m_iSigDcyTrICcSigDcyIFSts_trMap[iSigDcyTr];
                      if(m_vNSigDcyTr[iSigDcyTr]+m_vNCcSigDcyTr[iSigDcyTr]==1) m_iSigDcyTrICcDcyTrMap[iSigDcyTr]=-1*iCcDcyTr;
                      if(m_vNSigDcyIFSts_tr[iSigDcyIFSts_tr]+m_vNCcSigDcyIFSts_tr[iSigDcyIFSts_tr]==1) m_iSigDcyIFSts_trICcDcyIFStsMap[iSigDcyIFSts_tr]=-1*iCcDcyIFSts;
                      break;
                    }
                }
            }

          if(m_vSigDcyIFSts.size()>0)
            {
              strcpy(nmSigDcyIFSts,"");
              iSigDcyIFSts=-1;
              iCcSigDcyIFSts=-9999;
              for(unsigned int j=0;j<m_vSigDcyIFSts.size();j++)
                {
                  if(dcyIFSts==m_vSigDcyIFSts[j])
                    {
                      iSigDcyIFSts=j;
                      strcpy(nmSigDcyIFSts,m_vNm_sigDcyIFSts[iSigDcyIFSts].c_str());
                      if(m_anaTasksForSigIds=="TC") m_vNSigDcyIFSts[iSigDcyIFSts]++;
                      if(m_ccSwitch==true)
                        {
                          if(m_vNSigDcyIFSts[iSigDcyIFSts]+m_vNCcSigDcyIFSts[iSigDcyIFSts]==1) m_iSigDcyIFStsIDcyIFStsMap[iSigDcyIFSts]=iDcyIFSts;
                          iCcSigDcyIFSts=m_vICcSigDcyIFSts[iSigDcyIFSts];
                          if(m_vNSigDcyIFSts[j]+m_vNCcSigDcyIFSts[j]==1) m_iSigDcyIFStsICcDcyIFStsMap[iSigDcyIFSts]=iCcDcyIFSts;
                        }
                      else
                        {
                          if(m_vNSigDcyIFSts[iSigDcyIFSts]==1) m_iSigDcyIFStsIDcyIFStsMap[iSigDcyIFSts]=iDcyIFSts;
                        }
                      break;
                    }
                  else if(m_ccSwitch==true&&m_vICcSigDcyIFSts[j]!=0&&dcyIFSts==m_vCcSigDcyIFSts[j])
                    {
                      iSigDcyIFSts=j;
                      strcpy(nmSigDcyIFSts,m_vNm_sigDcyIFSts[iSigDcyIFSts].c_str());
                      if(m_anaTasksForSigIds=="TC") m_vNCcSigDcyIFSts[iSigDcyIFSts]++;
                      if(m_vNSigDcyIFSts[iSigDcyIFSts]+m_vNCcSigDcyIFSts[iSigDcyIFSts]==1) m_iSigDcyIFStsIDcyIFStsMap[iSigDcyIFSts]=iDcyIFSts;
                      iCcSigDcyIFSts=-1*m_vICcSigDcyIFSts[iSigDcyIFSts];
                      if(m_vNSigDcyIFSts[j]+m_vNCcSigDcyIFSts[j]==1) m_iSigDcyIFStsICcDcyIFStsMap[iSigDcyIFSts]=-1*iCcDcyIFSts;
                      break;                 
                    }
                }
            }

          if(m_vPid_sigP.size()>0)
            {
              for(unsigned int j=0;j<m_vPid_sigP.size();j++)
                {
                  nSigP[j]=0;
                  nCcSigP[j]=0;
                }
              for(unsigned int j=0;j<vPid.size();j++)
                {
                  for(unsigned int k=0;k<m_vPid_sigP.size();k++)
                    {
                      if(vPid[j]==m_vPid_sigP[k])
                        {
                          if(m_anaTasksForSigIds=="TC") m_vNSigP[k]++;
                          nSigP[k]++;
                          break;
                        }
                      else if(m_ccSwitch==true&&m_vICcSigP[k]!=0&&vPid[j]==m_vPid_ccSigP[k])
                        {
                          if(m_anaTasksForSigIds=="TC") m_vNCcSigP[k]++;
                          nCcSigP[k]++;
                          break;
                        }
                    }
                }
              if(m_ccSwitch==true) for(unsigned int j=0;j<m_vPid_sigP.size();j++) nAllSigP[j]=nSigP[j]+nCcSigP[j];
            }

          if(m_vSigDcyBr.size()>0)
            {
              for(unsigned int j=0;j<m_vSigDcyBr.size();j++)
                {
                  nSigDcyBr[j]=0;
                  for(unsigned int k=0;k<dcyTr.size();k++) if(dcyTr[k]==m_vSigDcyBr[j]) nSigDcyBr[j]++;
                  if(m_anaTasksForSigIds=="TC") m_vNSigDcyBr[j]=m_vNSigDcyBr[j]+nSigDcyBr[j];
                  if(m_ccSwitch==true)
                    {
                      nCcSigDcyBr[j]=0;
                      for(unsigned int k=0;k<dcyTr.size();k++) if(dcyTr[k]==m_vCcSigDcyBr[j]) nCcSigDcyBr[j]++;
                      if(m_anaTasksForSigIds=="TC") m_vNCcSigDcyBr[j]=m_vNCcSigDcyBr[j]+nCcSigDcyBr[j];
                      nAllSigDcyBr[j]=nSigDcyBr[j]+nCcSigDcyBr[j];
                    }
                }
            }

          if(m_vSigIncDcyBr.size()>0)
            {
              for(unsigned int j=0;j<m_vSigIncDcyBr.size();j++)
                {
                  nSigIncDcyBr[j]=countIncLiaInVlib(m_vSigIncDcyBr[j],dcyTr);
                  if(m_anaTasksForSigIds=="TC") m_vNSigIncDcyBr[j]=m_vNSigIncDcyBr[j]+nSigIncDcyBr[j];
                  if(m_ccSwitch==true)
                    {
                      nCcSigIncDcyBr[j]=countIncLiaInVlib(m_vCcSigIncDcyBr[j],dcyTr);
                      if(m_anaTasksForSigIds=="TC") m_vNCcSigIncDcyBr[j]=m_vNCcSigIncDcyBr[j]+nCcSigIncDcyBr[j];
                      nAllSigIncDcyBr[j]=nSigIncDcyBr[j]+nCcSigIncDcyBr[j];
                    }
                }
            }

          if(m_vSigCascDcyBr.size()>0)
            {
              for(unsigned int j=0;j<m_vSigCascDcyBr.size();j++)
                {
                  nSigCascDcyBr[j]=countCascDcyBrInDcyTr(m_vSigCascDcyBr[j],m_vVSigCascDcyBrIdxOfHead[j],m_vVSigCascDcyBrMidxOfHead[j],dcyTr,vIdxOfHead,vMidxOfHead);
                  if(m_anaTasksForSigIds=="TC") m_vNSigCascDcyBr[j]=m_vNSigCascDcyBr[j]+nSigCascDcyBr[j];
                  if(m_ccSwitch==true)
                    {
                      nCcSigCascDcyBr[j]=countCascDcyBrInDcyTr(m_vCcSigCascDcyBr[j],m_vVCcSigCascDcyBrIdxOfHead[j],m_vVCcSigCascDcyBrMidxOfHead[j],dcyTr,vIdxOfHead,vMidxOfHead);
                      if(m_anaTasksForSigIds=="TC") m_vNCcSigCascDcyBr[j]=m_vNCcSigCascDcyBr[j]+nCcSigCascDcyBr[j];
                      nAllSigCascDcyBr[j]=nSigCascDcyBr[j]+nCcSigCascDcyBr[j];
                    }
                }
            }

          if(m_vSigIncCascDcyBr.size()>0)
            {
              for(unsigned int j=0;j<m_vSigIncCascDcyBr.size();j++)
                {
                  nSigIncCascDcyBr[j]=countIncCascDcyBrInDcyTr(m_vVIIncSigIncCascDcyBr[j],m_vSigIncCascDcyBr[j],m_vVSigIncCascDcyBrIdxOfHead[j],m_vVSigIncCascDcyBrMidxOfHead[j],dcyTr,vIdxOfHead,vMidxOfHead);
                  if(m_anaTasksForSigIds=="TC") m_vNSigIncCascDcyBr[j]=m_vNSigIncCascDcyBr[j]+nSigIncCascDcyBr[j];
                  if(m_ccSwitch==true)
                    {
                      nCcSigIncCascDcyBr[j]=countIncCascDcyBrInDcyTr(m_vVIIncSigIncCascDcyBr[j],m_vCcSigIncCascDcyBr[j],m_vVCcSigIncCascDcyBrIdxOfHead[j],m_vVCcSigIncCascDcyBrMidxOfHead[j],dcyTr,vIdxOfHead,vMidxOfHead);
                      if(m_anaTasksForSigIds=="TC") m_vNCcSigIncCascDcyBr[j]=m_vNCcSigIncCascDcyBr[j]+nCcSigIncCascDcyBr[j];
                      nAllSigIncCascDcyBr[j]=nSigIncCascDcyBr[j]+nCcSigIncCascDcyBr[j];
                    }
                }
            }

          if(m_vSigIRADcyBr.size()>0)
            {
              for(unsigned int j=0;j<m_vSigIRADcyBr.size();j++)
                {
                  nSigIRADcyBr[j]=countIRADcyBr(vPid,vMidx,m_vSigIRADcyBr[j]);
                  if(m_anaTasksForSigIds=="TC") m_vNSigIRADcyBr[j]=m_vNSigIRADcyBr[j]+nSigIRADcyBr[j];
                  if(m_ccSwitch==true)
                    {
                      nCcSigIRADcyBr[j]=countIRADcyBr(vPid,vMidx,m_vCcSigIRADcyBr[j]);
                      if(m_anaTasksForSigIds=="TC") m_vNCcSigIRADcyBr[j]=m_vNCcSigIRADcyBr[j]+nCcSigIRADcyBr[j];
                      nAllSigIRADcyBr[j]=nSigIRADcyBr[j]+nCcSigIRADcyBr[j];
                    }
                }
            }

          if(m_vSigIncOrIRACascDcyBr.size()>0)
            {
              for(unsigned int j=0;j<m_vSigIncOrIRACascDcyBr.size();j++)
                {
                  nSigIncOrIRACascDcyBr[j]=countIncOrIRACascDcyBrInDcyTr(m_vVIIncSigIncOrIRACascDcyBr[j],m_vVIIRASigIncOrIRACascDcyBr[j],m_vSigIncOrIRACascDcyBr[j],m_vVSigIncOrIRACascDcyBrIdxOfHead[j],m_vVSigIncOrIRACascDcyBrMidxOfHead[j],dcyTr,vIdxOfHead,vMidxOfHead,vPid,vMidx);
                  if(m_anaTasksForSigIds=="TC") m_vNSigIncOrIRACascDcyBr[j]=m_vNSigIncOrIRACascDcyBr[j]+nSigIncOrIRACascDcyBr[j];
                  if(m_ccSwitch==true)
                    {
                      nCcSigIncOrIRACascDcyBr[j]=countIncOrIRACascDcyBrInDcyTr(m_vVIIncSigIncOrIRACascDcyBr[j],m_vVIIRASigIncOrIRACascDcyBr[j],m_vCcSigIncOrIRACascDcyBr[j],m_vVCcSigIncOrIRACascDcyBrIdxOfHead[j],m_vVCcSigIncOrIRACascDcyBrMidxOfHead[j],dcyTr,vIdxOfHead,vMidxOfHead,vPid,vMidx);
                      if(m_anaTasksForSigIds=="TC") m_vNCcSigIncOrIRACascDcyBr[j]=m_vNCcSigIncOrIRACascDcyBr[j]+nCcSigIncOrIRACascDcyBr[j];
                      nAllSigIncOrIRACascDcyBr[j]=nSigIncOrIRACascDcyBr[j]+nCcSigIncOrIRACascDcyBr[j];
                    }
                }
            }

          tr->Fill();

          if((i+1>=nEtrForTiming)&&(((i+1)%nEtrForTiming==0)||((i+1)==nEtrToBePrcsd)))
            {
              if((i+1)==nEtrForTiming) cout<<"Number of entries processed\tNumber of seconds elapsed"<<endl<<endl;
              cout<<setiosflags(ios::right)<<setw(14)<<i+1<<"\t\t\t"<<setiosflags(ios::fixed)<<setprecision(2)<<setw(14)<<(clock()-starttime)/((double) CLOCKS_PER_SEC)<<resetiosflags(ios::adjustfield)<<endl;
              if((i+1)==nEtrToBePrcsd) cout<<endl;
            }

          if(m_oneOptRootFlByOneIptRootFl==true)
            {
              closeTheOptRootFl1=((i+1)==vNEtr[iOptRootFls]);
              closeTheOptRootFl2=false;
            }
          else
            {
              closeTheOptRootFl1=((m_nEtrMaxInASngOptRootFl==ULONG_MAX)&&(((unsigned long) tr->GetTotBytes())>nTotBytesMaxOfASngOptRootFl));
              closeTheOptRootFl2=((m_nEtrMaxInASngOptRootFl!=ULONG_MAX)&&((i+1)%m_nEtrMaxInASngOptRootFl==0));
            }
          closeTheOptRootFl3=((i+1)==nEtrToBePrcsd);
          if(closeTheOptRootFl1||closeTheOptRootFl2||closeTheOptRootFl3)
            {
              fl->Write();
              delete tr; // Pay attention to that replacing the "delete tr" by "tr->Delete()" will result in a problem of "*** Break *** segmentation violation".
              fl->Close();
              delete fl;
              if(m_rmIptTBrs==true) rmIptBrs(NmOfOptRootFl);
              if(m_useArrayTBrsOpt==false) flatArrayBrs(NmOfOptRootFl);
              openANewOptRootFl=true;
              iOptRootFls++;
            }

          if(m_avoidOverCounting==true) isTheEvtPrcsd=true;
        }
      if(!m_cut.empty()) cout<<"Note that only "<<nEtrThroughTheCut<<" entries passed the cut."<<endl<<endl;
    }

  if(m_compAnaOfDcyTrs==true)
    {
      if(m_ccSwitch==true)
        {
          sortBySumOf1stAnd2ndFromLrgToSml(m_vNDcyTr,m_vNCcDcyTr,m_vDcyTr,m_vCcDcyTr,m_vIDcyTr,m_vICcDcyTr);
        }
      else
        {
          sortBy1stFromLrgToSml(m_vNDcyTr,m_vDcyTr,m_vIDcyTr);
        }
    }
  if(m_compAnaOfDcyIFSts==true)
    {
      if(m_ccSwitch==true)
        {
          sortBySumOf1stAnd2ndFromLrgToSml(m_vNDcyIFSts,m_vNCcDcyIFSts,m_vDcyIFSts,m_vCcDcyIFSts,m_vIDcyIFSts,m_vICcDcyIFSts);
        }
      else
        {
          sortBy1stFromLrgToSml(m_vNDcyIFSts,m_vDcyIFSts,m_vIDcyIFSts);
        }
    }

  if(m_vPid_compDcyBrP.size()>0)
    {
      for(unsigned int i=0;i<m_vPid_compDcyBrP.size();i++)
        {
          if(m_ccSwitch==true)
            {
              sortBySumOf1stAnd2ndFromLrgToSml(m_vVNDcyBrP[i],m_vVNDcyBrCcP[i],m_vVDcyBrP[i],m_vVDcyBrCcP[i],m_vVIDcyBrP[i],m_vVIDcyBrCcP[i]);
            }
          else
            {
              sortBy1stFromLrgToSml(m_vVNDcyBrP[i],m_vVDcyBrP[i],m_vVIDcyBrP[i]);
            }
        }      
    }

  if(m_vPid_compCascDcyBrP.size()>0)
    {
      for(unsigned int i=0;i<m_vPid_compCascDcyBrP.size();i++)
        {
          if(m_ccSwitch==true)
            {
              sortBySumOf1stAnd2ndFromLrgToSml(m_vVNCascDcyBrP[i],m_vVNCascDcyBrCcP[i],m_vVCascDcyBrP[i],m_vVCascDcyBrCcP[i],m_vVICascDcyBrP[i],m_vVICascDcyBrCcP[i]);
            }
          else
            {
              sortBy1stFromLrgToSml(m_vVNCascDcyBrP[i],m_vVCascDcyBrP[i],m_vVICascDcyBrP[i]);
            }
        }
    }

  if(m_vPid_compDcyFStP.size()>0)
    {
      for(unsigned int i=0;i<m_vPid_compDcyFStP.size();i++)
        {
          if(m_ccSwitch==true)
            {
              sortBySumOf1stAnd2ndFromLrgToSml(m_vVNDcyFStP[i],m_vVNDcyFStCcP[i],m_vVDcyFStP[i],m_vVDcyFStCcP[i],m_vVIDcyFStP[i],m_vVIDcyFStCcP[i]);
            }
          else
            {
              sortBy1stFromLrgToSml(m_vVNDcyFStP[i],m_vVDcyFStP[i],m_vVIDcyFStP[i]);
            }
        }
    }

  if(m_vPid_compProdBrP.size()>0)
    {
      for(unsigned int i=0;i<m_vPid_compProdBrP.size();i++)
        {
          if(m_ccSwitch==true)
            {
              sortBySumOf1stAnd2ndFromLrgToSml(m_vVNProdBrP[i],m_vVNProdBrCcP[i],m_vVProdBrP[i],m_vVProdBrCcP[i],m_vVIProdBrP[i],m_vVIProdBrCcP[i]);
            }
          else
            {
              sortBy1stFromLrgToSml(m_vVNProdBrP[i],m_vVProdBrP[i],m_vVIProdBrP[i]);
            }
        }      
    }

  if(m_vPid_compMP.size()>0)
    {
      for(unsigned int i=0;i<m_vPid_compMP.size();i++)
        {
          if(m_ccSwitch==true)
            {
              sortBySumOf1stAnd2ndFromLrgToSml(m_vVNMP[i],m_vVNMCcP[i],m_vVMpidP[i],m_vVMpidCcP[i],m_vVIMP[i],m_vVIMCcP[i]);
            }
          else
            {
              sortBy1stFromLrgToSml(m_vVNMP[i],m_vVMpidP[i],m_vVIMP[i]);
            }
        }      
    }

  if(m_vCompIncDcyBr.size()>0)
    {
      for(unsigned int i=0;i<m_vCompIncDcyBr.size();i++)
        {
          if(m_ccSwitch==true)
            {
              sortBySumOf1stAnd2ndFromLrgToSml(m_vVNDcyBrIncDcyBr[i],m_vVNDcyBrCcIncDcyBr[i],m_vVDcyBrIncDcyBr[i],m_vVDcyBrCcIncDcyBr[i],m_vVIDcyBrIncDcyBr[i],m_vVIDcyBrCcIncDcyBr[i]);
            }
          else
            {
              sortBy1stFromLrgToSml(m_vVNDcyBrIncDcyBr[i],m_vVDcyBrIncDcyBr[i],m_vVIDcyBrIncDcyBr[i]);
            }
        }
    }

  if(m_vCompIRADcyBr.size()>0)
    {
      for(unsigned int i=0;i<m_vCompIRADcyBr.size();i++)
        {
          if(m_ccSwitch==true)
            {
              sortBySumOf1stAnd2ndFromLrgToSml(m_vVNDcyBrIRADcyBr[i],m_vVNDcyBrCcIRADcyBr[i],m_vVDcyBrIRADcyBr[i],m_vVDcyBrCcIRADcyBr[i],m_vVIDcyBrIRADcyBr[i],m_vVIDcyBrCcIRADcyBr[i]);
            }
          else
            {
              sortBy1stFromLrgToSml(m_vVNDcyBrIRADcyBr[i],m_vVDcyBrIRADcyBr[i],m_vVIDcyBrIRADcyBr[i]);
            }
        }
    }

  if(m_vSigDcyTr.size()>0&&m_anaTasksForSigIds!="T")
    {
      if(m_anaTasksForSigIds=="C")
        {
          char cut[50];
          for(unsigned int i=0;i<m_vSigDcyTr.size();i++)
            {
              if(m_ccSwitch==true)
                {
                  if(m_vICcSigDcyTr[i]==1)
                    {
                      sprintf(cut, "iSigDcyTr==%d&&iCcSigDcyTr==1", m_vISigDcyTr[i]);
                      m_vNSigDcyTr[i]=chn->GetEntries(cut);
                      sprintf(cut, "iSigDcyTr==%d&&iCcSigDcyTr==-1", m_vISigDcyTr[i]);
                      m_vNCcSigDcyTr[i]=chn->GetEntries(cut);
                    }
                  else
                    {
                      sprintf(cut, "iSigDcyTr==%d&&iCcSigDcyTr==0", m_vISigDcyTr[i]);
                      m_vNSigDcyTr[i]=chn->GetEntries(cut);
                      m_vNCcSigDcyTr[i]=0;
                    }
                }
              else
                {
                  sprintf(cut, "iSigDcyTr==%d", m_vISigDcyTr[i]);
                  m_vNSigDcyTr[i]=chn->GetEntries(cut);
                }
            }

          for(unsigned int i=0;i<m_vSigDcyIFSts_tr.size();i++)
            {
              if(m_ccSwitch==true)
                {
                  if(m_vICcSigDcyIFSts_tr[i]==1)
                    {
                      sprintf(cut, "iSigDcyIFSts_tr==%d&&iCcSigDcyIFSts_tr==1", m_vISigDcyIFSts_tr[i]);
                      m_vNSigDcyIFSts_tr[i]=chn->GetEntries(cut);
                      sprintf(cut, "iSigDcyIFSts_tr==%d&&iCcSigDcyIFSts_tr==-1", m_vISigDcyIFSts_tr[i]);
                      m_vNCcSigDcyIFSts_tr[i]=chn->GetEntries(cut);
                    }
                  else
                    {
                      sprintf(cut, "iSigDcyIFSts_tr==%d&&iCcSigDcyIFSts_tr==0", m_vISigDcyIFSts_tr[i]);
                      m_vNSigDcyIFSts_tr[i]=chn->GetEntries(cut);
                      m_vNCcSigDcyIFSts_tr[i]=0;
                    }
                }
              else
                {
                  sprintf(cut, "iSigDcyIFSts_tr==%d", m_vISigDcyIFSts_tr[i]);
                  m_vNSigDcyIFSts_tr[i]=chn->GetEntries(cut);
                }
            }
        }

      if(m_sortSigsInTopoMapsRltdToSigIds==true)
        {
          if(m_ccSwitch==true)
            {
              sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigDcyTr,m_vNCcSigDcyTr,m_vSigDcyTr,m_vCcSigDcyTr,m_vISigDcyTr,m_vICcSigDcyTr);
              sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigDcyIFSts_tr,m_vNCcSigDcyIFSts_tr,m_vSigDcyIFSts_tr,m_vCcSigDcyIFSts_tr,m_vISigDcyIFSts_tr,m_vICcSigDcyIFSts_tr);
            }
          else
            {
              sortBy1stFromLrgToSml(m_vNSigDcyTr,m_vSigDcyTr,m_vISigDcyTr);
              sortBy1stFromLrgToSml(m_vNSigDcyIFSts_tr,m_vSigDcyIFSts_tr,m_vISigDcyIFSts_tr);
            }
        }
    }

  if(m_vSigDcyIFSts.size()>0&&m_anaTasksForSigIds!="T")
    {
      if(m_anaTasksForSigIds=="C")
        {
          char cut[50];
          for(unsigned int i=0;i<m_vSigDcyIFSts.size();i++)
            {
              if(m_ccSwitch==true)
                {
                  if(m_vICcSigDcyIFSts[i]==1)
                    {
                      sprintf(cut, "iSigDcyIFSts==%d&&iCcSigDcyIFSts==1", m_vISigDcyIFSts[i]);
                      m_vNSigDcyIFSts[i]=chn->GetEntries(cut);
                      sprintf(cut, "iSigDcyIFSts==%d&&iCcSigDcyIFSts==-1", m_vISigDcyIFSts[i]);
                      m_vNCcSigDcyIFSts[i]=chn->GetEntries(cut);
                    }
                  else
                    {
                      sprintf(cut, "iSigDcyIFSts==%d&&iCcSigDcyIFSts==0", m_vISigDcyIFSts[i]);
                      m_vNSigDcyIFSts[i]=chn->GetEntries(cut);
                      m_vNCcSigDcyIFSts[i]=0;
                    }
                }
              else
                {
                  sprintf(cut, "iSigDcyIFSts==%d", m_vISigDcyIFSts[i]);
                  m_vNSigDcyIFSts[i]=chn->GetEntries(cut);
                }
            }
        }

      if(m_sortSigsInTopoMapsRltdToSigIds==true)
        {
          if(m_ccSwitch==true)
            {
              sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigDcyIFSts,m_vNCcSigDcyIFSts,m_vSigDcyIFSts,m_vCcSigDcyIFSts,m_vISigDcyIFSts,m_vICcSigDcyIFSts);
            }
          else
            {      
              sortBy1stFromLrgToSml(m_vNSigDcyIFSts,m_vSigDcyIFSts,m_vISigDcyIFSts);
            }
        }
    }

  if(m_vPid_sigP.size()>0&&m_anaTasksForSigIds!="T")
    {  
      countAndSort(chn, m_vPid_sigP.size(), "SigP", m_vNm_sigP, m_vNSigP, m_vNCcSigP, m_vPid_sigP, m_vPid_ccSigP, m_vISigP, m_vICcSigP);
    }

  if(m_vSigDcyBr.size()>0&&m_anaTasksForSigIds!="T")
    { 
      countAndSort(chn, m_vSigDcyBr.size(), "SigDcyBr", m_vNm_sigDcyBr, m_vNSigDcyBr, m_vNCcSigDcyBr, m_vSigDcyBr, m_vCcSigDcyBr, m_vISigDcyBr, m_vICcSigDcyBr);
    }

  if(m_vSigIncDcyBr.size()>0&&m_anaTasksForSigIds!="T")
    { 
      countAndSort(chn, m_vSigIncDcyBr.size(), "SigIncDcyBr", m_vNm_sigIncDcyBr, m_vNSigIncDcyBr, m_vNCcSigIncDcyBr, m_vSigIncDcyBr, m_vCcSigIncDcyBr, m_vISigIncDcyBr, m_vICcSigIncDcyBr);
    }

  if(m_vSigCascDcyBr.size()>0&&m_anaTasksForSigIds!="T")
    {
      countAndSort(chn, m_vSigCascDcyBr.size(), "SigCascDcyBr", m_vNm_sigCascDcyBr, m_vNSigCascDcyBr, m_vNCcSigCascDcyBr, m_vSigCascDcyBr, m_vCcSigCascDcyBr, m_vISigCascDcyBr, m_vICcSigCascDcyBr, &m_vVSigCascDcyBrIdxOfHead, &m_vVCcSigCascDcyBrIdxOfHead, &m_vVSigCascDcyBrMidxOfHead, &m_vVCcSigCascDcyBrMidxOfHead);
    }

  if(m_vSigIncCascDcyBr.size()>0&&m_anaTasksForSigIds!="T")
    {
      countAndSort(chn, m_vSigIncCascDcyBr.size(), "SigIncCascDcyBr", m_vNm_sigIncCascDcyBr, m_vNSigIncCascDcyBr, m_vNCcSigIncCascDcyBr, m_vSigIncCascDcyBr, m_vCcSigIncCascDcyBr, m_vISigIncCascDcyBr, m_vICcSigIncCascDcyBr, &m_vVSigIncCascDcyBrIdxOfHead, &m_vVCcSigIncCascDcyBrIdxOfHead, &m_vVSigIncCascDcyBrMidxOfHead, &m_vVCcSigIncCascDcyBrMidxOfHead);
    }

  if(m_vSigIRADcyBr.size()>0&&m_anaTasksForSigIds!="T")
    {
      countAndSort(chn, m_vSigIRADcyBr.size(), "SigIRADcyBr", m_vNm_sigIRADcyBr, m_vNSigIRADcyBr, m_vNCcSigIRADcyBr, m_vSigIRADcyBr, m_vCcSigIRADcyBr, m_vISigIRADcyBr, m_vICcSigIRADcyBr);
    }

  if(m_vSigIncOrIRACascDcyBr.size()>0&&m_anaTasksForSigIds!="T")
    {
      countAndSort(chn, m_vSigIncOrIRACascDcyBr.size(), "SigIncOrIRACascDcyBr", m_vNm_sigIncOrIRACascDcyBr, m_vNSigIncOrIRACascDcyBr, m_vNCcSigIncOrIRACascDcyBr, m_vSigIncOrIRACascDcyBr, m_vCcSigIncOrIRACascDcyBr, m_vISigIncOrIRACascDcyBr, m_vICcSigIncOrIRACascDcyBr, &m_vVSigIncOrIRACascDcyBrIdxOfHead, &m_vVCcSigIncOrIRACascDcyBrIdxOfHead, &m_vVSigIncOrIRACascDcyBrMidxOfHead, &m_vVCcSigIncOrIRACascDcyBrMidxOfHead);
    }

  delete chn; // This statement is indispensable, or a tough problem will arise before the "return 0;" statement in the main function.

  for(unsigned int j=0;j<Nasterisks;j++) cout<<"*";
  cout<<endl<<endl;
}
