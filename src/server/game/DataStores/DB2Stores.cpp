/*
 * Copyright (C) 2011 TrintiyCore <http://www.trinitycore.org/>
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

#include "DB2Stores.h"
#include "DBCStores.h"
#include "DB2fmt.h"
#include "DB2Utility.h"
#include "Common.h"
#include "Log.h"
#include "World.h"

DB2Storage<HolidaysEntry>                   sHolidaysStore(HolidaysEntryfmt);
DB2Storage<ItemEntry>                       sItemStore(Itemfmt, &DB2Utilities::HasItemEntry, &DB2Utilities::WriteItemDbReply);
DB2Storage<ItemAppearanceEntry>             sItemAppearanceStore(ItemAppearanceEntryfmt);
ItemDisplayIDMap                            sItemDisplayIDMap;
DB2Storage<ItemCurrencyCostEntry>           sItemCurrencyCostStore(ItemCurrencyCostfmt);
DB2Storage<ItemExtendedCostEntry>           sItemExtendedCostStore(ItemExtendedCostEntryfmt);
DB2Storage<ItemEffectEntry>                 sItemEffectStore(ItemEffectEntryfmt);
DB2Storage<ItemSparseEntry>                 sItemSparseStore(ItemSparsefmt, &DB2Utilities::HasItemSparseEntry, &DB2Utilities::WriteItemSparseDbReply);
DB2Storage<KeyChainEntry>                   sKeyChainStore(KeyChainfmt);
DB2Storage<OverrideSpellDataEntry>          sOverrideSpellDataStore(OverrideSpellDataEntryfmt);
DB2Storage<PhaseGroupEntry>                 sPhaseGroupStore(PhaseGroupEntryfmt);
DB2Storage<SpellAuraRestrictionsEntry>      sSpellAuraRestrictionsStore(SpellAuraRestrictionsEntryfmt);
DB2Storage<SpellCastingRequirementsEntry>   sSpellCastingRequirementsStore(SpellCastingRequirementsEntryfmt);
DB2Storage<SpellClassOptionsEntry>          sSpellClassOptionsStore(SpellClassOptionsEntryfmt);
DB2Storage<SpellMiscEntry>                  sSpellMiscStore(SpellMiscEntryfmt);
DB2Storage<SpellPowerEntry>                 sSpellPowerStore(SpellPowerEntryfmt);
DB2Storage<SpellReagentsEntry>              sSpellReagentsStore(SpellReagentsEntryfmt);
DB2Storage<SpellRuneCostEntry>              sSpellRuneCostStore(SpellRuneCostEntryfmt);
DB2Storage<SpellTotemsEntry>                sSpellTotemsStore(SpellTotemsEntryfmt);
DB2Storage<TaxiNodesEntry>                  sTaxiNodesStore(TaxiNodesEntryfmt);
DB2Storage<TaxiPathEntry>                   sTaxiPathStore(TaxiPathEntryfmt);
DB2Storage<TaxiPathNodeEntry>               sTaxiPathNodeStore(TaxiPathNodeEntryfmt);
TaxiMask                                    sTaxiNodesMask;
TaxiMask                                    sOldContinentsNodesMask;
TaxiMask                                    sHordeTaxiNodesMask;
TaxiMask                                    sAllianceTaxiNodesMask;
TaxiMask                                    sDeathKnightTaxiNodesMask;
TaxiPathSetBySource                         sTaxiPathSetBySource;
TaxiPathNodesByPath                         sTaxiPathNodesByPath;

typedef std::list<std::string> DB2StoreProblemList;

typedef std::map<uint32 /*hash*/, DB2StorageBase*> DB2StorageMap;
DB2StorageMap DB2Stores;

uint32 DB2FilesCount = 0;

static bool LoadDB2_assert_print(uint32 fsize, uint32 rsize, std::string const& filename)
{
    TC_LOG_ERROR("misc", "Size of '%s' setted by format string (%u) not equal size of C++ structure (%u).", filename.c_str(), fsize, rsize);

    // ASSERT must fail after function call
    return false;
}

