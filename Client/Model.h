#pragma once


template<typename T>
class ResponseQ
{
public:
	ResponseQ(T data)
	{
		_lockQueue.Push(data);
	}

	static T Front(OUT bool& isEmpty)
	{
		return _lockQueue.Front(OUT isEmpty);
	}

	static T Pop(OUT bool& isEmpty)
	{
		return _lockQueue.Pop(OUT isEmpty);
	}

	static void PopAll(OUT vector<T>& datas)
	{
		_lockQueue.PopAll(OUT datas);
	}

	static void Clear()
	{
		_lockQueue.Clear();
	}
private:
	static LockQueue<T> _lockQueue;
};
template<typename T>
LockQueue<T> ResponseQ<T>::_lockQueue;

//////////// Auth /////////////
struct AuthData
{
	bool isSuccess;
	CString id;
	CString pasd;
};
class AuthModel : public ResponseQ<AuthData>
{
public:
	AuthModel(AuthData data) : ResponseQ(data)
	{

	}
};


//////////// Lobby /////////////
struct CreateRoomData
{
	bool isSuccess;
	CString roomName;
};
class CreateRoomModel : public ResponseQ<CreateRoomData>
{
public:
	CreateRoomModel(CreateRoomData data) : ResponseQ(data)
	{

	}
};

struct JoinRoomData
{
	bool isSuccess;
	CString roomName;
	queue<CString> userIds;
};
class JoinRoomModel : public ResponseQ<JoinRoomData>
{
public:
	JoinRoomModel(JoinRoomData data) : ResponseQ(data)
	{

	}
};

struct RefreshLobbyData
{
	bool isSuccess;
	queue<CString> roomNames;
	queue<int> userCounts;
};
class RefreshLobbyModel : public ResponseQ<RefreshLobbyData>
{
public:
	RefreshLobbyModel(RefreshLobbyData data) : ResponseQ(data)
	{

	}
};

struct RefreshRoomData
{
	queue<CString> userIds;
};
class RefreshRoomModel : public ResponseQ<RefreshRoomData>
{
public:
	RefreshRoomModel(RefreshRoomData data) : ResponseQ(data)
	{

	}
};

//////////// Room //////////////////

struct ChatLogData
{
	CString id;
	CString log;
};
class ChatLogModel : public ResponseQ<ChatLogData>
{
public:
	ChatLogModel(ChatLogData data) : ResponseQ(data)
	{

	}
};

struct RoomOutData
{
};
class RoomOutModel : public ResponseQ<RoomOutData>
{
public:
	RoomOutModel(RoomOutData data) : ResponseQ(data)
	{

	}
};