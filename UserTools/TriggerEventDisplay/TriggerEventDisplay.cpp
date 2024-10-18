#include "TriggerEventDisplay.h"
#include <vector>
#include"TGraph.h"
#include "TBufferJSON.h"

TriggerEventDisplay::TriggerEventDisplay():Tool(){}


bool TriggerEventDisplay::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  InitialiseConfiguration(configfile);

    //m_variables.Print();

  m_data= &data;

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  eventDisplay = EventDisplay("mPMTOrientations.csv","mPMT_2D_projection.csv","cardIDMapping.csv");  
  iEv =0; 
 

  
  return true;
}


bool TriggerEventDisplay::Execute(){
  // if(m_data->currentReadoutWindow==nullptr) return true;

  std::cout << "Call execute loop for monitoring program "<< std::endl;
  //always process the last event
  std::cout << "Readout window vector size" <<m_data->readout_window_vector.size() << std::endl;
  std::cout << "First Hit card " <<m_data->currentReadoutWindow->mpmt_hits[0].GetCardID() << " ch "<< m_data->currentReadoutWindow->mpmt_hits[0].GetChannel() << std::endl;
  
  std::string graphName = "Event display "+std::to_string(iEv);
  
  std::vector<int> hit_card_id;
  std::vector<int> hit_chanel_id;
  std::vector<double> hit_charge;
  
  for(int i=0; i<m_data->currentReadoutWindow->mpmt_hits.size(); i++){
    hit_chanel_id.push_back(m_data->currentReadoutWindow->mpmt_hits[i].GetChannel());
    hit_card_id.push_back(m_data->currentReadoutWindow->mpmt_hits[i].GetCardID());
    hit_charge.push_back(m_data->currentReadoutWindow->mpmt_hits[i].GetCharge());
  }
  std::cout << "Call makeEventDisplayPlot " << std::endl;
  TScatter* eventDisplayScatter = eventDisplay.makeEventDisplayPlot(graphName,hit_card_id,hit_chanel_id,hit_charge);
  eventDisplayScatter->SetMarkerStyle(20);

  // convert the TGraph to JSON
	std::cout<<"Converting to JSON"<<std::endl;
	std::string graph_json = TBufferJSON::ToJSON(eventDisplayScatter).Data();
	
  std::string graph_draw_options = "AP";
	
	// each time the same unique name is inserted, a new version of the plot is stored.
	// when saving a new version over TCP we may optionally pass a pointer to an integer,
	// which will be populated with the version number of the created entry.
	int graph_db_ver=0;
	
	// store it in the database
	// For now this setting only affects whether the request is sent over multicast (false, default) or tcp (true)
	// At a later date we may add the option for plots to only be stored in a temporary table.
	std::cout<<"Sending to database via TCP"<<std::endl;

  // m_data->services->SendTemporaryROOTplot(graphName, graph_draw_options, graph_json, &graph_db_ver, 0);
  // SendTemporaryROOTplot(plot_name, draw_options, json_data, version, timestamp);
	// bool ok = DAQ_inter.SendROOTplot(graph_name, graph_draw_options, graph_json, persistent, &graph_db_ver);

  // delete canvas;
  delete eventDisplayScatter;


  iEv++;
  return true;
}


bool TriggerEventDisplay::Finalise(){
  std::cout <<"Finalise TriggerEventDisplay" << std::endl;
  return true;
}

