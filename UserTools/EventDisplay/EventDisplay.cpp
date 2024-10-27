#include "EventDisplay.h"

EventDisplay::EventDisplay(const std::string& mPMTOrientations_csv, const std::string& mPMT_2D_projection_csv,const std::string& cardIDChannelMappingFile){
    std::cout << "Initialising EventDisplay" << std::endl;
    //first load all the files
    this->loadConfigurationFiles(mPMTOrientations_csv, mPMT_2D_projection_csv,cardIDChannelMappingFile);

    //gets an x y position on the event display for each slot id and pmt position
    this->initialiseChannelPositions();

    //make a map for each card id and each channel id to a physical location
    this->makeCardIDMap();
    std::cout << "Initialised EventDisplay" << std::endl;

}

void EventDisplay::loadConfigurationFiles(const std::string& mPMTOrientations_csv, const std::string& mPMT_2D_projection_csv,const std::string& cardIDChannelMappingFile){
    std::string directory = "UserTools/EventDisplay/inputs/";
    mPMTOrientations = ReadCSV_double(directory+mPMTOrientations_csv);
    mPMT_2D_projection = ReadCSV_double(directory+mPMT_2D_projection_csv);
    cardIDChannelMapping = ReadCSV_int(directory+cardIDChannelMappingFile);
    //total number of mPMTs before any slots are removed
    n_mPMTs=mPMT_2D_projection[1].size();

}

std::vector<double> EventDisplay::pmt_offset_from_mPMT(int pmt_position, double rotation_about_z){
    //return the 2d offset of a specific pmt_position in an mpmt
    //rotation_about_z is the orientation of the mPMT on the event display 
    double theta = 0;
    double radius = 0;
    double pi = 3.1415927;
    double x_offset =0;
    double y_offset=0;
    if(pmt_position==0){
        return {x_offset,y_offset};
    }
    if(pmt_position>6){
        //outer ring 
        radius =0.4;
        theta=2*pi*(19-pmt_position)/12;

    }else if(pmt_position>0 && pmt_position <= 6){
        //inner ring
        radius =0.2;
        theta=2*pi*(7-pmt_position)/6;
    }else{
        std::cout << "Bad PMT position given " << std::endl;
        throw("Error in pmt_offset_from_mPMT");
    }

    return {radius*sin(theta-rotation_about_z),radius*cos(theta-rotation_about_z)};
}

double EventDisplay::getEventDisplayOrientation(int mPMT_id){
    //gets the mPMT orientation for the event display from the orientations file
    //uses the mpmt_id (slot number) to figure out where we are in the detector
    //returns an angle to rotate the mPMT in the event display from the default mPMT layout
    //(mpmt x dir along x axis)
    double pi = 3.1415927;
    if(mPMT_id>20 && mPMT_id<85){
        //barrel mPMT +y = +y in detector
        if(mPMTOrientations[2][mPMT_id]>0.99){
            //x is pointing along +y direction rotate +90 deg
            return pi/2;
        }else if(mPMTOrientations[2][mPMT_id]<-0.99){
            //x is pointing along -y direction rotate -90 deg
            return -pi/2;
        }else{
            std::cout << "Event display not designed for this rotation "<< std::endl;
            std::cout << "mPMT " << mPMT_id << " orientation [" << mPMTOrientations[1][mPMT_id] << ","<< mPMTOrientations[2][mPMT_id] << ","<< mPMTOrientations[3][mPMT_id] << "]"<<std::endl;
            return 0;
        }
    }else if(mPMT_id<21){
        //bottom end cap +y = +z in detector
        if(mPMTOrientations[3][mPMT_id]>0.99){
            //x is pointing along +z direction rotate +90 deg
            return pi/2;
        }else if(mPMTOrientations[3][mPMT_id]<-0.99){
            //x is pointing along -z direction rotate -90 deg
            return -pi/2;
        }else{
            std::cout << "Event display not designed for this rotation "<< std::endl;
            std::cout << "mPMT " << mPMT_id << " orientation [" << mPMTOrientations[1][mPMT_id] << ","<< mPMTOrientations[2][mPMT_id] << ","<< mPMTOrientations[3][mPMT_id] << "]"<<std::endl;
            return 0;
        }
    }else if(mPMT_id>84 && !(mPMT_id==85 || mPMT_id==91 || mPMT_id==99)){
        //top end cap +y = -z in detector
        if(mPMTOrientations[3][mPMT_id]>0.99){
            //x is pointing along +z direction rotate -90 deg
            return -pi/2;
        }else if(mPMTOrientations[3][mPMT_id]<-0.99){
            //x is pointing along -z direction rotate +90 deg
            return +pi/2;
        }else{
            std::cout << "Event display not designed for this rotation "<< std::endl;
            std::cout << "mPMT " << mPMT_id << " orientation [" << mPMTOrientations[1][mPMT_id] << ","<< mPMTOrientations[2][mPMT_id] << ","<< mPMTOrientations[3][mPMT_id] << "]"<<std::endl;
            return 0;
        }
    }
    return 0;

}