template<class T>
inline void LoadDB2(uint32& availableDb2Locales, DB2StoreProblemList& errlist, DB2Storage<T>& storage, std::string const& db2_path, std::string const& filename)
{
    // compatibility format and C++ structure sizes
    ASSERT(DB2FileLoader::GetFormatRecordSize(storage.GetFormat()) == sizeof(T) || LoadDB2_assert_print(DB2FileLoader::GetFormatRecordSize(storage.GetFormat()), sizeof(T), filename));

    ++DB2FilesCount;

    std::string db2_filename = db2_path + filename;
    if (storage.Load(db2_filename.c_str(), uint32(sWorld->GetDefaultDbcLocale())))
    {
        for (uint32 i = 0; i < TOTAL_LOCALES; ++i)
        {
            if (!(availableDb2Locales & (1 << i)))
                continue;

            if (uint32(sWorld->GetDefaultDbcLocale()) == i)
                continue;

            std::string localizedName(db2_path);
            localizedName.append(localeNames[i]);
            localizedName.push_back('/');
            localizedName.append(filename);

            if (!storage.LoadStringsFrom(localizedName.c_str(), i))
                availableDb2Locales &= ~(1<<i);             // mark as not available for speedup next checks
        }
    }
    else
    {
        // sort problematic db2 to (1) non compatible and (2) nonexistent
        if (FILE* f = fopen(db2_filename.c_str(), "rb"))
        {
            std::ostringstream stream;
            stream << db2_filename << " exists, and has " << storage.GetFieldCount() << " field(s) (expected " << strlen(storage.GetFormat()) << "). Extracted file might be from wrong client version.";
            std::string buf = stream.str();
            errlist.push_back(buf);
            fclose(f);
        }
        else
            errlist.push_back(db2_filename);
    }

    DB2Stores[storage.GetHash()] = &storage;
}

