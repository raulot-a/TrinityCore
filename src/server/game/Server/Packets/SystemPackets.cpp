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

#include "SystemPackets.h"

WorldPacket const* WorldPackets::System::FeatureSystemStatus::Write()
{
    _worldPacket << uint8(ComplaintStatus);

    _worldPacket << uint32(ScrollOfResurrectionRequestsRemaining);
    _worldPacket << uint32(ScrollOfResurrectionMaxRequestsPerDay);
    _worldPacket << uint32(CfgRealmID);
    _worldPacket << int32(CfgRealmRecID);

    _worldPacket.WriteBit(VoiceEnabled);
    _worldPacket.WriteBit(EuropaTicketSystemStatus.HasValue);
    _worldPacket.WriteBit(ScrollOfResurrectionEnabled);
    _worldPacket.WriteBit(BpayStoreEnabled);
    _worldPacket.WriteBit(BpayStoreAvailable);
    _worldPacket.WriteBit(BpayStoreDisabledByParentalControls);
    _worldPacket.WriteBit(ItemRestorationButtonEnabled);
    _worldPacket.WriteBit(BrowserEnabled);
    _worldPacket.WriteBit(SessionAlert.HasValue);
    _worldPacket.WriteBit(RecruitAFriendSendingEnabled);
    _worldPacket.WriteBit(CharUndeleteEnabled);
    _worldPacket.WriteBit(UnkBit21);
    _worldPacket.WriteBit(UnkBit22);
    _worldPacket.WriteBit(UnkBit90);

    if (EuropaTicketSystemStatus.HasValue)
    {
        _worldPacket.WriteBit(EuropaTicketSystemStatus.value.UnkBit0);
        _worldPacket.WriteBit(EuropaTicketSystemStatus.value.UnkBit1);
        _worldPacket.WriteBit(EuropaTicketSystemStatus.value.TicketSystemEnabled);
        _worldPacket.WriteBit(EuropaTicketSystemStatus.value.SubmitBugEnabled);

        _worldPacket << uint32(EuropaTicketSystemStatus.value.ThrottleState.MaxTries);
        _worldPacket << uint32(EuropaTicketSystemStatus.value.ThrottleState.PerMilliseconds);
        _worldPacket << uint32(EuropaTicketSystemStatus.value.ThrottleState.TryCount);
        _worldPacket << uint32(EuropaTicketSystemStatus.value.ThrottleState.LastResetTimeBeforeNow);
    }

    if (SessionAlert.HasValue)
    {
        _worldPacket << int32(SessionAlert.value.Delay);
        _worldPacket << int32(SessionAlert.value.Period);
        _worldPacket << int32(SessionAlert.value.DisplayTime);
    }

    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::System::FeatureSystemStatusGlueScreen::Write()
{
    _worldPacket.WriteBit(BpayStoreEnabled);
    _worldPacket.WriteBit(BpayStoreAvailable);
    _worldPacket.WriteBit(BpayStoreDisabledByParentalControls);
    _worldPacket.WriteBit(CharUndeleteEnabled);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::System::MOTD::Write()
{
    ASSERT(Text);
    _worldPacket.WriteBits(Text->size(), 4);
    _worldPacket.FlushBits();

    for (std::string const& line : *Text)
    {
        _worldPacket.WriteBits(line.length(), 7);
        _worldPacket.FlushBits();
        _worldPacket.WriteString(line);
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::System::SetTimeZoneInformation::Write()
{
    _worldPacket.WriteBits(ServerTimeTZ.length(), 7);
    _worldPacket.WriteBits(GameTimeTZ.length(), 7);
    _worldPacket.WriteString(ServerTimeTZ);
    _worldPacket.WriteString(GameTimeTZ);

    return &_worldPacket;
}
