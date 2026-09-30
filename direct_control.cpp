#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <intrin.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <string>

namespace {

constexpr std::uint32_t kSupportedTimeDateStamp = 0x68CB9CFB;
constexpr std::uint32_t kSupportedSizeOfImage = 0x01476000;

// RVAs from the matching DoorKickers2.pdb shipped with game version 1.0.12.0.
constexpr std::uintptr_t kRvaGameInputUpdate = 0x00131360;
constexpr std::uintptr_t kRvaHumanClientUpdateLaserLight = 0x000B0180;
// g_pRenderer points to GameRenderer; embedded renderOptions +8, nvg +0x50.
constexpr std::uintptr_t kRvaRendererGlobal = 0x00FF0E98;
constexpr std::size_t kRendererNvgOffset = 0x58;
constexpr std::uintptr_t kRvaGameInputGlobal = 0x00FE8A68;
constexpr std::uintptr_t kRvaGameInputCheckKeyboardEvents = 0x0012E2F0;
constexpr std::uintptr_t kRvaGetSelectedHuman = 0x00137090;
constexpr std::uintptr_t kRvaCycleTrooperSelection = 0x00136C80;
constexpr std::uintptr_t kRvaGameRendererRenderPaths = 0x00170140;
constexpr std::uintptr_t kRvaRenderPath = 0x0016CAA0;
constexpr std::uintptr_t kRvaUpdateHumanProfile = 0x000EB640;
constexpr std::uintptr_t kRvaGuiFindChild = 0x000D85A0;
constexpr std::uintptr_t kRvaGuiCloneStaticText = 0x000DF250;
constexpr std::uintptr_t kRvaGuiSetName = 0x000D7DB0;
constexpr std::uintptr_t kRvaGuiChangeText = 0x000E0000;
constexpr std::uintptr_t kRvaGenerateSoundAwareness = 0x00263FB0;
constexpr std::uintptr_t kRvaFirearmSoundAwarenessReturn = 0x002BE1EA;
constexpr std::uintptr_t kRvaGameClientGlobal = 0x00FF0EA8;
constexpr std::uintptr_t kRvaGameServerGlobal = 0x00FF2838;
constexpr std::uintptr_t kRvaPointerCursor = 0x01358810;
constexpr std::uintptr_t kRvaConvertScreenToMapCoords = 0x00184E60;
constexpr std::uintptr_t kRvaPathAddToPath = 0x00198FA0;
constexpr std::uintptr_t kRvaPathAddToPathEnd = 0x00198800;
constexpr std::uintptr_t kRvaDeletePathFromIndexOnward = 0x000B9960;
constexpr std::uintptr_t kRvaNotifyServerOfPathModification = 0x000B9A50;
constexpr std::uintptr_t kRvaNetAddReliableEvent = 0x001F90D0;
constexpr std::uintptr_t kRvaBitPackerWriteBits = 0x0021FF70;
constexpr std::uintptr_t kRvaBitPackerReadBits = 0x002200C0;
constexpr std::uintptr_t kRvaHumanServerNetReceiveEvent = 0x0029CFF0;
constexpr std::uintptr_t kRvaProjectileClientNetReceiveEvent = 0x000C03A0;
constexpr std::uintptr_t kRvaProjectileServerDetonate = 0x002B5710;
constexpr std::uintptr_t kRvaFilterOutBulletCollisions = 0x002BD5B0;
constexpr std::uintptr_t kRvaHumanTemplateType = 0x01355EB0;
constexpr std::uintptr_t kRvaWaypointsAdvanceAlongPath = 0x00262370;
constexpr std::uintptr_t kRvaHumanServerUpdate = 0x00296BA0;
constexpr std::uintptr_t kRvaHumanServerGetMoveSpeed = 0x00297E10;
constexpr std::uintptr_t kRvaHumanServerCreateQueueCommand = 0x0029F910;
constexpr std::uintptr_t kRvaPathfinderTraceLine = 0x00253430;
constexpr std::uintptr_t kRvaPathfinderIsPointClear = 0x00253C50;
constexpr std::uintptr_t kRvaUpdateOrientationOnWaypoints = 0x00299380;
constexpr std::uintptr_t kRvaProcessCmdLookAim = 0x002A01D0;
constexpr std::uintptr_t kRvaProcessCmdFirearmReload = 0x002A0F90;
constexpr std::uintptr_t kRvaFirearmCancelReload = 0x002BF690;
constexpr std::uintptr_t kRvaProcessCmdItemEquip = 0x002A1910;
constexpr std::uintptr_t kRvaProcessCmdItemHolster = 0x002A1A60;
constexpr std::uintptr_t kRvaHumanServerCmdQueueCrouch = 0x0029FF20;
constexpr std::uintptr_t kRvaProcessCmdSprinting = 0x002A6BD0;
constexpr std::uintptr_t kRvaHumanCommandQueueRemove = 0x002B0220;
constexpr std::uintptr_t kRvaHumanServerCmdFirearmShoot = 0x002A10E0;
constexpr std::uintptr_t kRvaFirearmReadyAim = 0x002BD3B0;
constexpr std::uintptr_t kRvaFirearmGuard = 0x002BD4C0;
constexpr std::uintptr_t kRvaHumanGetLOS = 0x00271240;
constexpr std::uintptr_t kRvaFirearmGuardLOSReturn = 0x0028353C;
constexpr std::uintptr_t kRvaFirearmFire = 0x002BDDC0;
constexpr std::uintptr_t kRvaHumanTryShootingFirearm = 0x00272BE0;
constexpr std::uintptr_t kRvaCmdQueueGrenadeThrow = 0x002A3B60;
constexpr std::uintptr_t kRvaActionWPOnChangeAction = 0x0009BC60;
constexpr std::uintptr_t kRvaActionWPRequestCreation = 0x0009C750;
constexpr std::uintptr_t kRvaActionWPUpdateVisuals = 0x0009D2B0;
constexpr std::uintptr_t kRvaActionWPRemoveVisuals = 0x0009DD40;
constexpr std::uintptr_t kRvaActionWPOnGrenadeTargetMoved = 0x000A0570;
constexpr std::uintptr_t kRvaEntitySetTransform = 0x001F7BF0;
constexpr std::uintptr_t kRvaMatrix3Identity = 0x007CBE78;
constexpr std::uintptr_t kRvaActionDummy = 0x00025DB0;
constexpr std::uintptr_t kRvaGameRendererRenderLine = 0x001500B0;
constexpr std::uintptr_t kRvaGameClientUpdateCamera = 0x00187F70;
constexpr std::uintptr_t kRvaCameraUpdate = 0x001A0090;
constexpr std::uintptr_t kRvaGameGuiGlobal = 0x00FE71A8;
constexpr std::uintptr_t kRvaGameGuiIsAnyMenuOpened = 0x000FB320;
constexpr std::uintptr_t kRvaGameInputButton1Down = 0x001322E0;
constexpr std::uintptr_t kRvaGameInputButton1Click = 0x00132630;
constexpr std::uintptr_t kRvaGameInputButton1Drag = 0x00132C00;
constexpr std::uintptr_t kRvaGameInputButton2Down = 0x00133140;
constexpr std::uintptr_t kRvaGameInputButton2Click = 0x001331E0;
constexpr std::uintptr_t kRvaGameInputButton2Drag = 0x001332E0;
constexpr std::uintptr_t kRvaOptionsGameZoomSpeed = 0x007CAB78;
constexpr std::uintptr_t kRvaGameInputChangeSelection = 0x0012DFA0;
constexpr std::uintptr_t kRvaGameGuiOpenWaypointMenu = 0x000FD3F0;
constexpr std::uintptr_t kRvaPotentialActionWaypointGlobal = 0x00FE6F38;
constexpr std::uintptr_t kRvaActionWPClientTypeList = 0x007CB538;
constexpr std::uintptr_t kRvaDoorServerTypeList = 0x007CC260;
constexpr std::uintptr_t kRvaHumanServerCmdQueueDoorOpen = 0x002A2520;

constexpr std::size_t kHumanOriginOffset = 0x6C;
constexpr std::size_t kDoorServerTypeLinkOffset = 0x118;
constexpr std::size_t kDoorServerStateOffset = 0x138;
constexpr std::size_t kDoorServerLockedOffset = 0x149;
constexpr std::size_t kGameClientCameraOffset = 0x107274;
constexpr std::size_t kGameClientViewportOffset = 0x10755C;
constexpr std::size_t kCameraPositionOffset = 0x0C;
constexpr std::size_t kCameraMoveTargetCountOffset = 0x15C;
constexpr std::size_t kHumanHealthOffset = 0x28;
constexpr std::size_t kHumanIdOffset = 0x10;
constexpr std::size_t kHumanControllableOffset = 0x333;
constexpr std::size_t kHumanPathOffset = 0xD90;
constexpr std::size_t kHumanServerWaypointsOffset = 0x148;
constexpr std::size_t kGameEntityArrayOffset = 0x2E8;
constexpr std::size_t kGamePathfinderOffset = 0x18;
constexpr std::size_t kPathWaypointCountOffset = 0x38;
constexpr std::size_t kPathEndpointOffset = 0x78;

constexpr std::size_t kUpdateDetourLength = 18;
constexpr std::size_t kKeyboardDetourLength = 15;
constexpr std::size_t kRenderPathsDetourLength = 16;
constexpr std::size_t kRenderPathDetourLength = 15;
// Three complete register-save instructions; no relative operands.
constexpr std::size_t kUpdateHumanProfileDetourLength = 15;
// Register save, push, stack allocation, XMM save; no relative operands.
constexpr std::size_t kGenerateSoundAwarenessDetourLength = 15;
constexpr std::size_t kAdvanceAlongPathDetourLength = 14;
// Entire prologue through `sub rsp, 0F0h`; 18 bytes would split it.
constexpr std::size_t kHumanServerNetReceiveDetourLength = 19;
constexpr std::size_t kProjectileClientNetReceiveDetourLength = 16;
constexpr std::size_t kProjectileServerDetonateDetourLength = 14;
constexpr std::size_t kBulletCollisionFilterDetourLength = 15;
constexpr std::size_t kActionWPRequestCreationDetourLength = 16;
constexpr std::size_t kHumanServerUpdateDetourLength = 17;
constexpr std::size_t kHumanClientGetAnimsDetourLength = 16;
constexpr std::size_t kGetMoveSpeedDetourLength = 15;
constexpr std::size_t kOrientationDetourLength = 16;
constexpr std::size_t kCameraUpdateDetourLength = 16;
constexpr std::size_t kButton1DownDetourLength = 20;
constexpr std::size_t kButton1ClickDetourLength = 17;
constexpr std::size_t kButton1DragDetourLength = 19;
constexpr std::size_t kButton2DownPrefixLength = 16;
constexpr std::size_t kButton2ClickPrefixLength = 9;
constexpr std::size_t kButton2DragDetourLength = 21;
constexpr std::size_t kChangeSelectionDetourLength = 15;
constexpr std::size_t kOpenWaypointMenuDetourLength = 17;
constexpr std::size_t kHumanTryShootingFirearmDetourLength = 15;
constexpr std::size_t kHumanGetLOSDetourLength = 15;
constexpr std::size_t kFirearmFireDetourLength = 16;
constexpr std::size_t kAbsoluteJumpLength = 14;
constexpr int kMovementRefreshCooldownMs = 120;
constexpr float kMovementLookahead = 1.65F;
constexpr float kTraversalLookahead = 2.75F;
constexpr float kMovementRefreshLead = 0.65F;
constexpr int kMaxActionWaypoints = 32;
constexpr int kActionJumpOverObstacle = 13;
constexpr int kActionJumpThroughWindow = 14;
constexpr int kDirectControlProbeEvent = 38;
constexpr int kDirectControlProbeVersion = 1;
constexpr int kDirectControlInputEvent = 39;
constexpr int kDirectControlInputVersion = 13;
constexpr int kReadyHoldNone = 0;
constexpr int kReadyHoldHigh = 1;
constexpr int kReadyHoldLow = 2;
constexpr int kNativeClientToServerMask = 0x0F;
constexpr std::uint32_t kMaximumEntityId = 12000;
constexpr std::uint32_t kDirectMovementCollisionMask = 0x84;
constexpr int kInventorySlotCount = 27;
constexpr int kPrimaryWeaponInventoryType = 1;
constexpr int kSecondaryWeaponInventoryType = 5;
constexpr int kNoEquipmentRequest = -1;
constexpr int kHolsterEquipmentRequest = 0;
constexpr std::uint32_t kCrosshairColor = 0xFFE4E4E4;
constexpr std::uint32_t kCrosshairPendingColor = 0xFF0080FF;
constexpr std::uint32_t kCrosshairFiredColor = 0xFF00FF00;
constexpr std::uint32_t kCrosshairExpiredColor = 0xFF0000FF;
constexpr std::uint32_t kGrenadeUnavailableColor = 0xFF0000FF;
constexpr std::uint32_t kGrenadeQueuedColor = 0xFF0080FF;
constexpr std::uint32_t kGrenadeThrownColor = 0xFF00FF00;
constexpr std::uint32_t kGrenadeSelectionColor = 0xFF00A5FF;
constexpr float kCrosshairInnerRadius = 0.16F;
constexpr float kCrosshairOuterRadius = 0.42F;
constexpr float kCrosshairHalfThickness = 0.026F;
constexpr float kCrosshairShotExpansion = 0.17F;
constexpr ULONGLONG kCrosshairShotPulseMs = 130;
constexpr float kCrosshairHeightOffset = 0.08F;
constexpr float kCameraLookAheadHorizontalFraction = 0.85F;
constexpr float kCameraLookAheadVerticalFraction = 0.85F;
constexpr float kCameraLookAheadHorizontalMax = 20.0F;
constexpr float kCameraLookAheadVerticalMax = 10.0F;
constexpr float kCameraLookAheadSmoothingMs = 160.0F;
constexpr float kDirectControlWheelZoomMultiplier = 1.7F;
constexpr float kDoorInteractRange = 1.2F;
constexpr float kSpyCameraCursorTargetRadius = 1.5F;
constexpr float kDoorNearSideOffset = 0.6F;
constexpr float kDoorFacingMinimumDot = 0.35F;
constexpr float kGrenadeThrowAngleDegrees = 45.0F;
constexpr int kGrenadeHoldThresholdMs = 220;
constexpr float kGrenadeQuickThrowDistance = 6.0F;
constexpr float kGrenadeLaunchHeightOffset = 0.9F;
constexpr int kGrenadeThrowTimeoutMs = 5000;
constexpr int kShootRequestLifetimeMs = 2500;
constexpr ULONGLONG kShootFeedbackDurationMs = 350;
constexpr std::size_t kHumanServerInventoryOffset = 0x12C0;
constexpr std::size_t kHumanServerEquippedItemOffset = 0x1398;
constexpr std::size_t kHumanServerMovementFlagsOffset = 0x1FC;
constexpr std::size_t kHumanServerMovementStateOffset = 0x204;
constexpr std::size_t kHumanServerCommandQueueOffset = 0x310;
constexpr std::size_t kHumanServerCommandArrayOffset = 0x318;
constexpr std::size_t kHumanServerCommandWriteOffset = 0x1218;
constexpr std::size_t kHumanServerCommandReadOffset = 0x121C;
constexpr std::size_t kHumanServerCommandSize = 0x78;
constexpr int kSprintCommandType = 0x17;
constexpr int kReloadCommandType = 0x0A;
constexpr int kFirearmReloadingState = 10;

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Vector2 {
    float x;
    float z;
};

struct ActionWaypoint {
    std::array<std::byte, 0x54> storage{};

    ActionWaypoint() {
        constexpr float defaultDirectionX = 1.0F;
        constexpr std::int16_t defaultWaitForClearMs = 500;
        std::memcpy(storage.data() + 0x08, &defaultDirectionX, sizeof(defaultDirectionX));
        std::memcpy(
            storage.data() + 0x50,
            &defaultWaitForClearMs,
            sizeof(defaultWaitForClearMs));
    }
};

static_assert(sizeof(ActionWaypoint) == 0x54);

struct LookAimCommand {
    std::array<std::byte, 0x20> storage{};

    explicit LookAimCommand(const Vector3& direction) {
        constexpr float completionDot = 0.999F;
        std::memcpy(storage.data() + 0x0C, &completionDot, sizeof(completionDot));
        std::memcpy(storage.data() + 0x10, &direction, sizeof(direction));
    }
};

static_assert(sizeof(LookAimCommand) == 0x20);

struct ButtonDownState {
    Vector3 point{};
    Vector3 pointInEntitySpace{};
    Vector3 pointOnPath{};
    int pointOnPathIndex = -1;
    std::uint32_t entityId = 0;
    bool newSelection = false;
    std::array<std::byte, 3> padding{};
};

static_assert(sizeof(ButtonDownState) == 0x30);

// Exact layout from the shipped DoorKickers2.pdb. The door-shotgun path
// zero-initializes this POD, which suppresses critical chance and suppression.
// Direct-control shots use neutral multipliers for normal firearm behavior.
struct FirearmShoot {
    float targetDist = 0.0F;
    float targetRadius = 0.0F;
    float accuracyAdd = 0.0F;
    float critChanceAdd = 0.0F;
    float critChanceMul = 1.0F;
    float rpsAdd = 0.0F;
    float customRoundsPerSecond = 0.0F;
    float suppressionScale = 1.0F;
    int coverReduction = 0;
    bool ignoreArmor = false;
    bool noAISoundEvent = false;
    std::array<std::byte, 2> padding{};
};

static_assert(sizeof(FirearmShoot) == 0x28);

using GameInputUpdateFn = void(__fastcall*)(void*, int);
using GameInputCheckKeyboardEventsFn = void(__fastcall*)(void*);
using GameRendererRenderPathsFn = void(__fastcall*)(void*);
using UpdateHumanProfileFn = void(__fastcall*)(void*, const void*);
using GuiFindChildFn = void*(__fastcall*)(void*, const char*);
using GuiCloneStaticTextFn = void*(__fastcall*)(const void*);
using GuiSetNameFn = void(__fastcall*)(void*, const char*);
using GuiChangeTextFn = bool(__fastcall*)(void*, const char*);
using GuiSetImageFn = void(__fastcall*)(void*, const char*);
using GuiRebuildTextFn = void(__fastcall*)(void*);
using TranslateTextFn = const char*(__fastcall*)(const char*, bool);
using GenerateSoundAwarenessFn = void(__fastcall*)(void*, float, bool);
using RenderPathFn = void(__fastcall*)(
    void*, void*, void*, std::uint32_t*,
    int, int, int, int, int, int, int, int, void*);
using GetSelectedHumanFn = const void*(__fastcall*)(const void*);
using CycleTrooperSelectionFn = void(__fastcall*)(void*, int);
using ConvertScreenToMapCoordsFn = Vector3*(__fastcall*)(
    const void*, Vector3*, float, float);
using PathAddToPathFn = void(__fastcall*)(
    void*, const void*, float, float, int&, int&, ActionWaypoint*, int, int&);
using PathAddToPathEndFn = void(__fastcall*)(void*);
using DeletePathFn = void(__fastcall*)(void*, int, float, float);
using NotifyPathFn = void(__fastcall*)(
    void*, const void*, int, int, int, const ActionWaypoint*, bool);
using NetAddReliableEventFn = void*(__fastcall*)(void*, int, int);
using BitPackerWriteBitsFn = void(__fastcall*)(void*, int, int);
using BitPackerReadBitsFn = int(__fastcall*)(void*, int);
using HumanServerNetReceiveEventFn = void(__fastcall*)(
    void*, int, int, unsigned int, void*);
using ProjectileClientNetReceiveEventFn = void(__fastcall*)(
    void*, int, int, unsigned int, void*);
using ProjectileServerDetonateFn = void(__fastcall*)(
    void*, const Vector3*);
using BulletCollisionFilterFn = char(__fastcall*)(const void*, unsigned int, int);
using ActionWPRequestCreationFn = void(__fastcall*)(void*);
using AdvanceAlongPathFn = bool(__fastcall*)(
    void*, const Vector3*, float, int, Vector3*);
using HumanServerUpdateFn = void(__fastcall*)(void*, int);
using HumanClientGetAnimsFn = void(__fastcall*)(
    const void*, const void*, void*);
using HumanServerGetMoveSpeedFn = float(__fastcall*)(const void*);
using CreateQueueCommandFn = void*(__fastcall*)(
    void*, int, const void*, const void*);
using PathfinderTraceLineFn = bool(__fastcall*)(
    const void*, std::uint64_t, std::uint64_t, std::uint32_t, Vector2*);
using PathfinderIsPointClearFn = bool(__fastcall*)(
    const void*, std::uint64_t, std::uint32_t);
using UpdateOrientationOnWaypointsFn = void(__fastcall*)(void*);
using ProcessCmdLookAimFn = bool(__fastcall*)(void*, LookAimCommand*);
using GameRendererRenderLineFn = void(__fastcall*)(
    const void*, const Vector3*, const Vector3*, std::uint32_t);
using GameClientUpdateCameraFn = void(__fastcall*)(void*, int);
using CameraUpdateFn = void(__fastcall*)(void*, int);
using FirearmCancelReloadFn = void(__fastcall*)(void*);
using GameGuiIsAnyMenuOpenedFn = bool(__fastcall*)(const void*);
using GameInputButton1DownFn = void(__fastcall*)(void*, const void*);
using GameInputButton1ClickFn = void(__fastcall*)(
    void*, const void*, const Vector3*);
using GameInputButton2ClickFn = void(__fastcall*)(
    void*, const ButtonDownState*, const Vector3*);
using GameInputButton2DownFn = void(__fastcall*)(void*, const void*);
using GameInputChangeSelectionFn = void(__fastcall*)(void*, void*);
using GameGuiOpenWaypointMenuFn = void(__fastcall*)(void*, void*, int);
using HumanServerCmdFirearmShootFn = bool(__fastcall*)(
    void*, const FirearmShoot*);
using FirearmReadyAimFn = void(__fastcall*)(void*);
using FirearmGuardFn = void(__fastcall*)(void*, bool);
using HumanGetLOSFn = std::uint32_t*(__fastcall*)(
    std::uint32_t*, const void*, const Vector3*);
using FirearmFireFn = bool(__fastcall*)(
    void*, const Vector3*, const Vector3*, float, float,
    const FirearmShoot*);
using HumanTryShootingFirearmFn = int(__fastcall*)(void*, void*);
using CmdQueueGrenadeThrowFn = void(__fastcall*)(
    void*, int, const Vector3*, float, const Vector3*, void*);
using HumanServerCmdQueueCrouchFn = void(__fastcall*)(void*, bool);
using HumanServerCmdQueueDoorOpenFn = void(__fastcall*)(void*, void*, bool);
using HumanServerCmdQueueSpyCameraFn = void(__fastcall*)(
    void*, int, std::uint32_t, const Vector3*);
using HumanServerCmdQueueDoorBreachFn = void(__fastcall*)(void*, int, void*);
using HumanCommandQueueRemoveFn = void(__fastcall*)(void*, int);

std::uintptr_t g_moduleBase = 0;
using UpdateLaserLightFn = void(__fastcall*)(void*);
UpdateLaserLightFn g_originalUpdateLaserLight = nullptr;
using EntitySetAlphaFn = void(__fastcall*)(void*, std::uint8_t);
EntitySetAlphaFn g_entitySetAlpha = nullptr;
struct LaserPreference {
    const void* owner = nullptr;
    const void* itemTemplate = nullptr;
    bool enabled = true;
};
std::unordered_map<std::uint64_t, LaserPreference> g_laserPreferences;
HINSTANCE g_selfModule = nullptr;
float g_zoomMultiplier = kDirectControlWheelZoomMultiplier;
float g_crosshairScale = 1.0F;
unsigned int g_remoteInputIntervalMs = 80;
bool g_friendlyFire = false;
bool g_hostageBulletDamage = false;
bool g_nativeWheelActions = false;
GameInputUpdateFn g_originalUpdate = nullptr;
GameInputCheckKeyboardEventsFn g_originalCheckKeyboardEvents = nullptr;
GameRendererRenderPathsFn g_originalRenderPaths = nullptr;
UpdateHumanProfileFn g_originalUpdateHumanProfile = nullptr;
GuiFindChildFn g_guiFindChild = nullptr;
GuiCloneStaticTextFn g_guiCloneStaticText = nullptr;
GuiSetNameFn g_guiSetName = nullptr;
GuiChangeTextFn g_guiChangeText = nullptr;
GuiRebuildTextFn g_guiRebuildText = nullptr;
GuiCloneStaticTextFn g_guiCloneStaticImage = nullptr;
GuiSetImageFn g_guiSetImage = nullptr;
TranslateTextFn g_translateText = nullptr;
GenerateSoundAwarenessFn g_originalGenerateSoundAwareness = nullptr;
RenderPathFn g_originalRenderPath = nullptr;
GetSelectedHumanFn g_getSelectedHuman = nullptr;
CycleTrooperSelectionFn g_cycleTrooperSelection = nullptr;
ConvertScreenToMapCoordsFn g_convertScreenToMapCoords = nullptr;
PathAddToPathFn g_addToPath = nullptr;
PathAddToPathEndFn g_addToPathEnd = nullptr;
DeletePathFn g_deletePath = nullptr;
NotifyPathFn g_notifyPath = nullptr;
NetAddReliableEventFn g_netAddReliableEvent = nullptr;
BitPackerWriteBitsFn g_bitPackerWriteBits = nullptr;
BitPackerReadBitsFn g_bitPackerReadBits = nullptr;
HumanServerNetReceiveEventFn g_originalHumanServerNetReceiveEvent = nullptr;
ProjectileClientNetReceiveEventFn g_originalProjectileClientNetReceiveEvent = nullptr;
ProjectileServerDetonateFn g_originalProjectileServerDetonate = nullptr;
BulletCollisionFilterFn g_originalBulletCollisionFilter = nullptr;
ActionWPRequestCreationFn g_originalActionWPRequestCreation = nullptr;
AdvanceAlongPathFn g_originalAdvanceAlongPath = nullptr;
HumanServerUpdateFn g_originalHumanServerUpdate = nullptr;
HumanClientGetAnimsFn g_originalHumanClientGetAnims = nullptr;
HumanServerGetMoveSpeedFn g_originalGetMoveSpeed = nullptr;
CreateQueueCommandFn g_createQueueCommand = nullptr;
PathfinderTraceLineFn g_pathfinderTraceLine = nullptr;
PathfinderIsPointClearFn g_pathfinderIsPointClear = nullptr;
UpdateOrientationOnWaypointsFn g_originalUpdateOrientation = nullptr;
ProcessCmdLookAimFn g_processCmdLookAim = nullptr;
GameRendererRenderLineFn g_renderLine = nullptr;
GameClientUpdateCameraFn g_originalUpdateCamera = nullptr;
CameraUpdateFn g_cameraUpdate = nullptr;
FirearmCancelReloadFn g_firearmCancelReload = nullptr;
GameGuiIsAnyMenuOpenedFn g_isAnyMenuOpened = nullptr;
GameInputButton1DownFn g_originalButton1Down = nullptr;
GameInputButton1ClickFn g_originalButton1Click = nullptr;
GameInputButton1ClickFn g_originalButton1Drag = nullptr;
GameInputButton2ClickFn g_button2Click = nullptr;
GameInputButton2DownFn g_originalButton2Down = nullptr;
GameInputButton2ClickFn g_originalButton2Click = nullptr;
GameInputButton2ClickFn g_originalButton2Drag = nullptr;
GameInputChangeSelectionFn g_originalChangeSelection = nullptr;
GameGuiOpenWaypointMenuFn g_originalOpenWaypointMenu = nullptr;
HumanServerCmdFirearmShootFn g_cmdFirearmShoot = nullptr;
FirearmReadyAimFn g_firearmReadyAim = nullptr;
FirearmGuardFn g_firearmGuard = nullptr;
HumanGetLOSFn g_originalHumanGetLOS = nullptr;
FirearmFireFn g_originalFirearmFire = nullptr;
HumanTryShootingFirearmFn g_originalHumanTryShootingFirearm = nullptr;
CmdQueueGrenadeThrowFn g_queueGrenadeThrow = nullptr;
HumanServerCmdQueueCrouchFn g_queueCrouch = nullptr;
HumanServerCmdQueueDoorOpenFn g_queueDoorOpen = nullptr;
HumanServerCmdQueueSpyCameraFn g_queueSpyCamera = nullptr;
HumanServerCmdQueueDoorBreachFn g_queueDoorBreach = nullptr;
HumanCommandQueueRemoveFn g_removeHumanCommand = nullptr;
using EquipmentNewFn = void*(__fastcall*)(const char*);
using InventoryEquipFn = void(__fastcall*)(void*, void*, int, void*);
EquipmentNewFn g_equipmentNew = nullptr;
InventoryEquipFn g_inventoryEquip = nullptr;
using ClientSetInventoryFn = void(__fastcall*)(void*, const void* const*);
using GetEquipmentTemplateFn = const void*(__fastcall*)(const void*, std::uint32_t);
ClientSetInventoryFn g_originalClientSetInventory = nullptr;
GetEquipmentTemplateFn g_getEquipmentTemplate = nullptr;

bool g_enabled = false;
int g_readyHold = kReadyHoldNone;
int g_previousReadyHold = kReadyHoldNone;
enum class HighReadyPhase { None, Enter, Hold, Exit };
struct HighReadyPoseState {
    const void* animationSet = nullptr;
    HighReadyPhase phase = HighReadyPhase::None;
    bool restart = false;
};
std::unordered_map<const void*, HighReadyPoseState> g_highReadyPoses;
bool HighReadyWeaponEligible(const void* human);
bool HighReadyBlocksFire(const void* human) {
    const auto found = g_highReadyPoses.find(human);
    return found != g_highReadyPoses.end() && found->second.phase != HighReadyPhase::None;
}
bool g_cyclingSelectionWithTab = false;
bool g_openingDirectContextMenu = false;
thread_local bool g_renderingGrenadePreviewPaths = false;
bool g_wasMoving = false;
std::uint32_t g_activeHumanId = 0;
int g_accumulatedMs = 0;
Vector2 g_lastDirection{};
void* g_controlledClientHuman = nullptr;
void* g_controlledServerHuman = nullptr;
Vector2 g_directMovementDirection{};
int g_leanSide = 0;
bool g_leanReachedHold = false;
int g_leanPhysicalSide = 0;
ULONGLONG g_leanPressedAt = 0;
bool g_leanWasMoving = false;
int g_lastLeanReadyPhase = -1;
bool g_reloadRequested = false;
bool g_reloadSprintBlocked = false;
bool g_cancelReloadRequested = false;
void* g_reloadCanceledWeapon = nullptr;
Vector3 g_cursorWorld{};
bool g_cursorWorldValid = false;
Vector2 g_cameraLookAhead{};
std::uint32_t g_cameraFollowHumanId = 0;
bool g_shootRequested = false;
bool g_fireInputPressed = false;
bool g_fireHeld = false;
bool g_autoFireActive = false;
void* g_autoFireWeapon = nullptr;
Vector3 g_shootDirection{};
Vector3 g_shootTarget{};
int g_shootRequestRemainingMs = 0;
bool g_shootEquipQueued = false;
int g_equipmentRequest = kNoEquipmentRequest;
int g_selectedGearInventoryType = -1;
int g_preItemFirearmInventoryType = -1;
ULONGLONG g_crowbarSelectedAt = 0;
ULONGLONG g_spyCameraSelectedAt = 0;
bool g_spyCameraActionActive = false;
bool g_grenadeFireGuard = false;
ULONGLONG g_spyCameraRequestedAt = 0;
struct CrowbarAnimationPatch {
    const char* handle = nullptr;
    void* binding = nullptr;
    void* originalAnimation = nullptr;
};
std::array<CrowbarAnimationPatch, 14> g_crowbarAnimationPatches{};
int g_crowbarAnimationPatchCount = 0;
int g_selectedGrenadeHotkey = 0;
struct GrenadeChoice { int slot = -1; std::uint32_t itemId = 0; };
std::unordered_map<std::uint32_t, GrenadeChoice> g_grenadeChoices;
std::unordered_map<std::uint32_t, const void*> g_grenadeChoiceOwners;
GrenadeChoice g_lastRemoteGrenadeChoiceSent;
int g_selectedSpyCameraHotkey = 0;
int g_selectedCrowbarHotkey = 0;
bool g_grenadeAiming = false;
void* g_nativeGrenadePreviewWaypoint = nullptr;
std::array<std::byte, 0x54> g_nativePreviewSavedAction{};
Vector3 g_nativePreviewSavedOrigin{};
bool g_grenadeThrowRequested = false;
GrenadeChoice g_grenadeThrowChoice;
Vector3 g_grenadeTarget{};
Vector3 g_grenadeNativeThrowOrigin{};
float g_grenadeNativeThrowAngle = 0.0F;
int g_grenadeNativeThrowInventoryType = -1;
bool g_grenadeThrowUsesNativePreview = false;
int g_grenadeHoldMs = 0;
bool g_grenadeRequireRelease = false;
GrenadeChoice g_grenadeAimChoice;
ULONGLONG g_grenadeHoldStartTick = 0;
int g_grenadePendingSlot = -1;
void* g_grenadePendingItem = nullptr;
int g_grenadePendingQuantity = 0;
int g_grenadePendingMs = 0;
bool g_crouchHeld = false;
bool g_crouchCommandPending = false;
bool g_sprintDesired = false;
std::uint32_t g_doorInteractTargetId = 0;
std::uint32_t g_lastInteractedDoorId = 0;
bool g_traversalUsedThisPress = false;
bool g_traversalFWasDown = false;
bool g_traversalRepathRequested = false;
ULONGLONG g_traversalPathHoldUntil = 0;
std::uint32_t g_nativeWheelActionOwnerId = 0;
int g_nativeWheelActionType = 0;
ULONGLONG g_nativeWheelActionRequestedAt = 0;
ULONGLONG g_nativeWheelActionLastSeenAt = 0;
bool g_nativeWheelActionObserved = false;
bool g_nativeWheelActionCancelArmed = false;
bool g_menuSuspendedPath = false;
std::uint32_t g_coopProbeSentHumanId = 0;
ULONGLONG g_lastRemoteInputSendTick = 0;
Vector2 g_lastRemoteMovementSent{};
Vector2 g_lastRemoteAimSent{};
bool g_lastRemoteFireSent = false;
int g_lastRemoteReadySent = kReadyHoldNone;
bool g_lastRemoteSprintSent = false;
bool g_lastRemoteCrouchSent = false;
bool g_lastRemoteInteractSent = false;
std::uint32_t g_remoteInputSendCount = 0;
ULONGLONG g_lastVisualTraceTick = 0;
ULONGLONG g_lastStateTraceTick = 0;
unsigned int g_stateTraceIntervalMs = 0;
std::uint32_t g_crosshairRenderCount = 0;
enum class FireMode : int { Semi = 0, Auto = 1, Burst = 2 };
struct FireSelection { int mask = 1; int burst = 3; FireMode mode = FireMode::Semi; };
struct FireSequence { FireMode mode = FireMode::Semi; int remaining = 0; bool requireRelease = false; ULONGLONG deadline = 0; std::uint32_t weaponId = 0; };
wchar_t g_fireOptionsPath[MAX_PATH]{};
std::unordered_map<const void*, FireSelection> g_fireCapabilities;
std::unordered_map<std::uint32_t, std::unordered_map<const void*, FireMode>> g_fireSelections;
std::unordered_map<std::uint32_t, const void*> g_fireClientOwners;
FireSequence g_fireSequence;
FireMode g_displayFireMode = FireMode::Semi;
std::uint32_t g_displayFireWeapon = 0;
bool g_displayFireValid = false;
FireMode g_lastRemoteModeSent = FireMode::Semi;
std::uint32_t g_lastRemoteWeaponSent = 0;
enum class DoorGestureAction { None, Open, Kick, Lockpick };
struct DoorGestureResult { DoorGestureAction action = DoorGestureAction::None; std::uint32_t target = 0; };
struct DoorInteraction {
    bool down = false;
    bool consumed = false;
    bool pendingTap = false;
    ULONGLONG pressedAt = 0;
    ULONGLONG releasedAt = 0;
    std::uint32_t target = 0;
    bool lockpickActive = false;
    int lockpickSlot = -1;
};
DoorInteraction g_doorInteraction;
constexpr ULONGLONG kDoorDoubleTapMs = 250;
constexpr ULONGLONG kDoorHoldMs = 500;

// Timing is shared by local and authorized remote operators. Keep the door
// from the first press; aiming elsewhere never transfers a pending gesture.
DoorGestureResult AdvanceDoorGesture(DoorInteraction& state, bool down,
                                    std::uint32_t target, ULONGLONG now) {
    DoorGestureResult result;
    if (state.pendingTap && now - state.releasedAt > kDoorDoubleTapMs) {
        if (target == state.target) result = {DoorGestureAction::Open, state.target};
        state.pendingTap = false;
    }
    if (down && !state.down) {
        const bool second = state.pendingTap && target != 0 && target == state.target;
        state.pendingTap = false;
        state.target = target;
        state.pressedAt = now;
        state.consumed = second;
        if (second) result = {DoorGestureAction::Kick, target};
    }
    // F can be held just before the operator enters interaction range.
    // Acquire the first door, but never transfer an already acquired target.
    if (down && state.down && !state.consumed && state.target == 0 && target != 0) {
        state.target = target;
        state.pressedAt = now;
    }
    if (down && !state.consumed &&
        now - state.pressedAt >= kDoorHoldMs) {
        state.consumed = true;
        if (target == state.target) result = {DoorGestureAction::Lockpick, state.target};
    }
    if (!down && state.down && !state.consumed && state.target != 0) {
        state.pendingTap = true;
        state.releasedAt = now;
    }
    state.down = down;
    return result;
}
struct RemoteInput {
    void* human = nullptr;
    std::uint32_t humanId = 0;
    int senderMask = 0;
    Vector3 aim{};
    Vector2 movement{};
    float distance = 0.0F;
    ULONGLONG lastReceived = 0;
    ULONGLONG lastRateTrace = 0;
    std::uint32_t packetsReceived = 0;
    unsigned int maxInterPacketGapMs = 0;
    int shotRemainingMs = 0;
    bool fireHeld = false;
    FireSequence sequence;
    FireMode requestedMode = FireMode::Semi;
    std::uint32_t requestedWeapon = 0;
    int readyHold = kReadyHoldNone;
    int previousReadyHold = kReadyHoldNone;
    bool equipQueued = false;
    bool autoFire = false;
    void* autoFireWeapon = nullptr;
    bool sprintHeld = false;
    bool interactHeld = false;
    DoorInteraction doorInteraction;
    std::uint32_t lastInteractedDoorId = 0;
    bool crouchHeld = false;
    bool crouchCommandPending = false;
    bool reloadRequested = false;
    bool cancelReloadRequested = false;
    void* reloadCanceledWeapon = nullptr;
    bool grenadeRequested = false;
    bool grenadeLongThrow = false;
    bool grenadeNativePreview = false;
    Vector3 grenadeOriginOffset{};
    Vector3 grenadeTargetOffset{};
    float grenadeAngle = kGrenadeThrowAngleDegrees;
    ULONGLONG lastGrenadeThrow = 0;
    int equipmentRequest = kNoEquipmentRequest;
    int selectedGearInventoryType = -1;
    int preItemFirearmInventoryType = -1;
    GrenadeChoice grenadeChoice;
    GrenadeChoice grenadeThrowChoice;
};
std::unordered_map<std::uint32_t, RemoteInput> g_remoteInputs;

void* GetServerHuman(std::uint32_t entityId);
template <typename T> T& At(void* object, std::size_t offset);
template <typename T> const T& At(const void* object, std::size_t offset);


void TraceItemDetail(const char* line) {
    char path[MAX_PATH]{};
    const DWORD size = GetTempPathA(MAX_PATH, path);
    constexpr char name[] = "dk2_direct_action_items_trace.txt";
    if (!size || size + sizeof(name) > sizeof(path)) return;
    std::memcpy(path + size, name, sizeof(name));
    HANDLE file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    WriteFile(file, line, static_cast<DWORD>(std::strlen(line)), &written, nullptr);
    CloseHandle(file);
}

void TraceTraversal(const char* event, int a, int b, int c) {
    char path[MAX_PATH]{};
    const DWORD directoryLength = GetTempPathA(MAX_PATH, path);
    constexpr char fileName[] = "dk2_direct_action_traversal_trace.txt";
    if (directoryLength == 0 ||
        directoryLength + sizeof(fileName) > sizeof(path)) {
        return;
    }
    std::memcpy(path + directoryLength, fileName, sizeof(fileName));
    HANDLE file = CreateFileA(
        path,
        FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    char line[128];
    const int length = std::snprintf(
        line, sizeof(line), "%llu %s %d %d %d\r\n",
        static_cast<unsigned long long>(GetTickCount64()), event, a, b, c);
    if (length > 0 && length < static_cast<int>(sizeof(line))) {
        DWORD written = 0;
        WriteFile(file, line, static_cast<DWORD>(length), &written, nullptr);
    }
    CloseHandle(file);
}

void TraceLocalState(const void* human, std::uint32_t humanId,
                     const Vector2& movement, bool fireDown,
                     bool sprintDown, bool crouchDown) {
    if (!human || g_stateTraceIntervalMs == 0) return;
    const ULONGLONG now = GetTickCount64();
    if (now - g_lastStateTraceTick < g_stateTraceIntervalMs) return;
    g_lastStateTraceTick = now;
    const Vector3 position = At<Vector3>(human, kHumanOriginOffset);
    const float health = At<float>(human, kHumanHealthOffset);
    if (!std::isfinite(position.x) || !std::isfinite(position.z) ||
        !std::isfinite(health)) return;
    char path[MAX_PATH]{};
    const DWORD directoryLength = GetTempPathA(MAX_PATH, path);
    constexpr char fileName[] = "dk2_direct_action_state_trace.txt";
    if (directoryLength == 0 ||
        directoryLength + sizeof(fileName) > sizeof(path)) return;
    std::memcpy(path + directoryLength, fileName, sizeof(fileName));
    HANDLE file = CreateFileA(path, FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    char line[256]{};
    const int length = std::snprintf(line, sizeof(line),
        "%llu operator=%u x=%.3f z=%.3f hp=%.2f move_x=%.2f move_z=%.2f "
        "fire=%d sprint=%d ctrl=%d aim_x=%.3f aim_z=%.3f aim_valid=%d\r\n",
        static_cast<unsigned long long>(now), humanId, position.x,
        position.z, health, movement.x, movement.z, fireDown ? 1 : 0,
        sprintDown ? 1 : 0, crouchDown ? 1 : 0,
        g_cursorWorldValid ? g_cursorWorld.x : 0.0F,
        g_cursorWorldValid ? g_cursorWorld.z : 0.0F,
        g_cursorWorldValid ? 1 : 0);
    if (length > 0 && length < static_cast<int>(sizeof(line))) {
        DWORD written = 0;
        WriteFile(file, line, static_cast<DWORD>(length), &written, nullptr);
    }
    CloseHandle(file);
}

void TraceProjectileEffect(const char* stage, const void* projectile,
                           std::uint32_t ownerId, int eventType,
                           const Vector3& position) {
    if (!g_enabled || !projectile || !std::isfinite(position.x) ||
        !std::isfinite(position.z)) {
        return;
    }
    std::uint32_t humanId = 0;
    float distance = -1.0F;
    void* gameClient = *reinterpret_cast<void**>(
        g_moduleBase + kRvaGameClientGlobal);
    if (gameClient && g_controlledClientHuman &&
        g_activeHumanId < kMaximumEntityId &&
        At<void*>(gameClient, kGameEntityArrayOffset +
            static_cast<std::size_t>(g_activeHumanId) * sizeof(void*)) ==
            g_controlledClientHuman) {
        humanId = g_activeHumanId;
        const Vector3 origin = At<Vector3>(
            g_controlledClientHuman, kHumanOriginOffset);
        distance = std::hypot(position.x - origin.x,
                              position.z - origin.z);
    }
    char path[MAX_PATH]{};
    const DWORD directoryLength = GetTempPathA(MAX_PATH, path);
    constexpr char fileName[] = "dk2_direct_action_effect_trace.txt";
    if (directoryLength == 0 ||
        directoryLength + sizeof(fileName) > sizeof(path)) {
        return;
    }
    std::memcpy(path + directoryLength, fileName, sizeof(fileName));
    HANDLE file = CreateFileA(path, FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }
    char line[192]{};
    const int length = std::snprintf(line, sizeof(line),
        "%llu %s projectile=%u owner=%u event=%d x=%.2f z=%.2f "
        "operator=%u distance=%.2f\r\n",
        static_cast<unsigned long long>(GetTickCount64()), stage,
        At<std::uint32_t>(projectile, kHumanIdOffset), ownerId, eventType,
        position.x, position.z, humanId, distance);
    if (length > 0 && length < static_cast<int>(sizeof(line))) {
        DWORD written = 0;
        WriteFile(file, line, static_cast<DWORD>(length), &written, nullptr);
    }
    CloseHandle(file);
}

void LoadVisualOptions() {
    wchar_t path[MAX_PATH]{};
    DWORD length = GetModuleFileNameW(g_selfModule, path, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        return;
    }
    while (length > 0 && path[length - 1] != L'\\' &&
           path[length - 1] != L'/') {
        --length;
    }
    constexpr wchar_t fileName[] = L"direct_action.ini";
    if (length == 0 || length + std::size(fileName) > MAX_PATH) {
        return;
    }
    std::copy(std::begin(fileName), std::end(fileName), path + length);
    std::copy(std::begin(path), std::end(path), std::begin(g_fireOptionsPath));
    const int zoomPercent = static_cast<int>(std::clamp(
        GetPrivateProfileIntW(L"Visual", L"ZoomPercent", 170, path),
        50U, 400U));
    const int crosshairPercent = static_cast<int>(std::clamp(
        GetPrivateProfileIntW(L"Visual", L"CrosshairPercent", 100, path),
        75U, 175U));
    g_remoteInputIntervalMs = std::clamp(
        GetPrivateProfileIntW(L"Coop", L"InputIntervalMs", 80, path),
        30U, 100U);
    g_friendlyFire = GetPrivateProfileIntW(
        L"Combat", L"FriendlyFire", 0, path) != 0;
    g_hostageBulletDamage = GetPrivateProfileIntW(
        L"Combat", L"HostageBulletDamage", 0, path) != 0;
    g_nativeWheelActions = GetPrivateProfileIntW(
        L"Interactions", L"NativeWheelActions", 0, path) != 0;
    const unsigned int stateTraceInterval = GetPrivateProfileIntW(
        L"Diagnostics", L"StateTraceIntervalMs", 0, path);
    g_stateTraceIntervalMs = stateTraceInterval == 0 ? 0 :
        std::clamp(stateTraceInterval, 100U, 1000U);
    g_zoomMultiplier = static_cast<float>(zoomPercent) * 0.01F;
    g_crosshairScale = static_cast<float>(crosshairPercent) * 0.01F;
    TraceTraversal("visual_options", zoomPercent, crosshairPercent, 0);
    TraceTraversal("coop_input_options", static_cast<int>(
        g_remoteInputIntervalMs), 200, 500);
    TraceTraversal("state_trace_options", static_cast<int>(
        g_stateTraceIntervalMs), 0, 0);
    TraceTraversal("combat_options", g_friendlyFire ? 1 : 0,
        g_hostageBulletDamage ? 1 : 0, 0);
    TraceTraversal("wheel_options", g_nativeWheelActions ? 1 : 0, 0, 0);
}
bool g_doorInteractKick = false;
std::uint32_t g_crosshairFeedbackColor = kCrosshairColor;
ULONGLONG g_crosshairFeedbackUntil = 0;
ULONGLONG g_lastAcceptedShotTick = 0;

void StopDirectSprint(void* serverHuman);
void StopNativeGrenadePreview();
bool InventoryItemIsFirearm(const void* item);
void ReleaseLowReady(void* firearm);
void RestoreCrowbarIdleBinding();
void RestoreLeanPlaybackForHuman(const void* clientHuman, int keepSide = 0);
void RestoreGrenadeHeldPose(const void* clientHuman);
void CancelSpyCameraAction(void* serverHuman);
void CancelDoorInteraction(void* serverHuman, DoorInteraction& state);

void ClearDirectControlState() {
    g_highReadyPoses.clear();
    g_laserPreferences.clear();
    RestoreGrenadeHeldPose(g_controlledClientHuman);
    CancelDoorInteraction(g_controlledServerHuman, g_doorInteraction);
    if (g_crouchHeld && g_controlledServerHuman && g_queueCrouch) {
        g_queueCrouch(g_controlledServerHuman, false);
    }
    if (g_previousReadyHold == kReadyHoldLow &&
        g_controlledServerHuman) {
        ReleaseLowReady(At<void*>(g_controlledServerHuman,
            kHumanServerEquippedItemOffset));
    }
    RestoreLeanPlaybackForHuman(g_controlledClientHuman);
    CancelSpyCameraAction(g_controlledServerHuman);
    RestoreCrowbarIdleBinding();
    StopNativeGrenadePreview();
    StopDirectSprint(g_controlledServerHuman);
    g_wasMoving = false;
    g_activeHumanId = 0;
    g_accumulatedMs = 0;
    g_lastDirection = {};
    g_directMovementDirection = {};
    g_leanSide = 0;
    g_leanReachedHold = false;
    g_leanPhysicalSide = 0;
    g_leanPressedAt = 0;
    g_leanWasMoving = false;
    g_lastLeanReadyPhase = -1;
    g_readyHold = kReadyHoldNone;
    g_previousReadyHold = kReadyHoldNone;
    g_controlledClientHuman = nullptr;
    g_controlledServerHuman = nullptr;
    g_reloadRequested = false;
    g_reloadSprintBlocked = false;
    g_cancelReloadRequested = false;
    g_reloadCanceledWeapon = nullptr;
    g_cursorWorldValid = false;
    g_cameraLookAhead = {};
    g_cameraFollowHumanId = 0;
    g_shootRequested = false;
    g_fireInputPressed = false;
    g_fireHeld = false;
    g_fireSequence = {};
    g_displayFireValid = false;
    g_autoFireActive = false;
    g_autoFireWeapon = nullptr;
    g_shootDirection = {};
    g_shootTarget = {};
    g_shootRequestRemainingMs = 0;
    g_shootEquipQueued = false;
    g_equipmentRequest = kNoEquipmentRequest;
    g_selectedGearInventoryType = -1;
    g_preItemFirearmInventoryType = -1;
    g_crowbarSelectedAt = 0;
    g_spyCameraSelectedAt = 0;
    g_spyCameraRequestedAt = 0;
    g_selectedGrenadeHotkey = 0;
    g_selectedSpyCameraHotkey = 0;
    g_selectedCrowbarHotkey = 0;
    g_grenadeAiming = false;
    g_grenadeThrowRequested = false;
    g_grenadeRequireRelease = true;
    g_grenadeAimChoice = {};
    g_grenadeTarget = {};
    g_grenadeThrowUsesNativePreview = false;
    g_grenadeHoldMs = 0;
    g_grenadeHoldStartTick = 0;
    g_grenadePendingSlot = -1;
    g_grenadePendingItem = nullptr;
    g_grenadePendingQuantity = 0;
    g_grenadePendingMs = 0;
    g_crouchHeld = false;
    g_crouchCommandPending = false;
    g_sprintDesired = false;
    g_doorInteractTargetId = 0;
    g_lastInteractedDoorId = 0;
    g_traversalUsedThisPress = false;
    g_traversalFWasDown = false;
    g_traversalRepathRequested = false;
    g_traversalPathHoldUntil = 0;
    g_nativeWheelActionOwnerId = 0;
    g_nativeWheelActionType = 0;
    g_nativeWheelActionRequestedAt = 0;
    g_nativeWheelActionLastSeenAt = 0;
    g_nativeWheelActionObserved = false;
    g_nativeWheelActionCancelArmed = false;
    g_menuSuspendedPath = false;
    g_coopProbeSentHumanId = 0;
    g_lastRemoteInputSendTick = 0;
    g_remoteInputSendCount = 0;
    g_lastRemoteMovementSent = {};
    g_lastRemoteAimSent = {};
    g_lastRemoteFireSent = false;
    g_lastRemoteReadySent = kReadyHoldNone;
    g_lastRemoteSprintSent = false;
    g_lastRemoteCrouchSent = false;
    g_lastRemoteInteractSent = false;
    g_doorInteractKick = false;
    g_crosshairFeedbackColor = kCrosshairColor;
    g_crosshairFeedbackUntil = 0;
    g_lastAcceptedShotTick = 0;
}

template <typename T>
T& At(void* object, std::size_t offset) {
    return *reinterpret_cast<T*>(static_cast<std::byte*>(object) + offset);
}

template <typename T>
const T& At(const void* object, std::size_t offset) {
    return *reinterpret_cast<const T*>(static_cast<const std::byte*>(object) + offset);
}

bool RemoteInputIsAuthorized(const RemoteInput& input) {
    return input.human && input.humanId != 0 &&
        GetServerHuman(input.humanId) == input.human &&
        At<std::uint32_t>(input.human, kHumanIdOffset) == input.humanId &&
        input.senderMask != 0 &&
        (At<std::uint32_t>(input.human, 0x13C) & input.senderMask) != 0 &&
        At<float>(input.human, kHumanHealthOffset) > 0.0F &&
        At<bool>(input.human, 0x1520);
}

bool HasQueuedSprint(const void* serverHuman) {
    if (!serverHuman) {
        return false;
    }
    const int writeIndex = At<int>(
        serverHuman, kHumanServerCommandWriteOffset);
    const int readIndex = At<int>(
        serverHuman, kHumanServerCommandReadOffset);
    const int count = writeIndex - readIndex;
    for (int index = 0; index < count; ++index) {
        const int slot = (readIndex + index) & 0x1F;
        const auto commandOffset = kHumanServerCommandArrayOffset +
            static_cast<std::size_t>(slot) * kHumanServerCommandSize;
        if (At<int>(serverHuman, commandOffset) == kSprintCommandType) {
            return true;
        }
    }
    return false;
}

bool HasQueuedReload(const void* serverHuman) {
    if (!serverHuman) {
        return false;
    }
    const int writeIndex = At<int>(
        serverHuman, kHumanServerCommandWriteOffset);
    const int readIndex = At<int>(
        serverHuman, kHumanServerCommandReadOffset);
    const int count = std::clamp(writeIndex - readIndex, 0, 32);
    for (int index = 0; index < count; ++index) {
        const int slot = (readIndex + index) & 0x1F;
        const auto commandOffset = kHumanServerCommandArrayOffset +
            static_cast<std::size_t>(slot) * kHumanServerCommandSize;
        if (At<int>(serverHuman, commandOffset) == kReloadCommandType) {
            return true;
        }
    }
    return false;
}

void RemoveQueuedReload(void* serverHuman) {
    if (!serverHuman || !g_removeHumanCommand) {
        return;
    }
    int index = 0;
    while (index < At<int>(serverHuman, kHumanServerCommandWriteOffset) -
                       At<int>(serverHuman, kHumanServerCommandReadOffset)) {
        const int readIndex = At<int>(
            serverHuman, kHumanServerCommandReadOffset);
        const int slot = (readIndex + index) & 0x1F;
        const auto commandOffset = kHumanServerCommandArrayOffset +
            static_cast<std::size_t>(slot) * kHumanServerCommandSize;
        if (At<int>(serverHuman, commandOffset) == kReloadCommandType) {
            g_removeHumanCommand(
                static_cast<std::byte*>(serverHuman) +
                    kHumanServerCommandQueueOffset,
                index);
            continue;
        }
        ++index;
    }
}

bool FirearmIsReloading(const void* serverHuman) {
    if (!serverHuman) {
        return false;
    }
    const void* item = At<void*>(
        serverHuman, kHumanServerEquippedItemOffset);
    return InventoryItemIsFirearm(item) &&
        At<int>(item, 0x2C) == kFirearmReloadingState;
}

void StopDirectSprint(void* serverHuman) {
    if (!serverHuman || !g_removeHumanCommand) {
        return;
    }

    // This is the exact queue walk performed by ActionSprintEnd. Remove by
    // relative queue index because StaticQueue::Remove closes the gap.
    int index = 0;
    while (index < At<int>(serverHuman, kHumanServerCommandWriteOffset) -
                       At<int>(serverHuman, kHumanServerCommandReadOffset)) {
        const int readIndex = At<int>(
            serverHuman, kHumanServerCommandReadOffset);
        const int slot = (readIndex + index) & 0x1F;
        const auto commandOffset = kHumanServerCommandArrayOffset +
            static_cast<std::size_t>(slot) * kHumanServerCommandSize;
        if (At<int>(serverHuman, commandOffset) == kSprintCommandType) {
            g_removeHumanCommand(
                static_cast<std::byte*>(serverHuman) +
                    kHumanServerCommandQueueOffset,
                index);
            continue;
        }
        ++index;
    }

    auto& movementFlags = At<std::uint32_t>(
        serverHuman, kHumanServerMovementFlagsOffset);
    if ((movementFlags & 0x40U) != 0) {
        movementFlags &= ~0x40U;
        At<std::uint64_t>(serverHuman, kHumanServerMovementStateOffset) = 0;
    }
}

bool HasQueuedSpyCamera(const void* serverHuman) {
    if (!serverHuman) {
        return false;
    }
    const int queued = At<int>(serverHuman, kHumanServerCommandWriteOffset) -
        At<int>(serverHuman, kHumanServerCommandReadOffset);
    if (queued < 0 || queued > 32) {
        return false;
    }
    for (int index = 0; index < queued; ++index) {
        const int readIndex = At<int>(
            serverHuman, kHumanServerCommandReadOffset);
        const int slot = (readIndex + index) & 0x1F;
        if (At<int>(serverHuman, kHumanServerCommandArrayOffset +
                static_cast<std::size_t>(slot) * kHumanServerCommandSize) == 15) {
            return true;
        }
    }
    return false;
}

void CancelSpyCameraAction(void* serverHuman) {
    if (!g_spyCameraActionActive) {
        return;
    }
    g_spyCameraActionActive = false;
    g_spyCameraRequestedAt = 0;
    if (!serverHuman || !g_removeHumanCommand) {
        return;
    }
    int index = 0;
    while (index < At<int>(serverHuman, kHumanServerCommandWriteOffset) -
                       At<int>(serverHuman, kHumanServerCommandReadOffset)) {
        const int readIndex = At<int>(
            serverHuman, kHumanServerCommandReadOffset);
        const int slot = (readIndex + index) & 0x1F;
        const auto commandOffset = kHumanServerCommandArrayOffset +
            static_cast<std::size_t>(slot) * kHumanServerCommandSize;
        if (At<int>(serverHuman, commandOffset) == 15) {
            g_removeHumanCommand(
                static_cast<std::byte*>(serverHuman) +
                    kHumanServerCommandQueueOffset, index);
            TraceTraversal("spycam_cancel", index, 15, 0);
            continue;
        }
        ++index;
    }
}

bool KeyDown(int virtualKey) {
    return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

bool ForegroundIsGame() {
    DWORD foregroundPid = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &foregroundPid);
    return foregroundPid == GetCurrentProcessId();
}

bool IsGameMenuOpen() {
    void* gameGui = *reinterpret_cast<void**>(
        g_moduleBase + kRvaGameGuiGlobal);
    return gameGui && g_isAnyMenuOpened && g_isAnyMenuOpened(gameGui);
}

bool ChatBoxHasInputFocus(const void* gameGui, const void* guiManager) {
    const void* chat = gameGui ? At<void*>(gameGui, 0xF0) : nullptr;
    if (!chat || !guiManager) return false;
    // Native IsAnyMenuOpened explicitly excludes chat when it is the top
    // input receiver. A visible chat widget alone does not mean typing focus.
    const int count = At<int>(guiManager, 0x78);
    const void* receivers = At<void*>(guiManager, 0x70);
    return receivers && count > 0 && count <= 1024 &&
        At<void*>(receivers, static_cast<std::size_t>(count - 1) * sizeof(void*)) == chat;
}

bool IsChatBoxOpen() {
    return ChatBoxHasInputFocus(
        At<void*>(reinterpret_cast<void*>(g_moduleBase + kRvaGameGuiGlobal), 0),
        At<void*>(reinterpret_cast<void*>(g_moduleBase + 0x00FE7180), 0));
}

bool DirectControlCapturesMouse() {
    return g_enabled && ForegroundIsGame() && g_controlledClientHuman &&
        !IsGameMenuOpen();
}

bool DirectControlOwnsGameplayMouse() {
    return g_enabled && ForegroundIsGame() && !IsGameMenuOpen();
}

bool InventoryItemIsFirearm(const void* item) {
    if (!item) {
        return false;
    }
    const void* itemTemplate = At<void*>(item, 0x20);
    return itemTemplate && At<int>(itemTemplate, 0x20) == 0;
}

void ReleaseLowReady(void* firearm) {
    if (!InventoryItemIsFirearm(firearm) || !g_firearmReadyAim) {
        return;
    }
    const int state = At<int>(firearm, 0x2C);
    if (state == 3 || state == 6) {
        g_firearmReadyAim(firearm);
        TraceTraversal("ready_release", state, 0, 0);
    }
}

void ApplyReadyHold(void* firearm, int hold) {
    if (!InventoryItemIsFirearm(firearm) || !g_firearmGuard ||
        !g_firearmReadyAim) {
        return;
    }
    const int state = At<int>(firearm, 0x2C);
    if (hold == kReadyHoldLow &&
        (state == 5 || state == 11)) {
        g_firearmGuard(firearm, false);
        TraceTraversal("ready_low", state, 0, 0);
    } else if (hold == kReadyHoldHigh &&
               (state == 3 || state == 6)) {
        g_firearmReadyAim(firearm);
        TraceTraversal("ready_high", state, 0, 0);
    }
}

std::uint32_t FireWeaponId(const void* itemTemplate) {
    const char* name = itemTemplate ? At<const char*>(itemTemplate, 0x28) : nullptr;
    std::uint32_t hash = 2166136261U;
    if (!name) return 0;
    while (*name) { hash = (hash ^ static_cast<unsigned char>(*name++)) * 16777619U; }
    return hash;
}

FireSelection FireCapabilities(const void* itemTemplate) {
    if (!itemTemplate || At<int>(itemTemplate, 0x20) != 0) return {};
    auto found = g_fireCapabilities.find(itemTemplate);
    if (found != g_fireCapabilities.end()) return found->second;
    FireSelection result;
    const char* operation = At<const char*>(itemTemplate, 0x110);
    const auto is = [operation](const char* label) { return operation && std::strcmp(operation, label) == 0; };
    if (is("@firearm_operation_semifull_name")) { result.mask = 3; result.mode = FireMode::Auto; }
    else if (is("@firearm_operation_fullauto_name")) { result.mask = 2; result.mode = FireMode::Auto; }
    else if (is("@firearm_operation_semi3rdburst_name")) result.mask = 5;
    else if (is("@firearm_operation_mp5navy_name")) { result.mask = 7; result.mode = FireMode::Auto; }
    else if (is("@firearm_operation_fullauto2burst_name") || is("@firearm_operation_fullauto3burst_name")) {
        result.mask = 6; result.mode = FireMode::Auto;
        result.burst = is("@firearm_operation_fullauto2burst_name") ? 2 : 3;
    }
    else if (!is("@firearm_operation_semiauto_name") && !is("@firearm_operation_pumpaction_name") &&
             !is("@firearm_operation_boltaction_name") && !is("@firearm_operation_breakopen_name") &&
             !is("@firearm_operation_singleshot_name") && !is("@firearm_operation_singleshotthrow_name")) {
        TraceTraversal("fire_unknown_label", static_cast<int>(FireWeaponId(itemTemplate)), 0, 0);
    }
    const char* name = At<const char*>(itemTemplate, 0x28);
    wchar_t section[256]{};
    wchar_t wideName[220]{};
    if (name && MultiByteToWideChar(CP_UTF8, 0, name, -1, wideName, 220)) {
        swprintf_s(section, L"FireMode:%ls", wideName);
        const unsigned int mask = GetPrivateProfileIntW(section, L"Modes", result.mask, g_fireOptionsPath);
        if (mask >= 1 && mask <= 7) result.mask = static_cast<int>(mask);
        result.burst = static_cast<int>(std::clamp(GetPrivateProfileIntW(section, L"BurstRounds", result.burst, g_fireOptionsPath), 2U, 10U));
        const unsigned int mode = GetPrivateProfileIntW(section, L"Default", static_cast<int>(result.mode), g_fireOptionsPath);
        if (mode <= 2 && (result.mask & (1 << mode))) result.mode = static_cast<FireMode>(mode);
    }
    if (!(result.mask & (1 << static_cast<int>(result.mode)))) {
        for (int i = 0; i < 3; ++i) if (result.mask & (1 << i)) { result.mode = static_cast<FireMode>(i); break; }
    }
    TraceTraversal("fire_capabilities", static_cast<int>(FireWeaponId(itemTemplate)), result.mask, result.burst);
    g_fireCapabilities[itemTemplate] = result;
    return result;
}

FireMode& SelectedFireMode(std::uint32_t humanId, const void* itemTemplate) {
    auto& selections = g_fireSelections[humanId];
    return selections.try_emplace(itemTemplate, FireCapabilities(itemTemplate).mode).first->second;
}

void StartFireSequence(FireSequence& sequence, FireMode mode, int burst, std::uint32_t weaponId = 0) {
    sequence.mode = mode;
    sequence.remaining = mode == FireMode::Burst ? burst : 1;
    sequence.deadline = GetTickCount64() + 10000;
    sequence.requireRelease = true;
    sequence.weaponId = weaponId;
}

bool ContinueFireSequence(FireSequence& sequence, bool held) {
    sequence.deadline = GetTickCount64() + 10000;
    if (sequence.mode == FireMode::Auto) return held;
    if (sequence.mode == FireMode::Burst) return --sequence.remaining > 0;
    return false;
}

const void* ClientFireTemplate(const void* human) {
    if (!human) return nullptr;
    const int slot = At<int>(human, 0xD80);
    if (slot < 0 || slot >= kInventorySlotCount) return nullptr;
    const void* itemTemplate = At<void*>(human, 0x430 + static_cast<std::size_t>(slot) * 0x58);
    return itemTemplate && At<int>(itemTemplate, 0x20) == 0 ? itemTemplate : nullptr;
}

bool ToggleNvgFilter(void* renderer) {
    if (!renderer) return false;
    // Exact operation performed by GameGUI::OnEvent case 0x5D. This is
    // local display state and does not change inventory or server vision.
    At<bool>(renderer, kRendererNvgOffset) = !At<bool>(renderer, kRendererNvgOffset);
    return true;
}

std::uint64_t LaserPreferenceKey(const void* human) {
    return (static_cast<std::uint64_t>(At<std::uint32_t>(human, kHumanIdOffset)) << 32) |
        static_cast<std::uint32_t>(At<int>(human, 0xD80));
}

bool ToggleWeaponLaser(const void* human) {
    const void* itemTemplate = ClientFireTemplate(human);
    // UpdateLaserLight reads Firearm_Template::hasLaser at +0x196.
    if (!itemTemplate || !At<bool>(itemTemplate, 0x196)) return false;
    auto& preference = g_laserPreferences[LaserPreferenceKey(human)];
    if (preference.owner != human || preference.itemTemplate != itemTemplate) {
        preference = {human, itemTemplate, true};
    }
    preference.enabled = !preference.enabled;
    TraceTraversal("laser_toggle", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)),
        At<int>(human, 0xD80), preference.enabled ? 1 : 0);
    return true;
}

void ApplyLaserPreference(void* human) {
    if (!g_enabled || !human || At<float>(human, kHumanHealthOffset) <= 0.0F) return;
    const auto found = g_laserPreferences.find(LaserPreferenceKey(human));
    if (found == g_laserPreferences.end()) return;
    const auto& preference = found->second;
    if (preference.owner != human || preference.itemTemplate != ClientFireTemplate(human) ||
        preference.enabled) return;
    // Run native update first, preserving allocation, death cleanup, transform,
    // day/night, filter, and visibility rules. Suppress only its rendered light
    // and four beam vertices; never mutate shared weapon capability metadata.
    void* light = At<void*>(human, 0xF28);
    if (light) {
        if (g_entitySetAlpha) g_entitySetAlpha(light, 0);
        if (!At<bool>(light, 0x202)) {
            At<bool>(light, 0x202) = true;
            At<bool>(light, 0x200) = true;
        }
    }
    void* ray = At<void*>(human, 0xF30);
    if (ray) {
        for (std::size_t offset : {0x70U, 0x88U, 0xA0U, 0xB8U}) {
            At<std::uint32_t>(ray, offset) = 0;
        }
    }
}

void __fastcall HookedUpdateLaserLight(void* human) {
    g_originalUpdateLaserLight(human);
    ApplyLaserPreference(human);
}

bool InventoryItemIsGrenade(const void* item) {
    if (!item) {
        return false;
    }
    const void* itemTemplate = At<void*>(item, 0x20);
    // EquipmentType::GRENADE is 3. CmdQueueGrenadeThrow performs this exact
    // template check before it queues anything.
    return itemTemplate && At<int>(itemTemplate, 0x20) == 3 &&
        At<int>(item, 0x3C) > 0;
}

int FindFirearmInventoryType(void* serverHuman) {
    void* equippedItem = At<void*>(
        serverHuman, kHumanServerEquippedItemOffset);
    if (InventoryItemIsFirearm(equippedItem)) {
        for (int index = 1; index < kInventorySlotCount; ++index) {
            const auto offset = kHumanServerInventoryOffset +
                static_cast<std::size_t>(index) * sizeof(void*);
            if (At<void*>(serverHuman, offset) == equippedItem) {
                return index;
            }
        }
    }

    for (const int index : {
             kPrimaryWeaponInventoryType,
             kSecondaryWeaponInventoryType}) {
        const auto offset = kHumanServerInventoryOffset +
            static_cast<std::size_t>(index) * sizeof(void*);
        if (InventoryItemIsFirearm(At<void*>(serverHuman, offset))) {
            return index;
        }
    }
    return -1;
}

void QueueEquipInventoryType(void* serverHuman, int inventoryType) {
    if (!serverHuman || inventoryType < 0 ||
        inventoryType >= kInventorySlotCount) {
        return;
    }
    void* requestedItem = At<void*>(serverHuman,
        kHumanServerInventoryOffset +
            static_cast<std::size_t>(inventoryType) * sizeof(void*));
    if (!requestedItem || requestedItem == At<void*>(
            serverHuman, kHumanServerEquippedItemOffset)) {
        return;
    }
    void* command = g_createQueueCommand(serverHuman, 0,
        reinterpret_cast<const void*>(
            g_moduleBase + kRvaProcessCmdItemEquip), nullptr);
    if (command) {
        At<int>(command, 0x58) = inventoryType;
    }
}

std::uint32_t AnimationNameHash(const char* name) {
    std::uint32_t hash = 0x811C9DC5U;
    for (const auto* p = reinterpret_cast<const unsigned char*>(name);
         *p; ++p) {
        hash = (hash ^ *p) * 0x01000193U;
    }
    return hash;
}

void* FindAnimationSet(const char* name) {
    if (!g_moduleBase) {
        return nullptr;
    }
    void* list = *reinterpret_cast<void**>(g_moduleBase + 0x01355DC0);
    const int count = *reinterpret_cast<int*>(g_moduleBase + 0x01355DC8);
    if (!list || count < 1 || count > 512) {
        return nullptr;
    }
    const std::uint32_t hash = AnimationNameHash(name);
    for (int i = 0; i < count; ++i) {
        auto* set = static_cast<std::byte*>(list) + i * 0x30;
        if (At<std::uint32_t>(set, 0x08) == hash) {
            return set;
        }
    }
    return nullptr;
}

void* FindAnimationBinding(void* set, const char* handle) {
    if (!set) {
        return nullptr;
    }
    void* bindings = At<void*>(set, 0x20);
    const int count = At<int>(set, 0x28);
    if (!bindings || count < 1 || count > 256) {
        return nullptr;
    }
    const std::uint32_t hash = AnimationNameHash(handle);
    for (int i = 0; i < count; ++i) {
        auto* binding = static_cast<std::byte*>(bindings) + i * 0x30;
        if (At<std::uint32_t>(binding, 0x20) == hash) {
            return binding;
        }
    }
    return nullptr;
}

void RestoreCrowbarIdleBinding() {
    void* set = FindAnimationSet("crowbar");
    for (int i = 0; i < g_crowbarAnimationPatchCount; ++i) {
        const auto& patch = g_crowbarAnimationPatches[i];
        void* current = FindAnimationBinding(set, patch.handle);
        if (current && current == patch.binding) {
            At<void*>(current, 0x28) = patch.originalAnimation;
        }
        g_crowbarAnimationPatches[i] = {};
    }
    g_crowbarAnimationPatchCount = 0;
}

void InstallCrowbarIdleBinding() {
    if (g_crowbarAnimationPatchCount > 0) {
        return;
    }
    void* crowbarSet = FindAnimationSet("crowbar");
    void* hammerSet = FindAnimationSet("dynamic_hammer");
    if (!crowbarSet || !hammerSet) {
        TraceTraversal("crowbar_pose_missing", crowbarSet ? 1 : 0,
            hammerSet ? 1 : 0, 0);
        return;
    }
    // The stock Crowbar set inherits rifle locomotion. The hammer set uses
    // the game's tool-friendly idle and default unarmed locomotion. Retain
    // Crowbar's own change_in/out and operate_equipment bindings.
    constexpr std::array<const char*, 14> handles{
        "idle", "idle_low", "walk", "walk_low", "walk_b",
        "strafe_l", "strafe_r", "sprint", "idle_crouch",
        "walk_crouch", "walk_crouch_b", "crouch_down",
        "crouch_up", "idle_blind"};
    for (const char* handle : handles) {
        void* crowbar = FindAnimationBinding(crowbarSet, handle);
        void* hammer = FindAnimationBinding(hammerSet, handle);
        if (!crowbar || !hammer || !At<void*>(hammer, 0x28)) {
            continue;
        }
        auto& patch = g_crowbarAnimationPatches[g_crowbarAnimationPatchCount++];
        patch = {handle, crowbar, At<void*>(crowbar, 0x28)};
        At<void*>(crowbar, 0x28) = At<void*>(hammer, 0x28);
    }
    TraceTraversal("crowbar_pose_installed", g_crowbarAnimationPatchCount,
        static_cast<int>(handles.size()), 0);
}

void RestoreCrowbarBindingAfterSwitch() {
    // Keep the outgoing operator's locomotion bindings intact until the
    // client has actually equipped the replacement item.
    if (g_controlledClientHuman && At<int>(g_controlledClientHuman, 0xD80) == 16) return;
    RestoreCrowbarIdleBinding();
}

void SelectUtilityInventoryType(void* serverHuman, int inventoryType) {
    // The outgoing grenade remains equipped during the queued transition.
    // Its pose is released by actual state/command changes, not this keypress.
    CancelSpyCameraAction(serverHuman);
    if (g_selectedGearInventoryType == inventoryType) {
        g_selectedGearInventoryType = -1;
        g_crowbarSelectedAt = 0;
        g_spyCameraSelectedAt = 0;
        RestoreCrowbarBindingAfterSwitch();
        QueueEquipInventoryType(serverHuman,
            g_preItemFirearmInventoryType >= 0
                ? g_preItemFirearmInventoryType
                : FindFirearmInventoryType(serverHuman));
        g_preItemFirearmInventoryType = -1;
        return;
    }
    if (g_selectedGearInventoryType < 0) {
        g_preItemFirearmInventoryType = FindFirearmInventoryType(serverHuman);
    }
    g_selectedGearInventoryType = inventoryType;
    g_crowbarSelectedAt = inventoryType == 16 ? GetTickCount64() : 0;
    const void* selectedItem = At<void*>(serverHuman,
        kHumanServerInventoryOffset +
            static_cast<std::size_t>(inventoryType) * sizeof(void*));
    const void* selectedTemplate = selectedItem
        ? At<void*>(selectedItem, 0x20) : nullptr;
    const char* selectedName = selectedTemplate
        ? At<const char*>(selectedTemplate, 0x28) : nullptr;
    g_spyCameraSelectedAt = selectedName &&
        std::strcmp(selectedName, "Spy Camera") == 0
            ? GetTickCount64() : 0;
    if (inventoryType == 16) {
        InstallCrowbarIdleBinding();
    } else {
        RestoreCrowbarBindingAfterSwitch();
    }
    QueueEquipInventoryType(serverHuman, inventoryType);
}

int ResolveEquipmentNumber(void* serverHuman, int number) {
    if (number == 1) {
        return At<void*>(
            serverHuman,
            kHumanServerInventoryOffset +
                kPrimaryWeaponInventoryType * sizeof(void*))
            ? kPrimaryWeaponInventoryType
            : -1;
    }
    if (number == 2) {
        return At<void*>(
            serverHuman,
            kHumanServerInventoryOffset +
                kSecondaryWeaponInventoryType * sizeof(void*))
            ? kSecondaryWeaponInventoryType
            : -1;
    }
    if (number < 3 || number > 10) {
        return -1;
    }

    const int slot = number == 3 ? 16 : number >= 4 && number <= 6 ? number + 8 : -1;
    if (slot < 0) return -1;
    const void* item = At<void*>(serverHuman,
        kHumanServerInventoryOffset + static_cast<std::size_t>(slot) * sizeof(void*));
    const void* itemTemplate = item ? At<void*>(item, 0x20) : nullptr;
    if (!itemTemplate || At<int>(itemTemplate, 0x20) == 19) return -1;
    return number == 3 && At<int>(itemTemplate, 0x20) != 8 ? -1 : slot;
}

int ResolveClientEquipmentNumber(const void* clientHuman, int number) {
    if (!clientHuman) return -1;
    const int slot = number == 1 ? kPrimaryWeaponInventoryType :
        number == 2 ? kSecondaryWeaponInventoryType :
        number == 3 ? 16 : number >= 4 && number <= 6 ? number + 8 : -1;
    if (slot < 0) return -1;
    const void* itemTemplate = At<void*>(clientHuman,
        0x430 + static_cast<std::size_t>(slot) * 0x58);
    if (!itemTemplate || At<int>(itemTemplate, 0x20) == 19) return -1;
    return number == 3 && At<int>(itemTemplate, 0x20) != 8 ? -1 : slot;
}

GrenadeChoice GrenadeChoiceFor(const void* human) {
    if (!human) return {};
    const auto found = g_grenadeChoices.find(At<std::uint32_t>(human, kHumanIdOffset));
    return found == g_grenadeChoices.end() ? GrenadeChoice{} : found->second;
}

int ResolveGrenadeChoice(const void* serverHuman, GrenadeChoice choice) {
    if (!serverHuman || choice.slot < 1 || choice.slot >= kInventorySlotCount || choice.itemId == 0) return -1;
    const void* item = At<void*>(serverHuman,
        kHumanServerInventoryOffset + static_cast<std::size_t>(choice.slot) * sizeof(void*));
    if (!InventoryItemIsGrenade(item) || At<int>(item, 0x3C) <= 0) return -1;
    return FireWeaponId(At<void*>(item, 0x20)) == choice.itemId ? choice.slot : -1;
}

int FindGrenadeInventoryType(void* serverHuman) {
    return ResolveGrenadeChoice(serverHuman, GrenadeChoiceFor(serverHuman));
}

const void* ClientGrenadeChoiceTemplate(const void* human, GrenadeChoice choice) {
    if (!human || choice.slot < 1 || choice.slot >= kInventorySlotCount || choice.itemId == 0) return nullptr;
    const void* itemTemplate = At<void*>(human, 0x430 + static_cast<std::size_t>(choice.slot) * 0x58);
    return itemTemplate && At<int>(itemTemplate, 0x20) == 3 && FireWeaponId(itemTemplate) == choice.itemId
        ? itemTemplate : nullptr;
}

int FindClientGrenadeInventoryType(const void* human) {
    const auto choice = GrenadeChoiceFor(human);
    if (!ClientGrenadeChoiceTemplate(human, choice)) return -1;
    // UpdateHumanProfile uses the same client quantity field for non-firearms.
    return At<int>(human, 0x43C + static_cast<std::size_t>(choice.slot) * 0x58) > 0 ? choice.slot : -1;
}

// The native queue performs draw -> throw -> restore. When throwing an
// explicitly held grenade, restore the weapon remembered before selecting it.
bool QueueRememberedGrenadeThrow(void* human, GrenadeChoice choice, int returnSlot,
                                const Vector3& origin, float angle, const Vector3& target) {
    const int slot = ResolveGrenadeChoice(human, choice);
    if (slot < 0 || !g_queueGrenadeThrow) return false;
    const int before = At<int>(human, kHumanServerCommandWriteOffset);
    const int read = At<int>(human, kHumanServerCommandReadOffset);
    if (before - read < 0 || before - read > 27) return false;
    for (int i = read; i < before; ++i) {
        if (At<int>(human, kHumanServerCommandArrayOffset +
            static_cast<std::size_t>(i & 31) * kHumanServerCommandSize) == 2) return false;
    }
    g_queueGrenadeThrow(human, slot, &origin, angle, &target, nullptr);
    const int after = At<int>(human, kHumanServerCommandWriteOffset);
    if (after <= before) return false;
    if (returnSlot > 0 && returnSlot < kInventorySlotCount &&
        InventoryItemIsFirearm(At<void*>(human, kHumanServerInventoryOffset +
            static_cast<std::size_t>(returnSlot) * sizeof(void*))) && after - before == 5) {
        void* restore = static_cast<std::byte*>(human) + kHumanServerCommandArrayOffset +
            static_cast<std::size_t>((after - 2) & 31) * kHumanServerCommandSize;
        if (At<int>(restore, 0) == 0) At<int>(restore, 0x58) = returnSlot;
    }
    return true;
}
bool LinkNativeGrenadePreviewWaypoint(void* waypoint) {
    // UpdatePotentialActionWaypoint removes s_potential from this native
    // linked list at the start of an input update. RenderPaths iterates the
    // list to find grenade trajectory render objects, so reinsert it after
    // the direct-control preview has updated its action.
    auto* link = static_cast<std::byte*>(waypoint) + 0x1D0;
    void* next = At<void*>(link, 0x08);
    void* previous = At<void*>(link, 0x10);
    if (!next || !previous) {
        return false;
    }
    if (next != link || previous != link) {
        return At<void*>(next, 0x10) == link &&
            At<void*>(previous, 0x08) == link;
    }
    void* head = *reinterpret_cast<void**>(
        g_moduleBase + kRvaActionWPClientTypeList);
    if (!head) {
        return false;
    }
    void* tail = At<void*>(head, 0x10);
    if (!tail || At<void*>(tail, 0x08) != head) {
        return false;
    }
    At<void*>(link, 0x08) = head;
    At<void*>(link, 0x10) = tail;
    At<void*>(head, 0x10) = link;
    At<void*>(tail, 0x08) = link;
    At<void*>(link, 0x00) = head;
    return true;
}

void UnlinkNativeGrenadePreviewWaypoint(void* waypoint) {
    auto* link = static_cast<std::byte*>(waypoint) + 0x1D0;
    void* next = At<void*>(link, 0x08);
    void* previous = At<void*>(link, 0x10);
    if (!next || !previous ||
        At<void*>(next, 0x10) != link ||
        At<void*>(previous, 0x08) != link) {
        return;
    }
    At<void*>(next, 0x10) = previous;
    At<void*>(previous, 0x08) = next;
    At<void*>(link, 0x08) = link;
    At<void*>(link, 0x10) = link;
    At<void*>(link, 0x00) = link;
}

void StopNativeGrenadePreview() {
    if (!g_nativeGrenadePreviewWaypoint) {
        return;
    }
    void* potential = *reinterpret_cast<void**>(
        g_moduleBase + kRvaPotentialActionWaypointGlobal);
    if (potential == g_nativeGrenadePreviewWaypoint) {
        UnlinkNativeGrenadePreviewWaypoint(potential);
        using RemoveVisualsFn = void(__fastcall*)(void*);
        const auto removeVisuals = reinterpret_cast<RemoveVisualsFn>(
            g_moduleBase + kRvaActionWPRemoveVisuals);
        removeVisuals(potential);
        std::memcpy(
            static_cast<std::byte*>(potential) + 0x1F0,
            g_nativePreviewSavedAction.data(),
            g_nativePreviewSavedAction.size());
        At<Vector3>(potential, kHumanOriginOffset) =
            g_nativePreviewSavedOrigin;
    }
    g_nativeGrenadePreviewWaypoint = nullptr;
}

void CancelPreparedGrenade(bool inputHeld) {
    StopNativeGrenadePreview();
    g_grenadeThrowRequested = false;
    g_grenadeThrowUsesNativePreview = false;
    g_grenadeRequireRelease = inputHeld;
    g_grenadeHoldStartTick = 0;
    g_grenadeHoldMs = 0;
    g_grenadeAiming = false;
}

bool GrenadeGestureCanRelease(bool wasDown) {
    return wasDown && g_grenadeHoldStartTick != 0 && !g_grenadeRequireRelease;
}

bool UpdateNativeGrenadePreview(
    void* gameInput,
    void* clientHuman,
    int inventoryType,
    std::uint32_t humanId) {
    auto tracePreview = [&](int reason, const void* waypoint) {
        static ULONGLONG last = 0;
        const ULONGLONG now = GetTickCount64();
        if (now - last < 500) return;
        char line[400]{};
        std::snprintf(line, sizeof(line), "%llu PREVIEW reason=%d owner=%u slot=%d potential=%p action=%d target=%p field264=%u tracked=%p\r\n",
            static_cast<unsigned long long>(now), reason, humanId, inventoryType, waypoint,
            waypoint ? At<int>(waypoint, 0x1F0) : -1, waypoint ? At<void*>(waypoint, 0x248) : nullptr,
            waypoint ? At<unsigned>(waypoint, 0x264) : 0, g_nativeGrenadePreviewWaypoint);
        TraceItemDetail(line); last = now;
    };
    void* potential = *reinterpret_cast<void**>(
        g_moduleBase + kRvaPotentialActionWaypointGlobal);
    void* gameClient = *reinterpret_cast<void**>(
        g_moduleBase + kRvaGameClientGlobal);
    const bool clientMatchesOwner = gameClient &&
        humanId < kMaximumEntityId &&
        At<void*>(
            gameClient,
            kGameEntityArrayOffset +
                static_cast<std::size_t>(humanId) * sizeof(void*)) ==
            clientHuman;
    void* clientGrenadeTemplate = inventoryType >= 0 &&
        inventoryType < kInventorySlotCount && clientHuman
        ? At<void*>(
              clientHuman,
              0x430 + static_cast<std::size_t>(inventoryType) * 0x58)
        : nullptr;
    if (inventoryType < 0 || !potential || !g_cursorWorldValid ||
        !clientMatchesOwner || !clientGrenadeTemplate ||
        At<int>(clientGrenadeTemplate, 0x20) != 3) {
        tracePreview(clientMatchesOwner ? 1 : 2, potential);
        StopNativeGrenadePreview();
        return false;
    }
    if (g_nativeGrenadePreviewWaypoint &&
        g_nativeGrenadePreviewWaypoint != potential) {
        StopNativeGrenadePreview();
    }
    if (!g_nativeGrenadePreviewWaypoint && At<int>(potential, 0x1F0) == 23 &&
        At<std::uint32_t>(potential, 0x220) == humanId && At<void*>(potential, 0x248)) {
        // Stock input may have already created this operator's grenade
        // target. Reuse it instead of rejecting our own aiming action as busy.
        std::memcpy(g_nativePreviewSavedAction.data(), static_cast<std::byte*>(potential) + 0x1F0,
            g_nativePreviewSavedAction.size());
        const std::uint64_t noAction = 0;
        std::memcpy(g_nativePreviewSavedAction.data(), &noAction, sizeof(noAction));
        g_nativePreviewSavedOrigin = At<Vector3>(potential, kHumanOriginOffset);
        g_nativeGrenadePreviewWaypoint = potential;
        At<bool>(gameInput, 0x1B8) = false;
        tracePreview(7, potential);
    }
    if (!g_nativeGrenadePreviewWaypoint) {
        // s_potential is a client-only action waypoint. OnChangeAction does
        // not send its action to the server; UpdateVisuals creates DK2's
        // actual grenade target, trajectory object, and radius highlights.
        if (At<int>(potential, 0x1F0) != 0 ||
            At<void*>(potential, 0x248) != nullptr ||
            At<std::uint32_t>(potential, 0x264) != 0) {
            tracePreview(3, potential);
            return false;
        }
        std::memcpy(
            g_nativePreviewSavedAction.data(),
            static_cast<std::byte*>(potential) + 0x1F0,
            g_nativePreviewSavedAction.size());
        g_nativePreviewSavedOrigin = At<Vector3>(
            potential, kHumanOriginOffset);
        At<Vector3>(potential, kHumanOriginOffset) = At<Vector3>(
            clientHuman, kHumanOriginOffset);
        At<std::uint32_t>(potential, 0x220) = humanId;
        using OnChangeActionFn = void(__fastcall*)(
            void*, int, int, std::uint32_t);
        const auto onChangeAction = reinterpret_cast<OnChangeActionFn>(
            g_moduleBase + kRvaActionWPOnChangeAction);
        onChangeAction(potential, 23, inventoryType, 0);
        // Suppress the input-state capture that normally accompanies menu
        // targeting. G still owns the hold/release gesture.
        At<bool>(gameInput, 0x1B8) = false;
        using UpdateVisualsFn = void(__fastcall*)(void*);
        const auto updateVisuals = reinterpret_cast<UpdateVisualsFn>(
            g_moduleBase + kRvaActionWPUpdateVisuals);
        updateVisuals(potential);
        if (!At<void*>(potential, 0x248)) {
            tracePreview(4, potential);
            g_nativeGrenadePreviewWaypoint = potential;
            StopNativeGrenadePreview();
            return false;
        }
        g_nativeGrenadePreviewWaypoint = potential;
    }

    void* target = At<void*>(potential, 0x248);
    if (!target || At<std::uint32_t>(potential, 0x220) != humanId) {
        tracePreview(5, potential);
        StopNativeGrenadePreview();
        return false;
    }
    // The potential waypoint normally sits at a fixed planning point. In
    // direct control its throw origin must follow the walking operator so
    // DK2's native trajectory and queued throw use the same moving source.
    const Vector3 currentOrigin = At<Vector3>(
        clientHuman, kHumanOriginOffset);
    At<Vector3>(potential, kHumanOriginOffset) = currentOrigin;
    At<Vector3>(potential, 0x22C) = currentOrigin;
    using SetTransformFn = void(__fastcall*)(
        void*, const Vector3*, const void*, float);
    const auto setTransform = reinterpret_cast<SetTransformFn>(
        g_moduleBase + kRvaEntitySetTransform);
    setTransform(
        target,
        &g_cursorWorld,
        reinterpret_cast<const void*>(g_moduleBase + kRvaMatrix3Identity),
        At<float>(target, 0x9C));
    using OnGrenadeTargetMovedFn = void(__fastcall*)(void*, bool);
    const auto onGrenadeTargetMoved =
        reinterpret_cast<OnGrenadeTargetMovedFn>(
            g_moduleBase + kRvaActionWPOnGrenadeTargetMoved);
    onGrenadeTargetMoved(potential, false);
    // UpdateGrenadeTrajectory calls UpdateGrenadeHighlight after it has
    // calculated the landing point. Calling the highlight separately can
    // show an effect circle even when DK2 rejected the trajectory.
    if (!LinkNativeGrenadePreviewWaypoint(potential)) {
        tracePreview(6, potential);
        StopNativeGrenadePreview();
        return false;
    }
    tracePreview(0, potential);
    return true;
}

void SetCrosshairFeedback(std::uint32_t color) {
    g_crosshairFeedbackColor = color;
    g_crosshairFeedbackUntil = GetTickCount64() + kShootFeedbackDurationMs;
}

std::uint32_t GetCrosshairColor() {
    // Keep the bar reticle white through targeting, firing, and item actions.
    return kCrosshairColor;
}

void RenderCrosshairBars(
    void* renderer, const Vector3& center, std::uint32_t fillColor) {
    const float scale = g_crosshairScale;
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG shotAge = now - g_lastAcceptedShotTick;
    const float pulse = g_lastAcceptedShotTick != 0 &&
        shotAge < kCrosshairShotPulseMs
            ? 1.0F - static_cast<float>(shotAge) /
                static_cast<float>(kCrosshairShotPulseMs)
            : 0.0F;
    const float inner =
        (kCrosshairInnerRadius + kCrosshairShotExpansion * pulse) * scale;
    const float outer =
        (kCrosshairOuterRadius + kCrosshairShotExpansion * pulse) * scale;
    const auto stroke = [renderer, &center](
                            float inner, float outer, float offset,
                            std::uint32_t color) {
        const Vector3 leftOuter{center.x - outer, center.y,
            center.z + offset};
        const Vector3 leftInner{center.x - inner, center.y,
            center.z + offset};
        const Vector3 rightInner{center.x + inner, center.y,
            center.z + offset};
        const Vector3 rightOuter{center.x + outer, center.y,
            center.z + offset};
        const Vector3 topOuter{center.x + offset, center.y,
            center.z - outer};
        const Vector3 topInner{center.x + offset, center.y,
            center.z - inner};
        const Vector3 bottomInner{center.x + offset, center.y,
            center.z + inner};
        const Vector3 bottomOuter{center.x + offset, center.y,
            center.z + outer};
        g_renderLine(renderer, &leftOuter, &leftInner, color);
        g_renderLine(renderer, &rightInner, &rightOuter, color);
        g_renderLine(renderer, &topOuter, &topInner, color);
        g_renderLine(renderer, &bottomInner, &bottomOuter, color);
    };

    // Pack the strokes well below a screen pixel so their centers form an
    // even opaque fill instead of seven individually visible soft bands.
    for (int i = -24; i <= 24; ++i) {
        stroke(inner, outer,
            static_cast<float>(i) * kCrosshairHalfThickness *
                scale / 24.0F,
            fillColor);
    }
}

bool GuiChildListValid(void* parent) {
    if (!parent) return false;
    void* head = static_cast<std::byte*>(parent) + 0x48;
    void* tail = At<void*>(head, 0x10);
    return At<void*>(head, 0) == head && tail && At<void*>(tail, 8) == head;
}

bool AttachGuiChild(void* parent, void* child) {
    if (!child || !GuiChildListValid(parent)) return false;
    void* node = static_cast<std::byte*>(child) + 8;
    if (At<void*>(node, 0) != node || At<void*>(node, 8) != node ||
        At<void*>(node, 0x10) != node || At<void*>(node, 0x18) != child) return false;
    void* head = static_cast<std::byte*>(parent) + 0x48;
    void* tail = At<void*>(head, 0x10);
    // Mirror the game's GUI::Item copy-constructor child insertion. The
    // native profile owns the native-allocated clone and deletes it normally.
    At<void*>(node, 0) = head;
    At<void*>(node, 8) = head;
    At<void*>(node, 0x10) = tail;
    At<void*>(tail, 8) = node;
    At<void*>(head, 0x10) = node;
    At<void*>(child, 0x40) = parent;
    return true;
}

Vector2 NameGlyphOrigin(void* profile, void* name) {
    if (!name) return {40.0F, -40.0F};
    void* parent = At<void*>(name, 0x40);
    const Vector2 base = parent ? At<Vector2>(parent, 0x68) : Vector2{};
    const Vector2 profileOrigin = At<Vector2>(profile, 0x68);
    const int lineCount = At<int>(name, 0x400);
    const void* lines = lineCount > 0 && lineCount <= 16 ? At<void*>(name, 0x3F8) : nullptr;
    if (lines) {
        const Vector2 first = At<Vector2>(lines, 0);
        const Vector2 scroll = At<Vector2>(name, 0x408);
        if (std::isfinite(first.x) && std::isfinite(first.z) &&
            std::abs(first.x) <= At<int>(profile, 0x70) * 0.5F + 24.0F)
            return {base.x - profileOrigin.x + first.x + scroll.x,
                    base.z - profileOrigin.z + first.z + scroll.z};
    }
    // Font line coordinates use a separate renderer transform. Native Item
    // alignment and measured text size are in the profile's GUI coordinates.
    const int alignment = At<int>(name, 0x98);
    if (parent && (alignment & 8) && (alignment & 16)) {
        const Vector2 offset = At<Vector2>(name, 0x9C);
        return {base.x - profileOrigin.x + At<int>(parent, 0x70) * 0.5F -
                    At<int>(name, 0x70) + offset.x,
                base.z - profileOrigin.z - At<int>(parent, 0x74) * 0.5F + offset.z};
    }
    const Vector2 center = At<Vector2>(name, 0x68);
    return {center.x - profileOrigin.x - At<int>(name, 0x70) * 0.5F,
            center.z - profileOrigin.z - At<int>(name, 0x74) * 0.5F};
}

std::unordered_map<void*, Vector2> g_equippedGrenadeIconScales;

void FitEquippedGrenadeIcon(void* profile, const void* human) {
    const int slot = At<int>(human, 0xD80);
    if (slot < 1 || slot >= kInventorySlotCount) return;
    const void* item = At<void*>(human, 0x430 + static_cast<std::size_t>(slot) * 0x58);
    if (!item || At<int>(item, 0x20) != 3) return;
    void* icon = g_guiFindChild(profile, "#Icon");
    if (!icon) return;
    const float width = At<float>(icon, 0x3CC), height = At<float>(icon, 0x3D0);
    float& scaleX = At<float>(icon, 0x3C4);
    float& scaleY = At<float>(icon, 0x3C8);
    if (width <= 0 || height <= 0 || scaleX <= 0 || scaleY <= 0) return;
    g_equippedGrenadeIconScales.try_emplace(icon, Vector2{scaleX, scaleY});
    const float factor = std::min({1.0F, 150.0F / (width * scaleX), 44.0F / (height * scaleY)});
    if (!std::isfinite(factor)) return;
    scaleX *= factor; scaleY *= factor;
    At<int>(icon, 0x70) = static_cast<int>(width * scaleX);
    At<int>(icon, 0x74) = static_cast<int>(height * scaleY);
    At<bool>(icon, 0x398) = true;
}

void UpdateGrenadeHud(void* profile, const void* human) {
    const auto choice = GrenadeChoiceFor(human);
    const void* item = ClientGrenadeChoiceTemplate(human, choice);
    const bool visible = g_enabled && human == g_controlledClientHuman && item && !IsGameMenuOpen();
    void* label = g_guiFindChild(profile, "#DA_Grenade");
    void* icon = g_guiFindChild(profile, "#DA_GrenadeIcon");
    void* soldierName = g_guiFindChild(profile, "#Name");
    void* owner = profile;
    const auto create = [profile, owner](const char* sourceName, const char* name, GuiCloneStaticTextFn clone) -> void* {
        void* source = g_guiFindChild(profile, sourceName);
        if (!source || !clone || !GuiChildListValid(profile)) return nullptr;
        void* child = clone(source);
        if (!child) return nullptr;
        g_guiSetName(child, name);
        if (!AttachGuiChild(owner, child)) {
            using DeleteFn = void(__fastcall*)(void*, unsigned int);
            reinterpret_cast<DeleteFn>(At<void*>(At<void*>(child, 0), 0))(child, 1);
            return nullptr;
        }
        At<bool>(child, 0x90) = false;
        At<bool>(child, 0x91) = true;
        At<int>(child, 0x98) = 8 | 32;
        return child;
    };
    if (!label && visible) label = create(soldierName ? "#Name" : "#WeaponName", "#DA_Grenade", g_guiCloneStaticText);
    if (!icon && visible && g_guiSetImage) icon = create("#Icon", "#DA_GrenadeIcon", g_guiCloneStaticImage);
    if (label) At<bool>(label, 0x80) = !visible;
    if (icon) At<bool>(icon, 0x80) = !visible;
    if (!visible || !label) return;
    int quantity = std::max(0, At<int>(human, 0x43C + static_cast<std::size_t>(choice.slot) * 0x58));
    if (g_controlledServerHuman) {
        const void* serverItem = At<void*>(g_controlledServerHuman,
            kHumanServerInventoryOffset + static_cast<std::size_t>(choice.slot) * sizeof(void*));
        if (serverItem && FireWeaponId(At<void*>(serverItem, 0x20)) == choice.itemId)
            quantity = std::max(0, At<int>(serverItem, 0x3C));
    }
    const Vector2 glyph = NameGlyphOrigin(profile, soldierName);
    char text[160]{};
    std::snprintf(text, sizeof(text), "x%d", quantity);
    At<int>(label, 0x98) = 8 | 16;
    At<Vector2>(label, 0x9C) = {};
    g_guiChangeText(label, text);
    // Let native font layout determine the count's actual glyph origin, then
    // translate that origin to the name. Item alignment is not text layout.
    At<Vector2>(label, 0x408) = {};
    At<Vector2>(label, 0x418) = {};
    if (g_guiRebuildText) g_guiRebuildText(label);
    const void* countLines = At<int>(label, 0x400) > 0 ? At<void*>(label, 0x3F8) : nullptr;
    const Vector2 countOrigin = countLines ? At<Vector2>(countLines, 0) : Vector2{
        At<int>(profile, 0x70) * 0.5F - At<int>(label, 0x70),
        At<int>(profile, 0x74) * 0.5F - At<int>(label, 0x74)};
    At<Vector2>(label, 0x408) = {
        glyph.x - 40.0F - At<int>(label, 0x70) - countOrigin.x,
        glyph.z - countOrigin.z};
    if (icon) {
        const char* image = At<const char*>(item, 0x50);
        if (image && *image) g_guiSetImage(icon, image);
        const float width = At<float>(icon, 0x3CC), height = At<float>(icon, 0x3D0);
        if (width > 0 && height > 0) {
            const float scale = 28.0F / std::max(width, height);
            At<float>(icon, 0x3C4) = scale;
            At<float>(icon, 0x3C8) = scale;
            At<bool>(icon, 0x398) = true;
            At<int>(icon, 0x70) = static_cast<int>(width * scale);
            At<int>(icon, 0x74) = static_cast<int>(height * scale);
            const void* gameGui = At<void*>(reinterpret_cast<void*>(g_moduleBase + 0xFF0E98), 0);
            At<std::uint32_t>(icon, 0x364) = gameGui ? At<std::uint32_t>(gameGui, 0x8F8) : 0;
            At<std::uint64_t>(icon, 0x368) = 0;
        }
        // Item localOrigin is the image center in parent coordinates. With
        // flags0 native Update adds only the profile's global origin.
        At<int>(icon, 0x98) = 0;
        At<Vector2>(icon, 0x78) = {
            glyph.x - 20.0F,
            glyph.z + (soldierName ? At<int>(soldierName, 0x74) * 0.5F : 17.5F)};
    }
    static ULONGLONG lastHudTrace = 0;
    const ULONGLONG now = GetTickCount64();
    if (now - lastHudTrace >= 1000 && soldierName) {
        char line[1300]{};
        const void* parent = At<void*>(soldierName, 0x40);
        const void* lines = At<int>(soldierName, 0x400) > 0 ? At<void*>(soldierName, 0x3F8) : nullptr;
        const Vector2 first = lines ? At<Vector2>(lines, 0) : Vector2{};
        std::snprintf(line, sizeof(line),
            "%llu HUD profile=%p parent=%p nameParent=%p size=%d,%d global=%.1f,%.1f nameGlobal=%.1f,%.1f nameSize=%d,%d nameFlags=%x nameOffset=%.1f,%.1f nameTextOffset=%.1f,%.1f line=%.1f,%.1f glyph=%.1f,%.1f countGlobal=%.1f,%.1f countOffset=%.1f,%.1f iconGlobal=%.1f,%.1f\r\n",
            static_cast<unsigned long long>(now), profile, At<void*>(profile, 0x40), parent,
            At<int>(profile, 0x70), At<int>(profile, 0x74), At<float>(profile, 0x68), At<float>(profile, 0x6C),
            At<float>(soldierName, 0x68), At<float>(soldierName, 0x6C), At<int>(soldierName, 0x70), At<int>(soldierName, 0x74),
            At<unsigned>(soldierName, 0x98), At<float>(soldierName, 0x9C), At<float>(soldierName, 0xA0),
            At<float>(soldierName, 0x418), At<float>(soldierName, 0x41C), first.x, first.z, glyph.x, glyph.z,
            At<float>(label, 0x68), At<float>(label, 0x6C), At<float>(label, 0x418), At<float>(label, 0x41C),
            icon ? At<float>(icon, 0x68) : 0, icon ? At<float>(icon, 0x6C) : 0);
        TraceItemDetail(line);
        lastHudTrace = now;
    }
}

void __fastcall HookedUpdateHumanProfile(void* profile, const void* human) {
    if (profile && g_guiFindChild) {
        void* nativeIcon = g_guiFindChild(profile, "#Icon");
        const auto previous = g_equippedGrenadeIconScales.find(nativeIcon);
        if (previous != g_equippedGrenadeIconScales.end()) {
            At<float>(nativeIcon, 0x3C4) = previous->second.x;
            At<float>(nativeIcon, 0x3C8) = previous->second.z;
            g_equippedGrenadeIconScales.erase(previous);
        }
    }
    g_originalUpdateHumanProfile(profile, human);
    if (!profile || !human || !g_guiFindChild || !g_guiCloneStaticText ||
        !g_guiSetName || !g_guiChangeText) return;
    const char* name = At<const char*>(profile, 0x28);
    if (!name || std::strcmp(name, "HUD_Troop_Profile") != 0) return;
    FitEquippedGrenadeIcon(profile, human);
    UpdateGrenadeHud(profile, human);
    void* label = g_guiFindChild(profile, "#DA_FireMode");
    const void* itemTemplate = ClientFireTemplate(human);
    const bool visible = g_enabled && human == g_controlledClientHuman &&
        itemTemplate && g_selectedGearInventoryType < 0 &&
        g_selectedSpyCameraHotkey == 0 && g_selectedCrowbarHotkey == 0 &&
        !IsGameMenuOpen();
    if (!label && visible && GuiChildListValid(profile)) {
        void* source = g_guiFindChild(profile, "#WeaponName");
        if (!source) return;
        label = g_guiCloneStaticText(source);
        if (!label) return;
        g_guiSetName(label, "#DA_FireMode");
        if (!AttachGuiChild(profile, label)) {
            // Scalar deleting destructor, verified from StaticText's vtable.
            using DeleteGuiItemFn = void(__fastcall*)(void*, unsigned int);
            reinterpret_cast<DeleteGuiItemFn>(At<void*>(At<void*>(label, 0), 0))(label, 1);
            TraceTraversal("fire_hud_attach_failed", 0, 0, 0);
            return;
        }
        At<bool>(label, 0x90) = false;
        At<bool>(label, 0x91) = true;
        At<int>(label, 0x98) = 8 | 32; // Native right/bottom text alignment, like ammunition.
        TraceTraversal("fire_hud_created", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)), 0, 0);
    }
    if (!label) return;
    At<bool>(label, 0x80) = !visible;
    if (!visible) return;
    // GUI::Item::Update recalculates origin (+0x78) from alignOffset
    // (+0x9c) every frame. Change the persistent anchor offset, not origin.
    // Match ammunition's right inset and place the mode one line above it.
    At<Vector2>(label, 0x9c) = {-4.0F, 24.0F};
    const FireMode mode = SelectedFireMode(At<std::uint32_t>(human, kHumanIdOffset), itemTemplate);
    g_guiChangeText(label, mode == FireMode::Auto ? "AUTO" : mode == FireMode::Burst ? "BURST" : "SEMI");
}

void BeepForState(bool enabled) {
    Beep(enabled ? 920 : 420, 75);
}

bool IsSupportedExecutable() {
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(g_moduleBase);
    if (!dos || dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }

    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(
        g_moduleBase + static_cast<std::uintptr_t>(dos->e_lfanew));
    return nt->Signature == IMAGE_NT_SIGNATURE &&
           nt->FileHeader.TimeDateStamp == kSupportedTimeDateStamp &&
           nt->OptionalHeader.SizeOfImage == kSupportedSizeOfImage;
}

void EmitAbsoluteJump(std::byte* destination, const void* target) {
    // jmp qword ptr [rip]; <target>. Unlike mov rax/jmp rax, this preserves
    // every register used by the copied function prologue.
    destination[0] = std::byte{0xFF};
    destination[1] = std::byte{0x25};
    std::memset(destination + 2, 0, 4);
    const auto address = reinterpret_cast<std::uintptr_t>(target);
    std::memcpy(destination + 6, &address, sizeof(address));
}

bool InstallDetour(
    void* target,
    void* replacement,
    std::size_t detourLength,
    void** original) {
    if (!target || !replacement || !original ||
        detourLength < kAbsoluteJumpLength) {
        return false;
    }

    auto* targetBytes = static_cast<std::byte*>(target);
    const std::size_t trampolineSize = detourLength + kAbsoluteJumpLength;
    void* trampoline = VirtualAlloc(
        nullptr, trampolineSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        return false;
    }

    auto* trampolineBytes = static_cast<std::byte*>(trampoline);
    std::memcpy(trampolineBytes, targetBytes, detourLength);
    EmitAbsoluteJump(trampolineBytes + detourLength, targetBytes + detourLength);

    DWORD oldProtect = 0;
    if (!VirtualProtect(targetBytes, detourLength, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return false;
    }

    std::memset(targetBytes, 0x90, detourLength);
    EmitAbsoluteJump(targetBytes, replacement);
    FlushInstructionCache(GetCurrentProcess(), targetBytes, detourLength);

    DWORD ignored = 0;
    VirtualProtect(targetBytes, detourLength, oldProtect, &ignored);
    *original = trampoline;
    return true;
}

bool InstallGameClientLoadDetour(
    void* target,
    void* replacement,
    std::size_t prefixLength,
    void** original) {
    // Button2_Down and Button2_Click load g_pGameClient through a RIP-relative
    // instruction within the first 14 bytes. Rebuild that one instruction in
    // the trampoline instead of copying an invalid relative displacement.
    if (!target || !replacement || !original) {
        return false;
    }
    auto* targetBytes = static_cast<std::byte*>(target);
    const std::size_t detourLength = prefixLength + 7;
    if (targetBytes[prefixLength] != std::byte{0x48} ||
        targetBytes[prefixLength + 1] != std::byte{0x8B} ||
        targetBytes[prefixLength + 2] != std::byte{0x05}) {
        return false;
    }
    std::int32_t displacement = 0;
    std::memcpy(&displacement, targetBytes + prefixLength + 3,
                sizeof(displacement));
    const auto loadedAddress = reinterpret_cast<std::uintptr_t>(
        targetBytes + detourLength + displacement);
    if (loadedAddress != g_moduleBase + kRvaGameClientGlobal) {
        return false;
    }

    constexpr std::size_t replacementLoadLength = 13;
    const std::size_t trampolineSize = prefixLength +
        replacementLoadLength + kAbsoluteJumpLength;
    void* trampoline = VirtualAlloc(
        nullptr, trampolineSize, MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        return false;
    }
    auto* trampolineBytes = static_cast<std::byte*>(trampoline);
    std::memcpy(trampolineBytes, targetBytes, prefixLength);
    auto* load = trampolineBytes + prefixLength;
    load[0] = std::byte{0x48};
    load[1] = std::byte{0xB8}; // mov rax, absolute address
    const std::uintptr_t globalAddress =
        g_moduleBase + kRvaGameClientGlobal;
    std::memcpy(load + 2, &globalAddress, sizeof(globalAddress));
    load[10] = std::byte{0x48};
    load[11] = std::byte{0x8B};
    load[12] = std::byte{0x00}; // mov rax, [rax]
    EmitAbsoluteJump(load + replacementLoadLength,
                     targetBytes + detourLength);

    DWORD oldProtect = 0;
    if (!VirtualProtect(targetBytes, detourLength,
                        PAGE_EXECUTE_READWRITE, &oldProtect)) {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return false;
    }
    std::memset(targetBytes, 0x90, detourLength);
    EmitAbsoluteJump(targetBytes, replacement);
    FlushInstructionCache(GetCurrentProcess(), targetBytes, detourLength);
    DWORD ignored = 0;
    VirtualProtect(targetBytes, detourLength, oldProtect, &ignored);
    *original = trampoline;
    return true;
}

bool DirectControlMovementKeyDown() {
    return KeyDown('W') || KeyDown('A') || KeyDown('S') || KeyDown('D') ||
        KeyDown('Q') || KeyDown('E');
}

bool DirectControlEquipmentKeyDown() {
    if (KeyDown('H') || KeyDown('G') || KeyDown('F') ||
        KeyDown(VK_LCONTROL) || KeyDown(VK_LSHIFT)) {
        return true;
    }
    for (int key = '0'; key <= '9'; ++key) {
        if (KeyDown(key)) {
            return true;
        }
    }
    return false;
}

bool HumanCanBeControlled(const void* human) {
    return human && At<float>(human, kHumanHealthOffset) > 0.0F &&
           At<bool>(human, kHumanControllableOffset);
}

void StopHuman(void* human) {
    if (!HumanCanBeControlled(human)) {
        return;
    }

    auto* path = static_cast<std::byte*>(human) + kHumanPathOffset;
    const int waypointCount = At<int>(path, kPathWaypointCountOffset);
    const Vector3 origin = At<Vector3>(human, kHumanOriginOffset);
    if (waypointCount > 1) {
        g_deletePath(human, 0, origin.x, origin.z);
    }
}

void FinishIncrement(void* human) {
    auto* path = static_cast<std::byte*>(human) + kHumanPathOffset;
    const int oldCount = At<int>(path, kPathWaypointCountOffset);
    g_addToPathEnd(path);
    const int added = At<int>(path, kPathWaypointCountOffset) - oldCount;
    if (added > 0) {
        g_notifyPath(human, path, oldCount, added, 0, nullptr, false);
    }
}

void SuspendDirectControlForMenu(void* human, void* serverHuman) {
    g_grenadeRequireRelease = true;
    RestoreGrenadeHeldPose(human);
    CancelDoorInteraction(serverHuman, g_doorInteraction);
    if (g_crouchHeld && serverHuman && g_queueCrouch) {
        g_queueCrouch(serverHuman, false);
    }
    StopNativeGrenadePreview();
    if (g_wasMoving && human) {
        // The potential wheel action carries a native path waypoint index.
        // Trimming the helper path here can invalidate that index before the
        // item request reaches the server.
        g_menuSuspendedPath = true;
    }
    g_wasMoving = false;
    g_accumulatedMs = 0;
    g_lastDirection = {};
    g_directMovementDirection = {};
    RestoreLeanPlaybackForHuman(g_controlledClientHuman);
    g_leanSide = 0;
    g_leanReachedHold = false;
    g_leanPhysicalSide = 0;
    g_leanPressedAt = 0;
    g_leanWasMoving = false;
    g_readyHold = kReadyHoldNone;
    g_cursorWorldValid = false;
    g_reloadRequested = false;
    g_reloadSprintBlocked = false;
    g_cancelReloadRequested = false;
    g_reloadCanceledWeapon = nullptr;
    g_shootRequested = false;
    g_shootRequestRemainingMs = 0;
    g_shootEquipQueued = false;
    g_fireHeld = false;
    g_fireSequence = {};
    g_displayFireValid = false;
    g_autoFireActive = false;
    g_autoFireWeapon = nullptr;
    g_equipmentRequest = kNoEquipmentRequest;
    g_grenadeAiming = false;
    g_grenadeThrowRequested = false;
    g_grenadeThrowUsesNativePreview = false;
    g_grenadeHoldMs = 0;
    g_grenadeHoldStartTick = 0;
    g_crouchHeld = false;
    g_crouchCommandPending = false;
    g_sprintDesired = false;
    g_doorInteractTargetId = 0;
    g_lastInteractedDoorId = 0;
    g_traversalUsedThisPress = false;
    g_traversalFWasDown = false;
    g_traversalRepathRequested = false;
    g_traversalPathHoldUntil = 0;
    g_doorInteractKick = false;
    StopDirectSprint(serverHuman);
}

void AddMovementLookahead(void* human, Vector2 direction) {
    auto* path = static_cast<std::byte*>(human) + kHumanPathOffset;
    const Vector3 origin = At<Vector3>(human, kHumanOriginOffset);
    // F needs a destination beyond a window, especially when starting just
    // inside it. A short destination can end before the crossing action.
    const float lookahead = KeyDown('F')
        ? kTraversalLookahead : kMovementLookahead;
    const Vector2 endpoint{
        origin.x + direction.x * lookahead,
        origin.z + direction.z * lookahead};

    int modifiedStart = 0;
    int modifiedCount = 0;
    int actionCount = 0;
    std::array<ActionWaypoint, kMaxActionWaypoints> actions{};
    g_addToPath(
        path,
        human,
        endpoint.x,
        endpoint.z,
        modifiedStart,
        modifiedCount,
        actions.data(),
        static_cast<int>(actions.size()),
        actionCount);
    if (KeyDown('F')) {
        int firstType = -1;
        if (actionCount > 0) {
            std::memcpy(&firstType, actions[0].storage.data(), sizeof(firstType));
        }
        TraceTraversal("path", modifiedCount, actionCount, firstType);
    }

    if (modifiedCount > 0) {
        // The hidden path supplies speed and animation. Only forward a native
        // traversal action on F; its other inferred actions remain suppressed.
        const ActionWaypoint* traversal = nullptr;
        if (KeyDown('F') && !g_traversalUsedThisPress &&
            g_lastInteractedDoorId == 0) {
            for (int index = 0;
                 index < std::clamp(actionCount, 0, kMaxActionWaypoints);
                 ++index) {
                int type = 0;
                std::memcpy(&type, actions[index].storage.data(), sizeof(type));
                if (type == kActionJumpOverObstacle ||
                    type == kActionJumpThroughWindow) {
                    traversal = &actions[index];
                    break;
                }
            }
        }
        g_notifyPath(
            human,
            path,
            modifiedStart,
            modifiedCount,
            traversal ? 1 : 0,
            traversal,
            false);
        if (traversal) {
            int type = 0;
            int waypointIndex = -1;
            int targetId = -1;
            std::memcpy(&type, traversal->storage.data(), sizeof(type));
            std::memcpy(&waypointIndex,
                        traversal->storage.data() + 0x48,
                        sizeof(waypointIndex));
            std::memcpy(&targetId,
                        traversal->storage.data() + 0x04,
                        sizeof(targetId));
            TraceTraversal("sent", type, modifiedStart, modifiedCount);
            TraceTraversal("jump_wp", waypointIndex, targetId,
                           At<int>(path, kPathWaypointCountOffset));
            g_traversalUsedThisPress = true;
            g_traversalPathHoldUntil = GetTickCount64() + 1200;
        }
    }
}

bool UpdateMovementPath(
    void* human,
    std::uint32_t humanId,
    Vector2 direction,
    int dt) {
    const bool moving = direction.x != 0.0F || direction.z != 0.0F;
    if (humanId != g_activeHumanId) {
        g_wasMoving = false;
        g_accumulatedMs = 0;
        g_lastDirection = {};
        g_activeHumanId = humanId;
        g_traversalPathHoldUntil = 0;
    }

    // A jump action is attached to the path we just submitted. Preserve that
    // path while native traversal approaches the action waypoint, including
    // if the player briefly releases or changes WASD direction.
    if (GetTickCount64() < g_traversalPathHoldUntil) {
        return false;
    }

    if (!moving) {
        g_traversalPathHoldUntil = 0;
        const bool endingPath = g_wasMoving;
        if (endingPath) {
            StopNativeGrenadePreview();
            FinishIncrement(human);
            StopHuman(human);
        }
        g_wasMoving = false;
        g_accumulatedMs = 0;
        g_lastDirection = {};
        return endingPath;
    }

    bool rebasePath = !g_wasMoving || g_traversalRepathRequested;
    if (g_wasMoving) {
        const float directionDot =
            direction.x * g_lastDirection.x + direction.z * g_lastDirection.z;
        rebasePath = rebasePath || directionDot < 0.75F;
    }
    if (rebasePath) {
        g_traversalPathHoldUntil = 0;
        StopNativeGrenadePreview();
        StopHuman(human);
        g_accumulatedMs = kMovementRefreshCooldownMs;
    } else {
        g_accumulatedMs = std::min(
            g_accumulatedMs + std::max(dt, 0), 1000);
    }

    auto* path = static_cast<std::byte*>(human) + kHumanPathOffset;
    const Vector3 origin = At<Vector3>(human, kHumanOriginOffset);
    const Vector2 endpoint = At<Vector2>(path, kPathEndpointOffset);
    const float forwardLead =
        (endpoint.x - origin.x) * direction.x +
        (endpoint.z - origin.z) * direction.z;
    const bool pathNeedsRefresh = rebasePath ||
        At<int>(path, kPathWaypointCountOffset) <= 1 ||
        !std::isfinite(forwardLead) ||
        forwardLead < kMovementRefreshLead;
    bool changedPath = rebasePath;
    if (pathNeedsRefresh &&
        GetTickCount64() >= g_traversalPathHoldUntil &&
        g_accumulatedMs >= kMovementRefreshCooldownMs) {
        if (!rebasePath) {
            StopNativeGrenadePreview();
        }
        AddMovementLookahead(human, direction);
        g_traversalRepathRequested = false;
        g_accumulatedMs = 0;
        changedPath = true;
    }
    g_wasMoving = true;
    g_lastDirection = direction;
    return changedPath;
}

Vector2 ReadMovementDirection() {
    Vector2 direction{};
    if (KeyDown('W')) {
        direction.z -= 1.0F;
    }
    if (KeyDown('S')) {
        direction.z += 1.0F;
    }
    if (KeyDown('A')) {
        direction.x -= 1.0F;
    }
    if (KeyDown('D')) {
        direction.x += 1.0F;
    }

    const float lengthSquared = direction.x * direction.x + direction.z * direction.z;
    if (lengthSquared > 1.0F) {
        const float inverseLength = 1.0F / std::sqrt(lengthSquared);
        direction.x *= inverseLength;
        direction.z *= inverseLength;
    }
    return direction;
}

Vector2 ReadLeanIntent(Vector2 movement) {
    const bool leftDown = KeyDown('Q');
    const bool rightDown = KeyDown('E');
    const int physicalSide = leftDown != rightDown
        ? (leftDown ? 1 : -1) : 0;
    const ULONGLONG now = GetTickCount64();
    if (physicalSide != 0 && physicalSide != g_leanPhysicalSide) {
        g_leanPressedAt = now;
    }
    g_leanPhysicalSide = physicalSide;
    const bool stationary = movement.x == 0.0F && movement.z == 0.0F;
    const bool moving = !stationary;
    if (moving != g_leanWasMoving) {
        RestoreLeanPlaybackForHuman(g_controlledClientHuman);
        g_leanReachedHold = false;
    }
    g_leanWasMoving = moving;
    // A quick tap should finish a visible lean even after the key releases.
    const int side = physicalSide != 0
        ? physicalSide : stationary && g_leanSide != 0 &&
            g_leanPressedAt != 0 &&
            now - g_leanPressedAt < 420 ? g_leanSide : 0;
    if (side != g_leanSide) {
        TraceTraversal("lean_side", side, g_leanSide, 0);
        TraceTraversal("lean_input", leftDown ? 1 : 0,
            rightDown ? 1 : 0,
            movement.x != 0.0F || movement.z != 0.0F ? 1 : 0);
        RestoreLeanPlaybackForHuman(g_controlledClientHuman);
        g_leanReachedHold = false;
    }
    g_leanSide = side;
    return movement;
}

void* GetServerHuman(std::uint32_t entityId) {
    if (entityId >= kMaximumEntityId) {
        return nullptr;
    }

    void* gameServer = *reinterpret_cast<void**>(
        g_moduleBase + kRvaGameServerGlobal);
    if (!gameServer) {
        return nullptr;
    }

    auto* entitySlot = static_cast<std::byte*>(gameServer) +
        kGameEntityArrayOffset + static_cast<std::size_t>(entityId) * sizeof(void*);
    return *reinterpret_cast<void**>(entitySlot);
}

std::uint64_t PackVector2(Vector2 value) {
    std::uint64_t packed = 0;
    std::memcpy(&packed, &value, sizeof(value));
    return packed;
}

bool PathfinderStepIsClear(
    const void* pathfinder,
    Vector2 start,
    Vector2 end) {
    if (!pathfinder ||
        !g_pathfinderIsPointClear(
            pathfinder, PackVector2(end), kDirectMovementCollisionMask)) {
        return false;
    }

    Vector2 collisionPoint{};
    return !g_pathfinderTraceLine(
        pathfinder,
        PackVector2(start),
        PackVector2(end),
        kDirectMovementCollisionMask,
        &collisionPoint);
}

bool PathfinderRayIsClear(
    const void* pathfinder, Vector2 start, Vector2 end) {
    if (!pathfinder) {
        return false;
    }
    Vector2 collisionPoint{};
    return !g_pathfinderTraceLine(
        pathfinder, PackVector2(start), PackVector2(end),
        kDirectMovementCollisionMask, &collisionPoint);
}

struct LeanClearance {
    int mask = 0;
    float reach = 0.0F;
    Vector2 sideOffset{};
};

bool GetLeanClearance(const void* serverHuman, LeanClearance& result) {
    if (!serverHuman || g_leanSide == 0) {
        return false;
    }
    void* gameServer = *reinterpret_cast<void**>(
        g_moduleBase + kRvaGameServerGlobal);
    const void* pathfinder = gameServer
        ? At<void*>(gameServer, kGamePathfinderOffset) : nullptr;
    const void* firearm = At<void*>(serverHuman,
        kHumanServerEquippedItemOffset);
    if (!pathfinder || !InventoryItemIsFirearm(firearm)) {
        return false;
    }
    const Vector3 aim = At<Vector3>(serverHuman, 0x240);
    const float length = std::hypot(aim.x, aim.z);
    if (!std::isfinite(length) || length < 0.1F) {
        return false;
    }
    const float reach = At<float>(serverHuman, 0x2B8) +
        At<float>(firearm, 0x60);
    if (!std::isfinite(reach) || reach < 0.1F || reach > 4.0F) {
        return false;
    }
    const Vector3 origin = At<Vector3>(serverHuman, kHumanOriginOffset);
    const Vector2 center{origin.x, origin.z};
    const Vector2 forward{aim.x / length, aim.z / length};
    constexpr float kProbeSideOffset = 0.35F;
    // The rendered lean clips use the opposite lateral sign from the
    // server's aim-direction coordinate basis.
    const float side = -static_cast<float>(g_leanSide);
    const Vector2 shoulder{
        center.x - forward.z * side * kProbeSideOffset,
        center.z + forward.x * side * kProbeSideOffset};
    const Vector2 centerMuzzle{
        center.x + forward.x * reach,
        center.z + forward.z * reach};
    const Vector2 leanedMuzzle{
        shoulder.x + forward.x * reach,
        shoulder.z + forward.z * reach};
    result.mask =
        (PathfinderRayIsClear(pathfinder, center, centerMuzzle) ? 1 : 0) |
        (PathfinderStepIsClear(pathfinder, center, shoulder) ? 2 : 0) |
        (PathfinderRayIsClear(pathfinder, shoulder, leanedMuzzle) ? 4 : 0);
    result.reach = reach;
    result.sideOffset = {shoulder.x - center.x, shoulder.z - center.z};
    return true;
}

void TraceLeanClearance(const void* serverHuman) {
    static ULONGLONG lastTrace = 0;
    const ULONGLONG now = GetTickCount64();
    if (!serverHuman || g_leanSide == 0 || now - lastTrace < 250) {
        return;
    }
    LeanClearance result{};
    if (!GetLeanClearance(serverHuman, result)) {
        return;
    }
    const void* firearm = At<void*>(serverHuman,
        kHumanServerEquippedItemOffset);
    TraceTraversal("lean_clear_probe", result.mask,
        At<int>(firearm, 0x2C), static_cast<int>(result.reach * 100.0F));
    lastTrace = now;
}

std::uint32_t* __fastcall HookedHumanGetLOS(
    std::uint32_t* output, const void* serverHuman,
    const Vector3* direction) {
    std::uint32_t* result = g_originalHumanGetLOS(
        output, serverHuman, direction);
    if (!g_enabled || !g_remoteInputs.empty() || !result ||
        serverHuman != g_controlledServerHuman || g_leanSide == 0) {
        return result;
    }
    const bool firearmGuardCall =
        reinterpret_cast<std::uintptr_t>(_ReturnAddress()) ==
        g_moduleBase + kRvaFirearmGuardLOSReturn;
    static ULONGLONG lastHookTrace = 0;
    const ULONGLONG now = GetTickCount64();
    if (now - lastHookTrace >= 500) {
        TraceTraversal("lean_los_hook", static_cast<int>(result[0]),
            firearmGuardCall ? 1 : 0, g_leanReachedHold ? 1 : 0);
        lastHookTrace = now;
    }
    if (!firearmGuardCall || result[0] != 1 || !g_leanReachedHold) {
        return result;
    }
    LeanClearance clearance{};
    if (GetLeanClearance(serverHuman, clearance)) {
        static ULONGLONG lastCandidateTrace = 0;
        if (now - lastCandidateTrace >= 250) {
            TraceTraversal("lean_guard_probe", static_cast<int>(result[0]),
                clearance.mask, g_leanSide);
            lastCandidateTrace = now;
        }
    }
    if ((clearance.mask & 6) == 6) {
        // The native centered trace hit a wall within weapon reach, while
        // the sideways and forward leaned segments both cleared geometry.
        result[0] = 0;
        result[2] = 0;
        result[3] = 0;
        static ULONGLONG lastClearTrace = 0;
        if (now - lastClearTrace >= 250) {
            TraceTraversal("lean_guard_clear", clearance.mask,
                static_cast<int>(clearance.reach * 100.0F), g_leanSide);
            lastClearTrace = now;
        }
    }
    return result;
}

bool __fastcall HookedFirearmFire(
    void* firearm, const Vector3* origin, const Vector3* direction,
    float targetDistance, float damage, const FirearmShoot* shot) {
    if (!g_enabled || !g_remoteInputs.empty() || !origin ||
        !g_controlledServerHuman ||
        g_leanSide == 0 || !g_leanReachedHold ||
        firearm != At<void*>(g_controlledServerHuman,
            kHumanServerEquippedItemOffset)) {
        return g_originalFirearmFire(firearm, origin, direction,
            targetDistance, damage, shot);
    }
    LeanClearance clearance{};
    if (!GetLeanClearance(g_controlledServerHuman, clearance) ||
        (clearance.mask & 6) != 6) {
        return g_originalFirearmFire(firearm, origin, direction,
            targetDistance, damage, shot);
    }
    Vector3 leanedOrigin = *origin;
    leanedOrigin.x += clearance.sideOffset.x;
    leanedOrigin.z += clearance.sideOffset.z;
    TraceTraversal("lean_shot_origin", clearance.mask,
        static_cast<int>(clearance.sideOffset.x * 100.0F),
        static_cast<int>(clearance.sideOffset.z * 100.0F));
    return g_originalFirearmFire(firearm, &leanedOrigin, direction,
        targetDistance, damage, shot);
}

void* FindDoorById(std::uint32_t doorId) {
    void* head = *reinterpret_cast<void**>(
        g_moduleBase + kRvaDoorServerTypeList);
    if (!head || doorId == 0 || doorId >= kMaximumEntityId) {
        return nullptr;
    }
    void* link = At<void*>(head, 0x08);
    for (int count = 0; link && link != head && count < 1024;
         ++count, link = At<void*>(link, 0x08)) {
        void* door = At<void*>(link, 0x18);
        if (door &&
            static_cast<std::byte*>(door) + kDoorServerTypeLinkOffset == link &&
            At<std::uint32_t>(door, kHumanIdOffset) == doorId) {
            return door;
        }
    }
    return nullptr;
}

std::uint32_t FindNearbyDoor(
    const void* serverHuman, Vector2 facing, bool kick) {
    void* gameServer = *reinterpret_cast<void**>(
        g_moduleBase + kRvaGameServerGlobal);
    void* head = *reinterpret_cast<void**>(
        g_moduleBase + kRvaDoorServerTypeList);
    if (!gameServer || !head || !serverHuman) {
        return 0;
    }
    const float facingLength = std::hypot(facing.x, facing.z);
    if (!std::isfinite(facingLength) || facingLength < 0.01F) {
        return 0;
    }
    facing.x /= facingLength;
    facing.z /= facingLength;
    const Vector3 humanPosition = At<Vector3>(
        serverHuman, kHumanOriginOffset);
    if (!std::isfinite(humanPosition.x) ||
        !std::isfinite(humanPosition.z)) {
        return 0;
    }
    const void* pathfinder = At<void*>(
        gameServer, kGamePathfinderOffset);
    if (!pathfinder) {
        return 0;
    }
    std::uint32_t nearestId = 0;
    float nearestDistanceSquared =
        kDoorInteractRange * kDoorInteractRange;
    void* link = At<void*>(head, 0x08);
    for (int count = 0; link && link != head && count < 1024;
         ++count, link = At<void*>(link, 0x08)) {
        void* door = At<void*>(link, 0x18);
        if (!door ||
            static_cast<std::byte*>(door) + kDoorServerTypeLinkOffset != link ||
            At<int>(door, kDoorServerStateOffset) != 1 ||
            (!kick && At<bool>(door, kDoorServerLockedOffset))) {
            continue;
        }
        const Vector3 doorPosition = At<Vector3>(door, kHumanOriginOffset);
        const float dx = doorPosition.x - humanPosition.x;
        const float dz = doorPosition.z - humanPosition.z;
        const float distanceSquared = dx * dx + dz * dz;
        if (!std::isfinite(distanceSquared) ||
            distanceSquared <= 0.01F ||
            distanceSquared >= nearestDistanceSquared) {
            continue;
        }
        const float distance = std::sqrt(distanceSquared);
        if (dx * facing.x + dz * facing.z <
            distance * kDoorFacingMinimumDot) {
            continue;
        }
        const float nearSideDistance = std::max(
            distance - kDoorNearSideOffset, 0.0F);
        const Vector2 nearSide{
            humanPosition.x + dx / distance * nearSideDistance,
            humanPosition.z + dz / distance * nearSideDistance};
        if (!PathfinderStepIsClear(
                pathfinder,
                {humanPosition.x, humanPosition.z}, nearSide)) {
            continue;
        }
        const std::uint32_t doorId = At<std::uint32_t>(
            door, kHumanIdOffset);
        if (doorId != 0 && doorId < kMaximumEntityId) {
            nearestId = doorId;
            nearestDistanceSquared = distanceSquared;
        }
    }
    return nearestId;
}

int FindEquipmentTypeSlot(const void* human, int type) {
    if (!human) return -1;
    for (int slot = 1; slot < kInventorySlotCount; ++slot) {
        const void* item = At<void*>(human, kHumanServerInventoryOffset + slot * sizeof(void*));
        const void* itemTemplate = item ? At<void*>(item, 0x20) : nullptr;
        if (itemTemplate && At<int>(itemTemplate, 0x20) == type) return slot;
    }
    return -1;
}

void FillUniversalToolTemplates(std::array<const void*, kInventorySlotCount>& templates,
                               const void* lockpick, const void* crowbar) {
    for (const auto& tool : {std::pair{15, lockpick}, std::pair{16, crowbar}}) {
        const int type = tool.first == 15 ? 6 : 8;
        const bool carried = std::any_of(templates.begin(), templates.end(), [type](const void* value) {
            return value && At<int>(value, 0x20) == type;
        });
        const void* existing = templates[tool.first];
        if (!carried && tool.second && At<int>(tool.second, 0x20) == type &&
            (!existing || At<int>(existing, 0x20) == 19)) templates[tool.first] = tool.second;
    }
}

void __fastcall HookedClientSetInventory(void* human, const void* const* templates) {
    const void* humanTemplate = human ? At<void*>(human, 0x20) : nullptr;
    // Human_Template::m_humanType: GOODGUY=0. Preserve all enemy inventories.
    if (!templates || !humanTemplate || At<int>(humanTemplate, 0x100) != 0 || !g_getEquipmentTemplate) {
        g_originalClientSetInventory(human, templates);
        return;
    }
    std::array<const void*, kInventorySlotCount> augmented{};
    std::copy_n(templates, kInventorySlotCount, augmented.begin());
    const void* library = At<void*>(reinterpret_cast<void*>(g_moduleBase + 0x00FF2548), 0);
    if (library) FillUniversalToolTemplates(augmented,
        g_getEquipmentTemplate(library, AnimationNameHash("Lockpick")),
        g_getEquipmentTemplate(library, AnimationNameHash("Crowbar")));
    // Register models and animation sets during native construction, rather
    // than replacing an already initialized client inventory and its state.
    g_originalClientSetInventory(human, augmented.data());
}

void EnsureUniversalTools(void* human) {
    if (!human || !g_equipmentNew || !g_inventoryEquip) return;
    for (const auto& tool : {std::pair{15, "Lockpick"}, std::pair{16, "Crowbar"}}) {
        const int type = tool.first == 15 ? 6 : 8;
        if (FindEquipmentTypeSlot(human, type) >= 0) continue;
        const void* existing = At<void*>(human, kHumanServerInventoryOffset + tool.first * sizeof(void*));
        const void* existingTemplate = existing ? At<void*>(existing, 0x20) : nullptr;
        // Replace only the native EMPTY placeholder, never another carried tool.
        if (existing && (!existingTemplate || At<int>(existingTemplate, 0x20) != 19)) continue;
        void* item = g_equipmentNew(tool.second);
        if (!item) continue;
        g_inventoryEquip(static_cast<std::byte*>(human) + kHumanServerInventoryOffset,
            item, tool.first, human);
        TraceTraversal("universal_tool_grant", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)), tool.first, type);
    }
}

int FindManualLockpickSlot(const void* human) {
    if (!human) return -1;
    const auto isManual = [&](int slot) {
        const void* item = At<void*>(human, kHumanServerInventoryOffset + slot * sizeof(void*));
        const void* itemTemplate = item ? At<void*>(item, 0x20) : nullptr;
        return itemTemplate && At<int>(itemTemplate, 0x20) == 6;
    };
    // Native inherent Lockpick normally occupies SupportGear2 (15).
    // PICK_LOCK also supports HouseKeys and mod tools in other bindings.
    // LOCKPICK_MACHINE (7) belongs to a separate radial action.
    if (isManual(15)) return 15;
    for (int slot = 0; slot < kInventorySlotCount; ++slot)
        if (slot != 15 && isManual(slot)) return slot;
    return -1;
}

bool IsOurLockpickCommand(const void* command, int slot, std::uint32_t doorId) {
    return At<int>(command, 0) == 12 && At<int>(command, 0x58) == slot &&
        (doorId == 0 || At<std::uint32_t>(command, 0x64) == doorId);
}

bool HasDirectLockpickCommand(const void* human, int slot = -1, std::uint32_t doorId = 0) {
    if (!human) return false;
    if (slot < 0) slot = FindManualLockpickSlot(human);
    if (slot < 0) return false;
    const int read = At<int>(human, kHumanServerCommandReadOffset);
    const int count = std::clamp(At<int>(human, kHumanServerCommandWriteOffset) - read, 0, 32);
    for (int i = 0; i < count; ++i) {
        const void* command = static_cast<const std::byte*>(human) + kHumanServerCommandArrayOffset +
            static_cast<std::size_t>((read + i) & 31) * kHumanServerCommandSize;
        if (IsOurLockpickCommand(command, slot, doorId)) return true;
    }
    return false;
}

void CancelDoorInteraction(void* human, DoorInteraction& state) {
    if (state.lockpickActive && human && g_removeHumanCommand) {
        int i = 0;
        while (i < std::clamp(At<int>(human, kHumanServerCommandWriteOffset) -
                            At<int>(human, kHumanServerCommandReadOffset), 0, 32)) {
            const int read = At<int>(human, kHumanServerCommandReadOffset);
            const void* command = static_cast<const std::byte*>(human) + kHumanServerCommandArrayOffset +
                static_cast<std::size_t>((read + i) & 31) * kHumanServerCommandSize;
            if (IsOurLockpickCommand(command, state.lockpickSlot, state.target)) {
                g_removeHumanCommand(static_cast<std::byte*>(human) + kHumanServerCommandQueueOffset, i);
            } else ++i;
        }
        TraceTraversal("lockpick_cancel", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)), 0, 0);
    }
    state = {};
}

bool ProcessDoorInteraction(void* human, DoorInteraction& state, bool held,
                            Vector2 facing, bool moving, bool cancel, ULONGLONG now) {
    if (!human) return false;
    if (cancel || (moving && state.lockpickActive)) {
        CancelDoorInteraction(human, state);
        state.down = held;
        state.consumed = held;
        return false;
    }
    if (state.lockpickActive) {
        if (HasDirectLockpickCommand(human, state.lockpickSlot, state.target)) return true;
        state.lockpickActive = false;
        TraceTraversal("lockpick_end", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)), 0, 0);
    }
    const float length = std::hypot(facing.x, facing.z);
    if (std::isfinite(length) && length > 0.001F) {
        facing.x /= length; facing.z /= length;
    } else facing = {};
    const std::uint32_t target = FindNearbyDoor(human, facing, true);
    const DoorGestureResult gesture = AdvanceDoorGesture(state, held, target, now);
    if (gesture.action == DoorGestureAction::None) return false;
    const auto rejectLockpick = [&](int reason, int detail = 0) {
        TraceTraversal("lockpick_reject", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)), reason, detail);
        if (human == g_controlledServerHuman) SetCrosshairFeedback(kGrenadeUnavailableColor);
        return false;
    };
    if (gesture.target == 0 || target != gesture.target) {
        if (gesture.action == DoorGestureAction::Lockpick) return rejectLockpick(1);
        return false;
    }
    void* door = FindDoorById(gesture.target);
    const int queued = At<int>(human, kHumanServerCommandWriteOffset) - At<int>(human, kHumanServerCommandReadOffset);
    if (!door || At<int>(door, kDoorServerStateOffset) != 1 || queued < 0 || queued > 24) {
        if (gesture.action == DoorGestureAction::Lockpick) return rejectLockpick(2, queued);
        return false;
    }
    const bool locked = At<bool>(door, kDoorServerLockedOffset);
    if (gesture.action == DoorGestureAction::Lockpick) {
        const int slot = FindManualLockpickSlot(human);
        // EquipmentType::LOCKPICK is 6; dispatch the same native breach queue
        // as ACTION_PICK_LOCK. Its command handles time, sound, and restoration.
        if (moving) return rejectLockpick(3);
        if (!locked) return rejectLockpick(4);
        if (slot < 0) return rejectLockpick(5);
        if (!g_queueDoorBreach) return rejectLockpick(6);
        StopDirectSprint(human);
        if (human == g_controlledServerHuman && g_controlledClientHuman) {
            // Clear the hidden path before queuing the action: sending a path
            // replacement afterwards can discard freshly queued native work.
            StopNativeGrenadePreview();
            StopHuman(g_controlledClientHuman);
        }
        state.lockpickSlot = slot;
        g_queueDoorBreach(human, slot, door);
        state.lockpickActive = HasDirectLockpickCommand(human, slot, gesture.target);
        TraceTraversal("lockpick_tool", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)), slot, 6);
        TraceTraversal("lockpick_start", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)),
                       static_cast<int>(gesture.target), state.lockpickActive ? 1 : 0);
        if (!state.lockpickActive) return rejectLockpick(7, slot);
        if (human == g_controlledServerHuman) SetCrosshairFeedback(kCrosshairPendingColor);
        return true;
    }
    const bool kick = gesture.action == DoorGestureAction::Kick;
    if (g_queueDoorOpen && (kick || !locked)) {
        StopDirectSprint(human);
        g_queueDoorOpen(human, door, kick);
        TraceTraversal("door_gesture", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)),
                       static_cast<int>(gesture.target), kick ? 2 : 1);
    }
    return false;
}

std::uint32_t FindSpyCameraDoorAtCursor(const Vector3& cursor) {
    void* head = *reinterpret_cast<void**>(
        g_moduleBase + kRvaDoorServerTypeList);
    if (!head || !std::isfinite(cursor.x) || !std::isfinite(cursor.z)) {
        return 0;
    }
    std::uint32_t nearestId = 0;
    float nearestDistanceSquared =
        kSpyCameraCursorTargetRadius * kSpyCameraCursorTargetRadius;
    void* link = At<void*>(head, 0x08);
    for (int count = 0; link && link != head && count < 1024;
         ++count, link = At<void*>(link, 0x08)) {
        void* door = At<void*>(link, 0x18);
        if (!door ||
            static_cast<std::byte*>(door) + kDoorServerTypeLinkOffset != link ||
            At<int>(door, kDoorServerStateOffset) != 1) {
            continue;
        }
        const Vector3 position = At<Vector3>(door, kHumanOriginOffset);
        const float dx = position.x - cursor.x;
        const float dz = position.z - cursor.z;
        const float distanceSquared = dx * dx + dz * dz;
        if (!std::isfinite(distanceSquared) ||
            distanceSquared >= nearestDistanceSquared) {
            continue;
        }
        const std::uint32_t id = At<std::uint32_t>(
            door, kHumanIdOffset);
        if (id != 0 && id < kMaximumEntityId) {
            nearestId = id;
            nearestDistanceSquared = distanceSquared;
        }
    }
    return nearestId;
}

// A selected utility item owns the primary click. Only actions with a
// verified native queue command are dispatched here; other gear never falls
// through to firearm shooting while it is selected.
void UseSelectedDoorItem(void* serverHuman, const Vector3& cursor) {
    const int slot = g_selectedGearInventoryType;
    if (!serverHuman || slot < 0 || slot >= kInventorySlotCount) {
        return;
    }
    void* item = At<void*>(serverHuman,
        kHumanServerInventoryOffset + static_cast<std::size_t>(slot) * sizeof(void*));
    const void* itemTemplate = item ? At<void*>(item, 0x20) : nullptr;
    const char* name = itemTemplate ? At<const char*>(itemTemplate, 0x28) : nullptr;
    if (!name) {
        return;
    }
    const bool spyCamera = std::strcmp(name, "Spy Camera") == 0;
    if (spyCamera) {
        static ULONGLONG lastSpyCameraClickTick = 0;
        const ULONGLONG now = GetTickCount64();
        if (lastSpyCameraClickTick != 0 &&
            now - lastSpyCameraClickTick < 250) {
            return;
        }
        lastSpyCameraClickTick = now;
        TraceTraversal("spycam_click", slot,
            g_cursorWorldValid ? 1 : 0, 0);
    }
    const bool doorBreach = std::strcmp(name, "Crowbar") == 0 ||
        std::strcmp(name, "Dynamic Hammer") == 0 ||
        std::strcmp(name, "FeetOfSteel") == 0 ||
        std::strcmp(name, "FeetOfSteelTawr") == 0 ||
        std::strcmp(name, "Breacher Shotgun") == 0 ||
        std::strcmp(name, "Bolt Cutter") == 0;
    if (!spyCamera && !doorBreach) {
        TraceTraversal("item_unsupported", slot, 0, 0);
        return;
    }
    const Vector3 origin = At<Vector3>(serverHuman, kHumanOriginOffset);
    const Vector2 facing{cursor.x - origin.x, cursor.z - origin.z};
    const std::uint32_t cursorDoorId = spyCamera
        ? FindSpyCameraDoorAtCursor(cursor) : 0;
    const std::uint32_t doorId = cursorDoorId != 0
        ? cursorDoorId : FindNearbyDoor(serverHuman, facing, true);
    void* door = FindDoorById(doorId);
    if (!door) {
        TraceTraversal("item_no_door", slot, spyCamera ? 26 : 27, 0);
        SetCrosshairFeedback(kGrenadeUnavailableColor);
        return;
    }
    if (spyCamera) {
        TraceTraversal("spycam_door_found", slot,
            static_cast<int>(doorId), At<int>(itemTemplate, 0x20));
        // The item name above already identifies Spy Camera. Its template
        // category is not the action-waypoint type (26), and rejecting it
        // here silently discarded valid clicks found in the mission trace.
        // The detour is optional. Direct calls still use the verified native
        // entry point when that detour was not installed.
        const auto requestCreation = g_originalActionWPRequestCreation
            ? g_originalActionWPRequestCreation
            : reinterpret_cast<ActionWPRequestCreationFn>(g_moduleBase + kRvaActionWPRequestCreation);
        void* gameClient = *reinterpret_cast<void**>(
            g_moduleBase + kRvaGameClientGlobal);
        void* potential = *reinterpret_cast<void**>(
            g_moduleBase + kRvaPotentialActionWaypointGlobal);
        void* clientDoor = gameClient
            ? At<void*>(gameClient, kGameEntityArrayOffset +
                static_cast<std::size_t>(doorId) * sizeof(void*))
            : nullptr;
        if (!potential || !clientDoor ||
            At<std::uint32_t>(clientDoor, kHumanIdOffset) != doorId) {
            TraceTraversal("spycam_waypoint_missing", slot,
                static_cast<int>(doorId), 0);
            SetCrosshairFeedback(kGrenadeUnavailableColor);
            return;
        }
        using CanUseSpyCamFn = bool(__fastcall*)(
            const void*, const Vector3*, Vector3*, Vector3*);
        const auto canUseSpyCam = reinterpret_cast<CanUseSpyCamFn>(
            g_moduleBase + 0x0009B930);
        Vector3 cameraDirection{};
        Vector3 cameraOrigin{};
        // The native radial action chooses the approach side from the
        // operator's position. The click itself may sit on the door plane.
        if (!canUseSpyCam(clientDoor, &origin,
                &cameraDirection, &cameraOrigin) ||
            !std::isfinite(cameraOrigin.x) ||
            !std::isfinite(cameraOrigin.z)) {
            TraceTraversal("spycam_geometry_rejected", slot,
                static_cast<int>(doorId), 0);
            SetCrosshairFeedback(kGrenadeUnavailableColor);
            return;
        }
        const float doorDx = cameraOrigin.x - origin.x;
        const float doorDz = cameraOrigin.z - origin.z;
        if (doorDx * doorDx + doorDz * doorDz > 1.2F * 1.2F) {
            TraceTraversal("spycam_too_far", slot, static_cast<int>(doorId), 0);
            return;
        }
        // Request the same native action waypoint that the game's Spy Camera
        // menu uses. The camera command alone lacks the waypoint entity that
        // ProcessCmdSpyCamera records as the active camera view source.
        StopNativeGrenadePreview();
        StopHuman(g_controlledClientHuman);
        g_wasMoving = false;
        g_accumulatedMs = 0;
        std::array<std::byte, 0x58> savedSpyAction{};
        std::memcpy(savedSpyAction.data(), static_cast<std::byte*>(potential) + 0x1F0, savedSpyAction.size());
        At<int>(potential, 0x1F0) = 26;
        At<std::uint32_t>(potential, 0x1F4) = doorId;
        At<Vector3>(potential, 0x1F8) = cameraDirection;
        // Native OnChangeAction writes camera origin here; +0x22c is the
        // operator's waypoint position, not the camera position.
        At<Vector3>(potential, 0x204) = cameraOrigin;
        At<Vector3>(potential, 0x210) = {};
        At<int>(potential, 0x21C) = slot;
        At<std::uint32_t>(potential, 0x220) =
            At<std::uint32_t>(serverHuman, kHumanIdOffset);
        At<int>(potential, 0x224) = 0;
        At<int>(potential, 0x228) = 0;
        At<Vector3>(potential, 0x22C) = origin;
        At<int>(potential, 0x238) = 0;
        At<int>(potential, 0x23C) = 0;
        At<int>(potential, 0x240) = 500;
        requestCreation(potential);
        // RequestCreation serializes synchronously. The reusable potential
        // object must not retain spycam fields and block grenade preview.
        std::memcpy(static_cast<std::byte*>(potential) + 0x1F0, savedSpyAction.data(), savedSpyAction.size());
        g_spyCameraActionActive = true;
        g_spyCameraRequestedAt = GetTickCount64();
        TraceTraversal("spycam_waypoint_requested", slot,
            static_cast<int>(doorId), 26);
    } else if (At<bool>(door, kDoorServerLockedOffset) && g_queueDoorBreach) {
        g_queueDoorBreach(serverHuman, slot, door);
    } else if (g_queueDoorOpen) {
        g_queueDoorOpen(serverHuman, door, false);
    }
    TraceTraversal("item_click_queued", slot, spyCamera ? 26 : 27,
        static_cast<int>(doorId));
    SetCrosshairFeedback(kCrosshairPendingColor);
}

Vector2 ClampMovementToPathfinder(
    const void* pathfinder,
    Vector2 start,
    Vector2 proposed) {
    if (PathfinderStepIsClear(pathfinder, start, proposed)) {
        return proposed;
    }

    const Vector2 xOnly{proposed.x, start.z};
    const Vector2 zOnly{start.x, proposed.z};
    const bool xClear = PathfinderStepIsClear(pathfinder, start, xOnly);
    const bool zClear = PathfinderStepIsClear(pathfinder, start, zOnly);

    if (xClear && !zClear) {
        return xOnly;
    }
    if (zClear && !xClear) {
        return zOnly;
    }
    if (xClear && zClear) {
        const float xDistance = std::abs(xOnly.x - start.x);
        const float zDistance = std::abs(zOnly.z - start.z);
        return xDistance >= zDistance ? xOnly : zOnly;
    }
    return start;
}

void UpdateMouseAim(const void* clientHuman, void* serverHuman) {
    g_cursorWorldValid = false;
    void* gameClient = *reinterpret_cast<void**>(
        g_moduleBase + kRvaGameClientGlobal);
    if (!gameClient || !clientHuman) {
        return;
    }

    const Vector2 cursor = *reinterpret_cast<const Vector2*>(
        g_moduleBase + kRvaPointerCursor);
    if (!std::isfinite(cursor.x) || !std::isfinite(cursor.z)) {
        return;
    }

    Vector3 cursorWorld{};
    g_convertScreenToMapCoords(
        gameClient, &cursorWorld, cursor.x, cursor.z);
    if (!std::isfinite(cursorWorld.x) || !std::isfinite(cursorWorld.y) ||
        !std::isfinite(cursorWorld.z)) {
        return;
    }
    g_cursorWorld = cursorWorld;
    g_cursorWorldValid = true;
    const Vector3 origin = At<Vector3>(clientHuman, kHumanOriginOffset);
    Vector3 aimDirection{
        cursorWorld.x - origin.x,
        0.0F,
        cursorWorld.z - origin.z,
    };
    const float lengthSquared = aimDirection.x * aimDirection.x +
        aimDirection.z * aimDirection.z;
    if (!std::isfinite(lengthSquared) || lengthSquared < 0.0001F) {
        return;
    }

    if (serverHuman) {
        LookAimCommand command(aimDirection);
        g_processCmdLookAim(serverHuman, &command);
    }
}

void SendRemoteInput(const void* human, Vector2 movement,
                     bool fireDown, bool firePressed,
                     bool sprintDown, bool crouchDown,
                     bool interactDown,
                     bool reloadPressed, bool grenadeRequested,
                     bool grenadeLongThrow, bool grenadeNativePreview,
                     int equipmentRequest, int readyHold) {
    if (!human || !g_cursorWorldValid || !g_netAddReliableEvent ||
        !g_bitPackerWriteBits) {
        return;
    }
    const ULONGLONG now = GetTickCount64();
    const bool movementChanged =
        std::abs(movement.x - g_lastRemoteMovementSent.x) > 0.01F ||
        std::abs(movement.z - g_lastRemoteMovementSent.z) > 0.01F;
    const Vector3 origin = At<Vector3>(human, kHumanOriginOffset);
    const float dx = g_cursorWorld.x - origin.x;
    const float dz = g_cursorWorld.z - origin.z;
    const float distance = std::hypot(dx, dz);
    if (!std::isfinite(distance) || distance < 0.01F) {
        return;
    }
    const Vector2 aim{dx / distance, dz / distance};
    const bool aimChanged =
        aim.x * g_lastRemoteAimSent.x +
        aim.z * g_lastRemoteAimSent.z < 0.999F;
    const bool urgent = g_displayFireMode != g_lastRemoteModeSent ||
        GrenadeChoiceFor(human).slot != g_lastRemoteGrenadeChoiceSent.slot ||
        GrenadeChoiceFor(human).itemId != g_lastRemoteGrenadeChoiceSent.itemId ||
        g_displayFireWeapon != g_lastRemoteWeaponSent || firePressed ||
        fireDown != g_lastRemoteFireSent || movementChanged ||
        sprintDown != g_lastRemoteSprintSent ||
        readyHold != g_lastRemoteReadySent ||
        interactDown != g_lastRemoteInteractSent ||
        crouchDown != g_lastRemoteCrouchSent ||
        reloadPressed || grenadeRequested ||
        equipmentRequest != kNoEquipmentRequest;
    const bool active = fireDown || movement.x != 0.0F ||
        movement.z != 0.0F || sprintDown || interactDown ||
        readyHold != kReadyHoldNone || aimChanged;
    if (!urgent && now - g_lastRemoteInputSendTick <
        (active ? g_remoteInputIntervalMs : 200U)) {
        return;
    }
    const auto quantize = [](float value) {
        return static_cast<int>(std::lround(
            std::clamp(value, -1.0F, 1.0F) * 32767.0F));
    };
    const auto quantizeOffset = [](float value) {
        return static_cast<int>(std::lround(
            std::clamp(value, -327.67F, 327.67F) * 100.0F)) & 0xFFFF;
    };
    void* packet = g_netAddReliableEvent(
        const_cast<void*>(human), kDirectControlInputEvent,
        kNativeClientToServerMask);
    if (!packet) {
        return;
    }
    g_bitPackerWriteBits(packet, kDirectControlInputVersion, 8);
    g_bitPackerWriteBits(packet, quantize(aim.x) & 0xFFFF, 16);
    g_bitPackerWriteBits(packet, quantize(aim.z) & 0xFFFF, 16);
    g_bitPackerWriteBits(packet, static_cast<int>(
        std::lround(std::min(distance, 655.35F) * 100.0F)), 16);
    g_bitPackerWriteBits(packet, quantize(movement.x) & 0xFFFF, 16);
    g_bitPackerWriteBits(packet, quantize(movement.z) & 0xFFFF, 16);
    g_bitPackerWriteBits(packet, (fireDown ? 1 : 0) |
        (firePressed ? 2 : 0) | (sprintDown ? 4 : 0) |
        (crouchDown ? 8 : 0) | (reloadPressed ? 16 : 0) |
        (grenadeRequested ? 32 : 0) |
        (grenadeLongThrow ? 64 : 0) |
        (grenadeNativePreview ? 128 : 0), 8);
    g_bitPackerWriteBits(packet,
        equipmentRequest == kHolsterEquipmentRequest ? 11 :
        equipmentRequest == kNoEquipmentRequest ? 0 : equipmentRequest,
        4);
    g_bitPackerWriteBits(packet, interactDown ? 1 : 0, 1);
    if (grenadeNativePreview) {
        const Vector3 base = At<Vector3>(human, kHumanOriginOffset);
        const Vector3 originOffset{
            g_grenadeNativeThrowOrigin.x - base.x,
            g_grenadeNativeThrowOrigin.y - base.y,
            g_grenadeNativeThrowOrigin.z - base.z};
        const Vector3 targetOffset{
            g_grenadeTarget.x - base.x,
            g_grenadeTarget.y - base.y,
            g_grenadeTarget.z - base.z};
        for (const float component : {originOffset.x, originOffset.y,
                                      originOffset.z, targetOffset.x,
                                      targetOffset.y, targetOffset.z}) {
            g_bitPackerWriteBits(packet, quantizeOffset(component), 16);
        }
        g_bitPackerWriteBits(packet, static_cast<int>(std::lround(
            std::clamp(g_grenadeNativeThrowAngle, 0.0F, 90.0F) * 100.0F)),
            16);
    }
    const GrenadeChoice grenadeChoice = GrenadeChoiceFor(human);
    g_bitPackerWriteBits(packet, grenadeChoice.slot + 1, 5);
    g_bitPackerWriteBits(packet, static_cast<int>(grenadeChoice.itemId), 32);
    g_lastRemoteGrenadeChoiceSent = grenadeChoice;
    g_bitPackerWriteBits(packet, readyHold, 2);
    g_bitPackerWriteBits(packet, static_cast<int>(g_displayFireMode), 2);
    g_bitPackerWriteBits(packet, static_cast<int>(g_displayFireWeapon), 32);
    g_lastRemoteModeSent = g_displayFireMode;
    g_lastRemoteWeaponSent = g_displayFireWeapon;
    g_lastRemoteInputSendTick = now;
    ++g_remoteInputSendCount;
    g_lastRemoteMovementSent = movement;
    g_lastRemoteAimSent = aim;
    g_lastRemoteFireSent = fireDown;
    g_lastRemoteReadySent = readyHold;
    g_lastRemoteSprintSent = sprintDown;
    g_lastRemoteCrouchSent = crouchDown;
    g_lastRemoteInteractSent = interactDown;
    if (firePressed) {
        TraceTraversal("coop_fire_sent", static_cast<int>(
            At<std::uint32_t>(human, kHumanIdOffset)), 0, 0);
    }
}

bool YieldToNativeWheelAction(void* serverHuman, std::uint32_t humanId) {
    if (!serverHuman || g_nativeWheelActionOwnerId == 0 ||
        g_nativeWheelActionOwnerId != humanId) {
        return false;
    }
    const ULONGLONG now = GetTickCount64();
    if (!DirectControlMovementKeyDown()) {
        g_nativeWheelActionCancelArmed = true;
    } else if (g_nativeWheelActionCancelArmed) {
        TraceTraversal("wheel_action_cancel", static_cast<int>(humanId),
            g_nativeWheelActionType, 0);
        g_nativeWheelActionOwnerId = 0;
        return false;
    }

    // OnPlayerRequestedWaypointCreation keeps native action waypoints in two
    // per-human lists. Give the request time to arrive, then leave direct
    // steering suspended until those waypoints finish or the user moves.
    const int queued = At<int>(serverHuman, 0x1538) +
        At<int>(serverHuman, 0x1550);
    if (queued > 0) {
        g_nativeWheelActionObserved = true;
        g_nativeWheelActionLastSeenAt = now;
        return true;
    }
    if ((!g_nativeWheelActionObserved &&
         now - g_nativeWheelActionRequestedAt < 1500) ||
        (g_nativeWheelActionObserved &&
         now - g_nativeWheelActionLastSeenAt < 400)) {
        return true;
    }
    TraceTraversal("wheel_action_end", static_cast<int>(humanId),
        g_nativeWheelActionType, g_nativeWheelActionObserved ? 1 : 0);
    g_nativeWheelActionOwnerId = 0;
    return false;
}

void UpdateDirectControl(int dt) {
    static bool highButtonWasDown = false;
    const bool highButtonDown = KeyDown(VK_XBUTTON2);
    const bool highButtonPressed = highButtonDown && !highButtonWasDown;
    highButtonWasDown = highButtonDown;
    static bool nvgWasDown = false;
    static bool laserWasDown = false;
    const bool nvgDown = KeyDown('N');
    const bool laserDown = KeyDown('L');
    const bool nvgPressed = nvgDown && !nvgWasDown;
    const bool laserPressed = laserDown && !laserWasDown;
    nvgWasDown = nvgDown;
    laserWasDown = laserDown;
    if ((nvgPressed || laserPressed) && ForegroundIsGame()) {
        TraceTraversal("nvg_laser_key", nvgPressed ? 1 : 2,
            g_enabled ? 1 : 0, IsChatBoxOpen() ? 1 : 0);
    }
    static bool f6WasDown = false;
    static bool f7WasDown = false;
    static bool tabWasDown = false;
    static bool tabHoldTriggered = false;
    static ULONGLONG tabPressedAt = 0;
    static bool reloadWasDown = false;
    static bool fireWasDown = false;
    static bool holsterWasDown = false;
    static bool grenadeWasDown = false;
    static bool sprintWasDown = false;
    static bool rightMouseWasDown = false;
    static std::array<bool, 10> equipmentWasDown{};

    const bool f6Down = KeyDown(VK_F6);
    const bool f7Down = KeyDown(VK_F7);
    const bool tabDown = KeyDown(VK_TAB);
    static bool modeWasDown = false;
    const bool modeDown = KeyDown('X');
    const bool modePressed = modeDown && !modeWasDown;
    modeWasDown = modeDown;
    const bool reloadDown = KeyDown('R');
    const bool reloadPressed = reloadDown && !reloadWasDown;
    const bool fireDown = KeyDown(VK_LBUTTON);
    const bool holsterDown = KeyDown('H');
    const bool holsterPressed = holsterDown && !holsterWasDown;
    const bool rawGrenadeDown = KeyDown('G');
    bool grenadeDown = rawGrenadeDown;
    const bool rightMouseDown = KeyDown(VK_RBUTTON);
    const bool contextPressed = rightMouseDown && !rightMouseWasDown;
    const bool interactDown = KeyDown('F');
    if (interactDown && !g_traversalFWasDown) {
        g_traversalRepathRequested = true;
        void* potential = *reinterpret_cast<void**>(
            g_moduleBase + kRvaPotentialActionWaypointGlobal);
        TraceTraversal(
            "press", potential ? At<int>(potential, 0x21C) : -1,
            g_wasMoving ? 1 : 0, g_traversalUsedThisPress ? 1 : 0);
    }
    g_traversalFWasDown = interactDown;
    const bool crouchDown = KeyDown(VK_LCONTROL);
    const bool sprintDown = KeyDown(VK_LSHIFT);
    const bool sprintPressed = sprintDown && !sprintWasDown;
    int equipmentPressed = kNoEquipmentRequest;
    for (int index = 0; index < 6; ++index) {
        const int virtualKey = index < 9 ? '1' + index : '0';
        const bool down = KeyDown(virtualKey);
        if (down && !equipmentWasDown[index]) {
            equipmentPressed = index + 1;
        }
        equipmentWasDown[index] = down;
    }
    // Polling in the proven GameInput::Update hook is the authoritative edge.
    // The Button1_Down hook remains a second source because it also suppresses
    // the vanilla map action while direct control owns the mouse.
    const bool firePressed = g_fireInputPressed ||
        (fireDown && !fireWasDown);
    reloadWasDown = reloadDown;
    fireWasDown = fireDown;
    holsterWasDown = holsterDown;
    rightMouseWasDown = rightMouseDown;
    sprintWasDown = sprintDown;
    g_fireInputPressed = false;
    // GameClient::UpdateGame does not set RCX before calling GameInput::Update;
    // the stock function reloads the singleton from g_pGameInput instead. The
    // formal member-function `this` value can therefore be arbitrary here.
    void* gameInput = *reinterpret_cast<void**>(g_moduleBase + kRvaGameInputGlobal);
    void* human = gameInput
        ? const_cast<void*>(g_getSelectedHuman(gameInput))
        : nullptr;

    const bool tabAvailable = ForegroundIsGame() && !IsGameMenuOpen() &&
        !KeyDown(VK_LSHIFT) && !KeyDown(VK_RSHIFT);
    bool tabToggleRequested = false;
    if (tabDown && !tabWasDown) {
        tabPressedAt = tabAvailable ? GetTickCount64() : 0;
        tabHoldTriggered = false;
    }
    if (tabDown && !tabAvailable) {
        tabPressedAt = 0;
    }
    if (tabDown && tabPressedAt != 0 && !tabHoldTriggered &&
        GetTickCount64() - tabPressedAt >= 2000) {
        tabHoldTriggered = true;
        tabToggleRequested = g_enabled || HumanCanBeControlled(human);
        TraceTraversal("tab_hold", tabToggleRequested ?
            (g_enabled ? 0 : 1) : -1, 0, 0);
    }
    if (!tabDown && tabWasDown) {
        if (tabPressedAt != 0 && !tabHoldTriggered && tabAvailable &&
            gameInput && g_cycleTrooperSelection) {
            if (g_enabled && human) {
                StopHuman(human);
                ClearDirectControlState();
            }
            g_cyclingSelectionWithTab = true;
            g_cycleTrooperSelection(gameInput, 1);
            g_cyclingSelectionWithTab = false;
            human = const_cast<void*>(g_getSelectedHuman(gameInput));
            TraceTraversal("tab_cycle", human
                ? static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset))
                : 0, g_enabled ? 1 : 0, 0);
        }
        tabPressedAt = 0;
        tabHoldTriggered = false;
    }
    tabWasDown = tabDown;

    if (tabToggleRequested ||
        (ForegroundIsGame() && f6Down && !f6WasDown)) {
        g_enabled = !g_enabled;
        if (!g_enabled && human) {
            StopHuman(human);
        }
        ClearDirectControlState();
        BeepForState(g_enabled);
    }
    if (ForegroundIsGame() && f7Down && !f7WasDown) {
        g_enabled = false;
        if (human) {
            StopHuman(human);
        }
        ClearDirectControlState();
        BeepForState(false);
    }
    f6WasDown = f6Down;
    f7WasDown = f7Down;

    if (g_enabled && g_activeHumanId != 0 &&
        (!human || !HumanCanBeControlled(human))) {
        g_enabled = false;
        ClearDirectControlState();
        BeepForState(false);
    }

    if (!g_enabled || !ForegroundIsGame() || !HumanCanBeControlled(human)) {
        g_highReadyPoses.erase(g_controlledClientHuman);
        RestoreGrenadeHeldPose(g_controlledClientHuman);
        CancelDoorInteraction(g_controlledServerHuman, g_doorInteraction);
        StopNativeGrenadePreview();
        if (g_crouchHeld && g_controlledServerHuman && g_queueCrouch) {
            g_queueCrouch(g_controlledServerHuman, false);
        }
        StopDirectSprint(g_controlledServerHuman);
        g_directMovementDirection = {};
        RestoreLeanPlaybackForHuman(g_controlledClientHuman);
        g_leanSide = 0;
        g_leanReachedHold = false;
        g_leanPhysicalSide = 0;
        g_leanPressedAt = 0;
        g_leanWasMoving = false;
        g_readyHold = kReadyHoldNone;
        g_controlledClientHuman = nullptr;
        g_controlledServerHuman = nullptr;
        g_reloadRequested = false;
        g_reloadSprintBlocked = false;
        g_cancelReloadRequested = false;
        g_reloadCanceledWeapon = nullptr;
        g_cursorWorldValid = false;
        g_shootRequested = false;
        g_shootRequestRemainingMs = 0;
        g_shootEquipQueued = false;
        g_fireHeld = false;
        g_autoFireActive = false;
        g_autoFireWeapon = nullptr;
        g_equipmentRequest = kNoEquipmentRequest;
        g_grenadeAiming = false;
        g_grenadeThrowRequested = false;
        g_grenadeThrowUsesNativePreview = false;
        g_grenadeHoldMs = 0;
        g_grenadeHoldStartTick = 0;
        g_crouchHeld = false;
        g_crouchCommandPending = false;
        g_sprintDesired = false;
        g_doorInteractTargetId = 0;
        g_lastInteractedDoorId = 0;
        g_traversalUsedThisPress = false;
        g_doorInteractKick = false;
        grenadeWasDown = grenadeDown;
        return;
    }

    const std::uint32_t humanId = At<std::uint32_t>(human, kHumanIdOffset);
    void* serverHuman = GetServerHuman(humanId);
    if (g_activeHumanId != 0 && humanId != g_activeHumanId) {
        g_highReadyPoses.erase(g_controlledClientHuman);
        RestoreGrenadeHeldPose(g_controlledClientHuman);
        CancelDoorInteraction(g_controlledServerHuman, g_doorInteraction);
        g_shootRequested = false;
        g_autoFireActive = false;
        g_autoFireWeapon = nullptr;
        g_fireSequence.requireRelease = fireDown;
        StopNativeGrenadePreview();
        if (g_crouchHeld && g_controlledServerHuman && g_queueCrouch) {
            g_queueCrouch(g_controlledServerHuman, false);
        }
        g_crouchHeld = false;
        g_crouchCommandPending = false;
        StopDirectSprint(g_controlledServerHuman);
        RestoreLeanPlaybackForHuman(g_controlledClientHuman);
        g_selectedGrenadeHotkey = 0;
        g_selectedCrowbarHotkey = 0;
        g_doorInteractTargetId = 0;
        g_lastInteractedDoorId = 0;
        g_traversalUsedThisPress = false;
        g_leanSide = 0;
        g_leanReachedHold = false;
        g_leanPhysicalSide = 0;
        g_leanPressedAt = 0;
        g_leanWasMoving = false;
    }
    if (serverHuman && serverHuman != g_controlledServerHuman) {
        g_crouchHeld = false;
        g_crouchCommandPending =
            (At<std::uint32_t>(serverHuman,
                kHumanServerMovementFlagsOffset) & 0x20U) != 0;
    }
    g_controlledClientHuman = human;
    g_controlledServerHuman = serverHuman;
    if (serverHuman) EnsureUniversalTools(serverHuman);
    const GrenadeChoice remembered = GrenadeChoiceFor(human);
    if (remembered.slot >= 1) {
        const void* rememberedItem = serverHuman ? At<void*>(serverHuman,
            kHumanServerInventoryOffset + static_cast<std::size_t>(remembered.slot) * sizeof(void*)) : nullptr;
        const bool exhausted = serverHuman
            ? (!InventoryItemIsGrenade(rememberedItem) || At<int>(rememberedItem, 0x3C) <= 0)
            : (At<int>(human, 0xD80) == remembered.slot &&
               At<int>(human, 0x43C + static_cast<std::size_t>(remembered.slot) * 0x58) <= 0);
        if (exhausted) {
            g_grenadeChoices.erase(humanId);
            g_selectedGrenadeHotkey = 0;
            TraceTraversal("grenade_choice_empty", static_cast<int>(humanId), remembered.slot, 0);
        }
    }
    if (IsGameMenuOpen()) {
        g_highReadyPoses.erase(human);
        SuspendDirectControlForMenu(human, serverHuman);
        grenadeWasDown = grenadeDown;
        return;
    }
    if (nvgPressed && !IsChatBoxOpen()) {
        void* renderer = At<void*>(reinterpret_cast<void*>(g_moduleBase + kRvaRendererGlobal), 0);
        if (ToggleNvgFilter(renderer)) {
            TraceTraversal("nvg_filter_toggle", At<bool>(renderer, kRendererNvgOffset) ? 1 : 0, 0, 0);
        }
    }
    if (laserPressed && !IsChatBoxOpen() && !ToggleWeaponLaser(human)) {
        TraceTraversal("laser_unavailable", static_cast<int>(humanId), At<int>(human, 0xD80), 0);
    }
    if (g_spyCameraActionActive && serverHuman) {
        const bool queued = HasQueuedSpyCamera(serverHuman);
        const int nativeWaypoints = At<int>(serverHuman, 0x1538);
        if (!queued && nativeWaypoints <= 0 &&
            g_spyCameraRequestedAt != 0 &&
            GetTickCount64() - g_spyCameraRequestedAt >= 1500) {
            g_spyCameraActionActive = false;
            g_spyCameraRequestedAt = 0;
            TraceTraversal("spycam_end", static_cast<int>(humanId), 0, 0);
        } else if (equipmentPressed != kNoEquipmentRequest ||
                   holsterPressed || DirectControlMovementKeyDown()) {
            CancelSpyCameraAction(serverHuman);
            StopHuman(human);
        } else {
            // Let the native camera command control the path and facing.
            g_directMovementDirection = {};
            g_sprintDesired = false;
            g_wasMoving = false;
            g_accumulatedMs = 0;
            g_shootRequested = false;
            g_fireHeld = false;
            g_readyHold = kReadyHoldNone;
            grenadeWasDown = grenadeDown;
            return;
        }
    }
    if (YieldToNativeWheelAction(serverHuman, humanId)) {
        g_directMovementDirection = {};
        g_sprintDesired = false;
        g_wasMoving = false;
        g_accumulatedMs = 0;
        // Keep the crosshair position current without overwriting the
        // native item's aim or rebuilding its action waypoint path.
        UpdateMouseAim(human, nullptr);
        g_readyHold = kReadyHoldNone;
        grenadeWasDown = grenadeDown;
        return;
    }
    if (g_menuSuspendedPath) {
        StopHuman(human);
        g_menuSuspendedPath = false;
    }
    // Rear thumb button holds low ready; forward thumb button holds high.
    const int previousReadyInput = g_readyHold;
    const void* fireTemplate = ClientFireTemplate(human);
    if (g_grenadeChoiceOwners[humanId] != human) {
        g_grenadeChoices.erase(humanId);
        g_grenadeChoiceOwners[humanId] = human;
    }
    if (g_fireClientOwners[humanId] != human) {
        g_fireSelections.erase(humanId);
        g_fireClientOwners[humanId] = human;
    }
    g_displayFireValid = fireTemplate && g_selectedGearInventoryType < 0;
    g_displayFireWeapon = g_displayFireValid ? FireWeaponId(fireTemplate) : 0;
    if (!fireDown) g_fireSequence.requireRelease = false;
    if (g_displayFireValid) {
        auto& mode = SelectedFireMode(At<std::uint32_t>(human, kHumanIdOffset), fireTemplate);
        if (modePressed) {
            const FireMode previousMode = mode;
            const int mask = FireCapabilities(fireTemplate).mask;
            for (int i = 1; i <= 3; ++i) {
                const int next = (static_cast<int>(mode) + i) % 3;
                if (mask & (1 << next)) { mode = static_cast<FireMode>(next); break; }
            }
            if (mode != previousMode) {
                g_shootRequested = false;
                g_autoFireActive = false;
                g_autoFireWeapon = nullptr;
                g_fireSequence.requireRelease = fireDown;
                TraceTraversal("fire_mode", static_cast<int>(g_displayFireWeapon), static_cast<int>(mode), 0);
            }
        }
        g_displayFireMode = mode;
    }
    g_readyHold = KeyDown(VK_XBUTTON1) ? kReadyHoldLow :
        KeyDown(VK_XBUTTON2) ? kReadyHoldHigh : kReadyHoldNone;
    if (highButtonPressed) {
        const void* set = At<void*>(human, 0x180);
        TraceTraversal("vbss_high_request", HighReadyWeaponEligible(human) ? 1 : 0,
            fireTemplate ? static_cast<int>(At<std::uint32_t>(fireTemplate, 0x68)) : 0,
            set ? static_cast<int>(At<std::uint32_t>(set, 8)) : 0);
    }
    if (g_readyHold == kReadyHoldHigh && (crouchDown || !HighReadyWeaponEligible(human)))
        g_readyHold = kReadyHoldNone;
    if (g_readyHold != previousReadyInput) {
        TraceTraversal("ready_input", previousReadyInput, g_readyHold,
            KeyDown(VK_SPACE) ? 1 : 0);
    }
    if (equipmentPressed != kNoEquipmentRequest || holsterPressed) {
        CancelPreparedGrenade(rawGrenadeDown || fireDown);
        grenadeWasDown = false;
        const int inventoryType = holsterPressed ? -1 :
            ResolveClientEquipmentNumber(human, equipmentPressed);
        const void* itemTemplate = inventoryType >= 0
            ? At<void*>(human,
                0x430 + static_cast<std::size_t>(inventoryType) * 0x58)
            : nullptr;
        if (itemTemplate && At<int>(itemTemplate, 0x20) == 3) {
            g_grenadeChoices[humanId] = {inventoryType, FireWeaponId(itemTemplate)};
            TraceTraversal("grenade_choice", static_cast<int>(humanId), inventoryType,
                           static_cast<int>(FireWeaponId(itemTemplate)));
            g_selectedGrenadeHotkey =
                g_selectedGrenadeHotkey == equipmentPressed
                    ? 0 : equipmentPressed;
            g_selectedSpyCameraHotkey = 0;
            g_selectedCrowbarHotkey = 0;
        } else {
            g_selectedGrenadeHotkey = 0;
            const char* name = itemTemplate
                ? At<const char*>(itemTemplate, 0x28) : nullptr;
            g_selectedSpyCameraHotkey = name &&
                std::strcmp(name, "Spy Camera") == 0 &&
                g_selectedSpyCameraHotkey != equipmentPressed
                    ? equipmentPressed : 0;
            g_selectedCrowbarHotkey = name &&
                std::strcmp(name, "Crowbar") == 0 &&
                g_selectedCrowbarHotkey != equipmentPressed
                    ? equipmentPressed : 0;
        }
    }
    if (!serverHuman && humanId != g_coopProbeSentHumanId &&
        g_netAddReliableEvent && g_bitPackerWriteBits) {
        // Probe only. No gameplay state is changed until the host's sender
        // mask and ownership path have been observed in a two-player session.
        void* packet = g_netAddReliableEvent(
            human, kDirectControlProbeEvent, kNativeClientToServerMask);
        if (packet) {
            g_bitPackerWriteBits(packet, kDirectControlProbeVersion, 8);
            g_coopProbeSentHumanId = humanId;
            TraceTraversal("coop_probe_sent", static_cast<int>(humanId),
                           kDirectControlProbeVersion, 0);
        }
    }
    UpdateMouseAim(human, serverHuman);
    Vector2 direction = ReadLeanIntent(ReadMovementDirection());
    g_directMovementDirection = direction;
    const bool movementRequested =
        direction.x != 0.0F || direction.z != 0.0F;
    if (serverHuman && g_crouchHeld != crouchDown) {
        g_crouchHeld = crouchDown;
        g_crouchCommandPending = true;
        TraceTraversal("crouch_hold", static_cast<int>(humanId),
            crouchDown ? 1 : 0, 0);
    }
    g_sprintDesired = serverHuman && sprintDown && !crouchDown &&
        movementRequested;
    TraceLocalState(human, humanId, direction, fireDown, sprintDown,
                    crouchDown);
    const ULONGLONG visualNow = GetTickCount64();
    if (visualNow - g_lastVisualTraceTick >= 5000) {
        TraceTraversal("coop_visual_local", static_cast<int>(humanId),
            g_cursorWorldValid ? 1 : 0,
            static_cast<int>(g_crosshairRenderCount));
        g_lastVisualTraceTick = visualNow;
        g_crosshairRenderCount = 0;
        if (!serverHuman) {
            TraceTraversal("coop_send_rate", static_cast<int>(humanId),
                static_cast<int>(g_remoteInputSendCount), 0);
            g_remoteInputSendCount = 0;
        }
    }
    if (serverHuman) {
        Vector2 facing = direction;
        if (g_cursorWorldValid) {
            const Vector3 origin = At<Vector3>(human, kHumanOriginOffset);
            facing = {g_cursorWorld.x - origin.x, g_cursorWorld.z - origin.z};
        }
        const bool picking = ProcessDoorInteraction(serverHuman, g_doorInteraction,
            interactDown, facing, movementRequested,
            equipmentPressed != kNoEquipmentRequest || holsterPressed || firePressed || reloadPressed,
            GetTickCount64());
        g_lastInteractedDoorId = g_doorInteraction.down || g_doorInteraction.pendingTap
            ? g_doorInteraction.target : 0;
        if (picking) {
            g_directMovementDirection = {};
            g_sprintDesired = false;
            g_wasMoving = false;
            g_accumulatedMs = 0;
            g_shootRequested = false;
            g_autoFireActive = false;
            g_fireHeld = false;
            g_readyHold = kReadyHoldNone;
            grenadeWasDown = grenadeDown;
            return;
        }
    }
    if (!interactDown) g_traversalUsedThisPress = false;
    void* potentialActionWaypoint = *reinterpret_cast<void**>(
        g_moduleBase + kRvaPotentialActionWaypointGlobal);
    if (contextPressed && g_cursorWorldValid &&
        potentialActionWaypoint && !IsGameMenuOpen()) {
        ButtonDownState state{};
        state.point = g_cursorWorld;
        state.pointOnPath = At<Vector3>(human, kHumanOriginOffset);
        state.entityId = humanId;
        g_openingDirectContextMenu = true;
        g_button2Click(gameInput, &state, &g_cursorWorld);
        g_openingDirectContextMenu = false;
        if (IsGameMenuOpen()) {
            SuspendDirectControlForMenu(human, serverHuman);
            grenadeWasDown = grenadeDown;
            return;
        }
    }
    UpdateMovementPath(
        human, humanId, direction, dt);
    if (serverHuman && reloadPressed) {
        g_reloadRequested = true;
        g_reloadSprintBlocked = true;
        g_reloadCanceledWeapon = nullptr;
    }
    if (sprintPressed && serverHuman &&
        (g_reloadSprintBlocked || FirearmIsReloading(serverHuman) ||
         HasQueuedReload(serverHuman))) {
        g_cancelReloadRequested = true;
    }
    if (serverHuman && equipmentPressed != kNoEquipmentRequest &&
        (g_reloadRequested || FirearmIsReloading(serverHuman) ||
         HasQueuedReload(serverHuman))) {
        const int inventoryType = ResolveEquipmentNumber(
            serverHuman, equipmentPressed);
        if (inventoryType >= 0) {
            void* requestedItem = At<void*>(serverHuman,
                kHumanServerInventoryOffset +
                    static_cast<std::size_t>(inventoryType) * sizeof(void*));
            if (requestedItem && requestedItem != At<void*>(
                    serverHuman, kHumanServerEquippedItemOffset)) {
                g_cancelReloadRequested = true;
            }
        }
    }
    if (serverHuman && !IsGameMenuOpen()) {
        if (holsterPressed) {
            g_equipmentRequest = kHolsterEquipmentRequest;
        } else if (equipmentPressed != kNoEquipmentRequest) {
            const int inventoryType = ResolveEquipmentNumber(
                serverHuman, equipmentPressed);
            void* requestedItem = inventoryType >= 0
                ? At<void*>(serverHuman, kHumanServerInventoryOffset +
                    static_cast<std::size_t>(inventoryType) * sizeof(void*))
                : nullptr;
            const bool sameReloadingFirearm = requestedItem &&
                requestedItem == At<void*>(serverHuman,
                    kHumanServerEquippedItemOffset) &&
                InventoryItemIsFirearm(requestedItem) &&
                (g_reloadRequested || FirearmIsReloading(serverHuman) ||
                 HasQueuedReload(serverHuman));
            if (!sameReloadingFirearm) {
                g_equipmentRequest = equipmentPressed;
            }
        }
    }
    const GrenadeChoice currentGrenadeChoice = GrenadeChoiceFor(human);
    const bool heldGrenadeMouse = g_selectedGrenadeHotkey != 0 &&
        At<int>(human, 0xD80) == currentGrenadeChoice.slot &&
        ClientGrenadeChoiceTemplate(human, currentGrenadeChoice);
    grenadeDown = rawGrenadeDown || (heldGrenadeMouse && fireDown);
    if (!rawGrenadeDown && !fireDown) g_grenadeRequireRelease = false;
    bool remoteGrenadeReleased = false;
    bool remoteGrenadeLongThrow = false;
    bool remoteGrenadeNativePreview = false;
    if (g_cursorWorldValid && !g_grenadeRequireRelease) {
        const int availableGrenadeSlot = serverHuman
            ? FindGrenadeInventoryType(serverHuman)
            : FindClientGrenadeInventoryType(human);
        if (grenadeDown) {
            if (g_grenadeHoldStartTick == 0) {
                g_grenadeHoldStartTick = GetTickCount64();
                g_grenadeAimChoice = currentGrenadeChoice;
            }
            g_grenadeHoldMs = static_cast<int>(std::min<ULONGLONG>(
                GetTickCount64() - g_grenadeHoldStartTick,
                10000));
            g_grenadeAiming =
                g_grenadeHoldMs >= kGrenadeHoldThresholdMs &&
                availableGrenadeSlot >= 0;
            g_grenadeTarget = g_cursorWorld;
            if (g_grenadeAiming) {
                UpdateNativeGrenadePreview(gameInput, human, availableGrenadeSlot, humanId);
            } else {
                StopNativeGrenadePreview();
            }
        } else if (GrenadeGestureCanRelease(grenadeWasDown)) {
            g_grenadeThrowUsesNativePreview = false;
            const int grenadeSlot = availableGrenadeSlot;
            if (g_grenadeAiming && g_nativeGrenadePreviewWaypoint &&
                grenadeSlot >= 0 &&
                At<int>(g_nativeGrenadePreviewWaypoint, 0x21C) ==
                    grenadeSlot) {
                // ActionGrenade passes these exact three sActionWP fields to
                // CmdQueueGrenadeThrow in the stock server path.
                const Vector3 nativeOrigin = At<Vector3>(
                    g_nativeGrenadePreviewWaypoint, 0x1F8);
                const Vector3 nativeTarget = At<Vector3>(
                    g_nativeGrenadePreviewWaypoint, 0x204);
                const float nativeAngle = At<float>(
                    g_nativeGrenadePreviewWaypoint, 0x210);
                const float dx = nativeTarget.x - nativeOrigin.x;
                const float dz = nativeTarget.z - nativeOrigin.z;
                if (std::isfinite(nativeOrigin.x) &&
                    std::isfinite(nativeOrigin.y) &&
                    std::isfinite(nativeOrigin.z) &&
                    std::isfinite(nativeTarget.x) &&
                    std::isfinite(nativeTarget.y) &&
                    std::isfinite(nativeTarget.z) &&
                    std::isfinite(nativeAngle) &&
                    nativeAngle > 0.0F && nativeAngle < 90.0F &&
                    std::isfinite(dx * dx + dz * dz) &&
                    dx * dx + dz * dz >= 0.25F) {
                    g_grenadeNativeThrowOrigin = nativeOrigin;
                    g_grenadeTarget = nativeTarget;
                    g_grenadeNativeThrowAngle = nativeAngle;
                    g_grenadeNativeThrowInventoryType = grenadeSlot;
                    g_grenadeThrowUsesNativePreview = true;
                }
            }
            StopNativeGrenadePreview();
            if (grenadeSlot < 0 || currentGrenadeChoice.slot != g_grenadeAimChoice.slot ||
                currentGrenadeChoice.itemId != g_grenadeAimChoice.itemId) {
                SetCrosshairFeedback(kGrenadeUnavailableColor);
            } else {
                if (!g_grenadeThrowUsesNativePreview) {
                    g_grenadeTarget = g_cursorWorld;
                }
                if (g_grenadeHoldMs < kGrenadeHoldThresholdMs &&
                    !g_grenadeThrowUsesNativePreview) {
                    const Vector3 origin = At<Vector3>(
                        human, kHumanOriginOffset);
                    const float dx = g_cursorWorld.x - origin.x;
                    const float dz = g_cursorWorld.z - origin.z;
                    const float length = std::sqrt(dx * dx + dz * dz);
                    if (std::isfinite(length) && length > 0.001F) {
                        g_grenadeTarget.x = origin.x +
                            dx / length * kGrenadeQuickThrowDistance;
                        g_grenadeTarget.z = origin.z +
                            dz / length * kGrenadeQuickThrowDistance;
                        g_grenadeTarget.y = origin.y;
                    }
                }
                if (serverHuman) {
                    g_grenadeThrowChoice = currentGrenadeChoice;
                    g_grenadeThrowRequested = true;
                } else {
                    remoteGrenadeReleased = true;
                    remoteGrenadeLongThrow =
                        g_grenadeHoldMs >= kGrenadeHoldThresholdMs;
                    remoteGrenadeNativePreview =
                        g_grenadeThrowUsesNativePreview;
                    g_selectedGrenadeHotkey = 0;
                }
            }
            g_grenadeAiming = false;
            g_grenadeHoldMs = 0;
            g_grenadeHoldStartTick = 0;
        }
    } else {
        StopNativeGrenadePreview();
        g_grenadeAiming = false;
        if (!grenadeDown) {
            g_grenadeHoldMs = 0;
            g_grenadeHoldStartTick = 0;
        }
    }
    grenadeWasDown = grenadeDown;
    if (heldGrenadeMouse || grenadeDown || g_grenadeThrowRequested || remoteGrenadeReleased)
        g_grenadeFireGuard = true;
    const bool suppressGrenadeFire = g_grenadeFireGuard;
    const bool highReadyReturning = HighReadyBlocksFire(human);
    if (!fireDown && !grenadeDown && ClientFireTemplate(human) &&
        At<int>(human, 0x368) == 42 && g_grenadePendingSlot < 0)
        g_grenadeFireGuard = false;
    if (!serverHuman) {
        SendRemoteInput(human, direction,
            fireDown && !suppressGrenadeFire && !highReadyReturning && g_readyHold == kReadyHoldNone,
            firePressed && !suppressGrenadeFire && !highReadyReturning && g_readyHold == kReadyHoldNone,
            sprintDown && !crouchDown, crouchDown, interactDown,
            reloadPressed,
            remoteGrenadeReleased, remoteGrenadeLongThrow,
            remoteGrenadeNativePreview,
            holsterPressed ? kHolsterEquipmentRequest : equipmentPressed,
            g_readyHold);
    }
    const bool reloadActive = serverHuman &&
        (g_reloadRequested || g_reloadSprintBlocked ||
         FirearmIsReloading(serverHuman) || HasQueuedReload(serverHuman));
    const bool utilitySelected = serverHuman &&
        g_selectedGearInventoryType >= 0;
    if (utilitySelected) {
        // A held item click must never become automatic weapon fire.
        g_shootRequested = false;
        g_shootRequestRemainingMs = 0;
        g_shootEquipQueued = false;
        g_autoFireActive = false;
        g_autoFireWeapon = nullptr;
        if (firePressed && g_spyCameraSelectedAt != 0) {
            TraceTraversal("spycam_click_gate", g_selectedGearInventoryType,
                g_cursorWorldValid ? 1 : 0, serverHuman ? 1 : 0);
        }
        if (firePressed && g_cursorWorldValid && !heldGrenadeMouse) {
            UseSelectedDoorItem(serverHuman, g_cursorWorld);
        }
    }
    g_fireHeld = serverHuman && !utilitySelected && !suppressGrenadeFire && fireDown && g_cursorWorldValid &&
        g_readyHold == kReadyHoldNone && !highReadyReturning &&
        !reloadActive;
    if (reloadActive || g_readyHold != kReadyHoldNone || highReadyReturning) {
        g_shootRequested = false;
        g_shootRequestRemainingMs = 0;
        g_shootEquipQueued = false;
        g_autoFireActive = false;
        g_autoFireWeapon = nullptr;
    }
    if (g_autoFireActive && g_fireSequence.mode == FireMode::Auto && !g_fireHeld) {
        g_shootRequested = false;
        g_shootRequestRemainingMs = 0;
        g_shootEquipQueued = false;
        g_autoFireActive = false;
        g_autoFireWeapon = nullptr;
    }
    if (serverHuman && !utilitySelected && !suppressGrenadeFire && !reloadActive &&
        g_readyHold == kReadyHoldNone && !highReadyReturning && g_cursorWorldValid &&
        ((firePressed && !g_fireSequence.requireRelease) || g_autoFireActive)) {
        const Vector3 origin = At<Vector3>(human, kHumanOriginOffset);
        const Vector3 shootDirection{
            g_cursorWorld.x - origin.x,
            0.0F,
            g_cursorWorld.z - origin.z,
        };
        const float lengthSquared = shootDirection.x * shootDirection.x +
            shootDirection.z * shootDirection.z;
        if (std::isfinite(lengthSquared) && lengthSquared >= 0.0001F) {
            g_shootDirection = shootDirection;
            g_shootTarget = g_cursorWorld;
            if (firePressed && !g_autoFireActive && !g_fireSequence.requireRelease) {
                g_shootRequested = true;
                g_shootRequestRemainingMs = kShootRequestLifetimeMs;
                g_shootEquipQueued = false;
                SetCrosshairFeedback(kCrosshairPendingColor);
            }
        }
    }
}

void __fastcall HookedGameInputUpdate(void* gameInput, int dt) {
    // CheckMouseZoom runs inside GameInput::Update and reads this native
    // option. Restore it immediately so the user's saved setting and normal
    // gameplay keep their original zoom speed.
    float& zoomSpeed = *reinterpret_cast<float*>(
        g_moduleBase + kRvaOptionsGameZoomSpeed);
    const float normalZoomSpeed = zoomSpeed;
    if (g_enabled && ForegroundIsGame() && !IsGameMenuOpen() &&
        std::isfinite(normalZoomSpeed) && normalZoomSpeed > 0.0F) {
        zoomSpeed = normalZoomSpeed * g_zoomMultiplier;
    }
    g_originalUpdate(gameInput, dt);
    zoomSpeed = normalZoomSpeed;
    UpdateDirectControl(dt);
}

void __fastcall HookedCheckKeyboardEvents(void* gameInput) {
    // TAB is handled on release for cycling or after a two-second hold for
    // the mode toggle. Keep the native key handler from also acting on it.
    if (ForegroundIsGame() && !IsGameMenuOpen() && KeyDown(VK_TAB) &&
        !KeyDown(VK_LSHIFT) && !KeyDown(VK_RSHIFT)) {
        return;
    }
    // Reserve direct-control movement, lean, ready, and equipment inputs so
    // native shortcuts (including a side-button pause binding) cannot run.
    if (g_enabled && ForegroundIsGame() && !IsGameMenuOpen() &&
        (DirectControlMovementKeyDown() || KeyDown('Q') || KeyDown('E') ||
         KeyDown(VK_XBUTTON1) || KeyDown(VK_XBUTTON2) ||
         KeyDown('R') || KeyDown('X') ||
         ((KeyDown('N') || KeyDown('L')) && !IsChatBoxOpen()) ||
         DirectControlEquipmentKeyDown())) {
        return;
    }
    g_originalCheckKeyboardEvents(gameInput);
}

void __fastcall HookedButton1Down(void* gameInput, const void* state) {
    if (DirectControlCapturesMouse()) {
        g_fireInputPressed = true;
        return;
    }
    g_originalButton1Down(gameInput, state);
}

void __fastcall HookedButton1Click(
    void* gameInput,
    const void* state,
    const Vector3* mapPoint) {
    if (DirectControlCapturesMouse()) {
        // Preserve click-only utility actions. Firearms use Down/polling so
        // the native release-click cannot initiate a second semi shot/burst.
        if (!ClientFireTemplate(g_controlledClientHuman) ||
            g_selectedGearInventoryType >= 0 || g_selectedSpyCameraHotkey != 0 ||
            g_selectedCrowbarHotkey != 0) {
            g_fireInputPressed = true;
        }
        return;
    }
    g_originalButton1Click(gameInput, state, mapPoint);
}

void __fastcall HookedButton1Drag(
    void* gameInput,
    const void* state,
    const Vector3* mapPoint) {
    if (DirectControlCapturesMouse()) {
        return;
    }
    g_originalButton1Drag(gameInput, state, mapPoint);
}

void __fastcall HookedButton2Down(void* gameInput, const void* state) {
    if (DirectControlOwnsGameplayMouse()) {
        return;
    }
    g_originalButton2Down(gameInput, state);
}

void __fastcall HookedButton2Click(
    void* gameInput,
    const ButtonDownState* state,
    const Vector3* mapPoint) {
    if (DirectControlOwnsGameplayMouse()) {
        return;
    }
    g_originalButton2Click(gameInput, state, mapPoint);
}

void __fastcall HookedButton2Drag(
    void* gameInput,
    const ButtonDownState* state,
    const Vector3* mapPoint) {
    if (DirectControlOwnsGameplayMouse()) {
        return;
    }
    g_originalButton2Drag(gameInput, state, mapPoint);
}

void __fastcall HookedChangeSelection(void* gameInput, void* human) {
    if (DirectControlOwnsGameplayMouse() && !g_cyclingSelectionWithTab) {
        return;
    }
    g_originalChangeSelection(gameInput, human);
}

void __fastcall HookedOpenWaypointMenu(
    void* gameGui, void* waypoint, int option) {
    if (DirectControlOwnsGameplayMouse() && !g_openingDirectContextMenu) {
        return;
    }
    g_originalOpenWaypointMenu(gameGui, waypoint, option);
}

void __fastcall HookedActionWPRequestCreation(void* waypoint) {
    if (g_enabled && waypoint && g_controlledServerHuman &&
        waypoint == *reinterpret_cast<void**>(
            g_moduleBase + kRvaPotentialActionWaypointGlobal)) {
        const std::uint32_t ownerId = At<std::uint32_t>(waypoint, 0x220);
        const int actionType = At<int>(waypoint, 0x1F0);
        if (ownerId == At<std::uint32_t>(
                           g_controlledServerHuman, kHumanIdOffset) &&
            actionType >= 20 && actionType < 50) {
            g_nativeWheelActionOwnerId = ownerId;
            g_nativeWheelActionType = actionType;
            g_nativeWheelActionRequestedAt = GetTickCount64();
            g_nativeWheelActionLastSeenAt = 0;
            g_nativeWheelActionObserved = false;
            g_nativeWheelActionCancelArmed = false;
            TraceTraversal("wheel_action_request", static_cast<int>(ownerId),
                actionType, At<int>(waypoint, 0x238));
        }
    }
    g_originalActionWPRequestCreation(waypoint);
}

void __fastcall HookedGameClientUpdateCamera(void* gameClient, int dt) {
    g_originalUpdateCamera(gameClient, dt);

    if (g_enabled && ForegroundIsGame() && g_controlledClientHuman &&
        !IsGameMenuOpen()) {
        // Recenter after the stock camera update so ordinary pan and focus
        // requests cannot pull the controlled operator off screen. Keep the
        // existing height and projection; DK2 applies its normal map bounds
        // and refreshes the view matrix in Camera::Update.
        auto* camera = static_cast<std::byte*>(gameClient) +
            kGameClientCameraOffset;
        const Vector3 humanPosition = At<Vector3>(
            g_controlledClientHuman, kHumanOriginOffset);
        if (std::isfinite(humanPosition.x) &&
            std::isfinite(humanPosition.z)) {
            Vector2 desiredLookAhead{};
            if (g_cursorWorldValid) {
                // Use a screen-relative map displacement. A fixed world aim
                // point moves when the camera moves, creating feedback in the
                // look-ahead target even while the mouse stays still.
                const Vector2 cursor = *reinterpret_cast<const Vector2*>(
                    g_moduleBase + kRvaPointerCursor);
                const auto* viewport = static_cast<const std::byte*>(
                    gameClient) + kGameClientViewportOffset;
                const int viewportX = *reinterpret_cast<const int*>(viewport);
                const int viewportY = *reinterpret_cast<const int*>(viewport + 4);
                const int viewportWidth = *reinterpret_cast<const int*>(viewport + 8);
                const int viewportHeight = *reinterpret_cast<const int*>(viewport + 12);
                if (std::isfinite(cursor.x) && std::isfinite(cursor.z) &&
                    viewportWidth > 0 && viewportHeight > 0) {
                    Vector3 cursorPoint{};
                    Vector3 centerPoint{};
                    g_convertScreenToMapCoords(
                        gameClient, &cursorPoint, cursor.x, cursor.z);
                    g_convertScreenToMapCoords(
                        gameClient, &centerPoint,
                        static_cast<float>(viewportX) +
                            static_cast<float>(viewportWidth) * 0.5F,
                        static_cast<float>(viewportY) +
                            static_cast<float>(viewportHeight) * 0.5F);
                    if (std::isfinite(cursorPoint.x) &&
                        std::isfinite(cursorPoint.z) &&
                        std::isfinite(centerPoint.x) &&
                        std::isfinite(centerPoint.z)) {
                        desiredLookAhead.x =
                            (cursorPoint.x - centerPoint.x) *
                            kCameraLookAheadHorizontalFraction;
                        desiredLookAhead.z =
                            (cursorPoint.z - centerPoint.z) *
                            kCameraLookAheadVerticalFraction;
                    }
                }
                if (std::isfinite(desiredLookAhead.x) &&
                    std::isfinite(desiredLookAhead.z)) {
                    desiredLookAhead.x = std::clamp(
                        desiredLookAhead.x,
                        -kCameraLookAheadHorizontalMax,
                        kCameraLookAheadHorizontalMax);
                    desiredLookAhead.z = std::clamp(
                        desiredLookAhead.z,
                        -kCameraLookAheadVerticalMax,
                        kCameraLookAheadVerticalMax);
                } else {
                    desiredLookAhead = {};
                }
            }
            const std::uint32_t humanId = At<std::uint32_t>(
                g_controlledClientHuman, kHumanIdOffset);
            if (g_cameraFollowHumanId != humanId) {
                g_cameraLookAhead = desiredLookAhead;
                g_cameraFollowHumanId = humanId;
            } else {
                const float elapsedMs = static_cast<float>(
                    std::clamp(dt, 0, 100));
                const float blend = 1.0F - std::exp(
                    -elapsedMs / kCameraLookAheadSmoothingMs);
                g_cameraLookAhead.x +=
                    (desiredLookAhead.x - g_cameraLookAhead.x) * blend;
                g_cameraLookAhead.z +=
                    (desiredLookAhead.z - g_cameraLookAhead.z) * blend;
            }
            Vector3& cameraPosition = At<Vector3>(
                camera, kCameraPositionOffset);
            cameraPosition.x = humanPosition.x + g_cameraLookAhead.x;
            cameraPosition.z = humanPosition.z + g_cameraLookAhead.z;
            At<Vector3>(camera, 0) = {};
            At<int>(camera, kCameraMoveTargetCountOffset) = 0;
            g_cameraUpdate(camera, 0);
        }
        // Match DK2's own captured-camera behavior. Its normal camera update
        // first normalizes the cursor visible, so normalize it hidden here at
        // the end of that update rather than accumulating ShowCursor calls.
        while (ShowCursor(FALSE) >= 0) {
        }
    }
}

void __fastcall HookedHumanServerNetReceiveEvent(
    void* serverHuman,
    int senderMask,
    int eventType,
    unsigned int simulationTime,
    void* packet) {
    if (eventType == kDirectControlInputEvent) {
        if (!serverHuman || !packet || !g_bitPackerReadBits ||
            senderMask == 0 || (senderMask & (senderMask - 1)) != 0 ||
            (At<std::uint32_t>(serverHuman, 0x13C) & senderMask) == 0 ||
            At<float>(serverHuman, kHumanHealthOffset) <= 0.0F ||
            !At<bool>(serverHuman, 0x1520)) {
            return;
        }
        const int version = g_bitPackerReadBits(packet, 8);
        if (version != kDirectControlInputVersion) {
            return;
        }
        const auto signed16 = [](int bits) {
            bits &= 0xFFFF;
            return bits >= 0x8000 ? bits - 0x10000 : bits;
        };
        const float x = signed16(g_bitPackerReadBits(packet, 16)) /
            32767.0F;
        const float z = signed16(g_bitPackerReadBits(packet, 16)) /
            32767.0F;
        const float distance = g_bitPackerReadBits(packet, 16) * 0.01F;
        const float moveX = signed16(g_bitPackerReadBits(packet, 16)) /
            32767.0F;
        const float moveZ = signed16(g_bitPackerReadBits(packet, 16)) /
            32767.0F;
        const int flags = g_bitPackerReadBits(packet, 8);
        const int equipmentCode = g_bitPackerReadBits(packet, 4);
        const bool interactHeld = g_bitPackerReadBits(packet, 1) != 0;
        Vector3 grenadeOriginOffset{};
        Vector3 grenadeTargetOffset{};
        float grenadeAngle = 0.0F;
        if ((flags & 128) != 0) {
            grenadeOriginOffset = {
                signed16(g_bitPackerReadBits(packet, 16)) * 0.01F,
                signed16(g_bitPackerReadBits(packet, 16)) * 0.01F,
                signed16(g_bitPackerReadBits(packet, 16)) * 0.01F};
            grenadeTargetOffset = {
                signed16(g_bitPackerReadBits(packet, 16)) * 0.01F,
                signed16(g_bitPackerReadBits(packet, 16)) * 0.01F,
                signed16(g_bitPackerReadBits(packet, 16)) * 0.01F};
            grenadeAngle = g_bitPackerReadBits(packet, 16) * 0.01F;
        }
        const int grenadeChoiceCode = g_bitPackerReadBits(packet, 5);
        const std::uint32_t grenadeChoiceId = static_cast<std::uint32_t>(g_bitPackerReadBits(packet, 32));
        if (grenadeChoiceCode == 1 || grenadeChoiceCode > kInventorySlotCount ||
            ((grenadeChoiceCode == 0) != (grenadeChoiceId == 0))) return;
        const GrenadeChoice grenadeChoice{grenadeChoiceCode - 1, grenadeChoiceId};
        const int readyHold = g_bitPackerReadBits(packet, 2);
        const int requestedMode = g_bitPackerReadBits(packet, 2);
        const auto requestedWeapon = static_cast<std::uint32_t>(g_bitPackerReadBits(packet, 32));
        const float magnitude = std::hypot(x, z);
        if (!std::isfinite(magnitude) || magnitude < 0.8F ||
            magnitude > 1.2F || distance < 0.01F || distance > 655.35F ||
            !std::isfinite(moveX) || !std::isfinite(moveZ) ||
            std::hypot(moveX, moveZ) > 1.01F || (equipmentCode > 6 && equipmentCode != 11) ||
            readyHold > kReadyHoldLow || requestedMode > 2 ||
            ((flags & 128) != 0 &&
             ((flags & 32) == 0 ||
              std::hypot(grenadeOriginOffset.x,
                         grenadeOriginOffset.z) > 3.0F ||
              grenadeOriginOffset.y < -1.0F ||
              grenadeOriginOffset.y > 3.0F ||
              std::hypot(grenadeTargetOffset.x,
                         grenadeTargetOffset.z) > 40.0F ||
              std::abs(grenadeTargetOffset.y) > 10.0F ||
              grenadeAngle <= 0.0F || grenadeAngle >= 90.0F))) {
            return;
        }
        const std::uint32_t humanId = At<std::uint32_t>(
            serverHuman, kHumanIdOffset);
        RemoteInput& input = g_remoteInputs[humanId];
        if (input.human != serverHuman || input.senderMask != senderMask) {
            g_fireSelections.erase(humanId);
            input = {};
            input.human = serverHuman;
            input.humanId = humanId;
            input.senderMask = senderMask;
            input.crouchCommandPending =
                (At<std::uint32_t>(serverHuman,
                    kHumanServerMovementFlagsOffset) & 0x20U) != 0;
            TraceTraversal("coop_input_start", static_cast<int>(humanId),
                           senderMask, 0);
        }
        input.aim = {x / magnitude, 0.0F, z / magnitude};
        input.movement = {moveX, moveZ};
        input.distance = distance;
        input.grenadeChoice = grenadeChoice;
        const ULONGLONG receivedNow = GetTickCount64();
        if (input.lastReceived != 0 && receivedNow - input.lastReceived > 500) {
            CancelDoorInteraction(serverHuman, input.doorInteraction);
            input.doorInteraction.down = interactHeld;
            input.doorInteraction.consumed = interactHeld;
        }
        if (input.lastReceived != 0) {
            input.maxInterPacketGapMs = std::max(
                input.maxInterPacketGapMs,
                static_cast<unsigned int>(std::min<ULONGLONG>(
                    receivedNow - input.lastReceived, 60000)));
        }
        input.lastReceived = receivedNow;
        if (input.lastRateTrace == 0) {
            input.lastRateTrace = receivedNow;
        }
        ++input.packetsReceived;
        if (receivedNow - input.lastRateTrace >= 5000) {
            TraceTraversal("coop_recv_rate", static_cast<int>(humanId),
                static_cast<int>(input.packetsReceived),
                static_cast<int>(input.maxInterPacketGapMs));
            input.lastRateTrace = receivedNow;
            input.packetsReceived = 0;
            input.maxInterPacketGapMs = 0;
        }
        input.fireHeld = (flags & 1) != 0;
        if (!input.fireHeld) input.sequence.requireRelease = false;
        const bool equippedWeaponSynchronized = input.requestedWeapon == 0 && requestedWeapon != 0 &&
            input.autoFire && input.sequence.weaponId == requestedWeapon &&
            input.sequence.mode == static_cast<FireMode>(requestedMode);
        if (!equippedWeaponSynchronized &&
            (input.requestedMode != static_cast<FireMode>(requestedMode) || input.requestedWeapon != requestedWeapon)) {
            input.shotRemainingMs = 0; input.autoFire = false;
            input.sequence.requireRelease = input.fireHeld && (flags & 2) == 0;
        }
        input.requestedMode = static_cast<FireMode>(requestedMode);
        input.requestedWeapon = requestedWeapon;
        input.readyHold = readyHold;
        if (readyHold != kReadyHoldNone) {
            input.shotRemainingMs = 0;
            input.autoFire = false;
            input.fireHeld = false;
        }
        const bool sprintHeld = (flags & 4) != 0;
        if (sprintHeld && !input.sprintHeld &&
            (input.reloadRequested || FirearmIsReloading(serverHuman) ||
             HasQueuedReload(serverHuman))) {
            input.cancelReloadRequested = true;
        }
        input.sprintHeld = sprintHeld;
        input.interactHeld = interactHeld;
        // Consume every reliable F edge here, even if two packets arrive
        // between server updates. The server tick also advances hold timers.
        if (interactHeld || input.doorInteraction.down || input.doorInteraction.pendingTap || input.doorInteraction.lockpickActive) {
            ProcessDoorInteraction(serverHuman, input.doorInteraction, interactHeld,
                {x, z}, moveX != 0.0F || moveZ != 0.0F,
                equipmentCode != 0 || (flags & (1 | 2 | 16 | 32)) != 0,
                GetTickCount64());
        }
        if (!interactHeld) {
            input.lastInteractedDoorId = 0;
        }
        const bool crouchHeld = (flags & 8) != 0;
        if (crouchHeld != input.crouchHeld) {
            input.crouchHeld = crouchHeld;
            input.crouchCommandPending = true;
        }
        if ((flags & 16) != 0) {
            input.reloadRequested = true;
        }
        if ((flags & 32) != 0) {
            input.grenadeThrowChoice = grenadeChoice;
            input.grenadeRequested = true;
            input.grenadeLongThrow = (flags & 64) != 0;
            input.grenadeNativePreview = (flags & 128) != 0;
            input.grenadeOriginOffset = grenadeOriginOffset;
            input.grenadeTargetOffset = grenadeTargetOffset;
            input.grenadeAngle = grenadeAngle;
            TraceTraversal("coop_grenade_recv", static_cast<int>(humanId),
                           input.grenadeLongThrow ? 1 : 0, senderMask);
        }
        if (equipmentCode != 0) {
            input.equipmentRequest = equipmentCode == 11
                ? kHolsterEquipmentRequest : equipmentCode;
            if (input.equipmentRequest > 0) {
                const int inventoryType = ResolveEquipmentNumber(
                    serverHuman, input.equipmentRequest);
                void* requestedItem = inventoryType >= 0
                    ? At<void*>(serverHuman, kHumanServerInventoryOffset +
                        static_cast<std::size_t>(inventoryType) *
                            sizeof(void*))
                    : nullptr;
                if (requestedItem && requestedItem == At<void*>(
                        serverHuman, kHumanServerEquippedItemOffset) &&
                    InventoryItemIsFirearm(requestedItem) &&
                    (input.reloadRequested ||
                     FirearmIsReloading(serverHuman) ||
                     HasQueuedReload(serverHuman))) {
                    input.equipmentRequest = kNoEquipmentRequest;
                }
            }
            if (input.equipmentRequest > 0 &&
                (input.reloadRequested || FirearmIsReloading(serverHuman) ||
                 HasQueuedReload(serverHuman))) {
                const int inventoryType = ResolveEquipmentNumber(
                    serverHuman, input.equipmentRequest);
                if (inventoryType >= 0) {
                    void* requestedItem = At<void*>(serverHuman,
                        kHumanServerInventoryOffset +
                            static_cast<std::size_t>(inventoryType) *
                                sizeof(void*));
                    if (requestedItem && requestedItem != At<void*>(
                            serverHuman, kHumanServerEquippedItemOffset)) {
                        input.cancelReloadRequested = true;
                    }
                }
            }
            TraceTraversal("coop_equip_recv", static_cast<int>(humanId),
                           input.equipmentRequest, senderMask);
        }
        if ((flags & 2) != 0 && readyHold == kReadyHoldNone &&
            !input.autoFire && !input.sequence.requireRelease) {
            input.shotRemainingMs = kShootRequestLifetimeMs;
            input.equipQueued = false;
            TraceTraversal("coop_fire_recv", static_cast<int>(humanId),
                           senderMask, 0);
        }
        return;
    }
    if (eventType == kDirectControlProbeEvent) {
        const int version = packet && g_bitPackerReadBits
            ? g_bitPackerReadBits(packet, 8) : -1;
        const int humanId = serverHuman
            ? static_cast<int>(At<std::uint32_t>(
                  serverHuman, kHumanIdOffset)) : -1;
        TraceTraversal("coop_probe_recv", humanId, senderMask, version);
        return;
    }
    g_originalHumanServerNetReceiveEvent(
        serverHuman, senderMask, eventType, simulationTime, packet);
}

void __fastcall HookedProjectileClientNetReceiveEvent(
    void* projectile, int senderMask, int eventType,
    unsigned int simulationTime, void* packet) {
    if (projectile && (eventType == 14 || eventType == 15)) {
        TraceProjectileEffect("client_event", projectile,
            At<std::uint32_t>(projectile, 0x1F0), eventType,
            At<Vector3>(projectile, kHumanOriginOffset));
    }
    g_originalProjectileClientNetReceiveEvent(
        projectile, senderMask, eventType, simulationTime, packet);
}

void __fastcall HookedProjectileServerDetonate(
    void* projectile, const Vector3* detonationPoint) {
    if (projectile && detonationPoint) {
        TraceProjectileEffect("server_detonate", projectile,
            At<std::uint32_t>(projectile, 0x138), -1,
            *detonationPoint);
    }
    g_originalProjectileServerDetonate(projectile, detonationPoint);
}

char __fastcall HookedBulletCollisionFilter(
    const void* candidate, unsigned int collisionFlags, int shooterType) {
    const char nativeResponse = g_originalBulletCollisionFilter(
        candidate, collisionFlags, shooterType);
    if (nativeResponse != 0 || shooterType != 0 || !candidate ||
        (!g_friendlyFire && !g_hostageBulletDamage)) {
        return nativeResponse;
    }

    // This callback also sees non-human map entities. The template type check
    // must precede reading the human type and health fields.
    const auto vtable = At<std::uintptr_t>(candidate, 0);
    const auto getType = reinterpret_cast<const void*(__fastcall*)(const void*)>(
        *reinterpret_cast<const std::uintptr_t*>(vtable + 8));
    const auto humanTemplateType = At<const void*>(
        reinterpret_cast<const void*>(g_moduleBase + kRvaHumanTemplateType), 0);
    if (getType(candidate) != humanTemplateType ||
        At<float>(candidate, kHumanHealthOffset) <= 0.0F) {
        return nativeResponse;
    }

    const int targetType = At<int>(candidate, 0x138);
    if (!((targetType == 0 && g_friendlyFire) ||
          (targetType == 2 && g_hostageBulletDamage))) {
        return nativeResponse;
    }

    // TraceBullet excludes the shooter's collision index before invoking this
    // filter. Preserve the native TOUCH/BLOCK choice for any admitted hit.
    TraceTraversal("combat_bullet_collision", shooterType, targetType,
        static_cast<int>(At<std::uint32_t>(candidate, kHumanIdOffset)));
    return static_cast<char>((collisionFlags & 2U) != 0U ? 2 : 1);
}

bool HumanHasAnimationHandle(const void* clientHuman, const char* handle) {
    const void* animationSet = clientHuman
        ? At<void*>(clientHuman, 0x180) : nullptr;
    if (!animationSet || !handle) {
        return false;
    }
    std::uint32_t hash = 0x811C9DC5U;
    for (const unsigned char* letter =
             reinterpret_cast<const unsigned char*>(handle);
         *letter; ++letter) {
        hash = (hash ^ *letter) * 0x1000193U;
    }
    const int count = At<int>(animationSet, 0x28);
    const auto* entries = static_cast<const std::byte*>(
        At<void*>(animationSet, 0x20));
    if (!entries || count < 0 || count > 256) {
        return false;
    }
    for (int index = 0; index < count; ++index) {
        const auto* entry = entries + static_cast<std::size_t>(index) * 0x30;
        if (At<std::uint32_t>(entry, 0x20) == hash &&
            At<void*>(entry, 0x28)) {
            return true;
        }
    }
    return false;
}

bool HighReadyAnimationSetEligible(const void* set) {
    if (!set) return false;
    const void* idle = FindAnimationBinding(const_cast<void*>(set), "idle");
    const void* info = idle ? At<void*>(idle, 0x28) : nullptr;
    // Grip compatibility: custom rifle descendants keep support when their
    // loaded aimed idle is the native two-handed rifle pose. Changed grip
    // poses require a separately reviewed profile instead of a name guess.
    if (!info || At<std::uint32_t>(info, 8) != AnimationNameHash("rifle_character_idle_aim"))
        return false;
    for (int depth = 0; set && depth < 16; ++depth) {
        const char* name = At<const char*>(set, 0);
        if (name && (std::strstr(name, "pistol") || std::strstr(name, "shield") ||
            std::strstr(name, "shotgun") || std::strstr(name, "rpg") ||
            std::strstr(name, "dshk") || std::strstr(name, "beltfeed") ||
            std::strstr(name, "lmg") || std::strstr(name, "heavy"))) return false;
        if (At<std::uint32_t>(set, 8) == AnimationNameHash("rifle")) return true;
        const auto parent = At<std::uint32_t>(set, 0x18);
        if (!parent || !g_moduleBase) return false;
        const void* list = At<void*>(reinterpret_cast<void*>(g_moduleBase + 0x01355DC0), 0);
        const int count = At<int>(reinterpret_cast<void*>(g_moduleBase + 0x01355DC8), 0);
        if (!list || count < 1 || count > 512) return false;
        const void* next = nullptr;
        for (int i = 0; i < count; ++i) {
            const void* candidate = static_cast<const std::byte*>(list) + i * 0x30;
            if (At<std::uint32_t>(candidate, 8) == parent) { next = candidate; break; }
        }
        if (next == set) return false;
        set = next;
    }
    return false;
}

bool HighReadyWeaponEligible(const void* human) {
    const void* firearm = ClientFireTemplate(human);
    if (!firearm) return false;
    // Loaded Equipment_Template::category is a HashedString at +0x60.
    // Limit the first profile to rifle/SMG/carbine classes; DMR/LMG/shotgun
    // sizes and specialized grips need their own visual validation.
    const auto category = At<std::uint32_t>(firearm, 0x68);
    if (category != AnimationNameHash("rifle") && category != AnimationNameHash("smg") &&
        category != AnimationNameHash("carbine")) return false;
    if (!HighReadyAnimationSetEligible(At<void*>(human, 0x180)))
        return false;
    for (const char* handle : {"da_high_idle", "da_high_walk", "da_high_left",
                              "da_high_right", "da_high_enter", "da_high_exit"}) {
        if (!HumanHasAnimationHandle(human, handle)) return false;
    }
    return true;
}

bool HighReadyTransitionFinished(const void* human, const char* handle) {
    const void* set = At<void*>(human, 0x180);
    const void* binding = FindAnimationBinding(const_cast<void*>(set), handle);
    const void* info = binding ? At<void*>(binding, 0x28) : nullptr;
    const void* manager = At<void*>(human, 0x178);
    if (!info || !manager) return false;
    const int count = At<int>(manager, 0x6C50);
    const void* active = count > 0 && count <= 4 ? At<void*>(manager, 0x6C48) : nullptr;
    for (int i = 0; active && i < count; ++i) {
        const void* running = At<void*>(active, static_cast<std::size_t>(i) * sizeof(void*));
        if (!running || At<std::uint32_t>(running, 8) != At<std::uint32_t>(info, 8)) continue;
        if (At<bool>(running, 0x4E)) return true;
        const float time = At<float>(running, 0x30), end = At<float>(running, 0x34);
        const float speed = At<float>(running, 0x48);
        return std::isfinite(time) && std::isfinite(end) && end > 0 &&
            (speed < 0 ? time <= 0.0F : speed > 0 && time >= end);
    }
    return false;
}

void AdvanceHighReadyPhase(HighReadyPoseState& pose, bool requested, bool completed) {
    HighReadyPhase next = pose.phase;
    if (pose.phase == HighReadyPhase::None && requested) next = HighReadyPhase::Enter;
    else if (pose.phase == HighReadyPhase::Enter && completed)
        next = requested ? HighReadyPhase::Hold : HighReadyPhase::Exit;
    else if (pose.phase == HighReadyPhase::Hold && !requested) next = HighReadyPhase::Exit;
    else if (pose.phase == HighReadyPhase::Exit && completed)
        next = requested ? HighReadyPhase::Enter : HighReadyPhase::None;
    if (next != pose.phase) {
        pose.phase = next;
        pose.restart = next == HighReadyPhase::Enter || next == HighReadyPhase::Exit;
    }
}

void ApplyHighReadyPose(const void* human, const void* state, void* bodyAnims, int readyHold) {
    if (!human || !bodyAnims) return;
    auto* anims = static_cast<char*>(bodyAnims);
    const char* primary = anims, *secondary = anims + 0x44;
    const bool walking = std::strcmp(primary, "walk") == 0;
    const bool left = std::strcmp(primary, "strafe_l") == 0;
    const bool right = std::strcmp(primary, "strafe_r") == 0;
    const bool backward = std::strcmp(primary, "walk_b") == 0;
    const bool stationary = !*primary || std::strcmp(primary, "idle") == 0;
    const bool normalUpper = std::strcmp(secondary, "idle") == 0 ||
        std::strcmp(secondary, "idle_low") == 0 || std::strcmp(secondary, "raiseweapon") == 0 ||
        std::strcmp(secondary, "lowerweapon") == 0 || std::strcmp(secondary, primary) == 0;
    // Initial scope is standing rifle locomotion. Never substitute a reload,
    // sprint, crouch, breach, equip, or other action with a standing pose.
    // Firearms use 5 for aimed ready, 3 for low idle, and 4/6 while
    // raising/lowering. Utility ready state 3 alone excludes normal rifles.
    const int equipmentState = state ? At<int>(state, 0x90) : -1;
    const bool firearmReady = equipmentState >= 3 && equipmentState <= 6;
    const bool eligible = state && !At<bool>(state, 0x31) && firearmReady &&
        (stationary || walking || left || right || backward) && normalUpper &&
        readyHold != kReadyHoldLow && HighReadyWeaponEligible(human);
    if (!eligible) { g_highReadyPoses.erase(human); return; }
    auto found = g_highReadyPoses.find(human);
    if (readyHold != kReadyHoldHigh && found == g_highReadyPoses.end()) return;
    auto& pose = g_highReadyPoses[human];
    const void* set = At<void*>(human, 0x180);
    if (pose.animationSet != set) pose = {set};
    const char* transition = pose.phase == HighReadyPhase::Enter ? "da_high_enter" : "da_high_exit";
    const bool completed = !pose.restart && (pose.phase == HighReadyPhase::Enter ||
        pose.phase == HighReadyPhase::Exit) && HighReadyTransitionFinished(human, transition);
    const auto previous = pose.phase;
    AdvanceHighReadyPhase(pose, readyHold == kReadyHoldHigh, completed);
    if (previous != pose.phase) TraceTraversal("vbss_high_phase",
        static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)),
        static_cast<int>(previous), static_cast<int>(pose.phase));
    if (pose.phase == HighReadyPhase::None) { g_highReadyPoses.erase(human); return; }
    const char* upper = pose.phase == HighReadyPhase::Enter ? "da_high_enter" :
        pose.phase == HighReadyPhase::Exit ? "da_high_exit" : "da_high_idle";
    std::memcpy(anims + 0x44, upper, std::strlen(upper) + 1);
    At<int>(bodyAnims, 0x84) = 0;
    if (pose.restart) { At<bool>(bodyAnims, 0x88) = true; pose.restart = false; }
    if (pose.phase == HighReadyPhase::Hold && (walking || left || right)) {
        const char* lower = left ? "da_high_left" : right ? "da_high_right" : "da_high_walk";
        std::memcpy(anims, lower, std::strlen(lower) + 1);
        At<int>(bodyAnims, 0x40) = 0;
    }
    // Backward movement retains native lower-body walking with high idle above.
}

void RestoreLeanPlaybackForHuman(const void* clientHuman, int keepSide) {
    if (!clientHuman) {
        return;
    }
    const void* manager = At<void*>(clientHuman, 0x178);
    if (!manager) {
        return;
    }
    const int count = At<int>(manager, 0x6C38);
    const void* registry = count > 0 && count <= 256
        ? At<void*>(manager, 0x6C30) : nullptr;
    if (!registry) {
        return;
    }
    for (int index = 0; index < count; ++index) {
        void* running = At<void*>(registry,
            static_cast<std::size_t>(index) * sizeof(void*));
        const char* name = running ? At<const char*>(running, 0) : nullptr;
        if (!name || At<float>(running, 0x48) != 0.0F) {
            continue;
        }
        const bool left = std::strcmp(name, "rifle_character_lean_l") == 0 ||
            std::strcmp(name, "pistol_character_lean_l") == 0 ||
            std::strcmp(name, "rifle_character_lean_low_l") == 0 ||
            std::strcmp(name, "pistol_character_lean_low_l") == 0 ||
            std::strcmp(name, "rifle_character_lean_move_l") == 0 ||
            std::strcmp(name, "pistol_character_lean_move_l") == 0;
        const bool right = std::strcmp(name, "rifle_character_lean_r") == 0 ||
            std::strcmp(name, "pistol_character_lean_r") == 0 ||
            std::strcmp(name, "rifle_character_lean_low_r") == 0 ||
            std::strcmp(name, "pistol_character_lean_low_r") == 0 ||
            std::strcmp(name, "rifle_character_lean_move_r") == 0 ||
            std::strcmp(name, "pistol_character_lean_move_r") == 0;
        const int side = left ? 1 : (right ? -1 : 0);
        if (side != 0 && side != keepSide) {
            At<float>(running, 0x48) = 1.0F;
            TraceTraversal("lean_resume", side, index, 0);
        }
    }
}

bool PauseActiveLeanAnimation(const void* clientHuman, const char* handle,
                              bool resumeHold) {
    const void* manager = At<void*>(clientHuman, 0x178);
    void* binding = FindAnimationBinding(
        At<void*>(clientHuman, 0x180), handle);
    const void* clip = binding ? At<void*>(binding, 0x28) : nullptr;
    if (!manager || !clip) {
        return false;
    }
    const int count = At<int>(manager, 0x6C50);
    const void* list = count > 0 && count <= 4
        ? At<void*>(manager, 0x6C48) : nullptr;
    if (!list) {
        return false;
    }
    for (int index = 0; index < count; ++index) {
        void* running = At<void*>(list,
            static_cast<std::size_t>(index) * sizeof(void*));
        if (!running || At<int>(running, 0x8) != At<int>(clip, 0x8)) {
            continue;
        }
        float& time = At<float>(running, 0x30);
        const float duration = At<float>(running, 0x34);
        if (std::isfinite(time) && std::isfinite(duration) &&
            duration > 900.0F && duration < 1300.0F &&
            time >= 0.0F && time < duration + 50.0F) {
            if (time < 500.0F && !resumeHold) {
                // The authored clip takes about half a second to reach its
                // maximum lean. Speed only this clip's approach to the hold.
                if (At<float>(running, 0x48) > 0.0F) {
                    At<float>(running, 0x48) = 1.45F;
                }
                return false;
            }
            if (At<float>(running, 0x48) != 0.0F) {
                time = 500.0F;
                At<float>(running, 0x48) = 0.0F;
                TraceTraversal("lean_pause", static_cast<int>(time),
                    static_cast<int>(duration), index);
            }
            return true;
        }
    }
    return false;
}

struct HeldGrenadePose {
    void* running = nullptr; int nameId = 0; float speed = 1; int slot = -1;
    float originalEndTime = 0; int originalFrameCount = 0;
};
std::unordered_map<const void*, HeldGrenadePose> g_heldGrenadePoses;

// Traverse the live manager rather than dereferencing a cached animation:
// changing equipment or visibility can replace the native running instance.
void RestoreGrenadeHeldPose(const void* human) {
    const auto found = g_heldGrenadePoses.find(human);
    if (found == g_heldGrenadePoses.end()) return;
    const void* manager = human ? At<void*>(human, 0x178) : nullptr;
    const int count = manager ? At<int>(manager, 0x6C38) : 0;
    const void* list = count > 0 && count <= 256 ? At<void*>(manager, 0x6C30) : nullptr;
    for (int i = 0; list && i < count; ++i) {
        void* running = At<void*>(list, static_cast<std::size_t>(i) * sizeof(void*));
        if (running == found->second.running && At<int>(running, 8) == found->second.nameId) {
            At<float>(running, 0x34) = found->second.originalEndTime;
            At<int>(running, 0x40) = found->second.originalFrameCount;
            At<float>(running, 0x48) = found->second.speed;
            At<float>(running, 0x30) = 0;
            // The actual throw must create/reset its native animation rather
            // than continue the sampled hold pose halfway through a clip.
            At<bool>(running, 0x4E) = true;
        }
    }
    g_heldGrenadePoses.erase(found);
}

bool GrenadeHeldPoseEligible(const void* human, const void* state) {
    if (!human || !state || !g_enabled || At<float>(state, 0x2C) <= 0 ||
        At<bool>(state, 0x34) || At<bool>(state, 0x35) || At<bool>(state, 0x38) ||
        At<bool>(state, 0x39) || At<bool>(state, 0x3A)) return false;
    const int equipmentState = At<int>(state, 0x90);
    const int command = At<int>(state, 0x50);
    if (equipmentState != 1 && equipmentState != 2 && equipmentState != 3 &&
        !(equipmentState == 0 && (command == 0 || command == 1))) return false;
    if (command != -1 && command != 19 && command != 20 && command != 36 && command != 42 &&
        command != 0 && command != 1) return false;
    const int slot = At<int>(state, 0x8C);
    if (slot < 1 || slot >= kInventorySlotCount) return false;
    const void* itemTemplate = At<void*>(human, 0x430 + static_cast<std::size_t>(slot) * 0x58);
    return itemTemplate && At<int>(itemTemplate, 0x20) == 3 && At<int>(state, 0x9C) > 0;
}

void ApplyGrenadeHeldPose(const void* human, const void* state, void* bodyAnims) {
    if (!bodyAnims) return;
    if (!GrenadeHeldPoseEligible(human, state)) { RestoreGrenadeHeldPose(human); return; }
    // A static pre-release pose from the authored grenade clip. Only the
    // secondary (upper-body) animation changes; legs use native locomotion.
    // The clip stays stopped before its release event, never loops as a throw.
    constexpr char handle[] = "grenade_throw_low_right";
    void* binding = FindAnimationBinding(At<void*>(human, 0x180), handle);
    const void* clip = binding ? At<void*>(binding, 0x28) : nullptr;
    const void* manager = At<void*>(human, 0x178);
    if (!clip || !manager) { RestoreGrenadeHeldPose(human); return; }
    // A stale interaction handle can override BOTH native animation layers.
    // Keep the hold clip out of primary locomotion as well as freezing it.
    auto* primary = static_cast<char*>(bodyAnims);
    if (std::strncmp(primary, "grenade", 7) == 0 ||
        std::strcmp(primary, "operate_equipment") == 0 ||
        std::strcmp(primary, "change_in") == 0 || std::strcmp(primary, "change_out") == 0) {
        const bool moving = human == g_controlledClientHuman && g_wasMoving;
        const char* movement = At<bool>(state, 0x31)
            ? (moving ? "walk_crouch" : "idle_crouch") : (moving ? "walk" : "idle");
        std::memcpy(primary, movement, std::strlen(movement) + 1);
        At<int>(bodyAnims, 0x40) = 0;
    }
    std::memcpy(static_cast<char*>(bodyAnims) + 0x44, handle, sizeof(handle));
    At<int>(bodyAnims, 0x84) = 0;
    At<bool>(bodyAnims, 0x88) = false;
    At<bool>(bodyAnims, 0x89) = false;
    const int count = At<int>(manager, 0x6C38);
    const void* list = count > 0 && count <= 256 ? At<void*>(manager, 0x6C30) : nullptr;
    for (int i = 0; list && i < count; ++i) {
        void* running = At<void*>(list, static_cast<std::size_t>(i) * sizeof(void*));
        // Binding points to sAnimationInfo; running pAnim is a different
        // runtime asset object. Match their shared animation-name hash.
        if (!running || At<std::uint32_t>(running, 8) != At<std::uint32_t>(clip, 8)) continue;
        const float frameDuration = At<float>(running, 0x38);
        const auto previous = g_heldGrenadePoses.find(human);
        const bool tracked = previous != g_heldGrenadePoses.end() && previous->second.running == running;
        const float endTime = tracked ? previous->second.originalEndTime : At<float>(running, 0x34);
        if (!std::isfinite(frameDuration) || frameDuration <= 0 || frameDuration > 100 ||
            !std::isfinite(endTime) || endTime <= 12 * frameDuration) continue;
        auto& saved = g_heldGrenadePoses[human];
        if (saved.running != running || saved.slot != At<int>(state, 0x8C)) {
            const float speed = At<float>(running, 0x48) > 0 ? At<float>(running, 0x48) : saved.speed;
            const int frameCount = tracked ? saved.originalFrameCount : At<int>(running, 0x40);
            saved = {running, At<int>(running, 8), speed, At<int>(state, 0x8C), endTime, frameCount};
            // Begin the authored draw rather than jumping to its held frame.
            At<float>(running, 0x30) = 0;
            At<float>(running, 0x48) = saved.speed > 0 ? saved.speed : 1.0F;
            At<bool>(running, 0x4E) = false;
            TraceTraversal("grenade_hold_pose", static_cast<int>(At<std::uint32_t>(human, kHumanIdOffset)),
                           At<int>(state, 0x8C), 12);
        }
        // Stop the native playback range at the held frame as well as pausing
        // its timer. Layer changes can reset playback between GetAnims calls;
        // a bounded range cannot run through the rest of the throw first.
        At<float>(running, 0x34) = 12 * frameDuration;
        if (saved.originalFrameCount > 13) At<int>(running, 0x40) = 13;
        if (At<int>(state, 0x90) != 2 && At<int>(state, 0x90) != 0 &&
            At<float>(running, 0x30) < 12 * frameDuration) continue;
        At<float>(running, 0x30) = 12 * frameDuration;
        At<float>(running, 0x48) = 0;
        At<bool>(running, 0x4E) = false;
    }
}

void ApplyHeldToolLocomotion(const void* human, const void* state, void* bodyAnims, bool moving) {
    if (!g_enabled || !human || !state || !bodyAnims || At<float>(state, 0x2C) <= 0) return;
    const int slot = At<int>(state, 0x8C);
    if (slot < 1 || slot >= kInventorySlotCount) return;
    const void* item = At<void*>(human, 0x430 + static_cast<std::size_t>(slot) * 0x58);
    if (!item) return;
    const int type = At<int>(item, 0x20), command = At<int>(state, 0x50);
    const int equipmentState = At<int>(state, 0x90);
    const bool crouched = At<bool>(state, 0x31);
    // Spycam has no handheld draw. Crowbar retains its authored change_in/out;
    // replacing those with locomotion would make the prop pop into the hand.
    const bool toolTransition = type == 4 && (command == 0 || command == 1) &&
        (equipmentState == 0 || equipmentState == 1 || equipmentState == 2 || equipmentState == 3);
    const bool crowbarCrouchWalk = (type == 8 || type == 4) && crouched && equipmentState == 3 &&
        (command == 42 || command == 19 || command == 20 || command == 36);
    if (!toolTransition && !crowbarCrouchWalk) return;
    const char* handle = crouched ? (moving ? "walk_crouch" : "idle_crouch")
                                  : (moving ? "walk" : "idle");
    std::memcpy(bodyAnims, handle, std::strlen(handle) + 1);
    std::memcpy(static_cast<std::byte*>(bodyAnims) + 0x44, handle, std::strlen(handle) + 1);
    At<int>(bodyAnims, 0x40) = At<int>(bodyAnims, 0x84) = 0;
    At<bool>(bodyAnims, 0x88) = At<bool>(bodyAnims, 0x89) = false;
}

struct ToolDrawState { const void* human = nullptr; bool drawing = false; };
ToolDrawState g_toolDrawState;
void RestartCrowbarDraw(const void* human, const void* state, void* bodyAnims) {
    if (!human || !state || !bodyAnims) return;
    const bool drawing = g_enabled && At<int>(state, 0x8C) == 16 &&
        At<int>(state, 0x90) == 1 && At<int>(state, 0x50) == 0;
    if (drawing && (g_toolDrawState.human != human || !g_toolDrawState.drawing)) {
        At<bool>(bodyAnims, 0x88) = true;
        TraceTraversal("crowbar_draw_restart", At<int>(state, 0x8C), 1, 0);
    }
    g_toolDrawState = {human, drawing};
}

void __fastcall HookedHumanClientGetAnims(
    const void* clientHuman, const void* state, void* bodyAnims) {
    if (clientHuman == g_controlledClientHuman && state &&
        At<int>(state, 0x8C) != 16 && g_selectedGearInventoryType != 16)
        RestoreCrowbarBindingAfterSwitch();
    g_originalHumanClientGetAnims(clientHuman, state, bodyAnims);
    if (clientHuman == g_controlledClientHuman)
        RestartCrowbarDraw(clientHuman, state, bodyAnims);
    if (g_enabled && clientHuman == g_controlledClientHuman && state && bodyAnims) {
        static ULONGLONG last = 0;
        static int previousSlot = -1, previousCommand = -1, previousState = -1;
        const ULONGLONG now = GetTickCount64();
        const int slot = At<int>(state, 0x8C), cmd = At<int>(state, 0x50), equip = At<int>(state, 0x90);
        if (now - last >= 250 || slot != previousSlot || cmd != previousCommand || equip != previousState) {
            char line[500]{};
            std::snprintf(line, sizeof(line), "%llu ANIM slot=%d cmd=%d state=%d crouch=%d moving=%d primary=%.32s secondary=%.32s restart=%d blend=%d\r\n",
                static_cast<unsigned long long>(now), slot, cmd, equip, At<bool>(state, 0x31), g_wasMoving,
                static_cast<const char*>(bodyAnims), static_cast<const char*>(bodyAnims) + 0x44,
                At<bool>(bodyAnims, 0x88), At<bool>(bodyAnims, 0x89));
            TraceItemDetail(line);
            const void* manager = At<void*>(clientHuman, 0x178);
            const int count = manager ? At<int>(manager, 0x6C50) : 0;
            const void* active = count > 0 && count <= 4 ? At<void*>(manager, 0x6C48) : nullptr;
            for (int i = 0; active && i < count; ++i) {
                const void* running = At<void*>(active, static_cast<std::size_t>(i) * sizeof(void*));
                if (!running) continue;
                std::snprintf(line, sizeof(line), "%llu CLIP layer=%d hash=%u time=%.1f end=%.1f speed=%.2f weight=%.2f complete=%d\r\n",
                    static_cast<unsigned long long>(now), i, At<unsigned>(running, 8), At<float>(running, 0x30),
                    At<float>(running, 0x34), At<float>(running, 0x48), At<float>(running, 0x44), At<bool>(running, 0x4E));
                TraceItemDetail(line);
            }
            last = now; previousSlot = slot; previousCommand = cmd; previousState = equip;
        }
    }
    if (clientHuman == g_controlledClientHuman)
        ApplyHeldToolLocomotion(clientHuman, state, bodyAnims, g_wasMoving);
    ApplyGrenadeHeldPose(clientHuman, state, bodyAnims);
    if (g_enabled && clientHuman == g_controlledClientHuman && g_selectedGearInventoryType < 0)
        ApplyHighReadyPose(clientHuman, state, bodyAnims, g_readyHold);
    else g_highReadyPoses.erase(clientHuman);
    if (g_enabled && clientHuman == g_controlledClientHuman && bodyAnims &&
        g_readyHold == kReadyHoldLow &&
        g_selectedGearInventoryType < 0) {
        auto* anims = static_cast<char*>(bodyAnims);
        const char* secondary = anims + 0x44;
        const bool moving = std::strcmp(anims, "walk") == 0 ||
            std::strcmp(anims, "walk_b") == 0 ||
            std::strcmp(anims, "strafe_l") == 0 ||
            std::strcmp(anims, "strafe_r") == 0;
        const bool normal = std::strcmp(secondary, "idle") == 0 ||
            std::strcmp(secondary, "raiseweapon") == 0 ||
            std::strcmp(secondary, anims) == 0;
        const char* held = moving ? "walk_low" : "idle_low";
        const bool available = HumanHasAnimationHandle(clientHuman, held);
        if (normal && available) {
            std::memcpy(anims + 0x44, held, std::strlen(held) + 1);
            At<int>(bodyAnims, 0x84) = 0;
        }
    }
    if (clientHuman == g_controlledClientHuman && bodyAnims && g_enabled) {
        const char* secondary = static_cast<const char*>(bodyAnims) + 0x44;
        // Trace native readiness before the visual lean replaces idle.
        const int phase = std::strcmp(secondary, "idle_low") == 0 ? 1 :
            std::strcmp(secondary, "raiseweapon") == 0 ? 2 :
            std::strcmp(secondary, "idle") == 0 ? 3 :
            std::strcmp(secondary, "lowerweapon") == 0 ? 4 :
            std::strcmp(secondary, "fire") == 0 ? 5 : 0;
        if (phase != g_lastLeanReadyPhase && g_leanSide != 0) {
            TraceTraversal("lean_ready_phase", phase, g_lastLeanReadyPhase,
                g_leanSide);
        }
        g_lastLeanReadyPhase = phase;
    }
    const bool eligible = g_enabled &&
        clientHuman == g_controlledClientHuman && bodyAnims &&
        g_leanSide != 0 && g_selectedGearInventoryType < 0;
    if (clientHuman == g_controlledClientHuman && !eligible) {
        RestoreLeanPlaybackForHuman(clientHuman);
    }
    if (eligible) {
        auto* anims = static_cast<char*>(bodyAnims);
        const char* leanHandle = g_leanSide > 0 ? "lean_l" : "lean_r";
        const char* lowLeanHandle = g_leanSide > 0
            ? "lean_low_l" : "lean_low_r";
        const char* moveLeanHandle = g_leanSide > 0
            ? "lean_move_l" : "lean_move_r";
        const bool idle = (anims[0] == '\0' ||
            std::strcmp(anims, "idle") == 0) &&
            std::strcmp(anims + 0x44, "idle") == 0;
        const bool firing = (anims[0] == '\0' ||
            std::strcmp(anims, "idle") == 0) &&
            std::strcmp(anims + 0x44, "fire") == 0;
        const bool lowReady = anims[0] == '\0' &&
            (std::strcmp(anims + 0x44, "idle_low") == 0 ||
             std::strcmp(anims + 0x44, "raiseweapon") == 0 ||
             std::strcmp(anims + 0x44, "lowerweapon") == 0);
        const bool walking = std::strcmp(anims, "walk") == 0 ||
            std::strcmp(anims, "walk_b") == 0 ||
            std::strcmp(anims, "strafe_l") == 0 ||
            std::strcmp(anims, "strafe_r") == 0;
        const bool crouched = std::strcmp(anims, "idle_crouch") == 0 ||
            std::strcmp(anims, "walk_crouch") == 0 ||
            std::strcmp(anims, "walk_crouch_b") == 0 ||
            std::strcmp(anims + 0x44, "idle_crouch") == 0 ||
            std::strcmp(anims + 0x44, "walk_crouch") == 0 ||
            std::strcmp(anims + 0x44, "walk_crouch_b") == 0;
        const bool crouchLean = crouched &&
            (std::strcmp(anims + 0x44, anims) == 0 ||
             std::strcmp(anims + 0x44, "idle") == 0 ||
             std::strcmp(anims + 0x44, "idle_crouch") == 0 ||
             std::strcmp(anims + 0x44, "walk_crouch") == 0 ||
             std::strcmp(anims + 0x44, "walk_crouch_b") == 0);
        const bool movingLean = walking &&
            std::strcmp(anims + 0x44, anims) == 0;
        const bool handleAvailable = HumanHasAnimationHandle(
            clientHuman, leanHandle);
        const bool lowHandleAvailable = lowReady &&
            HumanHasAnimationHandle(clientHuman, lowLeanHandle);
        const bool moveHandleAvailable = (movingLean || crouchLean) &&
            HumanHasAnimationHandle(clientHuman, moveLeanHandle);
        const bool playLean = ((idle || firing) && handleAvailable) ||
            lowHandleAvailable || moveHandleAvailable;
        RestoreLeanPlaybackForHuman(clientHuman,
            playLean ? g_leanSide : 0);
        if (!playLean) {
            g_leanReachedHold = false;
        }
        const bool paused = playLean &&
            PauseActiveLeanAnimation(clientHuman,
                                     moveHandleAvailable ? moveLeanHandle :
                                     lowHandleAvailable ? lowLeanHandle :
                                         leanHandle,
                                     g_leanReachedHold);
        if (paused) {
            g_leanReachedHold = true;
        }
        static ULONGLONG lastLeanPoseTrace = 0;
        const ULONGLONG now = GetTickCount64();
        if (now - lastLeanPoseTrace >= 1000) {
            TraceTraversal("lean_pose", g_leanSide,
                handleAvailable ? 1 : 0,
                idle ? 1 : (firing ? 3 :
                    (crouched ? 4 : (lowHandleAvailable ? 2 : 0))));
            TraceTraversal("lean_pause_state", paused ? 1 : 0,
                g_leanReachedHold ? 1 : 0, 0);
            const void* manager = At<void*>(clientHuman, 0x178);
            const void* binding = FindAnimationBinding(
                At<void*>(clientHuman, 0x180), leanHandle);
            const void* clip = binding ? At<void*>(binding, 0x28) : nullptr;
            const int runningCount = manager
                ? At<int>(manager, 0x6C50) : 0;
            TraceTraversal("lean_runtime", runningCount,
                clip ? 1 : 0, 0);
            const void* runningList = manager && runningCount > 0 &&
                runningCount <= 4 ? At<void*>(manager, 0x6C48) : nullptr;
            for (int index = 0; runningList && index < runningCount;
                 ++index) {
                const void* running = At<void*>(runningList,
                    static_cast<std::size_t>(index) * sizeof(void*));
                if (running && clip &&
                    At<int>(running, 0x8) == At<int>(clip, 0x8)) {
                    TraceTraversal("lean_running", index,
                        static_cast<int>(At<float>(running, 0x30) * 1000.0F),
                        static_cast<int>(At<float>(running, 0x34) * 1000.0F));
                }
            }
            const void* currentSet = At<void*>(clientHuman, 0x180);
            const int currentCount = currentSet ? At<int>(currentSet, 0x28) : -1;
            TraceTraversal("lean_set", currentCount,
                HumanHasAnimationHandle(clientHuman, "idle") ? 1 : 0,
                FindAnimationBinding(FindAnimationSet("rifle"), leanHandle)
                    ? 1 : 0);
            char path[MAX_PATH]{};
            const DWORD pathLength = GetTempPathA(MAX_PATH, path);
            constexpr char traceName[] = "dk2_direct_action_lean_anims.txt";
            if (pathLength && pathLength + sizeof(traceName) <= sizeof(path)) {
                std::memcpy(path + pathLength, traceName, sizeof(traceName));
                HANDLE file = CreateFileA(path, FILE_APPEND_DATA,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, nullptr);
                if (file != INVALID_HANDLE_VALUE) {
                    char line[160]{};
                    const int length = std::snprintf(line, sizeof(line),
                        "%llu side=%d primary=%.32s secondary=%.32s set_count=%d\r\n",
                        static_cast<unsigned long long>(now), g_leanSide,
                        anims, anims + 0x44, currentCount);
                    if (length > 0 && length < static_cast<int>(sizeof(line))) {
                        DWORD written = 0;
                        WriteFile(file, line, static_cast<DWORD>(length),
                            &written, nullptr);
                    }
                    CloseHandle(file);
                }
            }
            lastLeanPoseTrace = now;
        }
        if (moveHandleAvailable) {
            std::memcpy(anims + 0x44, moveLeanHandle,
                std::strlen(moveLeanHandle) + 1);
            At<int>(bodyAnims, 0x84) = 0;
        } else if ((idle || firing) && handleAvailable) {
            std::memcpy(anims + 0x44, leanHandle,
                std::strlen(leanHandle) + 1);
            At<int>(bodyAnims, 0x84) = 0;
        } else if (lowHandleAvailable) {
            std::memcpy(anims, lowLeanHandle,
                std::strlen(lowLeanHandle) + 1);
            At<int>(bodyAnims, 0x40) = 0;
        }
    }
    const bool crowbarSelected = g_crowbarSelectedAt != 0;
    const bool spyCameraSelected = g_spyCameraSelectedAt != 0;
    if (!g_enabled || !state || !bodyAnims ||
        clientHuman != g_controlledClientHuman ||
        (!crowbarSelected && !spyCameraSelected) ||
        At<int>(state, 0x8C) != g_selectedGearInventoryType) {
        return;
    }
    const ULONGLONG now = GetTickCount64();
    static ULONGLONG lastTrace = 0;
    if (now - lastTrace >= 750) {
        TraceTraversal(crowbarSelected ? "crowbar_anim" : "spycam_anim",
            At<int>(state, 0x50),
            At<int>(state, 0x90),
            (std::strcmp(static_cast<const char*>(bodyAnims),
                "operate_equipment") == 0 ? 1 : 0) |
            (std::strcmp(static_cast<const char*>(bodyAnims) + 0x44,
                "operate_equipment") == 0 ? 2 : 0));
        lastTrace = now;
    }
    // Preserve the actual Crowbar breach and active Spy Camera use. Only
    // clear stale tool-use animations while a selected item is being held.
    const ULONGLONG selectedAt = crowbarSelected
        ? g_crowbarSelectedAt : g_spyCameraSelectedAt;
    if (At<int>(state, 0x90) != 3 ||
        (crowbarSelected && At<int>(state, 0x50) == 12) ||
        (spyCameraSelected && (g_spyCameraActionActive || At<int>(state, 0x50) == 15))) {
        return;
    }
    (void)selectedAt;
    auto* anims = static_cast<char*>(bodyAnims);
    const bool replaceFirst =
        std::strcmp(anims, "operate_equipment") == 0 ||
        std::strcmp(anims, "change_in") == 0 || std::strncmp(anims, "grenade", 7) == 0;
    const bool replaceSecond =
        std::strcmp(anims + 0x44, "operate_equipment") == 0 ||
        std::strcmp(anims + 0x44, "change_in") == 0 || std::strncmp(anims + 0x44, "grenade", 7) == 0;
    if (replaceFirst) {
        const char* idle = At<bool>(state, 0x31) ? "idle_crouch" : "idle";
        std::memcpy(anims, idle, std::strlen(idle) + 1);
        At<int>(bodyAnims, 0x40) = 0;
    }
    if (replaceSecond) {
        const char* idle = At<bool>(state, 0x31) ? "idle_crouch" : "idle";
        std::memcpy(anims + 0x44, idle, std::strlen(idle) + 1);
        At<int>(bodyAnims, 0x84) = 0;
    }
    if (replaceFirst || replaceSecond) {
        At<bool>(bodyAnims, 0x88) = false;
        At<bool>(bodyAnims, 0x89) = false;
        static ULONGLONG lastOverrideTrace = 0;
        if (now - lastOverrideTrace >= 750) {
            TraceTraversal(crowbarSelected ? "crowbar_idle_override" :
                "spycam_idle_override", At<int>(state, 0x50),
                At<int>(state, 0x90),
                (replaceFirst ? 1 : 0) | (replaceSecond ? 2 : 0));
            lastOverrideTrace = now;
        }
    }
}

void AccelerateSecondaryPutAway(void* human, int dt) {
    if (!human || dt <= 0 || At<float>(human, kHumanHealthOffset) <= 0) return;
    const int read = At<int>(human, kHumanServerCommandReadOffset);
    const int queued = At<int>(human, kHumanServerCommandWriteOffset) - read;
    if (queued <= 0 || queued > 32) return;
    const void* command = static_cast<const std::byte*>(human) + kHumanServerCommandArrayOffset +
        static_cast<std::size_t>(read & 31) * kHumanServerCommandSize;
    const int targetSlot = At<int>(command, 0x58);
    if (At<int>(command, 0) != 0 || (targetSlot != 1 && targetSlot != kSecondaryWeaponInventoryType)) return;
    const void* target = At<void*>(human, kHumanServerInventoryOffset + targetSlot * sizeof(void*));
    const void* targetTemplate = target ? At<void*>(target, 0x20) : nullptr;
    void* outgoing = At<void*>(human, kHumanServerEquippedItemOffset);
    if (!targetTemplate || At<int>(targetTemplate, 0x20) != 0 || !outgoing || outgoing == target ||
        At<int>(outgoing, 0x2C) != 2) return;
    const int elapsed = At<int>(outgoing, 0x34), duration = At<int>(outgoing, 0x38);
    if (elapsed < 0 || duration <= 1 || elapsed >= duration - 1) return;
    // Preserve the native completion step: reaching duration directly would
    // bypass Equipment::Update's transition to the next state.
    const int extra = std::min(std::max(1, dt / 2), duration - 1 - elapsed);
    At<int>(outgoing, 0x34) = elapsed + extra;
    static ULONGLONG last = 0;
    const ULONGLONG now = GetTickCount64();
    if (now - last >= 100) {
        char line[240]{};
        std::snprintf(line, sizeof(line), "%llu EQUIP target=%d outgoing=%p state=2 elapsed=%d duration=%d dt=%d extra=%d\r\n",
            static_cast<unsigned long long>(now), targetSlot, outgoing, elapsed, duration, dt, extra);
        TraceItemDetail(line); last = now;
    }
}

void RestoreHeldSpyCameraPeripheralVision(void* human) {
    if (!human || At<float>(human, kHumanHealthOffset) <= 0 || At<std::uint32_t>(human, 0x1220) != 0) return;
    const void* item = At<void*>(human, kHumanServerEquippedItemOffset);
    const void* itemTemplate = item ? At<void*>(item, 0x20) : nullptr;
    const void* unitTemplate = At<void*>(human, 0x20);
    if (!itemTemplate || !unitTemplate || At<int>(itemTemplate, 0x20) != 4) return;
    const float radius = At<float>(unitTemplate, 0x174);
    if (std::isfinite(radius) && radius > 0)
        At<float>(human, 0x28C) = std::max(At<float>(human, 0x28C), radius);
}

void __fastcall HookedHumanServerUpdate(void* serverHuman, int dt) {
    static ULONGLONG lastRemotePruneTick = 0;
    const ULONGLONG pruneNow = GetTickCount64();
    if (pruneNow - lastRemotePruneTick >= 5000) {
        for (auto it = g_remoteInputs.begin();
             it != g_remoteInputs.end();) {
            if (GetServerHuman(it->first) != it->second.human) {
                it = g_remoteInputs.erase(it);
            } else {
                ++it;
            }
        }
        lastRemotePruneTick = pruneNow;
    }
    const std::uint32_t serverHumanId = At<std::uint32_t>(
        serverHuman, kHumanIdOffset);
    auto remote = g_remoteInputs.find(serverHumanId);
    if (remote != g_remoteInputs.end() &&
        remote->second.human != serverHuman) {
        g_remoteInputs.erase(remote);
        remote = g_remoteInputs.end();
    }
    if (remote != g_remoteInputs.end() &&
        remote->second.human == serverHuman) {
        RemoteInput& input = remote->second;
        const bool authorized = RemoteInputIsAuthorized(input);
        if (authorized) EnsureUniversalTools(serverHuman);
        if (!authorized ||
            GetTickCount64() - input.lastReceived > 500) {
            CancelDoorInteraction(serverHuman, input.doorInteraction);
            input.movement = {};
            input.fireHeld = false;
            input.readyHold = kReadyHoldNone;
            input.shotRemainingMs = 0;
            input.autoFire = false;
            input.sprintHeld = false;
            input.interactHeld = false;
            input.lastInteractedDoorId = 0;
            if (input.crouchHeld) {
                input.crouchHeld = false;
                input.crouchCommandPending = true;
            }
            if (authorized && input.crouchCommandPending) {
                input.crouchCommandPending = false;
                g_queueCrouch(serverHuman, false);
                TraceTraversal("coop_crouch_timeout",
                    static_cast<int>(serverHumanId), 0, 0);
            }
            input.reloadRequested = false;
            input.cancelReloadRequested = false;
            input.grenadeRequested = false;
            input.grenadeNativePreview = false;
            input.equipmentRequest = kNoEquipmentRequest;
            input.equipQueued = false;
            if (!authorized) {
                input.reloadCanceledWeapon = nullptr;
            }
            StopDirectSprint(serverHuman);
        } else {
            if (input.cancelReloadRequested) {
                input.cancelReloadRequested = false;
                input.reloadRequested = false;
                void* item = At<void*>(serverHuman,
                    kHumanServerEquippedItemOffset);
                if (InventoryItemIsFirearm(item) &&
                    At<int>(item, 0x2C) == kFirearmReloadingState) {
                    g_firearmCancelReload(item);
                }
                RemoveQueuedReload(serverHuman);
                input.reloadCanceledWeapon = InventoryItemIsFirearm(item)
                    ? item : nullptr;
                TraceTraversal("coop_reload_cancel", static_cast<int>(
                    serverHumanId), 0, 0);
            }
            void* equippedItem = At<void*>(serverHuman,
                kHumanServerEquippedItemOffset);
            if (input.reloadCanceledWeapon &&
                equippedItem != input.reloadCanceledWeapon) {
                input.reloadCanceledWeapon = nullptr;
            }
            if (input.reloadCanceledWeapon) {
                if (FirearmIsReloading(serverHuman)) {
                    g_firearmCancelReload(input.reloadCanceledWeapon);
                }
                RemoveQueuedReload(serverHuman);
            }
            if (input.grenadeRequested) {
                input.grenadeRequested = false;
                const bool useNativePreview = input.grenadeNativePreview;
                input.grenadeNativePreview = false;
                const ULONGLONG now = GetTickCount64();
                const int inventoryType = ResolveGrenadeChoice(serverHuman, input.grenadeThrowChoice);
                const int commandsBefore =
                    At<int>(serverHuman, kHumanServerCommandWriteOffset) -
                    At<int>(serverHuman, kHumanServerCommandReadOffset);
                if (inventoryType >= 0 && commandsBefore <= 27 &&
                    (input.lastGrenadeThrow == 0 ||
                     now - input.lastGrenadeThrow >= 500)) {
                    const Vector3 origin = At<Vector3>(
                        serverHuman, kHumanOriginOffset);
                    Vector3 throwOrigin = origin;
                    if (useNativePreview) {
                        throwOrigin.x += input.grenadeOriginOffset.x;
                        throwOrigin.y += input.grenadeOriginOffset.y;
                        throwOrigin.z += input.grenadeOriginOffset.z;
                    } else {
                        throwOrigin.y += kGrenadeLaunchHeightOffset;
                    }
                    const float throwDistance = input.grenadeLongThrow
                        ? std::clamp(input.distance, 0.5F, 30.0F)
                        : kGrenadeQuickThrowDistance;
                    const Vector3 target = useNativePreview
                        ? Vector3{
                            origin.x + input.grenadeTargetOffset.x,
                            origin.y + input.grenadeTargetOffset.y,
                            origin.z + input.grenadeTargetOffset.z}
                        : Vector3{
                            origin.x + input.aim.x * throwDistance,
                            origin.y,
                            origin.z + input.aim.z * throwDistance};
                    const float dx = target.x - throwOrigin.x;
                    const float dz = target.z - throwOrigin.z;
                    if (dx * dx + dz * dz >= 0.25F) {
                        input.shotRemainingMs = 0;
                        input.autoFire = false;
                        input.equipQueued = false;
                        QueueRememberedGrenadeThrow(serverHuman, input.grenadeThrowChoice,
                            InventoryItemIsGrenade(At<void*>(serverHuman, kHumanServerEquippedItemOffset))
                                ? input.preItemFirearmInventoryType : -1,
                            throwOrigin, useNativePreview
                                ? input.grenadeAngle
                                : kGrenadeThrowAngleDegrees, target);
                        const int commandsAfter =
                            At<int>(serverHuman,
                                kHumanServerCommandWriteOffset) -
                            At<int>(serverHuman,
                                kHumanServerCommandReadOffset);
                        if (commandsAfter > commandsBefore) {
                            input.lastGrenadeThrow = now;
                            input.selectedGearInventoryType = -1;
                            input.preItemFirearmInventoryType = -1;
                            TraceTraversal("coop_grenade_host",
                                static_cast<int>(serverHumanId),
                                inventoryType,
                                useNativePreview ? 2 :
                                    input.grenadeLongThrow ? 1 : 0);
                        }
                    }
                }
            }
            if (input.reloadRequested) {
                input.reloadRequested = false;
                input.reloadCanceledWeapon = nullptr;
                input.shotRemainingMs = 0;
                input.autoFire = false;
                input.equipQueued = false;
                if (!FirearmIsReloading(serverHuman) &&
                    !HasQueuedReload(serverHuman)) {
                    void* command = g_createQueueCommand(serverHuman,
                        kReloadCommandType,
                        reinterpret_cast<const void*>(
                            g_moduleBase + kRvaProcessCmdFirearmReload),
                        reinterpret_cast<const void*>(
                            g_moduleBase + kRvaActionDummy));
                    if (command) {
                        At<int>(command, 0x08) = 0;
                        TraceTraversal("coop_reload_host", static_cast<int>(
                            serverHumanId), 0, 0);
                    }
                }
            }
            const bool movingForInteraction = input.movement.x != 0.0F || input.movement.z != 0.0F;
            if (input.interactHeld || input.doorInteraction.down || input.doorInteraction.pendingTap || input.doorInteraction.lockpickActive) {
                if (ProcessDoorInteraction(serverHuman, input.doorInteraction, input.interactHeld,
                        {input.aim.x, input.aim.z}, movingForInteraction,
                        input.equipmentRequest != kNoEquipmentRequest || input.reloadRequested || input.fireHeld,
                        GetTickCount64())) {
                    input.movement = {};
                    input.fireHeld = false;
                    input.shotRemainingMs = 0;
                    input.autoFire = false;
                    input.sprintHeld = false;
                }
            }
            if (input.crouchCommandPending) {
                input.crouchCommandPending = false;
                g_queueCrouch(serverHuman, input.crouchHeld);
                TraceTraversal("coop_crouch_host", static_cast<int>(
                    serverHumanId), input.crouchHeld ? 1 : 0, 0);
            }
            if (input.sprintHeld && !input.interactHeld &&
                (input.movement.x != 0.0F || input.movement.z != 0.0F) &&
                !FirearmIsReloading(serverHuman) &&
                !HasQueuedReload(serverHuman)) {
                const std::uint32_t movementFlags = At<std::uint32_t>(
                    serverHuman, kHumanServerMovementFlagsOffset);
                if ((movementFlags & 0x40U) == 0 &&
                    !HasQueuedSprint(serverHuman)) {
                    if ((movementFlags & 0x20U) != 0) {
                        g_queueCrouch(serverHuman, false);
                    }
                    g_createQueueCommand(serverHuman, kSprintCommandType,
                        reinterpret_cast<const void*>(
                            g_moduleBase + kRvaProcessCmdSprinting), nullptr);
                    TraceTraversal("coop_sprint_host", static_cast<int>(
                        serverHumanId), 1, 0);
                }
            } else {
                StopDirectSprint(serverHuman);
            }
            if (input.equipmentRequest != kNoEquipmentRequest) {
                const int request = input.equipmentRequest;
                input.equipmentRequest = kNoEquipmentRequest;
                input.shotRemainingMs = 0;
                input.autoFire = false;
                input.equipQueued = false;
                if (request >= 3) {
                    const int inventoryType =
                        ResolveEquipmentNumber(serverHuman, request);
                    if (inventoryType >= 0) {
                        if (input.selectedGearInventoryType < 0) input.preItemFirearmInventoryType = FindFirearmInventoryType(serverHuman);
                        input.selectedGearInventoryType =
                            input.selectedGearInventoryType == inventoryType
                                ? -1 : inventoryType;
                        QueueEquipInventoryType(serverHuman, input.selectedGearInventoryType >= 0
                            ? input.selectedGearInventoryType : input.preItemFirearmInventoryType);
                    }
                } else if (request == kHolsterEquipmentRequest) {
                    input.selectedGearInventoryType = -1;
                    g_createQueueCommand(serverHuman, 1,
                        reinterpret_cast<const void*>(
                            g_moduleBase + kRvaProcessCmdItemHolster),
                        nullptr);
                } else {
                    const int inventoryType =
                        ResolveEquipmentNumber(serverHuman, request);
                    if (inventoryType >= 0) {
                        const auto offset = kHumanServerInventoryOffset +
                            static_cast<std::size_t>(inventoryType) *
                                sizeof(void*);
                        void* requestedItem = At<void*>(serverHuman, offset);
                        if (!InventoryItemIsFirearm(requestedItem)) {
                            input.selectedGearInventoryType =
                                input.selectedGearInventoryType == inventoryType
                                    ? -1 : inventoryType;
                        } else if (equippedItem == requestedItem) {
                            input.selectedGearInventoryType = -1;
                            g_createQueueCommand(serverHuman, 1,
                                reinterpret_cast<const void*>(
                                    g_moduleBase + kRvaProcessCmdItemHolster),
                                nullptr);
                        } else {
                            input.selectedGearInventoryType = -1;
                            void* equipCommand = g_createQueueCommand(
                                serverHuman, 0,
                                reinterpret_cast<const void*>(
                                    g_moduleBase + kRvaProcessCmdItemEquip),
                                nullptr);
                            if (equipCommand) {
                                At<int>(equipCommand, 0x58) = inventoryType;
                            }
                        }
                    }
                }
                TraceTraversal("coop_equip_host", static_cast<int>(
                    serverHumanId), request, 0);
            }
            LookAimCommand command(input.aim);
            g_processCmdLookAim(serverHuman, &command);
            if (FirearmIsReloading(serverHuman) || HasQueuedReload(serverHuman)) {
                input.shotRemainingMs = 0;
                input.autoFire = false;
            }
            if (input.autoFire && ((input.sequence.mode == FireMode::Auto && !input.fireHeld) ||
                (input.sequence.mode == FireMode::Burst && GetTickCount64() > input.sequence.deadline))) {
                input.autoFire = false;
                input.shotRemainingMs = 0;
            }
            if (input.shotRemainingMs > 0) {
                if (!input.autoFire) {
                    input.shotRemainingMs -= std::max(dt, 0);
                }
                void* firearm = At<void*>(
                    serverHuman, kHumanServerEquippedItemOffset);
                if (!InventoryItemIsFirearm(firearm)) {
                    if (!input.equipQueued) {
                        const int inventoryType =
                            FindFirearmInventoryType(serverHuman);
                        if (inventoryType >= 0) {
                            void* equipCommand = g_createQueueCommand(
                                serverHuman, 0,
                                reinterpret_cast<const void*>(
                                    g_moduleBase + kRvaProcessCmdItemEquip),
                                nullptr);
                            if (equipCommand) {
                                At<int>(equipCommand, 0x58) = inventoryType;
                                input.equipQueued = true;
                            }
                        }
                    }
                } else {
                    if (input.autoFire &&
                        (firearm != input.autoFireWeapon ||
                         (At<std::uint32_t>(firearm, 0x3C) & 0xFFFFU) == 0)) {
                        input.autoFire = false;
                        input.shotRemainingMs = 0;
                    }
                    const void* itemTemplate = At<void*>(firearm, 0x20);
                    const auto capabilities = FireCapabilities(itemTemplate);
                    // A click while holstered may use the existing native
                    // auto-equip path. With no client weapon ID yet, use the
                    // host's remembered/default supported mode for that gun.
                    const bool autoEquippedWeapon = input.requestedWeapon == 0 &&
                        (input.equipQueued || (input.autoFire && input.autoFireWeapon == firearm));
                    if (!autoEquippedWeapon && (input.requestedWeapon != FireWeaponId(itemTemplate) ||
                        !(capabilities.mask & (1 << static_cast<int>(input.requestedMode))))) {
                        if (input.shotRemainingMs > 0) TraceTraversal("fire_mode_reject",
                            static_cast<int>(serverHumanId), static_cast<int>(input.requestedMode), capabilities.mask);
                        input.shotRemainingMs = 0; input.autoFire = false;
                    } else if (!autoEquippedWeapon) {
                        SelectedFireMode(serverHumanId, itemTemplate) = input.requestedMode;
                    }
                    const int firearmState = At<int>(firearm, 0x2C);
                    if (input.shotRemainingMs > 0 && firearmState == 3) {
                        g_firearmReadyAim(firearm);
                    } else if (input.shotRemainingMs > 0 &&
                               firearmState == 5) {
                        FirearmShoot shoot{};
                        shoot.targetDist = input.distance;
                        if (g_cmdFirearmShoot(serverHuman, &shoot)) {
                            if (!input.autoFire) {
                                StartFireSequence(input.sequence, SelectedFireMode(serverHumanId, itemTemplate), FireCapabilities(itemTemplate).burst, FireWeaponId(itemTemplate));
                            }
                            input.autoFire = ContinueFireSequence(input.sequence, input.fireHeld);
                            TraceTraversal("fire_selector_shot", static_cast<int>(serverHumanId),
                                static_cast<int>(input.sequence.mode), input.sequence.remaining);
                            input.autoFireWeapon = input.autoFire
                                ? firearm : nullptr;
                            if (!input.autoFire) {
                                input.shotRemainingMs = 0;
                            }
                            input.equipQueued = false;
                            TraceTraversal("coop_fire_host", static_cast<int>(
                                serverHumanId), input.autoFire ? 1 : 0, 0);
                        }
                    }
                }
            }
        }
    }
    if (g_enabled && serverHuman == g_controlledServerHuman) {
        if (g_cancelReloadRequested) {
            g_cancelReloadRequested = false;
            g_reloadRequested = false;
            void* item = At<void*>(
                serverHuman, kHumanServerEquippedItemOffset);
            if (InventoryItemIsFirearm(item) &&
                At<int>(item, 0x2C) == kFirearmReloadingState) {
                g_firearmCancelReload(item);
            }
            RemoveQueuedReload(serverHuman);
            g_reloadCanceledWeapon = InventoryItemIsFirearm(item)
                ? item : nullptr;
            g_reloadSprintBlocked = false;
        }
        void* equippedItem = At<void*>(
            serverHuman, kHumanServerEquippedItemOffset);
        if (g_reloadCanceledWeapon &&
            equippedItem != g_reloadCanceledWeapon) {
            g_reloadCanceledWeapon = nullptr;
        }
        if (g_reloadCanceledWeapon) {
            // Shotguns can queue another shell after CancelReload. Keep the
            // canceled reload cleared until the next explicit R press.
            if (FirearmIsReloading(serverHuman)) {
                g_firearmCancelReload(g_reloadCanceledWeapon);
            }
            RemoveQueuedReload(serverHuman);
        }
        if (g_crouchCommandPending) {
            g_crouchCommandPending = false;
            g_queueCrouch(serverHuman, g_crouchHeld);
        }

        if (g_sprintDesired && !g_reloadSprintBlocked &&
            !FirearmIsReloading(serverHuman)) {
            const bool isCrouched =
                (At<std::uint32_t>(
                     serverHuman, kHumanServerMovementFlagsOffset) &
                 0x20U) != 0;
            const bool isSprinting =
                (At<std::uint32_t>(
                     serverHuman, kHumanServerMovementFlagsOffset) &
                 0x40U) != 0;
            if (!isSprinting && !HasQueuedSprint(serverHuman)) {
                // ActionWP_Server::ActionSprint performs the same stand request
                // before appending its persistent sprint command.
                if (isCrouched) {
                    g_queueCrouch(serverHuman, false);
                }
                g_createQueueCommand(
                    serverHuman,
                    kSprintCommandType,
                    reinterpret_cast<const void*>(
                        g_moduleBase + kRvaProcessCmdSprinting),
                    nullptr);
            }
        } else {
            StopDirectSprint(serverHuman);
        }
    }
    if (g_enabled && serverHuman == g_controlledServerHuman &&
        g_equipmentRequest != kNoEquipmentRequest) {
        const int request = g_equipmentRequest;
        g_equipmentRequest = kNoEquipmentRequest;

        // Never let an earlier click fire later through newly selected gear.
        g_shootRequested = false;
        g_shootRequestRemainingMs = 0;
        g_shootEquipQueued = false;
        g_autoFireActive = false;
        g_autoFireWeapon = nullptr;

        if (request >= 3) {
            const int inventoryType =
                ResolveEquipmentNumber(serverHuman, request);
            if (inventoryType >= 0) {
                SelectUtilityInventoryType(serverHuman, inventoryType);
            }
        } else if (request == kHolsterEquipmentRequest) {
            g_selectedGearInventoryType = -1;
            g_crowbarSelectedAt = 0;
            g_spyCameraSelectedAt = 0;
            g_preItemFirearmInventoryType = -1;
            RestoreCrowbarBindingAfterSwitch();
            g_createQueueCommand(
                serverHuman,
                1,
                reinterpret_cast<const void*>(
                    g_moduleBase + kRvaProcessCmdItemHolster),
                nullptr);
        } else {
            const int inventoryType =
                ResolveEquipmentNumber(serverHuman, request);
            if (inventoryType >= 0) {
                void* equippedItem = At<void*>(
                    serverHuman, kHumanServerEquippedItemOffset);
                const auto offset = kHumanServerInventoryOffset +
                    static_cast<std::size_t>(inventoryType) * sizeof(void*);
                void* requestedItem = At<void*>(serverHuman, offset);
                if (!InventoryItemIsFirearm(requestedItem)) {
                    SelectUtilityInventoryType(serverHuman, inventoryType);
                } else if (equippedItem &&
                    equippedItem == At<void*>(serverHuman, offset)) {
                    g_selectedGearInventoryType = -1;
                    g_crowbarSelectedAt = 0;
                    g_spyCameraSelectedAt = 0;
                    g_preItemFirearmInventoryType = -1;
                    RestoreCrowbarBindingAfterSwitch();
                    g_createQueueCommand(
                        serverHuman,
                        1,
                        reinterpret_cast<const void*>(
                            g_moduleBase + kRvaProcessCmdItemHolster),
                        nullptr);
                } else {
                    g_selectedGearInventoryType = -1;
                    g_crowbarSelectedAt = 0;
                    g_spyCameraSelectedAt = 0;
                    g_preItemFirearmInventoryType = -1;
                    RestoreCrowbarBindingAfterSwitch();
                    void* command = g_createQueueCommand(
                        serverHuman,
                        0,
                        reinterpret_cast<const void*>(
                            g_moduleBase + kRvaProcessCmdItemEquip),
                        nullptr);
                    if (command) {
                        At<int>(command, 0x58) = inventoryType;
                    }
                }
            }
        }
    }
    if (g_enabled && serverHuman == g_controlledServerHuman &&
        g_grenadeThrowRequested) {
        g_grenadeThrowRequested = false;
        const int inventoryType = ResolveGrenadeChoice(serverHuman, g_grenadeThrowChoice);
        const bool useNativePreview = g_grenadeThrowUsesNativePreview &&
            inventoryType == g_grenadeNativeThrowInventoryType;
        g_grenadeThrowUsesNativePreview = false;
        if (inventoryType >= 0) {
            const int commandsBefore =
                At<int>(serverHuman, kHumanServerCommandWriteOffset) -
                At<int>(serverHuman, kHumanServerCommandReadOffset);
            if (commandsBefore <= 27 && g_grenadePendingSlot < 0) {
                void* grenadeItem = At<void*>(
                    serverHuman,
                    kHumanServerInventoryOffset +
                        static_cast<std::size_t>(inventoryType) *
                            sizeof(void*));
                Vector3 throwOrigin = useNativePreview
                    ? g_grenadeNativeThrowOrigin
                    : At<Vector3>(serverHuman, kHumanOriginOffset);
                if (!useNativePreview) {
                    throwOrigin.y += kGrenadeLaunchHeightOffset;
                }
                const float targetDx =
                    g_grenadeTarget.x - throwOrigin.x;
                const float targetDz =
                    g_grenadeTarget.z - throwOrigin.z;
                const float targetDistanceSquared =
                    targetDx * targetDx + targetDz * targetDz;
                if (std::isfinite(targetDistanceSquared) &&
                    targetDistanceSquared >= 0.25F) {
                    g_shootRequested = false;
                    g_shootRequestRemainingMs = 0;
                    g_shootEquipQueued = false;
                    g_autoFireActive = false;
                    g_autoFireWeapon = nullptr;
                    // Held-G uses the same launch point, target, and angle
                    // that DK2's ActionGrenade server action would pass here.
                    // Quick toss retains its established direct-control arc.
                    QueueRememberedGrenadeThrow(serverHuman, g_grenadeThrowChoice,
                        InventoryItemIsGrenade(At<void*>(serverHuman, kHumanServerEquippedItemOffset))
                            ? g_preItemFirearmInventoryType : -1,
                        throwOrigin, useNativePreview ? g_grenadeNativeThrowAngle : kGrenadeThrowAngleDegrees,
                        g_grenadeTarget);
                    const int commandsAfter =
                        At<int>(serverHuman,
                            kHumanServerCommandWriteOffset) -
                        At<int>(serverHuman,
                            kHumanServerCommandReadOffset);
                    if (commandsAfter > commandsBefore) {
                        g_selectedGearInventoryType = -1;
                        g_selectedGrenadeHotkey = 0;
                        g_selectedSpyCameraHotkey = 0;
                        g_selectedCrowbarHotkey = 0;
                        g_preItemFirearmInventoryType = -1;
                        RestoreCrowbarBindingAfterSwitch();
                        g_grenadePendingSlot = inventoryType;
                        g_grenadePendingItem = grenadeItem;
                        g_grenadePendingQuantity =
                            At<int>(grenadeItem, 0x3C);
                        g_grenadePendingMs = kGrenadeThrowTimeoutMs;
                        SetCrosshairFeedback(kGrenadeQueuedColor);
                    } else {
                        SetCrosshairFeedback(kGrenadeUnavailableColor);
                    }
                } else {
                    SetCrosshairFeedback(kGrenadeUnavailableColor);
                }
            } else {
                SetCrosshairFeedback(kGrenadeUnavailableColor);
            }
        } else {
            SetCrosshairFeedback(kGrenadeUnavailableColor);
        }
    }
    if (g_enabled && serverHuman == g_controlledServerHuman &&
        g_reloadRequested) {
        g_reloadRequested = false;
        g_shootRequested = false;
        g_autoFireActive = false;
        g_autoFireWeapon = nullptr;
        if (!FirearmIsReloading(serverHuman) &&
            !HasQueuedReload(serverHuman)) {
            void* command = g_createQueueCommand(
                serverHuman,
                kReloadCommandType,
                reinterpret_cast<const void*>(
                    g_moduleBase + kRvaProcessCmdFirearmReload),
                reinterpret_cast<const void*>(g_moduleBase + kRvaActionDummy));
            if (command) {
                At<int>(command, 0x08) = 0;
            }
        }
    }
    if (g_enabled && serverHuman == g_controlledServerHuman &&
        g_shootRequested) {
        if (g_autoFireActive &&
            ((g_fireSequence.mode == FireMode::Auto && !g_fireHeld) ||
             (g_fireSequence.mode == FireMode::Burst && GetTickCount64() > g_fireSequence.deadline) ||
             At<void*>(serverHuman, kHumanServerEquippedItemOffset) !=
                 g_autoFireWeapon)) {
            g_shootRequested = false;
            g_autoFireActive = false;
            g_autoFireWeapon = nullptr;
        }
        if (!g_autoFireActive) {
            g_shootRequestRemainingMs -= std::max(dt, 0);
        }
        if (g_shootRequested && g_shootRequestRemainingMs <= 0) {
            g_shootRequested = false;
            g_shootEquipQueued = false;
            SetCrosshairFeedback(kCrosshairExpiredColor);
        } else if (g_shootRequested) {
            void* firearm = At<void*>(
                serverHuman, kHumanServerEquippedItemOffset);
            if (!InventoryItemIsFirearm(firearm)) {
                if (!g_shootEquipQueued) {
                    const int inventoryType =
                        FindFirearmInventoryType(serverHuman);
                    if (inventoryType >= 0) {
                        void* command = g_createQueueCommand(
                            serverHuman,
                            0,
                            reinterpret_cast<const void*>(
                                g_moduleBase + kRvaProcessCmdItemEquip),
                            nullptr);
                        if (command) {
                            At<int>(command, 0x58) = inventoryType;
                            g_shootEquipQueued = true;
                        }
                    }
                }
            } else {
                if (g_autoFireActive &&
                    (At<std::uint32_t>(firearm, 0x3C) & 0xFFFFU) == 0) {
                    g_shootRequested = false;
                    g_autoFireActive = false;
                    g_autoFireWeapon = nullptr;
                    SetCrosshairFeedback(kCrosshairExpiredColor);
                }
                const int firearmState = At<int>(firearm, 0x2C);
                if (g_shootRequested && firearmState == 3) {
                    // Match AI::Human_ReadyFirearm: transition a guarded
                    // firearm into its native aim-ready animation/state.
                    g_firearmReadyAim(firearm);
                } else if (g_shootRequested && firearmState == 5) {
                    FirearmShoot shoot{};
                    const float lengthSquared =
                        g_shootDirection.x * g_shootDirection.x +
                        g_shootDirection.z * g_shootDirection.z;
                    shoot.targetDist = std::sqrt(lengthSquared);
                    if (g_cmdFirearmShoot(serverHuman, &shoot)) {
                        if (!g_autoFireActive) {
                            const void* itemTemplate = At<void*>(firearm, 0x20);
                            StartFireSequence(g_fireSequence, SelectedFireMode(serverHumanId, itemTemplate), FireCapabilities(itemTemplate).burst, FireWeaponId(itemTemplate));
                        }
                        if (ContinueFireSequence(g_fireSequence, g_fireHeld)) {
                            g_autoFireActive = true;
                            g_autoFireWeapon = firearm;
                        } else {
                            g_shootRequested = false;
                            g_autoFireActive = false;
                            g_autoFireWeapon = nullptr;
                        }
                        TraceTraversal("fire_selector_shot", static_cast<int>(serverHumanId),
                            static_cast<int>(g_fireSequence.mode), g_fireSequence.remaining);
                        g_shootEquipQueued = false;
                        g_lastAcceptedShotTick = GetTickCount64();
                        SetCrosshairFeedback(kCrosshairFiredColor);
                    }
                }
            }
        }
    }
    g_originalHumanServerUpdate(serverHuman, dt);
    if (g_enabled && (serverHuman == g_controlledServerHuman ||
        (remote != g_remoteInputs.end() && RemoteInputIsAuthorized(remote->second)))) {
        AccelerateSecondaryPutAway(serverHuman, dt);
        RestoreHeldSpyCameraPeripheralVision(serverHuman);
    }
    int readyHold = kReadyHoldNone;
    int* previousReadyHold = nullptr;
    if (g_enabled && serverHuman == g_controlledServerHuman) {
        readyHold = g_selectedGearInventoryType < 0
            ? g_readyHold : kReadyHoldNone;
        previousReadyHold = &g_previousReadyHold;
    } else {
        const auto readyRemote = g_remoteInputs.find(serverHumanId);
        if (readyRemote != g_remoteInputs.end() &&
            readyRemote->second.human == serverHuman) {
            previousReadyHold = &readyRemote->second.previousReadyHold;
        }
        if (previousReadyHold &&
            readyRemote->second.selectedGearInventoryType < 0 &&
            RemoteInputIsAuthorized(readyRemote->second) &&
            GetTickCount64() - readyRemote->second.lastReceived <= 500) {
            readyHold = readyRemote->second.readyHold;
        }
    }
    void* readyFirearm = At<void*>(serverHuman,
        kHumanServerEquippedItemOffset);
    if (previousReadyHold) {
        if (*previousReadyHold != readyHold &&
            InventoryItemIsFirearm(readyFirearm)) {
            TraceTraversal("ready_state", *previousReadyHold, readyHold,
                At<int>(readyFirearm, 0x2C));
        }
        if (*previousReadyHold == kReadyHoldLow &&
            readyHold != kReadyHoldLow) {
            ReleaseLowReady(readyFirearm);
        }
        // Native movement/aim logic can return to state 5 while the button
        // stays held. Reissuing Guard then restarts lowerweapon every time.
        if (readyHold != kReadyHoldLow ||
            *previousReadyHold != kReadyHoldLow) {
            ApplyReadyHold(readyFirearm, readyHold);
        }
        *previousReadyHold = readyHold;
    }
    if (g_enabled && serverHuman == g_controlledServerHuman) {
        TraceLeanClearance(serverHuman);
    }
    auto remoteAfterUpdate = g_remoteInputs.find(serverHumanId);
    if (remoteAfterUpdate != g_remoteInputs.end() &&
        remoteAfterUpdate->second.human == serverHuman &&
        RemoteInputIsAuthorized(remoteAfterUpdate->second) &&
        remoteAfterUpdate->second.reloadCanceledWeapon) {
        RemoteInput& input = remoteAfterUpdate->second;
        if (At<void*>(serverHuman, kHumanServerEquippedItemOffset) !=
            input.reloadCanceledWeapon) {
            input.reloadCanceledWeapon = nullptr;
        } else if (FirearmIsReloading(serverHuman)) {
            g_firearmCancelReload(input.reloadCanceledWeapon);
        }
        if (input.reloadCanceledWeapon) {
            RemoveQueuedReload(serverHuman);
        }
    }
    if (g_enabled && serverHuman == g_controlledServerHuman &&
        g_reloadCanceledWeapon) {
        if (At<void*>(serverHuman, kHumanServerEquippedItemOffset) !=
            g_reloadCanceledWeapon) {
            g_reloadCanceledWeapon = nullptr;
        } else if (FirearmIsReloading(serverHuman)) {
            g_firearmCancelReload(g_reloadCanceledWeapon);
        }
        if (g_reloadCanceledWeapon) {
            RemoveQueuedReload(serverHuman);
        }
    }
    if (g_enabled && serverHuman == g_controlledServerHuman &&
        g_reloadSprintBlocked && !g_reloadRequested &&
        !FirearmIsReloading(serverHuman) &&
        !HasQueuedReload(serverHuman)) {
        g_reloadSprintBlocked = false;
    }
    if (g_enabled && serverHuman == g_controlledServerHuman &&
        g_grenadePendingSlot >= 0) {
        void* currentItem = At<void*>(
            serverHuman,
            kHumanServerInventoryOffset +
                static_cast<std::size_t>(g_grenadePendingSlot) *
                    sizeof(void*));
        if (currentItem != g_grenadePendingItem ||
            (currentItem &&
             At<int>(currentItem, 0x3C) < g_grenadePendingQuantity)) {
            g_grenadePendingSlot = -1;
            g_grenadePendingItem = nullptr;
            SetCrosshairFeedback(kGrenadeThrownColor);
        } else {
            g_grenadePendingMs -= std::max(dt, 0);
            if (g_grenadePendingMs <= 0) {
                g_grenadePendingSlot = -1;
                g_grenadePendingItem = nullptr;
                SetCrosshairFeedback(kGrenadeUnavailableColor);
            }
        }
    }
}

int __fastcall HookedHumanTryShootingFirearm(
    void* serverHuman,
    void* firearmTarget) {
    if (g_enabled && serverHuman == g_controlledServerHuman) {
        // BrainPlayer's normal combat loop is a separate shot producer from
        // direct-control clicks. Returning the routine's native "no shot"
        // result disables only that automatic producer for this operator.
        return 0;
    }
    return g_originalHumanTryShootingFirearm(serverHuman, firearmTarget);
}

void __fastcall HookedRenderPath(
    void* human,
    void* selectedHuman,
    void* hoveredHuman,
    std::uint32_t* colors,
    int arg5,
    int arg6,
    int arg7,
    int arg8,
    int arg9,
    int arg10,
    int arg11,
    int arg12,
    void* lights) {
    // The grenade trajectory is rendered later in RenderPaths. Its temporary
    // planning flags must not expose the hidden WASD path (or other plans).
    if (g_renderingGrenadePreviewPaths) {
        return;
    }
    g_originalRenderPath(
        human, selectedHuman, hoveredHuman, colors,
        arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, lights);
}

void __fastcall HookedRenderPaths(void* renderer) {
    if (g_enabled) {
        // Let DK2 draw its own grenade target, trajectory, and effect-radius
        // preview. All ordinary planned paths stay hidden outside G aiming.
        void* potential = *reinterpret_cast<void**>(
            g_moduleBase + kRvaPotentialActionWaypointGlobal);
        if (renderer && g_grenadeAiming &&
            g_nativeGrenadePreviewWaypoint &&
            potential == g_nativeGrenadePreviewWaypoint) {
            // RenderPaths skips its entire pass if waypoints or plans are
            // disabled, and skips the trajectory objects separately when
            // actionWaypoints is disabled. Live direct control can leave
            // those ordinary planning flags off. Temporarily enable them
            // only while DK2 renders its own grenade trajectory.
            bool& plans = At<bool>(renderer, 0x08);
            bool& actionWaypoints = At<bool>(renderer, 0x15);
            int& waypoints = At<int>(renderer, 0x3C);
            const bool savedPlans = plans;
            const bool savedActionWaypoints = actionWaypoints;
            const int savedWaypoints = waypoints;
            plans = true;
            actionWaypoints = true;
            if (waypoints == 0) {
                waypoints = 1;
            }
            g_renderingGrenadePreviewPaths = true;
            g_originalRenderPaths(renderer);
            g_renderingGrenadePreviewPaths = false;
            waypoints = savedWaypoints;
            actionWaypoints = savedActionWaypoints;
            plans = savedPlans;
            return;
        }
        if (g_cursorWorldValid && !g_grenadeAiming) {
            ++g_crosshairRenderCount;
            const std::uint32_t crosshairColor = GetCrosshairColor();
            const Vector3 center{
                g_cursorWorld.x,
                g_cursorWorld.y + kCrosshairHeightOffset,
                g_cursorWorld.z,
            };
            RenderCrosshairBars(renderer, center, crosshairColor);
        }
        return;
    }
    g_originalRenderPaths(renderer);
}

bool __fastcall HookedAdvanceAlongPath(
    void* waypoints,
    const Vector3* currentPosition,
    float distance,
    int dt,
    Vector3* outputPosition) {
    const bool result = g_originalAdvanceAlongPath(
        waypoints, currentPosition, distance, dt, outputPosition);

    // A queued traversal belongs to DK2's path and is attached to one of its
    // waypoints. Let native advancement reach that waypoint before resuming
    // direct steering; clamping every step to WASD kept the jump unreachable.
    if (GetTickCount64() < g_traversalPathHoldUntil) {
        return result;
    }

    auto* controlledServerWaypoints = g_controlledServerHuman
        ? static_cast<std::byte*>(g_controlledServerHuman) +
            kHumanServerWaypointsOffset
        : nullptr;
    auto* controlledClientWaypoints = g_controlledClientHuman
        ? static_cast<std::byte*>(g_controlledClientHuman) + kHumanPathOffset
        : nullptr;
    const bool isServer = waypoints == controlledServerWaypoints;
    const bool isClient = waypoints == controlledClientWaypoints;
    Vector2 direction = g_directMovementDirection;
    bool remoteServer = false;
    const ULONGLONG now = GetTickCount64();
    for (const auto& [id, input] : g_remoteInputs) {
        (void)id;
        if (input.human &&
            waypoints == static_cast<std::byte*>(input.human) +
                kHumanServerWaypointsOffset &&
            now - input.lastReceived <= 500 &&
            RemoteInputIsAuthorized(input)) {
            direction = input.movement;
            remoteServer = true;
            break;
        }
    }
    if (((g_enabled && (isServer || isClient)) || remoteServer) &&
        currentPosition &&
        outputPosition && std::isfinite(distance) && distance > 0.0F &&
        (direction.x != 0.0F || direction.z != 0.0F)) {
        void* game = *reinterpret_cast<void**>(
            g_moduleBase + ((isServer || remoteServer)
                ? kRvaGameServerGlobal : kRvaGameClientGlobal));
        const void* pathfinder = game
            ? At<void*>(game, kGamePathfinderOffset)
            : nullptr;
        const Vector2 start{currentPosition->x, currentPosition->z};
        const Vector2 proposed{
            currentPosition->x + direction.x * distance,
            currentPosition->z + direction.z * distance,
        };
        const Vector2 clamped = ClampMovementToPathfinder(
            pathfinder, start, proposed);

        // Keep path-derived height, speed, and animation state. Both client
        // prediction and server authority receive the same horizontal movement.
        outputPosition->x = clamped.x;
        outputPosition->z = clamped.z;
        return true;
    }
    return result;
}

float __fastcall HookedGetMoveSpeed(const void* serverHuman) {
    const float originalSpeed = g_originalGetMoveSpeed(serverHuman);
    Vector2 direction = g_directMovementDirection;
    bool remoteServer = false;
    for (const auto& [id, input] : g_remoteInputs) {
        (void)id;
        if (input.human == serverHuman &&
            GetTickCount64() - input.lastReceived <= 500 &&
            RemoteInputIsAuthorized(input)) {
            direction = input.movement;
            remoteServer = true;
            break;
        }
    }
    if (!(remoteServer ||
          (g_enabled && serverHuman == g_controlledServerHuman)) ||
        (direction.x == 0.0F && direction.z == 0.0F) ||
        !std::isfinite(originalSpeed) || originalSpeed > 0.0F) {
        return originalSpeed;
    }

    // The stock routine returns zero after the hidden path cursor reaches the
    // final waypoint. Path additions arrive in small batches, which otherwise
    // creates a stop/start pulse even while WASD remains held. Keep the native
    // calculated speed only for this exhausted-path case; command and movement
    // state restrictions continue to use the stock result.
    const int waypointIndex = At<int>(serverHuman, 0x15C);
    const int waypointCount = At<int>(serverHuman, 0x180);
    const std::uint32_t movementFlags = At<std::uint32_t>(serverHuman, 0x1FC);
    const float calculatedSpeed = At<float>(serverHuman, 0x290);
    if (waypointCount > 0 && waypointIndex >= waypointCount &&
        (movementFlags & 0x81U) == 0 &&
        std::isfinite(calculatedSpeed) && calculatedSpeed > 0.0F) {
        return calculatedSpeed;
    }
    return originalSpeed;
}

void __fastcall HookedUpdateOrientationOnWaypoints(void* serverHuman) {
    if (g_enabled && serverHuman == g_controlledServerHuman) {
        return;
    }
    g_originalUpdateOrientation(serverHuman);
}

bool HideControlledSilencedShotCircle(const void* event, std::uintptr_t caller) {
    // HEAR_SUSPICIOUS is also used by other noises. Match the verified call
    // from Firearm::Fire so door/tool/alarm cues cannot be mistaken for shots.
    if (!event || caller != g_moduleBase + kRvaFirearmSoundAwarenessReturn ||
        At<int>(event, 0) != 9) return false;
    const void* emitter = At<void*>(event, 0x10);
    if (!emitter) return false;
    if (g_enabled && emitter == g_controlledServerHuman) return true;
    const auto found = g_remoteInputs.find(At<std::uint32_t>(event, 8));
    return found != g_remoteInputs.end() && found->second.human == emitter &&
        GetTickCount64() - found->second.lastReceived <= 500 &&
        RemoteInputIsAuthorized(found->second);
}

void ApplySoundAwarenessPolicy(void* event, float radius, bool createCircle, std::uintptr_t caller) {
    if (createCircle && HideControlledSilencedShotCircle(event, caller)) {
        createCircle = false;
        TraceTraversal("silenced_shot_circle_hidden", static_cast<int>(At<std::uint32_t>(event, 8)), 0, 0);
    }
    // The original still performs spatial queries and sends awareness events
    // to AI brains. Only the separate visual circle network command is omitted.
    g_originalGenerateSoundAwareness(event, radius, createCircle);
}

void __fastcall HookedGenerateSoundAwareness(void* event, float radius, bool createCircle) {
    ApplySoundAwarenessPolicy(event, radius, createCircle,
        reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
}

DWORD WINAPI Initialize(void*) {
    g_moduleBase = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
    if (!IsSupportedExecutable()) {
        MessageBoxW(
            nullptr,
            L"DK2 Direct Control supports only Door Kickers 2 v1.0.12.0. No hook was installed.",
            L"DK2 Direct Control",
            MB_OK | MB_ICONERROR);
        return 1;
    }
    constexpr unsigned char profileProlog[]{
        0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18};
    constexpr unsigned char soundProlog[]{
        0x48,0x89,0x6C,0x24,0x20,0x57,0x48,0x83,0xEC,0x40,0x0F,0x29,0x74,0x24,0x30};
    constexpr unsigned char laserProlog[]{
        0x40,0x55,0x53,0x48,0x8D,0x6C,0x24,0xB8,0x48,0x81,0xEC,0x48,0x01,0x00,0x00};
    constexpr unsigned char inventoryProlog[]{
        0x48,0x89,0x54,0x24,0x10,0x53,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56};
    if (std::memcmp(reinterpret_cast<const void*>(g_moduleBase + 0x000B9DE0),
            inventoryProlog, sizeof(inventoryProlog)) != 0 ||
        std::memcmp(reinterpret_cast<const void*>(g_moduleBase + kRvaUpdateHumanProfile),
            profileProlog, sizeof(profileProlog)) != 0 ||
        std::memcmp(reinterpret_cast<const void*>(g_moduleBase + kRvaGenerateSoundAwareness),
            soundProlog, sizeof(soundProlog)) != 0 ||
        std::memcmp(reinterpret_cast<const void*>(g_moduleBase + kRvaHumanClientUpdateLaserLight),
            laserProlog, sizeof(laserProlog)) != 0) {
        MessageBoxW(nullptr, L"Direct Action HUD/sound/laser hook signatures do not match. No hook was installed.",
            L"DK2 Direct Control", MB_OK | MB_ICONERROR);
        return 1;
    }
    LoadVisualOptions();
    TraceTraversal("nvg_laser_build", 3, 0, 0);
    TraceTraversal("vbss_high_build", 2, 0, 0);
    TraceTraversal("lockpick_build", 1, 0, 0);
    TraceTraversal("universal_tools_build", 2, 0, 0);
    g_equipmentNew = reinterpret_cast<EquipmentNewFn>(g_moduleBase + 0x002BC070);
    g_inventoryEquip = reinterpret_cast<InventoryEquipFn>(g_moduleBase + 0x002C03F0);
    g_getEquipmentTemplate = reinterpret_cast<GetEquipmentTemplateFn>(g_moduleBase + 0x0023D3F0);
    if (!InstallDetour(reinterpret_cast<void*>(g_moduleBase + 0x000B9DE0),
            reinterpret_cast<void*>(&HookedClientSetInventory), sizeof(inventoryProlog),
            reinterpret_cast<void**>(&g_originalClientSetInventory))) return 1;
    g_entitySetAlpha = reinterpret_cast<EntitySetAlphaFn>(g_moduleBase + 0x000BD1D0);
    g_guiFindChild = reinterpret_cast<GuiFindChildFn>(g_moduleBase + kRvaGuiFindChild);
    g_guiCloneStaticText = reinterpret_cast<GuiCloneStaticTextFn>(g_moduleBase + kRvaGuiCloneStaticText);
    g_guiCloneStaticImage = reinterpret_cast<GuiCloneStaticTextFn>(g_moduleBase + 0xDED80);
    g_guiSetImage = reinterpret_cast<GuiSetImageFn>(g_moduleBase + 0xDF160);
    g_translateText = reinterpret_cast<TranslateTextFn>(g_moduleBase + 0x18C2B0);
    g_guiSetName = reinterpret_cast<GuiSetNameFn>(g_moduleBase + kRvaGuiSetName);
    g_guiChangeText = reinterpret_cast<GuiChangeTextFn>(g_moduleBase + kRvaGuiChangeText);
    g_guiRebuildText = reinterpret_cast<GuiRebuildTextFn>(g_moduleBase + 0xDFBD0);

    g_getSelectedHuman = reinterpret_cast<GetSelectedHumanFn>(
        g_moduleBase + kRvaGetSelectedHuman);
    g_cycleTrooperSelection = reinterpret_cast<CycleTrooperSelectionFn>(
        g_moduleBase + kRvaCycleTrooperSelection);
    g_convertScreenToMapCoords = reinterpret_cast<ConvertScreenToMapCoordsFn>(
        g_moduleBase + kRvaConvertScreenToMapCoords);
    g_addToPath = reinterpret_cast<PathAddToPathFn>(g_moduleBase + kRvaPathAddToPath);
    g_addToPathEnd = reinterpret_cast<PathAddToPathEndFn>(
        g_moduleBase + kRvaPathAddToPathEnd);
    g_deletePath = reinterpret_cast<DeletePathFn>(
        g_moduleBase + kRvaDeletePathFromIndexOnward);
    g_notifyPath = reinterpret_cast<NotifyPathFn>(
        g_moduleBase + kRvaNotifyServerOfPathModification);
    g_netAddReliableEvent = reinterpret_cast<NetAddReliableEventFn>(
        g_moduleBase + kRvaNetAddReliableEvent);
    g_bitPackerWriteBits = reinterpret_cast<BitPackerWriteBitsFn>(
        g_moduleBase + kRvaBitPackerWriteBits);
    g_bitPackerReadBits = reinterpret_cast<BitPackerReadBitsFn>(
        g_moduleBase + kRvaBitPackerReadBits);
    g_pathfinderTraceLine = reinterpret_cast<PathfinderTraceLineFn>(
        g_moduleBase + kRvaPathfinderTraceLine);
    g_pathfinderIsPointClear = reinterpret_cast<PathfinderIsPointClearFn>(
        g_moduleBase + kRvaPathfinderIsPointClear);
    g_processCmdLookAim = reinterpret_cast<ProcessCmdLookAimFn>(
        g_moduleBase + kRvaProcessCmdLookAim);
    g_createQueueCommand = reinterpret_cast<CreateQueueCommandFn>(
        g_moduleBase + kRvaHumanServerCreateQueueCommand);
    g_renderLine = reinterpret_cast<GameRendererRenderLineFn>(
        g_moduleBase + kRvaGameRendererRenderLine);
    g_cameraUpdate = reinterpret_cast<CameraUpdateFn>(
        g_moduleBase + kRvaCameraUpdate);
    g_firearmCancelReload = reinterpret_cast<FirearmCancelReloadFn>(
        g_moduleBase + kRvaFirearmCancelReload);
    g_isAnyMenuOpened = reinterpret_cast<GameGuiIsAnyMenuOpenedFn>(
        g_moduleBase + kRvaGameGuiIsAnyMenuOpened);
    g_button2Click = reinterpret_cast<GameInputButton2ClickFn>(
        g_moduleBase + kRvaGameInputButton2Click);
    g_cmdFirearmShoot = reinterpret_cast<HumanServerCmdFirearmShootFn>(
        g_moduleBase + kRvaHumanServerCmdFirearmShoot);
    g_firearmReadyAim = reinterpret_cast<FirearmReadyAimFn>(
        g_moduleBase + kRvaFirearmReadyAim);
    g_firearmGuard = reinterpret_cast<FirearmGuardFn>(
        g_moduleBase + kRvaFirearmGuard);
    g_queueGrenadeThrow = reinterpret_cast<CmdQueueGrenadeThrowFn>(
        g_moduleBase + kRvaCmdQueueGrenadeThrow);
    g_queueCrouch = reinterpret_cast<HumanServerCmdQueueCrouchFn>(
        g_moduleBase + kRvaHumanServerCmdQueueCrouch);
    g_queueDoorOpen = reinterpret_cast<HumanServerCmdQueueDoorOpenFn>(
        g_moduleBase + kRvaHumanServerCmdQueueDoorOpen);
    g_queueSpyCamera = reinterpret_cast<HumanServerCmdQueueSpyCameraFn>(
        g_moduleBase + 0x002A6520);
    g_queueDoorBreach = reinterpret_cast<HumanServerCmdQueueDoorBreachFn>(
        g_moduleBase + 0x002A5D80);
    g_removeHumanCommand = reinterpret_cast<HumanCommandQueueRemoveFn>(
        g_moduleBase + kRvaHumanCommandQueueRemove);

    if (!InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaHumanClientUpdateLaserLight),
            reinterpret_cast<void*>(&HookedUpdateLaserLight),
            sizeof(laserProlog),
            reinterpret_cast<void**>(&g_originalUpdateLaserLight)) ||
        !InstallDetour(
            reinterpret_cast<void*>(
                g_moduleBase + kRvaGameInputCheckKeyboardEvents),
            reinterpret_cast<void*>(&HookedCheckKeyboardEvents),
            kKeyboardDetourLength,
            reinterpret_cast<void**>(&g_originalCheckKeyboardEvents)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameInputButton1Down),
            reinterpret_cast<void*>(&HookedButton1Down),
            kButton1DownDetourLength,
            reinterpret_cast<void**>(&g_originalButton1Down)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameInputButton1Click),
            reinterpret_cast<void*>(&HookedButton1Click),
            kButton1ClickDetourLength,
            reinterpret_cast<void**>(&g_originalButton1Click)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameInputButton1Drag),
            reinterpret_cast<void*>(&HookedButton1Drag),
            kButton1DragDetourLength,
            reinterpret_cast<void**>(&g_originalButton1Drag)) ||
        !InstallGameClientLoadDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameInputButton2Down),
            reinterpret_cast<void*>(&HookedButton2Down),
            kButton2DownPrefixLength,
            reinterpret_cast<void**>(&g_originalButton2Down)) ||
        !InstallGameClientLoadDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameInputButton2Click),
            reinterpret_cast<void*>(&HookedButton2Click),
            kButton2ClickPrefixLength,
            reinterpret_cast<void**>(&g_originalButton2Click)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameInputButton2Drag),
            reinterpret_cast<void*>(&HookedButton2Drag),
            kButton2DragDetourLength,
            reinterpret_cast<void**>(&g_originalButton2Drag)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameInputChangeSelection),
            reinterpret_cast<void*>(&HookedChangeSelection),
            kChangeSelectionDetourLength,
            reinterpret_cast<void**>(&g_originalChangeSelection)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameGuiOpenWaypointMenu),
            reinterpret_cast<void*>(&HookedOpenWaypointMenu),
            kOpenWaypointMenuDetourLength,
            reinterpret_cast<void**>(&g_originalOpenWaypointMenu)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameRendererRenderPaths),
            reinterpret_cast<void*>(&HookedRenderPaths),
            kRenderPathsDetourLength,
            reinterpret_cast<void**>(&g_originalRenderPaths)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaUpdateHumanProfile),
            reinterpret_cast<void*>(&HookedUpdateHumanProfile),
            kUpdateHumanProfileDetourLength,
            reinterpret_cast<void**>(&g_originalUpdateHumanProfile)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGenerateSoundAwareness),
            reinterpret_cast<void*>(&HookedGenerateSoundAwareness),
            kGenerateSoundAwarenessDetourLength,
            reinterpret_cast<void**>(&g_originalGenerateSoundAwareness)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaRenderPath),
            reinterpret_cast<void*>(&HookedRenderPath),
            kRenderPathDetourLength,
            reinterpret_cast<void**>(&g_originalRenderPath)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaWaypointsAdvanceAlongPath),
            reinterpret_cast<void*>(&HookedAdvanceAlongPath),
            kAdvanceAlongPathDetourLength,
            reinterpret_cast<void**>(&g_originalAdvanceAlongPath)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaHumanServerNetReceiveEvent),
            reinterpret_cast<void*>(&HookedHumanServerNetReceiveEvent),
            kHumanServerNetReceiveDetourLength,
            reinterpret_cast<void**>(&g_originalHumanServerNetReceiveEvent)) ||
        !InstallDetour(
            reinterpret_cast<void*>(
                g_moduleBase + kRvaProjectileClientNetReceiveEvent),
            reinterpret_cast<void*>(&HookedProjectileClientNetReceiveEvent),
            kProjectileClientNetReceiveDetourLength,
            reinterpret_cast<void**>(
                &g_originalProjectileClientNetReceiveEvent)) ||
        !InstallDetour(
            reinterpret_cast<void*>(
                g_moduleBase + kRvaProjectileServerDetonate),
            reinterpret_cast<void*>(&HookedProjectileServerDetonate),
            kProjectileServerDetonateDetourLength,
            reinterpret_cast<void**>(&g_originalProjectileServerDetonate)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaHumanServerUpdate),
            reinterpret_cast<void*>(&HookedHumanServerUpdate),
            kHumanServerUpdateDetourLength,
            reinterpret_cast<void**>(&g_originalHumanServerUpdate)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + 0x000AD5E0),
            reinterpret_cast<void*>(&HookedHumanClientGetAnims),
            kHumanClientGetAnimsDetourLength,
            reinterpret_cast<void**>(&g_originalHumanClientGetAnims)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaHumanGetLOS),
            reinterpret_cast<void*>(&HookedHumanGetLOS),
            kHumanGetLOSDetourLength,
            reinterpret_cast<void**>(&g_originalHumanGetLOS)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaFirearmFire),
            reinterpret_cast<void*>(&HookedFirearmFire),
            kFirearmFireDetourLength,
            reinterpret_cast<void**>(&g_originalFirearmFire)) ||
        !InstallDetour(
            reinterpret_cast<void*>(
                g_moduleBase + kRvaHumanTryShootingFirearm),
            reinterpret_cast<void*>(&HookedHumanTryShootingFirearm),
            kHumanTryShootingFirearmDetourLength,
            reinterpret_cast<void**>(&g_originalHumanTryShootingFirearm)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameClientUpdateCamera),
            reinterpret_cast<void*>(&HookedGameClientUpdateCamera),
            kCameraUpdateDetourLength,
            reinterpret_cast<void**>(&g_originalUpdateCamera)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaHumanServerGetMoveSpeed),
            reinterpret_cast<void*>(&HookedGetMoveSpeed),
            kGetMoveSpeedDetourLength,
            reinterpret_cast<void**>(&g_originalGetMoveSpeed)) ||
        !InstallDetour(
            reinterpret_cast<void*>(
                g_moduleBase + kRvaUpdateOrientationOnWaypoints),
            reinterpret_cast<void*>(&HookedUpdateOrientationOnWaypoints),
            kOrientationDetourLength,
            reinterpret_cast<void**>(&g_originalUpdateOrientation)) ||
        !InstallDetour(
            reinterpret_cast<void*>(g_moduleBase + kRvaGameInputUpdate),
            reinterpret_cast<void*>(&HookedGameInputUpdate),
            kUpdateDetourLength,
            reinterpret_cast<void**>(&g_originalUpdate))) {
        MessageBoxW(
            nullptr,
            L"A direct-control hook could not be installed.",
            L"DK2 Direct Control",
            MB_OK | MB_ICONERROR);
        return 2;
    }

    if (g_nativeWheelActions) {
        auto* request = reinterpret_cast<void*>(
            g_moduleBase + kRvaActionWPRequestCreation);
        constexpr std::array<std::uint8_t, 16> expectedPrologue{
            0x48, 0x8B, 0xC4, 0x55, 0x53, 0x48, 0x8D, 0x68,
            0xA1, 0x48, 0x81, 0xEC, 0xB8, 0x00, 0x00, 0x00};
        if (std::memcmp(request, expectedPrologue.data(),
                        expectedPrologue.size()) != 0 ||
            !InstallDetour(request,
                reinterpret_cast<void*>(&HookedActionWPRequestCreation),
                kActionWPRequestCreationDetourLength,
                reinterpret_cast<void**>(&g_originalActionWPRequestCreation))) {
            MessageBoxW(nullptr,
                L"The experimental wheel-action hook could not be installed.",
                L"DK2 Direct Control", MB_OK | MB_ICONERROR);
            return 2;
        }
    }

    if (g_friendlyFire || g_hostageBulletDamage) {
        auto* filter = reinterpret_cast<void*>(
            g_moduleBase + kRvaFilterOutBulletCollisions);
        // The copied 15-byte prologue contains complete instructions and no
        // relative references in the supported 1.0.12.0 executable.
        constexpr std::array<std::uint8_t, 15> expectedPrologue{
            0x48, 0x89, 0x5C, 0x24, 0x08, 0x48, 0x89, 0x74,
            0x24, 0x10, 0x57, 0x48, 0x83, 0xEC, 0x20};
        if (std::memcmp(filter, expectedPrologue.data(),
                        expectedPrologue.size()) != 0 ||
            !InstallDetour(filter,
                reinterpret_cast<void*>(&HookedBulletCollisionFilter),
                kBulletCollisionFilterDetourLength,
                reinterpret_cast<void**>(&g_originalBulletCollisionFilter))) {
            MessageBoxW(nullptr,
                L"The experimental bullet-collision hook could not be installed.",
                L"DK2 Direct Control", MB_OK | MB_ICONERROR);
            return 2;
        }
    }

    g_button2Click = g_originalButton2Click;
    return 0;
}

} // namespace

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_selfModule = instance;
        DisableThreadLibraryCalls(instance);
        if (HANDLE thread = CreateThread(nullptr, 0, Initialize, nullptr, 0, nullptr)) {
            CloseHandle(thread);
        }
    }
    return TRUE;
}
