#include <fstream>
#include <iomanip>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <interface_main/SQRun.h>
#include <interface_main/SQHitVector.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQRun.h>
#include <interface_main/SQTrackVector.h>
#include <interface_main/SQDimuonVector.h>
#include <ktracker/SRecEvent.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHIODataNode.h>
#include <phool/getClass.h>
#include <geom_svc/GeomSvc.h>
#include "DimuAnaRUS.h"
using namespace std;

DimuAnaRUS::DimuAnaRUS(const std::string& name)
  : SubsysReco  (name),
    m_file(0),
    m_tree(0),
    m_tree_name("tree"),
    m_file_name("output.root"),
    m_evt(0),
    m_sp_map(0),
    m_hit_vec(0),
    m_sq_trk_vec(0),
    m_sq_dim_vec(0),
    saveDimuonOnly(false),
    true_mode(false),
    reco_mode(false),
    data_trig_mode(false),
    mc_trig_mode(true)
{
  ;
}

DimuAnaRUS::~DimuAnaRUS()
{
  ;
}

int DimuAnaRUS::Init(PHCompositeNode* startNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int DimuAnaRUS::InitRun(PHCompositeNode* startNode)
{
	m_file = new TFile(m_file_name.c_str(), "RECREATE");

	if (!m_file || m_file->IsZombie()) {
		std::cerr << "Error: Could not create file " << m_file_name << std::endl;
		exit(1);
	} else {
		std::cout << "File " << m_file->GetName() << " opened successfully." << std::endl;
	}

	m_tree = new TTree(m_tree_name.c_str(), "Tree for storing events");
	if (!m_tree) {
		std::cerr << "Error: Could not create tree " << m_tree_name << std::endl;
		exit(1);
	} else {
		std::cout << "Tree " << m_tree->GetName() << " created successfully." << std::endl;
	}

	m_tree->Branch("eventID", &eventID, "eventID/I");
/*
	m_tree->Branch("runID", &runID, "runID/I");
	m_tree->Branch("spillID", &spillID, "spillID/I");
	m_tree->Branch("eventID", &eventID, "eventID/I");
	m_tree->Branch("rfID", &rfID, "rfID/I");
	m_tree->Branch("turnID", &turnID, "turnID/I");
	m_tree->Branch("rfIntensity", rfIntensity, "rfIntensity[33]/I");
	m_tree->Branch("fpgaTrigger", fpgaTrigger, "fpgaTrigger[5]/I");
	m_tree->Branch("nimTrigger", nimTrigger, "nimTrigger[5]/I");
*/
	m_tree->Branch("hitID", &hitID);
	m_tree->Branch("hit_trackID", &hit_trackID);
	m_tree->Branch("processID", &processID);
	m_tree->Branch("detectorID", &detectorID);
	m_tree->Branch("elementID", &elementID);
	m_tree->Branch("tdcTime", &tdcTime);
	m_tree->Branch("driftDistance", &driftDistance);

	if (true_mode==true){
		m_tree->Branch("gCharge", &gCharge);
		m_tree->Branch("trackID", &trackID);
		m_tree->Branch("gvx", &gvx);
		m_tree->Branch("gvy", &gvy);
		m_tree->Branch("gvz", &gvz);
		m_tree->Branch("gpx", &gpx);
		m_tree->Branch("gpy", &gpy);
		m_tree->Branch("gpz", &gpz);
	}
	if (reco_mode==true){
		m_tree->Branch("rec_vx_mup", &rec_vx_mup);
		m_tree->Branch("rec_vy_mup", &rec_vy_mup);
		m_tree->Branch("rec_vz_mup", &rec_vz_mup);
		m_tree->Branch("rec_px_mup", &rec_px_mup);
		m_tree->Branch("rec_py_mup", &rec_py_mup);
		m_tree->Branch("rec_pz_mup", &rec_pz_mup);

		m_tree->Branch("rec_vx_mum", &rec_vx_mum);
		m_tree->Branch("rec_vy_mum", &rec_vy_mum);
		m_tree->Branch("rec_vz_mum", &rec_vz_mum);
		m_tree->Branch("rec_px_mum", &rec_px_mum);
		m_tree->Branch("rec_py_mum", &rec_py_mum);
		m_tree->Branch("rec_pz_mum", &rec_pz_mum);

		m_tree->Branch("rec_dimu_vx", &rec_dimu_vx);
		m_tree->Branch("rec_dimu_vy", &rec_dimu_vy);
		m_tree->Branch("rec_dimu_vz", &rec_dimu_vz);
		m_tree->Branch("rec_dimu_px", &rec_dimu_px);
		m_tree->Branch("rec_dimu_py", &rec_dimu_py);
		m_tree->Branch("rec_dimu_pz", &rec_dimu_pz);
		m_tree->Branch("rec_dimu_mass", &rec_dimu_mass);
		m_tree->Branch("rec_dimu_xf", &rec_dimu_xf);
		m_tree->Branch("rec_dimu_x1", &rec_dimu_x1);
		m_tree->Branch("rec_dimu_x2", &rec_dimu_x2);
		m_tree->Branch("top_bot", &top_bot);
		m_tree->Branch("bot_top", &bot_top);
	}

	if (true_mode) {
		m_vec_trk = findNode::getClass<SQTrackVector>(startNode, "SQTruthTrackVector");
		if (!m_vec_trk) {
			return Fun4AllReturnCodes::ABORTEVENT;
		}
	}

	if (reco_mode) {
		m_sq_trk_vec = findNode::getClass<SQTrackVector>(startNode, "SQRecTrackVector");
		m_sq_dim_vec = findNode::getClass<SQDimuonVector>(startNode, "SQRecDimuonVector_PM");

		if (!m_sq_trk_vec) {
			std::cerr << "Error: m_sq_trk_vec is null!" << std::endl;
			return Fun4AllReturnCodes::ABORTEVENT;
		}

		if (!m_sq_dim_vec) {
			std::cerr << "Error: m_sq_dim_vec is null!" << std::endl;
			return Fun4AllReturnCodes::ABORTEVENT;
		}
	}
	m_evt = findNode::getClass<SQEvent>(startNode, "SQEvent");
	m_hit_vec = findNode::getClass<SQHitVector>(startNode, "SQHitVector");
	if (!m_evt || !m_hit_vec) {
		return Fun4AllReturnCodes::ABORTEVENT;
	}

	if(reco_mode ==true && data_trig_mode ==true){
		cout << "inside the data roadset mode: "<<endl;
		SQRun* sq_run = findNode::getClass<SQRun>(startNode, "SQRun");
		if (!sq_run) std::cout << "Error: SQRun  is null!" << std::endl;
		if (!sq_run) return Fun4AllReturnCodes::ABORTEVENT;
		int LBtop = sq_run->get_v1495_id(2);
		int LBbot = sq_run->get_v1495_id(3);
		int ret = m_rs.LoadConfig(LBtop, LBbot);
		if (ret != 0) {
			cout << "!!WARNING!!  OnlMonTrigEP::InitRunOnlMon():  roadset.LoadConfig returned " << ret << ".\n";
		}
		cout <<"Roadset " << m_rs.str(1) << endl;

	}

	cout << "beginning of mc trig "<< endl;
	if(reco_mode ==true && mc_trig_mode ==true){
		int ret = m_rs.LoadConfig(131);
		if (ret != 0) {
			cout << "!!WARNING!!  OnlMonTrigEP::InitRunOnlMon():  roadset.LoadConfig returned " << ret << ".\n";
		}
		//cout <<"Roadset " << m_rs.str(1) << endl;
	}
	return Fun4AllReturnCodes::EVENT_OK;
}


int DimuAnaRUS::process_event(PHCompositeNode* startNode)
{
/*
	if (! m_evt->get_trigger(SQEvent::MATRIX1)) {
		return Fun4AllReturnCodes::EVENT_OK;
 	 }
*/
	if (saveDimuonOnly && m_sq_dim_vec->empty()) {
		return 0;  // Skip this event if no dimuons are present and the mode is enabled
        }

	eventID = m_evt->get_event_id();
/*
	runID = m_evt->get_run_id();
	spillID = m_evt->get_spill_id();
	rfID = m_evt->get_qie_rf_id();
	turnID = m_evt->get_qie_turn_id();

	fpgaTrigger[0] = m_evt->get_trigger(SQEvent::MATRIX1);
	fpgaTrigger[1] = m_evt->get_trigger(SQEvent::MATRIX2);
	fpgaTrigger[2] = m_evt->get_trigger(SQEvent::MATRIX3);
	fpgaTrigger[3] = m_evt->get_trigger(SQEvent::MATRIX4);
	fpgaTrigger[4] = m_evt->get_trigger(SQEvent::MATRIX5);

	nimTrigger[0] = m_evt->get_trigger(SQEvent::NIM1);
	nimTrigger[1] = m_evt->get_trigger(SQEvent::NIM2);
	nimTrigger[2] = m_evt->get_trigger(SQEvent::NIM3);
	nimTrigger[3] = m_evt->get_trigger(SQEvent::NIM4);
	nimTrigger[4] = m_evt->get_trigger(SQEvent::NIM5);
	for (int i = -16; i <= 16; ++i) {
    	rfIntensity[i + 16] = m_evt->get_qie_rf_intensity(i);
}
*/

if(true_mode == true){
	ResetTrueBranches();
	ResetHitBranches();
	for (unsigned int ii = 0; ii < m_vec_trk->size(); ii++) {
		SQTrack* trk = m_vec_trk->at(ii);

		gCharge.push_back(trk->get_charge());
		trackID.push_back(trk->get_track_id());
		gvx.push_back(trk->get_pos_vtx().X());
		gvy.push_back(trk->get_pos_vtx().Y());
		gvz.push_back(trk->get_pos_vtx().Z());
		gpx.push_back(trk->get_mom_vtx().Px());
		gpy.push_back(trk->get_mom_vtx().Py());
		gpz.push_back(trk->get_mom_vtx().Pz());
		if (m_hit_vec) {
			for (int ihit = 0; ihit < m_hit_vec->size(); ++ihit) {
				SQHit* hit = m_hit_vec->at(ihit);
				if(hit->get_track_id() != trk->get_track_id()) continue;
				int processID_;
				if(trk->get_charge() >0) processID_ =14;
				else processID_ =24;
				int sourceFlag_= 2;
				unsigned int encodedValue = EncodeProcess(processID_, sourceFlag_);
				cout << "charge: "<< trk->get_charge() <<endl;
				cout << "encodedValue: "<< encodedValue<< endl;
				cout << "DecodeSourceFlag: "<< DecodeSourceFlag(encodedValue) << endl;
				cout << "DecodeProcessID: "<< DecodeProcessID(encodedValue) << endl;
				processID.push_back(encodedValue);
				hitID.push_back(hit->get_hit_id());
				hit_trackID.push_back(hit->get_track_id());
				detectorID.push_back(hit->get_detector_id());
				elementID.push_back(hit->get_element_id());
				tdcTime.push_back(hit->get_tdc_time());
				driftDistance.push_back(hit->get_drift_distance());
			}   
		}
	}
}

//cout << "reco mode: "<< reco_mode << endl;
//std::cout << "Size of m_sq_dim_vec: " << m_sq_dim_vec->size() << std::endl;

if(reco_mode == true){
    ResetRecoBranches();
    for (auto it = m_sq_dim_vec->begin(); it != m_sq_dim_vec->end(); it++) {
	    SRecDimuon& sdim = dynamic_cast<SRecDimuon&>(**it);
	    sdim.calcVariables(); //eventually need to assign 1 or 2 depending on the target or dump; more at https://github.com/E1039-Collaboration/e1039-core/pull/149
	    int trk_id_pos = sdim.get_track_id_pos();
	    int trk_id_neg = sdim.get_track_id_neg();
	    SRecTrack& trk_pos = dynamic_cast<SRecTrack&>(*(m_sq_trk_vec->at(trk_id_pos))); 
	    SRecTrack& trk_neg = dynamic_cast<SRecTrack&>(*(m_sq_trk_vec->at(trk_id_neg))); 
	    // Fill dimuon vectors
	    rec_dimu_vx.push_back(sdim.get_pos().X());
	    rec_dimu_vy.push_back(sdim.get_pos().Y());
	    rec_dimu_vz.push_back(sdim.get_pos().Z());
	    rec_dimu_px.push_back(sdim.get_mom().Px());
	    rec_dimu_py.push_back(sdim.get_mom().Py());
	    rec_dimu_pz.push_back(sdim.get_mom().Pz());
	    rec_dimu_mass.push_back(sdim.get_mom().M());
	    rec_dimu_x1.push_back(sdim.x1);
	    rec_dimu_x2.push_back(sdim.x2);
	    rec_dimu_xf.push_back(sdim.xF);
	    // Fill mu_plus vectors
	    rec_vx_mup.push_back(trk_pos.get_pos_vtx().X());
	    rec_vy_mup.push_back(trk_pos.get_pos_vtx().Y());
	    rec_vz_mup.push_back(trk_pos.get_pos_vtx().Z());
	    rec_px_mup.push_back(trk_pos.get_mom_vtx().X());
	    rec_py_mup.push_back(trk_pos.get_mom_vtx().Y());
	    rec_pz_mup.push_back(trk_pos.get_mom_vtx().Z());

	    rec_vx_mum.push_back(trk_neg.get_pos_vtx().X());
            rec_vy_mum.push_back(trk_neg.get_pos_vtx().Y());
            rec_vz_mum.push_back(trk_neg.get_pos_vtx().Z());
            rec_px_mum.push_back(trk_neg.get_mom_vtx().X());
            rec_py_mum.push_back(trk_neg.get_mom_vtx().Y());
            rec_pz_mum.push_back(trk_neg.get_mom_vtx().Z());

	    bool pos_top = m_rs.PosTop()->FindRoad(trk_pos.getTriggerRoad());
	    bool pos_bot = m_rs.PosBot()->FindRoad(trk_pos.getTriggerRoad());
	    bool neg_top = m_rs.NegTop()->FindRoad(trk_neg.getTriggerRoad());
	    bool neg_bot = m_rs.NegBot()->FindRoad(trk_neg.getTriggerRoad());
	    bool top_bot_ = pos_top && neg_bot;
	    bool bot_top_ =  pos_bot && neg_top;
	    top_bot.push_back(top_bot_);
	    bot_top.push_back(bot_top_);
    }   
}
	m_tree->Fill();
	return Fun4AllReturnCodes::EVENT_OK;
}

int DimuAnaRUS::End(PHCompositeNode* startNode)
{
  m_file->cd();
  m_file->Write();
  m_file->Close();  
  return Fun4AllReturnCodes::EVENT_OK;
}


void DimuAnaRUS::ResetHitBranches() {
 	hitID.clear();
	processID.clear();
 	hit_trackID.clear();
	detectorID.clear();
	elementID.clear();
	tdcTime.clear();
	driftDistance.clear();
}
void DimuAnaRUS::ResetTrueBranches() {
	//mc variables
	gCharge.clear();
 	trackID.clear();
	gvx.clear();
	gvy.clear();
	gvz.clear();
	gpx.clear();
	gpy.clear();
	gpz.clear();
}
void DimuAnaRUS::ResetRecoBranches() {
	//reco vearibles
	rec_vx_mup.clear();
        rec_vy_mup.clear();
        rec_vz_mup.clear();
        rec_px_mup.clear();
        rec_py_mup.clear();
        rec_pz_mup.clear();

        rec_vx_mum.clear();
        rec_vy_mum.clear();
        rec_vz_mum.clear();
        rec_px_mum.clear();
        rec_py_mum.clear();
        rec_pz_mum.clear();
	
	rec_dimu_vx.clear();
	rec_dimu_vy.clear();
	rec_dimu_vz.clear();
	rec_dimu_px.clear();
	rec_dimu_py.clear();
	rec_dimu_pz.clear();
	rec_dimu_mass.clear();
	rec_dimu_xf.clear();
	rec_dimu_x1.clear();
	rec_dimu_x2.clear();
	top_bot;
	bot_top;
}

unsigned int DimuAnaRUS::EncodeProcess(int processID, int sourceFlag) {
        return ( (sourceFlag & 0x3) << 5 ) |  // 2 bits for sourceFlag (1-3)
	                (processID & 0x1F);            // 5 bits for processID (11-24)
			               }

int DimuAnaRUS::DecodeSourceFlag(unsigned int encoded) { 
	return (encoded >> 5) & 0x3;
}
int DimuAnaRUS::DecodeProcessID(unsigned int encoded) {
	return encoded & 0x1F;
}

