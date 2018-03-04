#pragma once
#include "PersonalFace.h"

namespace dev
{
	
namespace aqua
{
class KeyManager;
class AccountHolder;
class Interface;
}

namespace rpc
{

class Personal: public dev::rpc::PersonalFace
{
public:
	Personal(dev::aqua::KeyManager& _keyManager, dev::aqua::AccountHolder& _accountHolder, aqua::Interface& _aqua);
	virtual RPCModules implementedModules() const override
	{
		return RPCModules{RPCModule{"personal", "1.0"}};
	}
	virtual std::string personal_newAccount(std::string const& _password) override;
	virtual bool personal_unlockAccount(std::string const& _address, std::string const& _password, int _duration) override;
	virtual std::string personal_signAndSendTransaction(Json::Value const& _transaction, std::string const& _password) override;
	virtual std::string personal_sendTransaction(Json::Value const& _transaction, std::string const& _password) override;
	virtual Json::Value personal_listAccounts() override;

private:
	dev::aqua::KeyManager& m_keyManager;
	dev::aqua::AccountHolder& m_accountHolder;
	dev::aqua::Interface& m_aqua;
};

}
}
