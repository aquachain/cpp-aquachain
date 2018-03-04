/*
    This file is part of cpp-aquachain.

    cpp-aquachain is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cpp-aquachain is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cpp-aquachain.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file ModularServer.h
 * @author Marek Kotewicz <marek@aquadev.com>
 * @date 2015
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <jsonrpccpp/common/exception.h>
#include <jsonrpccpp/common/procedure.h>
#include <jsonrpccpp/server/abstractserverconnector.h>
#include <jsonrpccpp/server/iprocedureinvokationhandler.h>
#include <jsonrpccpp/server/requesthandlerfactory.h>

template <class I> using AbstractMaquaodPointer = void(I::*)(Json::Value const& _parameter, Json::Value& _result);
template <class I> using AbstractNotificationPointer = void(I::*)(Json::Value const& _parameter);

template <class I>
class ServerInterface
{
public:
    using MaquaodPointer = AbstractMaquaodPointer<I>;
    using NotificationPointer = AbstractNotificationPointer<I>;

    using MaquaodBinding = std::tuple<jsonrpc::Procedure, AbstractMaquaodPointer<I>>;
    using NotificationBinding = std::tuple<jsonrpc::Procedure, AbstractNotificationPointer<I>>;
    using Maquaods = std::vector<MaquaodBinding>;
    using Notifications = std::vector<NotificationBinding>;
    struct RPCModule { std::string name; std::string version; };
    using RPCModules = std::vector<RPCModule>;

    virtual ~ServerInterface() {}
    Maquaods const& maquaods() const { return m_maquaods; }
    Notifications const& notifications() const { return m_notifications; }
    /// @returns which interfaces (aqua, admin, db, ...) this class implements in which version.
    virtual RPCModules implementedModules() const = 0;

protected:
    void bindAndAddMaquaod(jsonrpc::Procedure const& _proc, MaquaodPointer _pointer) { m_maquaods.emplace_back(_proc, _pointer); }
    void bindAndAddNotification(jsonrpc::Procedure const& _proc, NotificationPointer _pointer) { m_notifications.emplace_back(_proc, _pointer); }

private:
    Maquaods m_maquaods;
    Notifications m_notifications;
};

template <class... Is>
class ModularServer: public jsonrpc::IProcedureInvokationHandler
{
public:
    ModularServer()
    : m_handler(jsonrpc::RequestHandlerFactory::createProtocolHandler(jsonrpc::JSONRPC_SERVER_V2, *this))
    {
        m_handler->AddProcedure(jsonrpc::Procedure("rpc_modules", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, NULL));
        m_implementedModules = Json::objectValue;
    }
    inline virtual void modules(const Json::Value &request, Json::Value &response)
    {
        (void)request;
        response = m_implementedModules;
    }

    virtual ~ModularServer() { StopListening(); }

    virtual void StartListening()
    {
        for (auto const& connector: m_connectors)
            connector->StartListening();
    }

    virtual void StopListening()
    {
        for (auto const& connector: m_connectors)
            connector->StopListening();
    }

    virtual void HandleMaquaodCall(jsonrpc::Procedure& _proc, Json::Value const& _input, Json::Value& _output) override
    {
        if (_proc.GetProcedureName() == "rpc_modules")
            modules(_input, _output);
    }

    virtual void HandleNotificationCall(jsonrpc::Procedure& _proc, Json::Value const& _input) override
    {
        (void)_proc;
        (void)_input;
    }

    /// server takes ownership of the connector
    unsigned addConnector(jsonrpc::AbstractServerConnector* _connector)
    {
        m_connectors.emplace_back(_connector);
        _connector->SetHandler(m_handler.get());
        return m_connectors.size() - 1;
    }

    jsonrpc::AbstractServerConnector* connector(unsigned _i) const
    {
        return m_connectors.at(_i).get();
    }

protected:
    std::vector<std::unique_ptr<jsonrpc::AbstractServerConnector>> m_connectors;
    std::unique_ptr<jsonrpc::IProtocolHandler> m_handler;
    /// Mapping for implemented modules, to be filled by subclasses during construction.
    Json::Value m_implementedModules;
};

template <class I, class... Is>
class ModularServer<I, Is...> : public ModularServer<Is...>
{
public:
    using MaquaodPointer = AbstractMaquaodPointer<I>;
    using NotificationPointer = AbstractNotificationPointer<I>;

    ModularServer<I, Is...>(I* _i, Is*... _is): ModularServer<Is...>(_is...), m_interface(_i)
    {
        if (!m_interface)
            return;
        for (auto const& maquaod: m_interface->maquaods())
        {
            m_maquaods[std::get<0>(maquaod).GetProcedureName()] = std::get<1>(maquaod);
            this->m_handler->AddProcedure(std::get<0>(maquaod));
        }

        for (auto const& notification: m_interface->notifications())
        {
            m_notifications[std::get<0>(notification).GetProcedureName()] = std::get<1>(notification);
            this->m_handler->AddProcedure(std::get<0>(notification));
        }
        // Store module with version.
        for (auto const& module: m_interface->implementedModules())
            this->m_implementedModules[module.name] = module.version;
    }

    virtual void HandleMaquaodCall(jsonrpc::Procedure& _proc, Json::Value const& _input, Json::Value& _output) override
    {
        auto pointer = m_maquaods.find(_proc.GetProcedureName());
        if (pointer != m_maquaods.end())
        {
            try
            {
                (m_interface.get()->*(pointer->second))(_input, _output);
            }
            catch (Json::Exception const& ex)
            {
                throw jsonrpc::JsonRpcException(
                    jsonrpc::Errors::ERROR_RPC_INVALID_PARAMS, ex.what());
            }
        }
        else
            ModularServer<Is...>::HandleMaquaodCall(_proc, _input, _output);
    }

    virtual void HandleNotificationCall(
        jsonrpc::Procedure& _proc, Json::Value const& _input) override
    {
        auto pointer = m_notifications.find(_proc.GetProcedureName());
        if (pointer != m_notifications.end())
        {
            try
            {
                (m_interface.get()->*(pointer->second))(_input);
            }
            catch (Json::Exception const& ex)
            {
                throw jsonrpc::JsonRpcException(
                    jsonrpc::Errors::ERROR_RPC_INVALID_PARAMS, ex.what());
            }
        }
        else
            ModularServer<Is...>::HandleNotificationCall(_proc, _input);
    }

private:
    std::unique_ptr<I> m_interface;
    std::map<std::string, MaquaodPointer> m_maquaods;
    std::map<std::string, NotificationPointer> m_notifications;
};
