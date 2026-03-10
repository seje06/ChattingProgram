#pragma once

#include "framework.h"

constexpr UINT WMU_UI_EVENT = WM_APP + 1;

enum class UIEvent : WPARAM
{
    None = 0,

    //로그인
    LoginClicked_Login,
    LoginCompleted_Login, // 뒤 접미사는 해당 핸들러에서 처리함을 의미
    LoginCompleted_Lobby,

    //회원가입
    RegisterClicked_Register,
    RegisterCompleted_Register,
    RegisterCompleted_Lobby,

    //로비
    CreateRoomClicked_Lobby,
    CreateRoomCompleted_Lobby,
    CreateRoomCompleted_Room,

    JoinRoomClicked_Lobby,
    JoinRoomCompleted_Lobby,
    JoinRoomCompleted_Room,

    RefreshClicked_Lobby,
    RefreshCompleted_Lobby,

    //룸
    SendChatClicked_Room,
    SendChatCompleted_Room,
    LeaveRoomClicked_Room,
    LeaveRoomCompleted_Room,
    LeaveRoomCompleted_Lobby,
    RefreshRoomCompleted

};
