#pragma once
#include "Model.h"

struct AuthData
{
	bool isSuccess;
	CString id;
	CString pasd;
};

class AuthModel : public Model<AuthData>
{
public:
	AuthModel(AuthData data);
};

