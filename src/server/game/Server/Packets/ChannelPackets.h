/*
* Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
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

#ifndef ChannelPackets_h__
#define ChannelPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace Channel
    {
        class ChannelList final : public ServerPacket
        {
        public:
            struct ChannelPlayer
            {
                ChannelPlayer(ObjectGuid const& guid, uint32 realm, uint8 flags) :
                    Guid(guid), VirtualRealmAddress(realm), Flags(flags) { }

                ObjectGuid Guid; ///< Player Guid
                uint32 VirtualRealmAddress;
                uint8 Flags = 0; ///< @see enum ChannelMemberFlags
            };

            ChannelList() : ServerPacket(SMSG_CHANNEL_LIST) { }

            WorldPacket const* Write() override;

            std::vector<ChannelPlayer> Members;
            std::string Channel; ///< Channel Name
            uint8 ChannelFlags = 0; ///< @see enum ChannelFlags
            bool Display = false;
        };

        class ChannelNotify final : public ServerPacket
        {
        public:
            ChannelNotify() : ServerPacket(SMSG_CHANNEL_NOTIFY, 80) { }

            WorldPacket const* Write() override;

            std::string Sender;
            ObjectGuid SenderGuid;
            ObjectGuid SenderBnetAccountID;
            uint8 Type                = 0; ///< @see enum ChatNotify
            uint8 OldFlags            = 0; ///< @see enum ChannelMemberFlags
            uint8 NewFlags            = 0; ///< @see enum ChannelMemberFlags
            std::string Channel;
            uint32 SenderVirtualRealm = 0;
            ObjectGuid TargetGuid;
            uint32 TargetVirtualRealm = 0;
            int32 ChatChannelID       = 0;
        };

        class ChannelNotifyJoined final : public ServerPacket
        {
        public:
            ChannelNotifyJoined() : ServerPacket(SMSG_CHANNEL_NOTIFY_JOINED, 50) { }

            WorldPacket const* Write() override;

            std::string ChannelWelcomeMsg;
            int32 ChatChannelID = 0;
            int32 InstanceID    = 0;
            uint8 ChannelFlags  = 0; ///< @see enum ChannelFlags
            std::string Channel;
        };

        class ChannelNotifyLeft final : public ServerPacket
        {
            public:
            ChannelNotifyLeft() : ServerPacket(SMSG_CHANNEL_NOTIFY_LEFT, 30) { }

            WorldPacket const* Write() override;

            std::string Channel;
            int32 ChatChannelID = 0;
            bool Suspended = false;
        };

        class JoinChannel final : public ClientPacket
        {
        public:
            JoinChannel(WorldPacket&& packet) : ClientPacket(CMSG_JOIN_CHANNEL, std::move(packet)) { }

            void Read() override;

            std::string Password;
            std::string ChannelName;
            bool CreateVoiceSession = false;
            int32 ChatChannelId         = 0;
            bool Internal           = false;
        };
    }
}

#endif // ChannelPackets_h__
