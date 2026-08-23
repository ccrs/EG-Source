/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AuthSocketMgr_h__
#define AuthSocketMgr_h__

#include "SocketMgr.h"
#include "AuthSession.h"
#include "Config.h"

class AuthSocketThread : public Trinity::Net::NetworkThread<AuthSession>
{
public:
    void SocketAdded(std::shared_ptr<AuthSession> const& sock) override;
    void SocketRemoved(std::shared_ptr<AuthSession> const& sock) override;
};

class AuthSocketMgr : public Trinity::Net::SocketMgr<AuthSession>
{
    typedef SocketMgr<AuthSession> BaseSocketMgr;

public:
    static AuthSocketMgr& Instance()
    {
        static AuthSocketMgr instance;
        return instance;
    }

    bool StartNetwork(Trinity::Asio::IoContext& ioContext, std::string const& bindIp, uint16 port, int threadCount = 1) override
    {
        // EG - cached at startup, the authserver has no runtime config reload
        int32 maxConnectionsPerAddress = sConfigMgr->GetIntDefault("Network.MaxConnectionsPerIp", 10);
        if (maxConnectionsPerAddress < 0)
        {
            TC_LOG_ERROR("server.authserver", "Network.MaxConnectionsPerIp ({}) must be 0 or greater (0 disables the check). Set to 10.", maxConnectionsPerAddress);
            maxConnectionsPerAddress = 10;
        }
        _maxConnectionsPerAddress = uint32(maxConnectionsPerAddress);

        if (!BaseSocketMgr::StartNetwork(ioContext, bindIp, port, threadCount))
            return false;

        _acceptor->AsyncAccept([this](Trinity::Net::IoContextTcpSocket&& sock, uint32 threadIndex)
        {
            OnSocketOpen(std::move(sock), threadIndex);
        });
        return true;
    }

protected:
    Trinity::Net::NetworkThread<AuthSession>* CreateThreads() const override
    {
        return new AuthSocketThread[1];
    }

    uint32 GetMaxConnectionsPerAddress() const override { return _maxConnectionsPerAddress; }

private:
    uint32 _maxConnectionsPerAddress = 0;
};

#define sAuthSocketMgr AuthSocketMgr::Instance()

inline void AuthSocketThread::SocketAdded(std::shared_ptr<AuthSession> const& sock)
{
    sAuthSocketMgr.AddConnectionForAddress(sock->GetRemoteIpAddress());
}

inline void AuthSocketThread::SocketRemoved(std::shared_ptr<AuthSession> const& sock)
{
    sAuthSocketMgr.RemoveConnectionForAddress(sock->GetRemoteIpAddress());
}

#endif // AuthSocketMgr_h__