void EventDisplay::initialiseChannelPositions(){
    //initialise vector x_pmt and y_pmt to plot a channel position on 2d event display
    //vectors are indexed by mpmt_slotif*19 + pmt id 
    x_pmt.clear();
    y_pmt.clear();

    for(int i=0; i<n_mPMTs; i++){
        double mPMT_x = (mPMT_2D_projection[1])[i];
        double mPMT_y = (mPMT_2D_projection[2])[i];

        double eventDisplayRotation =this->getEventDisplayOrientation(i);
        
        for(int ch =0; ch<19; ch++){
            x_pmt.push_back(mPMT_x+pmt_offset_from_mPMT(ch,eventDisplayRotation)[0]);
            y_pmt.push_back(mPMT_y+pmt_offset_from_mPMT(ch,eventDisplayRotation)[1]);
        }
    }
    return;
}

std::vector<std::vector<double>> EventDisplay::ReadCSV_double(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<double>> columns;
    std::string line;


    // Clear the vectors and prepare to read
    columns.clear();
    
    if(!file){
        std::cout << "Problem opening file :" <<  filename << " returning null vector" <<std::endl;
        return columns;
    }
    // Read header line (skip if needed)
    std::getline(file, line);

    // Read data lines
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;

        int columnIndex = 0;
        while (std::getline(ss, item, ',')) {
            double value = std::stod(item);

            // Resize columns vector if necessary
            if (columnIndex >= columns.size()) {
                columns.resize(columnIndex + 1);
            }

            // Append value to the appropriate column
            columns[columnIndex].push_back(value);
            ++columnIndex;
        }
    }

    return columns;
}

std::vector<std::vector<int>> EventDisplay::ReadCSV_int(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<int>> columns;
    std::string line;

    // Clear the vectors and prepare to read
    columns.clear();
    
    if(!file){
        std::cout << "Problem opening file :" <<  filename << " returning null vector" <<std::endl;
        return columns;
    }

    // Read header line (skip if needed)
    std::getline(file, line);

    // Read data lines
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;

        int columnIndex = 0;
        while (std::getline(ss, item, ',')) {
            int value = std::stoi(item);

            // Resize columns vector if necessary
            if (columnIndex >= columns.size()) {
                columns.resize(columnIndex + 1);
            }

            // Append value to the appropriate column
            columns[columnIndex].push_back(value);
            ++columnIndex;
        }
    }

    return columns;
}

