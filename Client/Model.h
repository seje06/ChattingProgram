#pragma once


template<typename T>
class Model
{
public:
	Model(T data)
	{
		_lockQueue.Push(data);
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
LockQueue<T> Model<T>::_lockQueue;

//////////// Auth /////////////
struct AuthData
{
	bool isSuccess;
	CString id;
	CString pasd;
};
class AuthModel : public Model<AuthData>
{
public:
	AuthModel(AuthData data) : Model(data)
	{

	}
};


//////////// Lobby /////////////
struct CreateRoomData
{
	bool isSuccess;
	CString roomName;
};
class CreateRoomModel : public Model<CreateRoomData>
{
public:
	CreateRoomModel(CreateRoomData data) : Model(data)
	{

	}
};

struct JoinRoomData
{
	bool isSuccess;
	CString roomName;
	queue<CString> userIds;
};
class JoinRoomModel : public Model<JoinRoomData>
{
public:
	JoinRoomModel(JoinRoomData data) : Model(data)
	{

	}
};

struct RefreshLobbyData
{
	bool isSuccess;
	queue<CString> roomNames;
	queue<int> userCounts;
};
class RefreshLobbyModel : public Model<RefreshLobbyData>
{
public:
	RefreshLobbyModel(RefreshLobbyData data) : Model(data)
	{

	}
};

struct RefreshRoomData
{
	queue<CString> userIds;
};
class RefreshRoomModel : public Model<RefreshRoomData>
{
public:
	RefreshRoomModel(RefreshRoomData data) : Model(data)
	{

	}
};

//////////// Room //////////////////

struct ChatLogData
{
	CString id;
	CString log;
};
class ChatLogModel : public Model<ChatLogData>
{
public:
	ChatLogModel(ChatLogData data) : Model(data)
	{

	}
};