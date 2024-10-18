#ifndef EVENT_DISPLAY_H 
#define EVENT_DISPLAY_H 
#include<iostream> 
#include<string>
#include<vector>
#include<cmath>
#include<algorithm> 
#include <map>
#include <utility>  // for std::pair
#include <variant>
#include <fstream>
#include <sstream>
#include "TScatter.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TH2F.h"

class EventDisplay{
public:
    EventDisplay(){return;};
    EventDisplay(const std::string& mPMTOrientations_csv, const std::string& mPMT_2D_projection_csv,const std::string& cardIDChannelMappingFile);
    TScatter* makeEventDisplayPlot(std::string graphName,std::vector<int> cardID_vector, std::vector<int> channelID_vector, std::vector<double> hit_data);
    
    std::vector<std::vector<int>> debug_getcardIDChannelMapping(){return cardIDChannelMapping;}

    // void setMaskedMPMTList(std::vector<int> masked_mpmts_list) : masked_mpmts_list(masked_mpmts_list){return;}

    // void plotEventDisplay();

private:
    void loadConfigurationFiles(const std::string& mPMTOrientations_csv, const std::string& mPMT_2D_projection_csv,const std::string& cardIDChannelMappingFile);
    void initialiseChannelPositions();
    std::vector<double> pmt_offset_from_mPMT(int pmt_position, double rotation_about_z);
    double getEventDisplayOrientation(int mPMT_id);
    std::vector<std::vector<double>> ReadCSV_double(const std::string& filename);
    std::vector<std::vector<int>> ReadCSV_int(const std::string& filename);
    void makeCardIDMap(); 
    std::tuple<int, std::vector<double>, std::vector<double>, std::vector<double>> processData(std::vector<int> cardID_vector, std::vector<int> channelID_vector, std::vector<double> hit_data, bool sum_data);

    std::vector<std::vector<double>> mPMTOrientations;
    std::vector<std::vector<double>> mPMT_2D_projection;
    std::vector<std::vector<int>> cardIDChannelMapping;

    //map each card id and channel to a unique index
    std::map<int, int> cardIDChannelIndex;
    //map each unique index to x y location
    std::map<int, std::pair<double, double>> cardIDChannelIndexPositionMap;

    
    int n_mPMTs;
    //total number of channels as defined as the length of the cardIDChannelMap

    int total_channels;
    std::vector<double> x_pmt;
    std::vector<double> y_pmt;

    // std::vector<int> masked_mpmts_list;

};

#endif