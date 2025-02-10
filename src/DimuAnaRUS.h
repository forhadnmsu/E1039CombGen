#ifndef _DIMU_ANA_RUS_H_
#define _DIMU_ANA_RUS_H_
#include <fun4all/SubsysReco.h>
#include <UtilAna/TrigRoadset.h>

class TFile;
class TTree;
class SQRun;
class SQSpillMap;
class SQEvent;
class SQHitVector;
class SQTrackVector;
class SQDimuonVector;

class DimuAnaRUS: public SubsysReco {
	SQEvent* m_evt;
	SQSpillMap* m_sp_map;
	SQHitVector* m_hit_vec;
	SQTrackVector * m_vec_trk;
	SQTrackVector*  m_sq_trk_vec;
	SQDimuonVector* m_sq_dim_vec;
	SQDimuonVector* m_true_vec_dim;
	SQRun* sq_run;

	std::string m_file_name;
	std::string m_tree_name;
	TFile*      m_file;
	TTree*      m_tree;
	public:

	UtilTrigger::TrigRoadset m_rs;
	bool true_mode;
	bool reco_mode;
	bool data_trig_mode;
	bool mc_trig_mode;
	bool saveDimuonOnly;
	bool true_dimu_mode;

	void SetMCTrueMode(bool enable) { true_mode = enable; }
	void SetMCDimuonMode(bool enable) { true_dimu_mode = enable; }
	void SetRecoMode(bool enable) { reco_mode = enable; }
	void SetDataTriggerEmu(bool enable) { 
		data_trig_mode = enable; 
		mc_trig_mode = !enable; // Automatically disable mc_trig_mode when data_trig_mode is enabled
	}   

	void SetMCTriggerEmu(bool enable) { 
		mc_trig_mode = enable; 
		data_trig_mode = !enable; // Automatically disable data_trig_mode when mc_trig_mode is enabled
	}   

	void SetSaveOnlyDimuon(bool enable) { saveDimuonOnly = enable; }

	DimuAnaRUS(const std::string& name="DimuAnaRUS");
	virtual ~DimuAnaRUS();
	int Init(PHCompositeNode *startNode);
	int InitRun(PHCompositeNode *startNode);
	int process_event(PHCompositeNode *startNode);
	int End(PHCompositeNode *startNode);
	void SetOutputFileName(const std::string name) { m_file_name = name; }

	void SetTreeName(const std::string& name) { m_tree_name = name; }
	void SetFileName(const std::string& name) { m_file_name = name; }
	void ResetHitBranches();
	void ResetTrueBranches();
	void ResetRecoBranches();

	private:

	// Scalar variables
	int eventID;
	/*
	int runID;
	int spillID;
	int eventID;
	int rfID;
	int turnID;
	int rfIntensity[33];
	int fpgaTrigger[5] = {0};
	int nimTrigger[5] = {0};
	*/

	std::vector<int> hitID;
	std::vector<int> trackID;
	std::vector<int> detectorID;
	std::vector<int> elementID;
	std::vector<double> tdcTime;
	std::vector<double> driftDistance;
	std::vector<int> processID;
	std::vector<int> sourceFlag;
	//std::vector<bool> hitsInTime;
	// True track data
	std::vector<int> gCharge;
	//std::vector<int> processID;
	//std::vector<int> sourceFlag;
	std::vector<double> gvx;
	std::vector<double> gvy;
	std::vector<double> gvz;
	std::vector<double> gpx;
	std::vector<double> gpy;
	std::vector<double> gpz;

	std::vector<double> rec_dimu_vx;
        std::vector<double> rec_dimu_vy;
        std::vector<double> rec_dimu_vz;
        std::vector<double> rec_dimu_px;
        std::vector<double> rec_dimu_py;
        std::vector<double> rec_dimu_pz;
        std::vector<double> rec_dimu_mass;
        std::vector<double> rec_dimu_x1;
        std::vector<double> rec_dimu_x2;
        std::vector<double> rec_dimu_xf;

        std::vector<double> rec_vx_mup;
        std::vector<double> rec_vy_mup;
        std::vector<double> rec_vz_mup;
        std::vector<double> rec_px_mup;
        std::vector<double> rec_py_mup;
        std::vector<double> rec_pz_mup;

	std::vector<double> rec_vx_mum;
        std::vector<double> rec_vy_mum;
        std::vector<double> rec_vz_mum;
        std::vector<double> rec_px_mum;
        std::vector<double> rec_py_mum;
        std::vector<double> rec_pz_mum;

	std::vector<bool> top_bot;
	std::vector<bool> bot_top;
};

#endif // _DimuAnaRUS.h_
