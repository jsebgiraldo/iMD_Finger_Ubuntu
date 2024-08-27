#include "fingerprint.h"
#include "libs/fpcore.h"

Fingerprint::Fingerprint()
{

    finger_position_enum_map["FINGER_POSITION_RIGHT_FOUR"] = FINGER_POSITION_RIGHT_FOUR;
    finger_position_enum_map["FINGER_POSITION_LEFT_FOUR"] = FINGER_POSITION_LEFT_FOUR;
    finger_position_enum_map["FINGER_POSITION_BOTH_THUMBS"] = FINGER_POSITION_BOTH_THUMBS;
    finger_position_enum_map["FINGER_POSITION_ANY_FINGER"] = FINGER_POSITION_ANY_FINGER;

    finger_position_enum_map["FINGER_POSITION_RIGHT_THUMB"] = FINGER_POSITION_RIGHT_THUMB;
    finger_position_enum_map["FINGER_POSITION_RIGHT_INDEX"] = FINGER_POSITION_RIGHT_INDEX;
    finger_position_enum_map["FINGER_POSITION_RIGHT_MIDDLE"] = FINGER_POSITION_RIGHT_MIDDLE;
    finger_position_enum_map["FINGER_POSITION_RIGHT_RING"] = FINGER_POSITION_RIGHT_RING;
    finger_position_enum_map["FINGER_POSITION_RIGHT_LITTLE"] = FINGER_POSITION_RIGHT_LITTLE;

    finger_position_enum_map["FINGER_POSITION_LEFT_THUMB"] = FINGER_POSITION_LEFT_THUMB;
    finger_position_enum_map["FINGER_POSITION_LEFT_INDEX"] = FINGER_POSITION_LEFT_INDEX;
    finger_position_enum_map["FINGER_POSITION_LEFT_MIDDLE"] = FINGER_POSITION_LEFT_MIDDLE;
    finger_position_enum_map["FINGER_POSITION_LEFT_RING"] = FINGER_POSITION_LEFT_RING;
    finger_position_enum_map["FINGER_POSITION_LEFT_LITTLE"] = FINGER_POSITION_LEFT_LITTLE;


    finger_position_string_map[FINTER_POSITION_UNKNOW_FINGER] = "unknow";

    finger_position_string_map[FINGER_POSITION_RIGHT_THUMB] = "Right Thumb";
    finger_position_string_map[FINGER_POSITION_RIGHT_INDEX] = "Right Index";
    finger_position_string_map[FINGER_POSITION_RIGHT_MIDDLE] = "Right Middle";
    finger_position_string_map[FINGER_POSITION_RIGHT_RING] = "Right Ring";
    finger_position_string_map[FINGER_POSITION_RIGHT_LITTLE] = "Right Little";

    finger_position_string_map[FINGER_POSITION_LEFT_THUMB] = "Left Thumb";
    finger_position_string_map[FINGER_POSITION_LEFT_INDEX] = "Left Index";
    finger_position_string_map[FINGER_POSITION_LEFT_MIDDLE] = "Left Middle";
    finger_position_string_map[FINGER_POSITION_LEFT_RING] = "Left Ring";
    finger_position_string_map[FINGER_POSITION_LEFT_LITTLE] = "Left Little";

    finger_path_map[FINTER_POSITION_UNKNOW_FINGER] = "unknow";

    finger_path_map[FINGER_POSITION_RIGHT_THUMB] = "right_thumb";
    finger_path_map[FINGER_POSITION_RIGHT_INDEX] = "right_index";
    finger_path_map[FINGER_POSITION_RIGHT_MIDDLE] = "right_middle";
    finger_path_map[FINGER_POSITION_RIGHT_RING] = "right_ring";
    finger_path_map[FINGER_POSITION_RIGHT_LITTLE] = "right_little";

    finger_path_map[FINGER_POSITION_LEFT_THUMB] = "left_thumb";
    finger_path_map[FINGER_POSITION_LEFT_INDEX] = "left_index";
    finger_path_map[FINGER_POSITION_LEFT_MIDDLE] = "left_middle";
    finger_path_map[FINGER_POSITION_LEFT_RING] = "left_ring";
    finger_path_map[FINGER_POSITION_LEFT_LITTLE] = "left_little";
}
