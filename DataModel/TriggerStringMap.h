#ifndef TRIGGER_STRING_MAP
#define TRIGGER_STRING_MAP
//this will need ot be adjusted when the real trigger types are set in the raw data 

inline std::string triggerTypeToString(TriggerType type) {
    switch (type) {
        case TriggerType::LASER: return "LASER";
        case TriggerType::NHITS: return "NHITS";
        case TriggerType::LED: return "LED";
        case TriggerType::BEAM: return "BEAM";
        case TriggerType::NONE: return "NONE";
        default: return "UNKNOWN"; // Handle invalid case
    }
};

#endif