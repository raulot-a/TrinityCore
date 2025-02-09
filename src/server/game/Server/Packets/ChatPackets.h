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

#ifndef ChatPackets_h__
#define ChatPackets_h__

#include "Packet.h"

namespace WorldPackets
{
    namespace Chat
    {
        // CMSG_MESSAGECHAT_GUILD
        // CMSG_MESSAGECHAT_OFFICER
        // CMSG_MESSAGECHAT_YELL
        // CMSG_MESSAGECHAT_SAY
        // CMSG_MESSAGECHAT_PARTY
        // CMSG_MESSAGECHAT_RAID
        // CMSG_MESSAGECHAT_RAID_WARNING
        class ChatMessage final : public ClientPacket
        {
        public:
            ChatMessage(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            std::string Text;
            int32 Language = LANG_UNIVERSAL;
        };

        // CMSG_MESSAGECHAT_WHISPER
        class ChatMessageWhisper final : public ClientPacket
        {
        public:
            ChatMessageWhisper(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            int32 Language = LANG_UNIVERSAL;
            std::string Text;
            std::string Target;
        };

        // CMSG_MESSAGECHAT_CHANNEL
        class ChatMessageChannel final : public ClientPacket
        {
        public:
            ChatMessageChannel(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            int32 Language = LANG_UNIVERSAL;
            std::string Text;
            std::string Target;
        };

        // CMSG_MESSAGECHAT_ADDON_GUILD
        // CMSG_MESSAGECHAT_ADDON_OFFICER
        // CMSG_MESSAGECHAT_ADDON_PARTY
        // CMSG_MESSAGECHAT_ADDON_RAID
        class ChatAddonMessage final : public ClientPacket
        {
        public:
            ChatAddonMessage(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            std::string Prefix;
            std::string Text;
        };

        // CMSG_MESSAGECHAT_ADDON_WHISPER
        class ChatAddonMessageWhisper final : public ClientPacket
        {
        public:
            ChatAddonMessageWhisper(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            std::string Prefix;
            std::string Target;
            std::string Text;
        };

        class ChatMessageDND final : public ClientPacket
        {
        public:
            ChatMessageDND(WorldPacket&& packet) : ClientPacket(CMSG_MESSAGECHAT_DND, std::move(packet)) { }

            void Read() override;

            std::string Text;
        };

        class ChatMessageAFK final : public ClientPacket
        {
        public:
            ChatMessageAFK(WorldPacket&& packet) : ClientPacket(CMSG_MESSAGECHAT_AFK, std::move(packet)) { }

            void Read() override;

            std::string Text;
        };

        class ChatMessageEmote final : public ClientPacket
        {
        public:
            ChatMessageEmote(WorldPacket&& packet) : ClientPacket(CMSG_MESSAGECHAT_EMOTE, std::move(packet)) { }

            void Read() override;

            std::string Text;
        };

        // SMSG_MESSAGECHAT
        class Chat final : public ServerPacket
        {
        public:
            Chat() : ServerPacket(SMSG_MESSAGECHAT, 1+1+8+8+8+8+8+4+4+4+1+4+20) { }

            WorldPacket const* Write() override;

            uint8 SlashCmd = 0;
            uint8 Language = LANG_UNIVERSAL;
            ObjectGuid SenderGUID;
            ObjectGuid SenderGuildGUID;
            ObjectGuid SenderAccountGUID; // Not in JAM messages but appears in packet?
            ObjectGuid TargetGUID;
            ObjectGuid PartyGUID;
            uint32 SenderVirtualAddress;
            uint32 TargetVirtualAddress;
            std::string SenderName;
            std::string TargetName;
            std::string Prefix;
            std::string Channel;
            std::string ChatText;
            uint32 AchievementID = 0;
            uint8 ChatFlags = 0;
            float DisplayTime = 0.0f;
            bool HideChatLog = false;
            bool FakeSenderName = false;
        };

        class Emote final : public ServerPacket
        {
        public:
            Emote() : ServerPacket(SMSG_EMOTE, 8+4) { }

            WorldPacket const* Write() override;

            ObjectGuid Guid;
            int32 EmoteID;
        };

        class CTextEmote final : public ClientPacket
        {
        public:
            CTextEmote(WorldPacket&& packet) : ClientPacket(CMSG_TEXT_EMOTE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Target;
            int32 EmoteID;
            int32 SoundIndex;
        };

        class STextEmote final : public ServerPacket
        {
        public:
            STextEmote() : ServerPacket(SMSG_TEXT_EMOTE, 8+8+4+4) { }

            WorldPacket const* Write() override;

            ObjectGuid SourceGUID;
            ObjectGuid SourceAccountGUID; // Not in JAM
            ObjectGuid TargetGUID;
            int32 SoundIndex;
            int32 EmoteID;
        };
    }
}

#endif // ChatPackets_h__
