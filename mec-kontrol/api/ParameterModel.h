#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include "Entity.h"
#include "Parameter.h"

namespace mec {
class Preferences;
}

namespace Kontrol {

// todo: parameters probably need to be
// slot:paramid
// do we need pageid? probably not

// possibly
// map<slotId, vector<page.vector<paramid>>
// map<slotId, map<paramid,parameter>
// slots dont need order (here!?), pages do need an order, parameters need order in page
// when broadcast, we will need slot/paramid
// change will also need slotid/paramid


class ParameterCallback {
public:
	ParameterCallback() {
		;
	}
	virtual ~ParameterCallback() {
		;
	}
	virtual void stop() { ; }
	virtual void addClient(const std::string& host, unsigned port) = 0;
	virtual void page(ParameterSource src, const Page&) = 0 ;
	virtual void param(ParameterSource src, const Parameter&) = 0;
	virtual void changed(ParameterSource src, const Parameter&) = 0;
};


class ParameterModel;

class ParameterModel {
public:
	static std::shared_ptr<ParameterModel> model();
	// static void free();

	void addClient(const std::string& host, unsigned port);
	bool addParam(ParameterSource src, const std::vector<ParamValue>& args);
	bool addPage(
	    ParameterSource src,
	    const std::string& id,
	    const std::string& displayName,
	    const std::vector<std::string> paramIds
	);

	bool  changeParam(ParameterSource src, const std::string& id, const ParamValue& value);

	void clearCallbacks() {
		for (auto p : listeners_) {
			(p.second)->stop();
		}

		listeners_.clear();
	}
	void removeCallback(const std::string& id) {
		auto p = listeners_.find(id);
		if (p != listeners_.end()) {
			(p->second)->stop();
			listeners_.erase(id);
		}
	}

	void removeCallback(std::shared_ptr<ParameterCallback>) {
		// for(std::vector<std::shared_ptr<ParameterCallback> >::iterator i(listeners_) : listeners_) {
		// 	if(*i == listener) {
		// 		i.remove();
		// 		return;
		// 	}
		// }
	}
	void addCallback(const std::string& id, std::shared_ptr<ParameterCallback> listener) {
		auto p = listeners_[id];
		if (p != nullptr) p->stop();
		listeners_[id] = listener;
	}

	void publishMetaData() const;

	unsigned 	getPageCount() { return pageIds_.size();}
	std::string getPageId(unsigned pageNum) { return pageNum < pageIds_.size() ? pageIds_[pageNum] : "";}
	std::shared_ptr<Page> getPage(const std::string& pageId) { return pages_[pageId]; }

	std::string getParamId(const std::string& pageId, unsigned paramNum);
	std::shared_ptr<Parameter> getParam(const std::string paramId) { return parameters_[paramId]; }

	bool loadParameterDefinitions(const std::string& filename);
	bool loadParameterDefinitions(const mec::Preferences& prefs);

	bool loadPatchSettings(const std::string& filename);
	bool loadPatchSettings(const mec::Preferences& prefs);

	bool savePatchSettings();
	bool savePatchSettings(const std::string& filename);

	void dumpParameters();
	void dumpCurrentValues();
	void dumpPatchSettings();

	bool applyPreset(std::string presetId);
	bool savePreset(std::string presetId);
	bool changeMidiCC(unsigned midiCC, unsigned midiValue);
	void addMidiCCMapping(unsigned midiCC, std::string paramId);
	std::string currentPreset() { return currentPreset_;}
	std::vector<std::string> getPresetList();


private:
	ParameterModel();
	std::string patchSettingsFile_;
	std::string currentPreset_;

	std::shared_ptr<mec::Preferences> paramDefinitions_;
	std::shared_ptr<mec::Preferences> patchSettings_;

	std::unordered_map<unsigned, std::string> midi_mapping_; // key CC id, value = paramId
	std::unordered_map<std::string, std::vector<Preset>> presets_; // key = presetid

	std::vector<std::string> pageIds_; // ordered list of page id, for presentation

	std::unordered_map<std::string, std::shared_ptr<Parameter> >parameters_; // key = paramId
	std::unordered_map<std::string, std::shared_ptr<Page> >pages_; // key = pageId

	std::unordered_map<std::string, std::shared_ptr<ParameterCallback> > listeners_; // key = source : host:ip
};

} //namespace
