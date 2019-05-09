#pragma once

class GSIManager
{
public:
	virtual ~GSIManager() {}

	virtual void Initialize() {};
	virtual void Finalize() {};
	virtual void Update() {};
};