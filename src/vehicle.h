#pragma once
#include "pch.h"

class Vehicle
{
private:
    static inline bool m_bAutoUnflip;
    static inline bool m_bBikeFly;
    static inline bool m_bDontFallBike;
    static inline bool m_bVehHeavy;
    static inline bool m_bVehWatertight;
    static inline bool m_bNoDamage;
    static inline int m_nDoorMenuButton;
    static inline std::string m_DoorNames[6] =
    { "Hood", "Boot", "Front left door", "Front right door", "Rear left door", "Rear right door" };
    static inline int m_nVehRemoveRadius;
    static inline bool m_bLockSpeed;
    static inline float m_fLockSpeed;
    static inline std::vector<std::vector<float>> m_CarcolsColorData;
    struct PaintData
    {
        static inline bool m_bMatFilter = true;
        static inline int m_nRadioButton = 1;
        static inline float m_fColorPicker[3] { 0, 0, 0 };
        static inline std::vector<std::string> m_vecNames{"Default"};
        static inline std::string m_Selected = "Default";
    };

#ifdef GTASA
    static inline bool m_bDisableColDetection;
    static inline bool m_bVehFlipNoBurn;
    static inline bool m_bMoreTrainCams;
    static inline bool m_bNoDerail;
    static inline std::map<int, std::string> m_VehicleIDE;
    struct NeonData
    {
        static inline float m_fColorPicker[3] { 0, 0, 0 };
        static inline bool m_bRainbowEffect;
        static inline uint m_nRainbowTimer;
        static inline bool m_bApplyOnTraffic;
        static inline uint m_bTrafficTimer;
    };
    static inline ResourceStore m_TuneData { "components", eResourceType::TYPE_IMAGE_TEXT, ImVec2(100, 80) };
#endif

    struct Spawner
    {
#ifdef GTASA
        static inline ResourceStore m_VehData { "vehicles", eResourceType::TYPE_IMAGE_TEXT, ImVec2(100, 75)};
#else
        static inline ResourceStore m_VehData {"vehicles", eResourceType::TYPE_TEXT};
#endif
        static inline bool m_bSpawnInside = true;
        static inline bool m_bSpawnInAir = true;
        static inline char m_nLicenseText[9];
    };
    struct UnlimitedNitro
    {
        static inline bool m_bEnabled;
        static inline bool m_bCompAdded;
    };

#ifdef GTASA
    static inline std::vector<std::string> m_HandlingFlagNames = // 32 flags
    {
        "1G_BOOST", "2G_BOOST", "NPC_ANTI_ROLL", "NPC_NEUTRAL_HANDL", "NO_HANDBRAKE", "STEER_REARWHEELS",
        "HB_REARWHEEL_STEER", "ALT_STEER_OPT",
        "WHEEL_F_NARROW2", "WHEEL_F_NARROW", "WHEEL_F_WIDE", "WHEEL_F_WIDE2", "WHEEL_R_NARROW2", "WHEEL_R_NARROW",
        "WHEEL_R_WIDE", "WHEEL_R_WIDE2",
        "HYDRAULIC_GEOM", "HYDRAULIC_INST", "HYDRAULIC_NONE", "NOS_INST", "OFFROAD_ABILITY", "OFFROAD_ABILITY2",
        "HALOGEN_LIGHTS", "PROC_REARWHEEL_1ST",
        "USE_MAXSP_LIMIT", "LOW_RIDER", "STREET_RACER", "SWINGING_CHASSIS", "Unused 1", "Unused 2", "Unused 3",
        "Unused 4"
    };

    static inline std::vector<std::string> m_ModelFlagNames = // 32 flags
    {
        "IS_VAN", "IS_BUS", "IS_LOW", "IS_BIG", "REVERSE_BONNET", "HANGING_BOOT", "TAILGATE_BOOT", "NOSWING_BOOT",
        "NO_DOORS", "TANDEM_SEATS",
        "SIT_IN_BOAT", "CONVERTIBLE", "NO_EXHAUST", "DOUBLE_EXHAUST", "NO1FPS_LOOK_BEHIND", "FORCE_DOOR_CHECK",
        "AXLE_F_NOTILT", "AXLE_F_SOLID", "AXLE_F_MCPHERSON",
        "AXLE_F_REVERSE", "AXLE_R_NOTILT", "AXLE_R_SOLID", "AXLE_R_MCPHERSON", "AXLE_R_REVERSE", "IS_BIKE", "IS_HELI",
        "IS_PLANE", "IS_BOAT", "BOUNCE_PANELS",
        "DOUBLE_RWHEELS", "FORCE_GROUND_CLEARANCE", "IS_HATCHBAC1K"
    };
#endif

private:

#ifdef GTASA
    static void AddComponent(const std::string& component, bool display_message = true);
    static void RemoveComponent(const std::string& component, bool display_message = true);
    static int GetRandomTrainIdForModel(int model);
#endif

public:
    Vehicle() = delete;
    Vehicle(const Vehicle&) = delete;

#ifdef GTASA
    static void SpawnVehicle(std::string& name);
#else
    static void SpawnVehicle(std::string& rootkey, std::string& vehName, std::string& model);
#endif

    static void Init();
    static std::string GetNameFromModel(int model);
    static int GetModelFromName(const char* name);
    static void ShowPage();
};