std::tuple<int, std::vector<double>, std::vector<double>, std::vector<double>> EventDisplay::processData(std::vector<int> cardID_vector, std::vector<int> channelID_vector, std::vector<double> hit_data, bool sum_data=false){
    //takes a series of hits for specific card ids and channels
    //returns x of hits, y of hits in event display
    //value for the event display

    //the value to plot for each channel
    std::vector<double> allChannelData(total_channels,0.0); 
    //a vector to keep track of which channels had data
    std::vector<bool> channelFilled(total_channels,false); 
    

    for(int i=0; i<hit_data.size(); i++){
        int cardID = cardID_vector[i];
        
        int channelID = channelID_vector[i];
        int key = cardID * 100 + channelID;
        
        if (cardIDChannelIndex.count(key) == 0){
            std::cout << "cardID " <<cardID << " channelID " << channelID << " not in event display " << std::endl;
            continue;
        }
        int channelIndex= cardIDChannelIndex[key];
        
        if(channelFilled[channelIndex]==false){
            //this channel has had no data before
            allChannelData[channelIndex] = hit_data[i];            
            
        }else if(channelFilled[channelIndex]==true){
            //this channel has already had a hit
            if(sum_data){
                //sum the data for that channel
                allChannelData[channelIndex]+=hit_data[i];  
            }else{
                //plot the lowest value - earliest hit for that channel
                if(allChannelData[channelIndex]>hit_data[i]){
                    allChannelData[channelIndex] = hit_data[i];
                }
            }
        }
        channelFilled[channelIndex]=true;
    }

    // now loop over the channels to get the format for plotting
    int nHits = 0;
    std::vector<double> hit_x;
    std::vector<double> hit_y;
    std::vector<double> hitValue;

    for(int i=0; i<total_channels; i++){
        //loop over the channel indices
        if(channelFilled[i]){
            //this channel had a hit
            nHits++;
            hit_x.push_back(cardIDChannelIndexPositionMap[i].first);
            hit_y.push_back(cardIDChannelIndexPositionMap[i].second);
            hitValue.push_back(allChannelData[i]);
        }
    }
    
    return std::make_tuple(nHits, hit_x, hit_y, hitValue);

}

TScatter EventDisplay::makeEventDisplayPlot(std::string graphName,std::vector<int> cardID_vector, std::vector<int> channelID_vector, std::vector<double> hit_data){
    int nHits;
    std::vector<double> x ,y, hit_value;

    std::tie(nHits,x ,y, hit_value) = processData(cardID_vector,channelID_vector, hit_data, false);
    TScatter scatter(nHits, &x[0], &y[0],&hit_value[0]);
    
    scatter.GetHistogram()->GetXaxis()->SetLimits(-2, 17);
    scatter.GetHistogram()->GetYaxis()->SetLimits(-7, 9);
    
    // scatter.GetHistogram()->GetXaxis()->SetLabelSize(0);
    // scatter.GetHistogram()->GetYaxis()->SetLabelSize(0);
    // scatter.GetHistogram()->GetZaxis()->SetLabelSize(10);
    // scatter.GetHistogram()->GetXaxis()->SetTickLength(0);
    // scatter.GetHistogram()->GetYaxis()->SetTickLength(0);


    scatter.SetMarkerStyle(20);
    scatter.SetMarkerSize(1.0);

    return scatter;

}


void EventDisplay::makeCardIDMap(){
    //create a map to map each cardid*100 + pomt channel id to a physical location on the event display
    cardIDChannelIndexPositionMap.clear();
    //also make a map that assigns an index to each cardIDChannelPosition
    //used internally to process
    cardIDChannelIndex.clear();
    total_channels = cardIDChannelMapping[0].size();

    for(int i=0; i<cardIDChannelMapping[0].size(); i++){
        int cardID_channel = cardIDChannelMapping[0][i]; //100*card id+ channel ID
        int mPMT = cardIDChannelMapping[1][i]/100;
        int pmt_position = cardIDChannelMapping[1][i]%100;

        double eventDisplay_x = x_pmt[mPMT*19+pmt_position];
        double eventDisplay_y = y_pmt[mPMT*19+pmt_position];
        //uniquely define this channel to be at index i
        cardIDChannelIndex[cardID_channel] = i;
        //record the x, y position of index i in the event display
        cardIDChannelIndexPositionMap[i]= std::make_pair(eventDisplay_x, eventDisplay_y);     
    }
    return;
}