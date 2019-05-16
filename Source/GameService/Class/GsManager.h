#pragma once

class IGsManager
{
public:
	virtual ~IGsManager() {}

	virtual void Initialize() {};
	virtual void Finalize() {};
	virtual void Update() {};
};