void LoadDB2Stores(std::string const& dataPath)
{
    std::string db2Path = dataPath + "dbc/";

    DB2StoreProblemList bad_db2_files;
    uint32 availableDb2Locales = 0xFF;

    LoadDB2(availableDb2Locales, bad_db2_files, sHolidaysStore,             db2Path,    "Holidays.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sItemStore,                 db2Path,    "Item.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sItemAppearanceStore,       db2Path,    "ItemAppearance.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sItemCurrencyCostStore,     db2Path,    "ItemCurrencyCost.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sItemSparseStore,           db2Path,    "Item-sparse.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sItemExtendedCostStore,     db2Path,    "ItemExtendedCost.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sItemEffectStore,           db2Path,    "ItemEffect.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sKeyChainStore,             db2Path,    "KeyChain.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sOverrideSpellDataStore,    db2Path,    "OverrideSpellData.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sPhaseGroupStore,           db2Path,    "PhaseXPhaseGroup.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sSpellAuraRestrictionsStore, db2Path,   "SpellAuraRestrictions.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sSpellCastingRequirementsStore, db2Path, "SpellCastingRequirements.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sSpellClassOptionsStore,    db2Path,    "SpellClassOptions.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sSpellMiscStore,            db2Path,    "SpellMisc.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sSpellPowerStore,           db2Path,    "SpellPower.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sSpellReagentsStore,        db2Path,    "SpellReagents.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sSpellRuneCostStore,        db2Path,    "SpellRuneCost.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sSpellTotemsStore,          db2Path,    "SpellTotems.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sTaxiNodesStore,            db2Path,    "TaxiNodes.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sTaxiPathStore,             db2Path,    "TaxiPath.db2");
    LoadDB2(availableDb2Locales, bad_db2_files, sTaxiPathNodeStore,         db2Path,    "TaxiPathNode.db2");
    
    for (uint32 i = 0; i < sItemAppearanceStore.GetNumRows(); ++i)
        if (ItemAppearanceEntry const* entry = sItemAppearanceStore.LookupEntry(i))
            sItemDisplayIDMap[entry->AppearanceID] = entry->DisplayID;
    
    for (uint32 i = 1; i < sTaxiPathStore.GetNumRows(); ++i)
        if (TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(i))
            sTaxiPathSetBySource[entry->From][entry->To] = TaxiPathBySourceAndDestination(entry->ID, entry->Cost);
    uint32 pathCount = sTaxiPathStore.GetNumRows();

    // Calculate path nodes count
    std::vector<uint32> pathLength;
    pathLength.resize(pathCount);                           // 0 and some other indexes not used
    for (uint32 i = 1; i < sTaxiPathNodeStore.GetNumRows(); ++i)
    {
        if (TaxiPathNodeEntry const* entry = sTaxiPathNodeStore.LookupEntry(i))
        {
            if (pathLength[entry->PathID] < entry->NodeIndex + 1)
                pathLength[entry->PathID] = entry->NodeIndex + 1;
        }
    }
    
    // Set path length
    sTaxiPathNodesByPath.resize(pathCount);                 // 0 and some other indexes not used
    for (uint32 i = 1; i < sTaxiPathNodesByPath.size(); ++i)
        sTaxiPathNodesByPath[i].resize(pathLength[i]);
    
    // fill data
    for (uint32 i = 1; i < sTaxiPathNodeStore.GetNumRows(); ++i)
        if (TaxiPathNodeEntry const* entry = sTaxiPathNodeStore.LookupEntry(i))
            sTaxiPathNodesByPath[entry->PathID].set(entry->NodeIndex, entry);

    // Initialize global taxinodes mask
    // include existed nodes that have at least single not spell base (scripted) path
    {
        std::set<uint32> spellPaths;
        for (uint32 i = 1; i < sSpellEffectStore.GetNumRows(); ++i)
            if (SpellEffectEntry const* sInfo = sSpellEffectStore.LookupEntry (i))
                if (sInfo->Effect == SPELL_EFFECT_SEND_TAXI)
                    spellPaths.insert(sInfo->EffectMiscValue);

        memset(sTaxiNodesMask, 0, sizeof(sTaxiNodesMask));
        memset(sOldContinentsNodesMask, 0, sizeof(sOldContinentsNodesMask));
        memset(sHordeTaxiNodesMask, 0, sizeof(sHordeTaxiNodesMask));
        memset(sAllianceTaxiNodesMask, 0, sizeof(sAllianceTaxiNodesMask));
        memset(sDeathKnightTaxiNodesMask, 0, sizeof(sDeathKnightTaxiNodesMask));
        for (uint32 i = 1; i < sTaxiNodesStore.GetNumRows(); ++i)
        {
            TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(i);
            if (!node)
                continue;

            TaxiPathSetBySource::const_iterator src_i = sTaxiPathSetBySource.find(i);
            if (src_i != sTaxiPathSetBySource.end() && !src_i->second.empty())
            {
                bool ok = false;
                for (TaxiPathSetForSource::const_iterator dest_i = src_i->second.begin(); dest_i != src_i->second.end(); ++dest_i)
                {
                    // not spell path
                    if (spellPaths.find(dest_i->second.ID) == spellPaths.end())
                    {
                        ok = true;
                        break;
                    }
                }

                if (!ok)
                    continue;
            }

            // valid taxi network node
            uint8  field   = (uint8)((i - 1) / 8);
            uint32 submask = 1 << ((i-1) % 8);

            sTaxiNodesMask[field] |= submask;
            if (node->MountCreatureID[0] && node->MountCreatureID[0] != 32981)
                sHordeTaxiNodesMask[field] |= submask;
            if (node->MountCreatureID[1] && node->MountCreatureID[1] != 32981)
                sAllianceTaxiNodesMask[field] |= submask;
            if (node->MountCreatureID[0] == 32981 || node->MountCreatureID[1] == 32981)
                sDeathKnightTaxiNodesMask[field] |= submask;

            // old continent node (+ nodes virtually at old continents, check explicitly to avoid loading map files for zone info)
            if (node->MapID < 2 || i == 82 || i == 83 || i == 93 || i == 94)
                sOldContinentsNodesMask[field] |= submask;

            // fix DK node at Ebon Hold and Shadow Vault flight master
            if (i == 315 || i == 333)
                ((TaxiNodesEntry*)node)->MountCreatureID[1] = 32981;
        }
    }

    // error checks
    if (bad_db2_files.size() >= DB2FilesCount)
    {
        TC_LOG_ERROR("misc", "\nIncorrect DataDir value in worldserver.conf or ALL required *.db2 files (%d) not found by path: %sdb2", DB2FilesCount, dataPath.c_str());
        exit(1);
    }
    else if (!bad_db2_files.empty())
    {
        std::string str;
        for (std::list<std::string>::iterator i = bad_db2_files.begin(); i != bad_db2_files.end(); ++i)
            str += *i + "\n";

        TC_LOG_ERROR("misc", "\nSome required *.db2 files (%u from %d) not found or not compatible:\n%s", (uint32)bad_db2_files.size(), DB2FilesCount, str.c_str());
        exit(1);
    }

    // Check loaded DB2 files proper version
    if (!sItemStore.LookupEntry(83086)             ||       // last item added in 4.3.4 (15595)
        !sItemExtendedCostStore.LookupEntry(3872)  )        // last item extended cost added in 4.3.4 (15595)
    {
        TC_LOG_ERROR("misc", "Please extract correct db2 files from client 4.3.4 15595.");
        exit(1);
    }

    TC_LOG_INFO("misc", ">> Initialized %d DB2 data stores.", DB2FilesCount);
}

DB2StorageBase const* GetDB2Storage(uint32 type)
{
    DB2StorageMap::const_iterator itr = DB2Stores.find(type);
    if (itr != DB2Stores.end())
        return itr->second;

    return NULL;
}

uint32 GetItemDisplayID(uint32 appearanceID)
{
    auto itr = sItemDisplayIDMap.find(appearanceID);
    if (itr != sItemDisplayIDMap.end())
        return itr->second;
    return 0;
}
