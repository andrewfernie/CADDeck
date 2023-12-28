#pragma once
#include "LCDKnob.h"

enum ActionEnum
{
    Action_NoAction = 0,
    Action_Delay = 1,
    Action_TabArrow = 2,
    Action_Media = 3,
    Action_Char = 4,
    Action_Option = 5,
    Action_FnKey = 6,
    Action_Number = 7,
    Action_SpecialChar = 8,
    Action_Combos = 9,
    Action_Helpers = 10,
    Action_SpecialFn = 11,
    Action_Numpad = 12,
    Action_CustomFn = 13,
    Action_ChangePage = 14,
    Action_CADFn = 15,
    Action_CADProgram = 16,
    Action_MouseButton = 17,
    Action_PreviousPage = 18,
    Action_DefaultJoyMode = 19,
    Action_SpaceMouseButton = 20
};

enum MouseButton {
    MouseButton_PL = 1,
    MouseButton_PR = 2,
    MouseButton_PM = 3,
    MouseButton_PLM = 4, 
    MouseButton_PRM = 5,
    MouseButton_RL = 6,
    MouseButton_RR = 7,
    MouseButton_RM = 8,
    MouseButton_RLM = 9,
    MouseButton_RRM = 10,
    MouseButton_RLRM = 11
};

enum OptionKeys {
    OptionKey_LCtrl = 1,
    OptionKey_LShift = 2,
    OptionKey_LAlt = 3,
    OptionKey_LGui = 4,
    OptionKey_RCtrl = 5,
    OptionKey_RShift = 6,
    OptionKey_RAlt = 7,
    OptionKey_RGui = 8,
    OptionKey_ReleaseAll = 9
};

enum ActionNumpadKeys {
    ActionNumpadKey_0 = 0,
    ActionNumpadKey_1 = 1,
    ActionNumpadKey_2 = 2,
    ActionNumpadKey_3 = 3,
    ActionNumpadKey_4 = 4,
    ActionNumpadKey_5 = 5,
    ActionNumpadKey_6 = 6,
    ActionNumpadKey_7 = 7,
    ActionNumpadKey_8 = 8,
    ActionNumpadKey_9 = 9,
    ActionNumpadKey_Slash = 10,
    ActionNumpadKey_Asterix = 11,
    ActionNumpadKey_Minus = 12,
    ActionNumpadKey_Plus = 13,
    ActionNumpadKey_Enter = 14,
    ActionNumpadKey_Period = 15
};

enum ActionCustomFnKeys
{
    ActionCustomFnKey_1 = 1,
    ActionCustomFnKey_2 = 2,
    ActionCustomFnKey_3 = 3,
    ActionCustomFnKey_4 = 4,
    ActionCustomFnKey_5 = 5,
    ActionCustomFnKey_6 = 6,
    ActionCustomFnKey_7 = 7
};

enum ActionFnKeys
{
    ActionFnKey_F1 = 1,
    ActionFnKey_F2 = 2,
    ActionFnKey_F3 = 3,
    ActionFnKey_F4 = 4,
    ActionFnKey_F5 = 5,
    ActionFnKey_F6 = 6,
    ActionFnKey_F7 = 7,
    ActionFnKey_F8 = 8,
    ActionFnKey_F9 = 9,
    ActionFnKey_F10 = 10,
    ActionFnKey_F11 = 11,
    ActionFnKey_F12 = 12,
    ActionFnKey_F13 = 13,
    ActionFnKey_F14 = 14,
    ActionFnKey_F15 = 15,
    ActionFnKey_F16 = 16,
    ActionFnKey_F17 = 17,
    ActionFnKey_F18 = 18,
    ActionFnKey_F19 = 19,
    ActionFnKey_F20 = 20,
    ActionFnKey_F21 = 21,
    ActionFnKey_F22 = 22,
    ActionFnKey_F23 = 23,
    ActionFnKey_F24 = 24
};

enum ActionTabArrowKeys
{
    ActionTabArrowKey_Up = 1,
    ActionTabArrowKey_Down = 2,
    ActionTabArrowKey_Left = 3,
    ActionTabArrowKey_Right = 4,
    ActionTabArrowKey_Backspace = 5,
    ActionTabArrowKey_Tab = 6,
    ActionTabArrowKey_Enter = 7,
    ActionTabArrowKey_PageUp = 8,
    ActionTabArrowKey_PageDown = 9,
    ActionTabArrowKey_Delete = 10,
    ActionTabArrowKey_PrtSc = 11,
    ActionTabArrowKey_Esc = 12,
    ActionTabArrowKey_Home = 13,
    ActionTabArrowKey_End = 14
};

enum ActionMediaKeys
{
    ActionMediaKey_Mute = 1,
    ActionMediaKey_VolumeDown = 2,
    ActionMediaKey_VolumeUp = 3,
    ActionMediaKey_PlayPause = 4,
    ActionMediaKey_Stop = 5,
    ActionMediaKey_NextTrack = 6,
    ActionMediaKey_PreviousTrack = 7,
    ActionMediaKey_WWWHome = 8,
    ActionMediaKey_LocalMachineBrowser = 9,
    ActionMediaKey_Calculator = 10,
    ActionMediaKey_WWWBookmarks = 11,
    ActionMediaKey_WWWSearch = 12,
    ActionMediaKey_WWWStop = 13,
    ActionMediaKey_WWWBack = 14,
    ActionMediaKey_ConsumerControlConfiguration = 15,
    ActionMediaKey_EmailReader = 16

};