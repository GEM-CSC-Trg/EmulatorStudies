// system include files
#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigi.h"

using ChamToDigiMap = std::map<CSCDetId, std::vector<CSCCorrelatedLCTDigi>>;


class TriggerPrimitiveReader : public edm::one::EDAnalyzer<> 
{
public:
  	explicit TriggerPrimitiveReader(const edm::ParameterSet&);
  	~TriggerPrimitiveReader() override;

  	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
 	void analyze(const edm::Event&, const edm::EventSetup&) override;
	ChamToDigiMap FillCmpMap(edm::Handle<CSCCorrelatedLCTDigiCollection> const& handle);
	std::pair<int, int> GetCmpCounts(ChamToDigiMap const& data_map, ChamToDigiMap const& emul_map);

	edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> m_emul_token;
	edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> m_data_token;

	bool debug;
};


TriggerPrimitiveReader::TriggerPrimitiveReader(const edm::ParameterSet& iConfig) 
{
	std::string data_str = iConfig.getUntrackedParameter<std::string>("Data", "muonCSCDigis");
    std::string emul_str = iConfig.getUntrackedParameter<std::string>("Emul", "cscTriggerPrimitiveDigis");

	m_data_token = consumes<CSCCorrelatedLCTDigiCollection>(edm::InputTag(data_str, "MuonCSCCorrelatedLCTDigi"));
	m_emul_token = consumes<CSCCorrelatedLCTDigiCollection>(edm::InputTag(emul_str));

	debug = iConfig.getParameter<bool>("debug");
}

TriggerPrimitiveReader::~TriggerPrimitiveReader() {}

ChamToDigiMap TriggerPrimitiveReader::FillCmpMap(edm::Handle<CSCCorrelatedLCTDigiCollection> const& handle)
{
	std::map<CSCDetId, std::vector<CSCCorrelatedLCTDigi>> res;

	for (auto it = handle->begin(); it != handle->end(); ++it)
	{
		auto const& [chamber_id, digi_range] = *it;
		auto [begin, end] = digi_range;
		
		res[chamber_id] = std::vector<CSCCorrelatedLCTDigi>(begin, end);
	}

	return res;
}

std::pair<int, int> TriggerPrimitiveReader::GetCmpCounts(ChamToDigiMap const& data_map, ChamToDigiMap const& emul_map)
{
	int match_cnt = 0;
	int diff_cnt = 0;

	// loop over data map and compare digis
	for (auto const& [id, data_digi_vec]: data_map)
	{
		if (emul_map.count(id) == 0)
		{
			diff_cnt += data_digi_vec.size();
			continue;
		}

		for (auto const& digi: data_digi_vec)
		{
			std::cout << digi.getBX() << "\n";
		}

		auto const& emul_digi_vec = emul_map.at(id);

		if (debug)
		{
			std::cout << "chamber: " << id << "\n";
			std::cout << "emulator digis:\n";	
			for (auto const& digi: emul_digi_vec)
			{
				std::cout << digi << "\n";
			}

			std::cout << "data digis:\n";	
			for (auto const& digi: data_digi_vec)
			{
				std::cout << digi << "\n";
			}

			std::cout << "===============================\n";
		}

		if (emul_digi_vec == data_digi_vec)
		{
			match_cnt += data_digi_vec.size();
		}
		else 
		{
			for (auto const& digi: data_digi_vec)
			{
				auto it = std::find(emul_digi_vec.cbegin(), emul_digi_vec.cend(), digi);
				it != emul_digi_vec.cend() ? ++match_cnt : ++diff_cnt;
			}
		}
	}

	return {match_cnt, diff_cnt};
}

void TriggerPrimitiveReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) 
{
	edm::Handle<CSCCorrelatedLCTDigiCollection> emul_handle;
	edm::Handle<CSCCorrelatedLCTDigiCollection> data_handle;

  	iEvent.getByToken(m_emul_token, emul_handle);
	iEvent.getByToken(m_data_token, data_handle);

	auto data_map = FillCmpMap(data_handle);
	auto emul_map = FillCmpMap(emul_handle);

	auto [match_cnt, diff_cnt] = GetCmpCounts(data_map, emul_map);
	
	if (debug)
	{
		std::cout << "\tmatch count=" << match_cnt << ", diff count=" << diff_cnt << "\n";
	}
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void TriggerPrimitiveReader::fillDescriptions(edm::ConfigurationDescriptions& descriptions) 
{
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TriggerPrimitiveReader);
