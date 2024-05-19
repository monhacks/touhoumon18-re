#include "global.h"
#include "gflib.h"
#include "mail_data.h"
#include "pokemon_icon.h"
#include "graphics.h"

#define POKE_ICON_BASE_PAL_TAG 56000

struct MonIconSpriteTemplate
{
    const struct OamData * oam;
    const u8 *image;
    const union AnimCmd *const * anims;
    const union AffineAnimCmd *const * affineAnims;
    SpriteCallback callback;
    u16 paletteTag;
};

static u8 CreateMonIconSprite(const struct MonIconSpriteTemplate * template, s16 x, s16 y, u8 subpriority);
static void DestroyMonIconInternal(struct Sprite *sprite);

const u16 gMonIconPalettes[][16] = {
    INCBIN_U16("graphics/pokemon/icon_palettes/icon_palette_0.gbapal"),
    INCBIN_U16("graphics/pokemon/icon_palettes/icon_palette_1.gbapal"),
    INCBIN_U16("graphics/pokemon/icon_palettes/icon_palette_2.gbapal")
};

const u8 *const gMonIconTable[] = {
    // Chibi forms currently use the same icons as evolved froms. Shard forms have a letter denoting the speciality.
    [SPECIES_NONE]        = gMonIcon_QuestionMark,
    [SPECIES_CHIBI_SANAE]   = gMonIcon_ChibiSanae,
    [SPECIES_SANAE]     = gMonIcon_Sanae,
    [SPECIES_CHIBI_ALICE]    = gMonIcon_ChibiAlice,
    [SPECIES_ALICE]  = gMonIcon_Alice,
    [SPECIES_CHIBI_REISEN]  = gMonIcon_ChibiReisen,
    [SPECIES_REISEN]   = gMonIcon_Reisen,
    [SPECIES_CHIBI_LILY_WHITE]    = gMonIcon_ChibiLilyWhite,
    [SPECIES_LILY_WHITE]   = gMonIcon_LilyWhite,
    [SPECIES_CHIBI_LILY_BLACK]   = gMonIcon_ChibiLilyBlack,
    [SPECIES_LILY_BLACK]    = gMonIcon_LilyBlack,
    [SPECIES_CHIBI_MOMIJI]     = gMonIcon_ChibiMomiji,
    [SPECIES_MOMIJI]  = gMonIcon_Momiji,
    [SPECIES_CHIBI_SHIZUHA]      = gMonIcon_ChibiShizuha,
    [SPECIES_SHIZUHA]      = gMonIcon_Shizuha,
    [SPECIES_CHIBI_MEDICINE]    = gMonIcon_ChibiMedicine,
    [SPECIES_MEDICINE]      = gMonIcon_Medicine,
    [SPECIES_CHIBI_KOAKUMA]   = gMonIcon_ChibiKoakuma,
    [SPECIES_KOAKUMA]     = gMonIcon_Koakuma,
    [SPECIES_CHIBI_NAZRIN]     = gMonIcon_ChibiNazrin,
    [SPECIES_NAZRIN]    = gMonIcon_Nazrin,
    [SPECIES_CHIBI_TOKIKO]     = gMonIcon_ChibiTokiko,
    [SPECIES_TOKIKO]      = gMonIcon_Tokiko,
    [SPECIES_CHIBI_REMILIA]       = gMonIcon_ChibiRemilia,
    [SPECIES_REMILIA]       = gMonIcon_Remilia,
    [SPECIES_CHIBI_CIRNO]     = gMonIcon_ChibiCirno,
    [SPECIES_CIRNO]      = gMonIcon_Cirno,
    [SPECIES_CHIBI_RUMIA]   = gMonIcon_ChibiRumia,
    [SPECIES_RUMIA]   = gMonIcon_Rumia,
    [SPECIES_SHANGHAI]   = gMonIcon_Shanghai,
    [SPECIES_CHIBI_FLANDRE]    = gMonIcon_ChibiFlandre,
    [SPECIES_FLANDRE]   = gMonIcon_Flandre,
    [SPECIES_HOURAI]   = gMonIcon_Hourai,
    [SPECIES_CHIBI_YOUMU]    = gMonIcon_ChibiYoumu,
    [SPECIES_YOUMU]    = gMonIcon_Youmu,
    [SPECIES_CHIBI_YUKARI]    = gMonIcon_ChibiYukari,
    [SPECIES_YUKARI]    = gMonIcon_Yukari,
    [SPECIES_CHIBI_SUIKA]      = gMonIcon_ChibiSuika,
    [SPECIES_SUIKA]   = gMonIcon_Suika,
    [SPECIES_CHIBI_MYSTIA]  = gMonIcon_ChibiMystia,
    [SPECIES_MYSTIA]  = gMonIcon_Mystia,
    [SPECIES_CHIBI_MINORIKO]       = gMonIcon_ChibiMinoriko,
    [SPECIES_MINORIKO]      = gMonIcon_Minoriko,
    [SPECIES_CHIBI_KEINE]      = gMonIcon_ChibiKeine,
    [SPECIES_KEINE]       = gMonIcon_Keine,
    [SPECIES_ATTACK_KEINE]   = gMonIcon_AttackKeine,
    [SPECIES_CHIBI_IKU]       = gMonIcon_ChibiIku,
    [SPECIES_IKU]    = gMonIcon_Iku,
    [SPECIES_CHIBI_KOISHI]     = gMonIcon_ChibiKoishi,
    [SPECIES_KOISHI]    = gMonIcon_Koishi,
    [SPECIES_CHIBI_YAMAME]     = gMonIcon_ChibiYamame,
    [SPECIES_YAMAME]     = gMonIcon_Yamame,
    [SPECIES_CHIBI_CHEN]      = gMonIcon_ChibiChen,
    [SPECIES_CHEN]     = gMonIcon_Chen,
    [SPECIES_CHIBI_YORIHIME]     = gMonIcon_ChibiYorihime,
    [SPECIES_YORIHIME]     = gMonIcon_Yorihime,
    [SPECIES_CHIBI_KAGUYA]      = gMonIcon_ChibiKaguya,
    [SPECIES_KAGUYA]    = gMonIcon_Kayuga,
    [SPECIES_CHIBI_MOKOU]   = gMonIcon_ChibiMokou,
    [SPECIES_MOKOU]    = gMonIcon_Mokou,
    [SPECIES_CHIBI_HATATE]     = gMonIcon_ChibiHatate,
    [SPECIES_HATATE]   = gMonIcon_Hatate,
    [SPECIES_CHIBI_LETTY]   = gMonIcon_ChibiLetty,
    [SPECIES_LETTY]        = gMonIcon_Letty,
    [SPECIES_CHIBI_PATCHOULI]     = gMonIcon_ChibiPatchouli,
    [SPECIES_PATCHOULI]    = gMonIcon_Patchouli,
    [SPECIES_CHIBI_YUUGI]      = gMonIcon_ChibiYuugi,
    [SPECIES_YUUGI]     = gMonIcon_Yuugi,
    [SPECIES_CHIBI_TENSHI]     = gMonIcon_ChibiTenshi,
    [SPECIES_TENSHI]  = gMonIcon_Tenshi,
    [SPECIES_CHIBI_PARSEE]  = gMonIcon_ChibiParsee,
    [SPECIES_PARSEE]  = gMonIcon_Parsee,
    [SPECIES_CHIBI_MURASA]   = gMonIcon_ChibiMurasa,
    [SPECIES_MURASA]  = gMonIcon_Murasa,
    [SPECIES_CHIBI_KISUME]     = gMonIcon_ChibiKisume,
    [SPECIES_KISUME]    = gMonIcon_Kisume,
    [SPECIES_CHIBI_KOGASA]       = gMonIcon_ChibiKogasa,
    [SPECIES_KOGASA]      = gMonIcon_Kogasa,
    [SPECIES_CHIBI_SATORI]    = gMonIcon_ChibiSatori,
    [SPECIES_SATORI]    = gMonIcon_Satori,
    [SPECIES_CHIBI_EIRIN]     = gMonIcon_ChibiEirin,
    [SPECIES_EIRIN]   = gMonIcon_Eirin,
    [SPECIES_CHIBI_RAN]    = gMonIcon_ChibiRan,
    [SPECIES_RAN]   = gMonIcon_Ran,
    [SPECIES_YOUSEI]       = gMonIcon_Yousei,
    [SPECIES_DAIYOUSEI]      = gMonIcon_Daiyousei,
    [SPECIES_CHIBI_NITORI]        = gMonIcon_ChibiNitori,
    [SPECIES_NITORI]     = gMonIcon_Nitori,
    [SPECIES_CHIBI_HINA]      = gMonIcon_ChibiHina,
    [SPECIES_HINA]         = gMonIcon_Hina,
    [SPECIES_CHIBI_KOMACHI]    = gMonIcon_ChibiKomachi,
    [SPECIES_KOMACHI]    = gMonIcon_Komachi,
    [SPECIES_CHIBI_YUYUKO]      = gMonIcon_ChibiYuyuko,
    [SPECIES_YUYUKO]     = gMonIcon_Yuyuko,
    [SPECIES_CHIBI_MEILING]      = gMonIcon_ChibiMeiling,
    [SPECIES_MEILING]        = gMonIcon_Meiling,
    [SPECIES_CHIBI_SAKUYA]     = gMonIcon_ChibiSakuya,
    [SPECIES_SAKUYA]       = gMonIcon_Sakuya,
    [SPECIES_CHIBI_LUNASA]      = gMonIcon_ChibiLunasa,
    [SPECIES_LUNASA]     = gMonIcon_Lunasa,
    [SPECIES_CHIBI_MERLIN]     = gMonIcon_ChibiMerlin,
    [SPECIES_MERLIN]   = gMonIcon_Merlin,
    [SPECIES_CHIBI_LYRICA]   = gMonIcon_ChibiLyrica,
    [SPECIES_LYRICA]   = gMonIcon_Lyrica,
    [SPECIES_CHIBI_LAYLA]      = gMonIcon_ChibiLayla,
    [SPECIES_LAYLA]     = gMonIcon_Layla,
    [SPECIES_CHIBI_KANAKO]   = gMonIcon_ChibiKanako,
    [SPECIES_KANAKO]  = gMonIcon_Kanako,
    [SPECIES_CHIBI_RIN]   = gMonIcon_ChibiRin,
    [SPECIES_RIN]     = gMonIcon_Rin,
    [SPECIES_CHIBI_UTSUHO]     = gMonIcon_ChibiUtsuho,
    [SPECIES_UTSUHO]     = gMonIcon_Utsuho,
    [SPECIES_CHIBI_ICHIRIN]      = gMonIcon_ChibiIchirin,
    [SPECIES_ICHIRIN]     = gMonIcon_Ichirin,
    [SPECIES_CHIBI_SUNNY_MILK]     = gMonIcon_ChibiSunnyMilk,
    [SPECIES_SUNNY_MILK]  = gMonIcon_SunnyMilk,
    [SPECIES_CHIBI_LUNA_CHILD]      = gMonIcon_ChibiLunaChild,
    [SPECIES_LUNA_CHILD]      = gMonIcon_LunaChild,
    [SPECIES_CHIBI_STAR_SAPPHIRE]     = gMonIcon_ChibiStarSapphire,
    [SPECIES_STAR_SAPPHIRE]     = gMonIcon_StarSapphire,
    [SPECIES_CHIBI_YUUKA]      = gMonIcon_ChibiYuuka,
    [SPECIES_YUUKA]     = gMonIcon_Yuuka,
    [SPECIES_CHIBI_AYA]     = gMonIcon_ChibiAya,
    [SPECIES_AYA]     = gMonIcon_Aya,
    [SPECIES_CHIBI_EIKI]        = gMonIcon_ChibiEiki,
    [SPECIES_EIKI]  = gMonIcon_Eiki,
    [SPECIES_CHIBI_SHOU]      = gMonIcon_ChibiShou,
    [SPECIES_SHOU]      = gMonIcon_Shou,
    [SPECIES_CHIBI_WRIGGLE]      = gMonIcon_ChibiWriggle,
    [SPECIES_WRIGGLE]    = gMonIcon_Wriggle,
    [SPECIES_CHIBI_TEWI]    = gMonIcon_ChibiTewi,
    [SPECIES_TEWI]      = gMonIcon_Tewi,
    [SPECIES_KEDAMA]       = gMonIcon_Kedama,
    [SPECIES_CHIBI_NUE]       = gMonIcon_ChibiNue,
    [SPECIES_NUE]    = gMonIcon_Nue,
    [SPECIES_CHIBI_SUWAKO]     = gMonIcon_ChibiSuwako,
    [SPECIES_SUWAKO]     = gMonIcon_Suwako,
    [SPECIES_GYOKUTO]     = gMonIcon_Gyokuto,
    [SPECIES_CHIBI_MARISA]     = gMonIcon_ChibiMarisa,
    [SPECIES_MARISA]     = gMonIcon_Marisa,
    [SPECIES_CHIBI_REIMU]      = gMonIcon_ChibiReimu,
    [SPECIES_REIMU]    = gMonIcon_Reimu,
    [SPECIES_CHIBI_BYAKUREN]  = gMonIcon_ChibiByakuren,
    [SPECIES_BYAKUREN]     = gMonIcon_Byakuren,
    [SPECIES_KONGALA]    = gMonIcon_Kongala,
    [SPECIES_KIKURI]      = gMonIcon_Kikuri,
    [SPECIES_YUUGEN]     = gMonIcon_Yuugen,
    [SPECIES_CHIBI_TOYOHIME]     = gMonIcon_ChibiToyohime,
    [SPECIES_TOYOHIME]   = gMonIcon_Toyohime,
    [SPECIES_GOLIATH]   = gMonIcon_Goliath,
    [SPECIES_SARIEL]      = gMonIcon_Sariel,
    [SPECIES_SATSUKI]         = gMonIcon_Satsuki,
    [SPECIES_AKYUU]   = gMonIcon_Akyuu,
    [SPECIES_GENJI]     = gMonIcon_Genji,
    [SPECIES_TORI]    = gMonIcon_Tori,
    [SPECIES_OONAMAZU]   = gMonIcon_Oonamazu,
    [SPECIES_CHIBI_ELLY]     = gMonIcon_ChibiElly,
    [SPECIES_ELLY]  = gMonIcon_Elly,
    [SPECIES_CHIBI_ORANGE]    = gMonIcon_ChibiOrange,
    [SPECIES_ORANGE]    = gMonIcon_Orange,
    [SPECIES_YOUKI]  = gMonIcon_Youki,
    [SPECIES_CHIBI_CHIYURI]     = gMonIcon_ChibiChiyuri,
    [SPECIES_CHIYURI]      = gMonIcon_Chiyuri,
    [SPECIES_CHIBI_YUMEMI]    = gMonIcon_ChibiYumemi,
    [SPECIES_YUMEMI]     = gMonIcon_Yumemi,
    [SPECIES_CHIBI_ELLEN]      = gMonIcon_ChibiEllen,
    [SPECIES_ELLEN]      = gMonIcon_Ellen,
    [SPECIES_CHIBI_KAZAMI]    = gMonIcon_ChibiKazami,
    [SPECIES_KAZAMI]     = gMonIcon_Kazami,
    [SPECIES_CHIBI_MEIRA]      = gMonIcon_ChibiMeira,
    [SPECIES_MEIRA]    = gMonIcon_Meira,
    [SPECIES_CHIBI_SARA]     = gMonIcon_ChibiSara,
    [SPECIES_SARA]       = gMonIcon_Sara,
    [SPECIES_CHIBI_LOUISE]      = gMonIcon_ChibiLouise,
    [SPECIES_LOUISE]   = gMonIcon_Louise,
    [SPECIES_CHIBI_ALICE_98]      = gMonIcon_ChibiAlice98,
    [SPECIES_ALICE_98]     = gMonIcon_Alice98,
    [SPECIES_CHIBI_KANA]        = gMonIcon_ChibiKana,
    [SPECIES_KANA]        = gMonIcon_Kana,
    [SPECIES_CHIBI_HAKUREI]      = gMonIcon_ChibiHakurei,
    [SPECIES_HAKUREI]     = gMonIcon_Hakurei,
    [SPECIES_CHIBI_KIRISAME]    = gMonIcon_ChibiKirisame,
    [SPECIES_KIRISAME]   = gMonIcon_Kirisame,
    [SPECIES_CHIBI_KOTOHIME]      = gMonIcon_ChibiKotohime,
    [SPECIES_KOTOHIME]   = gMonIcon_Kotohime,
    [SPECIES_CHIBI_YUMEKO]   = gMonIcon_ChibiYumeko,
    [SPECIES_YUMEKO]    = gMonIcon_Yumeko,
    [SPECIES_CHIBI_SHINKI]      = gMonIcon_ChibiShinki,
    [SPECIES_SHINKI]    = gMonIcon_Shinki,
    [SPECIES_MIMI_CHAN]    = gMonIcon_MimiChan,
    [SPECIES_RUUKOTO]       = gMonIcon_Ruukoto,
    [SPECIES_CHIBI_RIKA]     = gMonIcon_ChibiRika,
    [SPECIES_RIKA]    = gMonIcon_Rika,
    [SPECIES_CHIBI_MUGETSU]       = gMonIcon_ChibiMugetsu,
    [SPECIES_MUGETSU]      = gMonIcon_Mugetsu,
    [SPECIES_CHIBI_GENGETSU]    = gMonIcon_ChibiGengetsu,
    [SPECIES_GENGETSU]      = gMonIcon_Gengetsu,
    [SPECIES_CHIBI_KURUMI]     = gMonIcon_ChibiKurumi,
    [SPECIES_KURUMI]     = gMonIcon_Kurumi,
    [SPECIES_CHIBI_RIKAKO]    = gMonIcon_ChibiRikako,
    [SPECIES_RIKAKO]  = gMonIcon_Rikako,
    [SPECIES_UNOWN]       = gMonIcon_UnownA,
    [SPECIES_CHIBI_YUKI]   = gMonIcon_ChibiYuugi,
    [SPECIES_YUKI]   = gMonIcon_Yuugi,
    [SPECIES_CHIBI_MAI]      = gMonIcon_ChibiMai,
    [SPECIES_MAI]  = gMonIcon_Mai,
    [SPECIES_DEFENSE_SUNNY_MILK]   = gMonIcon_DefenseSunnyMilk,
    [SPECIES_TECHNICAL_LUNA_CHILD]      = gMonIcon_TechnicalLunaChild,
    [SPECIES_SUPPORT_STAR_SAPPHIRE]     = gMonIcon_SupportStarSapphire,
    [SPECIES_ATTACK_NITORI]    = gMonIcon_AttackNitori,
    [SPECIES_TECHNICAL_NITORI]    = gMonIcon_TechnicalNitori,
    [SPECIES_ATTACK_MOKOU]    = gMonIcon_AttackMokou,
    [SPECIES_DEFENSE_MOKOU]      = gMonIcon_DefenseMokou,
    [SPECIES_DEFENSE_REISEN]     = gMonIcon_DefenseReisen,
    [SPECIES_TECHNICAL_REISEN]   = gMonIcon_TechnicalReisen,
    [SPECIES_TECHNICAL_SAKUYA]     = gMonIcon_TechnicalSakuya,
    [SPECIES_SUPPORT_SAKUYA]   = gMonIcon_SupportSakuya,
    [SPECIES_ATTACK_RUMIA]    = gMonIcon_AttackRumia,
    [SPECIES_SPEED_RUMIA]      = gMonIcon_SpeedRumia,
    [SPECIES_SUPPORT_MYSTIA]    = gMonIcon_SupportMystia,
    [SPECIES_ATTACK_MYSTIA]      = gMonIcon_AttackMystia,
    [SPECIES_ATTACK_CHEN]   = gMonIcon_AttackChen,
    [SPECIES_TECHNICAL_CHEN]     = gMonIcon_TechnicalChen,
    [SPECIES_DEFENSE_YOUMU]    = gMonIcon_DefenseYoumu,
    [SPECIES_SPEED_YOUMU]   = gMonIcon_SpeedYoumu,
    [SPECIES_ATTACK_YUUKA]    = gMonIcon_AttackYuuka,
    [SPECIES_TECHNICAL_YUUKA]     = gMonIcon_TechnicalYuuka,
    [SPECIES_SPEED_AYA]    = gMonIcon_SpeedAya,
    [SPECIES_TECHNICAL_AYA]    = gMonIcon_TechnicalAya,
    [SPECIES_ATTACK_TENSHI]    = gMonIcon_AttackTenshi,
    [SPECIES_DEFENSE_TENSHI]     = gMonIcon_DefenseTenshi,
    [SPECIES_DEFENSE_PARSEE]      = gMonIcon_DefenseParsee,
    [SPECIES_TECHNICAL_PARSEE]     = gMonIcon_TechnicalParsee,
    [SPECIES_ATTACK_KOISHI]    = gMonIcon_AttackKoishi,
    [SPECIES_SPEED_KOISHI]    = gMonIcon_SpeedKoishi,
    [SPECIES_DEFENSE_BYAKUREN]    = gMonIcon_DefenseByakuren,
    [SPECIES_TECHNICAL_BYAKUREN]     = gMonIcon_TechnicalByakuren,
    [SPECIES_DEFENSE_ICHIRIN]   = gMonIcon_DefenseIchirin,
    [SPECIES_TECHNICAL_ICHIRIN]    = gMonIcon_TechnicalIchirin,
    [SPECIES_ATTACK_KANAKO]      = gMonIcon_AttackKanako,
    [SPECIES_DEFENSE_KANAKO]       = gMonIcon_DefenseKanako,
    [SPECIES_ATTACK_KAGUYA]     = gMonIcon_AttackKaguya,
    [SPECIES_DEFENSE_KAGUYA]     = gMonIcon_DefenseKaguya,
    [SPECIES_SHINGYOKU_ORB]      = gMonIcon_ShingyokuOrb,
    [SPECIES_SHINGYOKU_MALE]       = gMonIcon_ShingyokuMale,
    [SPECIES_SHINGYOKU_FEMALE]     = gMonIcon_ShingyokuFemale,
    [SPECIES_ATTACK_UTSUHO]    = gMonIcon_AttackUtsuho,
    [SPECIES_SPEED_UTSUHO]     = gMonIcon_SpeedUtsuho,
    [SPECIES_DEFENSE_MOMIJI]   = gMonIcon_DefenseMomiji,
    [SPECIES_ELIS]       = gMonIcon_Elis,
    [SPECIES_MIMA]       = gMonIcon_Mima,
    [SPECIES_ISAMI]      = gMonIcon_Isami,
    [SPECIES_DUMMY1] = gMonIcon_QuestionMark,
    [SPECIES_DUMMY2] = gMonIcon_QuestionMark,
    [SPECIES_DUMMY3] = gMonIcon_QuestionMark,
    [SPECIES_DUMMY4] = gMonIcon_QuestionMark,
    [SPECIES_DUMMY5] = gMonIcon_QuestionMark,
    [SPECIES_DUMMY6] = gMonIcon_QuestionMark,
    [SPECIES_CHIBI_KYOUKO] = gMonIcon_ChibiKyouko,
    [SPECIES_KYOUKO] = gMonIcon_Kyouko,
    [SPECIES_CHIBI_MIYAKO] = gMonIcon_ChibiMiyako,
    [SPECIES_MIYAKO] = gMonIcon_Miyako,
    [SPECIES_CHIBI_SEIGA] = gMonIcon_ChibiSeiga,
    [SPECIES_SEIGA] = gMonIcon_Seiga,
    [SPECIES_CHIBI_TOJIKO] = gMonIcon_ChibiTojiko,
    [SPECIES_CHIBI_MIKO] = gMonIcon_ChibiMiko,
    [SPECIES_MIKO] = gMonIcon_Miko,
    [SPECIES_CHIBI_MAMIZOU] = gMonIcon_ChibiMamizou,
    [SPECIES_DUMMY7] = gMonIcon_QuestionMark,
    [SPECIES_DUMMY8] = gMonIcon_QuestionMark,
    [SPECIES_TECHNICAL_SENDAI] = gMonIcon_TechnicalSendai,
    [SPECIES_PRISMRIVER_SISTERS] = gMonIcon_PrismriverSisters,
    [SPECIES_MAMIZOU] = gMonIcon_Mamizou,
    [SPECIES_ADVENT_YOUMU] = gMonIcon_AdventYoumu,
    [SPECIES_ADVENT_YUYUKO] = gMonIcon_AdventYuyuko,
    [SPECIES_ADVENT_EIRIN] = gMonIcon_AdventEirin,
    [SPECIES_SPEED_TENMA] = gMonIcon_SpeedTenma,
    [SPECIES_SPEED_KOMACHI]     = gMonIcon_SpeedKomachi,
    [SPECIES_ATTACK_KOMACHI]     = gMonIcon_AttackKomachi,
    [SPECIES_ATTACK_EIRIN]    = gMonIcon_AdventEirin,
    [SPECIES_SUPPORT_EIRIN]     = gMonIcon_SupportEirin,
    [SPECIES_DEFENSE_YUKARI]   = gMonIcon_DefenseYukari,
    [SPECIES_TECHNICAL_YUKARI]    = gMonIcon_TechnicalYukari,
    [SPECIES_ATTACK_ALICE]      = gMonIcon_AttackAlice,
    [SPECIES_TECHNICAL_ALICE]   = gMonIcon_TechnicalAlice,
    [SPECIES_ATTACK_REIMU]    = gMonIcon_AttackReimu,
    [SPECIES_DEFENSE_REIMU]   = gMonIcon_DefenseReimu,
    [SPECIES_SPEED_CIRNO]   = gMonIcon_SpeedCirno,
    [SPECIES_TECHNICAL_CIRNO]   = gMonIcon_TechnicalCirno,
    [SPECIES_DEFENSE_YUYUKO]     = gMonIcon_DefenseYuyuko,
    [SPECIES_ATTACK_YUYUKO]     = gMonIcon_AdventYuyuko,
    [SPECIES_SUPPORT_LUNASA]     = gMonIcon_SupportLunasa,
    [SPECIES_SUPPORT_MERLIN]   = gMonIcon_SupportMerlin,
    [SPECIES_SUPPORT_LYRICA]     = gMonIcon_SupportLyrica,
    [SPECIES_ATTACK_RAN]      = gMonIcon_AttackRan,
    [SPECIES_SUPPORT_RAN]       = gMonIcon_SupportRan,
    [SPECIES_ATTACK_MEDICINE]      = gMonIcon_AttackMedicine,
    [SPECIES_TECHNICAL_MEDICINE]    = gMonIcon_TechnicalMedicine,
    [SPECIES_ATTACK_HINA]      = gMonIcon_AttackHina,
    [SPECIES_DEFENSE_HINA]     = gMonIcon_DefenseHina,
    [SPECIES_SPEED_SHIZUHA]     = gMonIcon_SpeedShizuha,
    [SPECIES_SPEED_RIN]     = gMonIcon_SpeedRin,
    [SPECIES_ATTACK_RIN]     = gMonIcon_AttackRin,
    [SPECIES_ZOMBIE_FAIRY]    = gMonIcon_ZombieFairy,
    [SPECIES_SUPPORT_SHIZUHA]     = gMonIcon_SupportShizuha,
    [SPECIES_ATTACK_SUWAKO]     = gMonIcon_AttackSuwako,
    [SPECIES_ATTACK_SHOU]   = gMonIcon_AttackShou,
    [SPECIES_TECHNICAL_SHOU]     = gMonIcon_TechnicalShou,
    [SPECIES_ATTACK_TOKIKO]      = gMonIcon_AttackTokiko,
    [SPECIES_DEFENSE_SUWAKO]     = gMonIcon_DefenseSuwako,
    [SPECIES_SPEED_YAMAME]    = gMonIcon_SpeedYamame,
    [SPECIES_SPEED_KOGASA]     = gMonIcon_SpeedKogasa,
    [SPECIES_TECHNICAL_KOGASA]  = gMonIcon_TechnicalKogasa,
    [SPECIES_DEFENSE_REMILIA]     = gMonIcon_DefenseRemilia,
    [SPECIES_SPEED_FLANDRE]     = gMonIcon_SpeedFlandre,
    [SPECIES_DEFENSE_WRIGGLE]      = gMonIcon_DefenseWriggle,
    [SPECIES_DEFENSE_MEILING]    = gMonIcon_DefenseMeiling,
    [SPECIES_TENSOKU]     = gMonIcon_Tensoku,
    [SPECIES_ATTACK_SUNNY_MILK]      = gMonIcon_AttackSunnyMilk,
    [SPECIES_ATTACK_LUNA_CHILD]     = gMonIcon_AttackLunaChild,
    [SPECIES_SPEED_WRIGGLE]    = gMonIcon_SpeedWriggle,
    [SPECIES_SUPPORT_KOAKUMA]     = gMonIcon_SupportKoakuma,
    [SPECIES_SPEED_MEILING]     = gMonIcon_SpeedMeiling,
    [SPECIES_CHIBI_ELIS]    = gMonIcon_ChibiElis,
    [SPECIES_CHIBI_SARIEL]    = gMonIcon_ChibiSariel,
    [SPECIES_ADVENT_MYSTIA]     = gMonIcon_AdventMystia,
    [SPECIES_TECHNICAL_HATATE]    = gMonIcon_TechnicalHatate,
    [SPECIES_DEFENSE_HATATE]   = gMonIcon_DefenseHatate,
    [SPECIES_AKI_SISTERS]      = gMonIcon_AkiSisters,
    [SPECIES_DEFENSE_LAYLA]     = gMonIcon_DefenseLayla,
    [SPECIES_DEFENSE_KEINE]    = gMonIcon_DefenseKeine,
    [SPECIES_ATTACK_REMILIA]    = gMonIcon_AttackRemilia,
    [SPECIES_DEFENSE_TEWI]    = gMonIcon_DefenseTewi,
    [SPECIES_ATTACK_TEWI]     = gMonIcon_AttackTewi,
    [SPECIES_DEFENSE_EIKI]      = gMonIcon_DefenseEiki,
    [SPECIES_DEFENSE_MINORIKO]    = gMonIcon_DefenseMinoriko,
    [SPECIES_TECHNICAL_SUIKA]    = gMonIcon_TechnicalSuika,
    [SPECIES_ATTACK_SANAE]   = gMonIcon_AttackSanae,
    [SPECIES_TECHNICAL_SANAE]   = gMonIcon_TechnicalSanae,
    [SPECIES_ATTACK_FLANDRE]       = gMonIcon_AttackFlandre,
    [SPECIES_ATTACK_DAIYOUSEI]    = gMonIcon_AttackDaiyousei,
    [SPECIES_TOJIKO]      = gMonIcon_Tojiko,
    [SPECIES_CHIBI_FUTO]      = gMonIcon_ChibiFuto,
    [SPECIES_FUTO]     = gMonIcon_Futo,
    [SPECIES_ATTACK_EIKI]      = gMonIcon_AttackEiki,
    [SPECIES_TECHNICAL_TOYOHIME]    = gMonIcon_TechnicalToyohime,
    [SPECIES_MISSING_POWER_SUIKA]     = gMonIcon_MissingPowerSuika,
    [SPECIES_SCHOOLGIRL_SANAE]      = gMonIcon_SchoolgirlSanae,
    [SPECIES_CHIBI_KONGALA]    = gMonIcon_ChibiKongala,
    [SPECIES_CHIBI_YUUGEN]     = gMonIcon_ChibiYuugen,
    [SPECIES_ATTACK_SUIKA]     = gMonIcon_AttackSuika,
    [SPECIES_SUPPORT_LILY_WHITE]      = gMonIcon_SupportLilyWhite,
    [SPECIES_SUPPORT_LILY_BLACK]     = gMonIcon_SupportLilyBlack,
    [SPECIES_ATTACK_NUE]      = gMonIcon_AttackNue,
    [SPECIES_TECHNICAL_NUE]       = gMonIcon_TechnicalNue,
    [SPECIES_DEFENSE_PATCHOULI]      = gMonIcon_DefensePatchouli,
    [SPECIES_DEFENSE_LETTY]    = gMonIcon_DefenseLetty,
    [SPECIES_SUPPORT_LETTY]    = gMonIcon_SupportLetty,
    [SPECIES_SPEED_YORIHIME]      = gMonIcon_SpeedYorihime,
    [SPECIES_CHIBI_SHINGYOKU]     = gMonIcon_ChibiShingyoku,
    [SPECIES_ANGEL_VIVIT]      = gMonIcon_AngelVIVIT,
    [SPECIES_ATTACK_LILY_BLACK]     = gMonIcon_AttackLilyBlack,
    [SPECIES_CHIBI_KASEN]    = gMonIcon_ChibiKasen,
    [SPECIES_DEFENSE_YUUGI]     = gMonIcon_DefenseYuugi,
    [SPECIES_TECHNICAL_NAZRIN]     = gMonIcon_TechnicalNazrin,
    [SPECIES_SUPPORT_NAZRIN]    = gMonIcon_SupportNazrin,
    [SPECIES_TECHNICAL_KISUME]     = gMonIcon_TechnicalKisume,
    [SPECIES_ATTACK_YUUGI]      = gMonIcon_AttackYuugi,
    [SPECIES_SUPPORT_KEINE]      = gMonIcon_SupportKeine,
    [SPECIES_KASEN]     = gMonIcon_Kasen,
    [SPECIES_DEFENSE_IKU]     = gMonIcon_DefenseIku,
    [SPECIES_TECHNICAL_IKU]     = gMonIcon_TechnicalIku,
    [SPECIES_ATTACK_MINORIKO]     = gMonIcon_AttackMinoriko,
    [SPECIES_RINNOSUKE]    = gMonIcon_Rinnosuke,
    [SPECIES_ADVENT_CIRNO]     = gMonIcon_AdventCirno,
    [SPECIES_ADVENT_REISEN]    = gMonIcon_AdventReisen,
    [SPECIES_VIVIT]       = gMonIcon_VIVIT,
    [SPECIES_SPEED_SARIEL]     = gMonIcon_SpeedSariel,
    [SPECIES_ATTACK_LILY_WHITE]     = gMonIcon_AttackLilyWhite,
    [SPECIES_CHIBI_MIMA]     = gMonIcon_ChibiMima,
    [SPECIES_CHIBI_KIKURI]    = gMonIcon_ChibiKikuri,
    [SPECIES_ADVENT_MEILING]   = gMonIcon_AdventMeiling,
    [SPECIES_ATTACK_PATCHOULI]        = gMonIcon_AttackPatchouli,
    [SPECIES_SPEED_MARISA]      = gMonIcon_SpeedMarisa,
    [SPECIES_ATTACK_MARISA]      = gMonIcon_AttackMarisa,
    [SPECIES_FAIRY]    = gMonIcon_Fairy,
    [SPECIES_DEFENSE_MURASA]     = gMonIcon_DefenseMurasa,
    [SPECIES_ATTACK_MURASA]    = gMonIcon_AttackMurasa,
    [SPECIES_ATTACK_STAR_SAPPHIRE]      = gMonIcon_AttackStarSapphire,
    [SPECIES_ATTACK_KAZAMI]     = gMonIcon_AttackKazami,
    [SPECIES_SPEED_MEDICINE]     = gMonIcon_SpeedMedicine,
    [SPECIES_DEFENSE_DAIYOUSEI]     = gMonIcon_DefenseDaiyousei,
    [SPECIES_TECHNICAL_YAMAME]       = gMonIcon_TechnicalYamame,
    [SPECIES_TECHNICAL_SATORI]      = gMonIcon_TechnicalSatori,
    [SPECIES_DEFENSE_SATORI]   = gMonIcon_DefenseSatori,
    [SPECIES_ADVENT_LETTY]       = gMonIcon_AdventLetty,
    [SPECIES_ADVENT_TEWI]     = gMonIcon_AdventTewi,
    [SPECIES_ADVENT_MARISA]   = gMonIcon_AdventMarisa,
    [SPECIES_ADVENT_ALICE]      = gMonIcon_AdventAlice,
    [SPECIES_ADVENT_MOKOU]      = gMonIcon_AdventMokou,
    [SPECIES_ADVENT_YUKARI]   = gMonIcon_AdventYukari,
    [SPECIES_ADVENT_RAN]    = gMonIcon_AdventRan,
    [SPECIES_ADVENT_CHEN]      = gMonIcon_AdventChen,
    [SPECIES_MARISA_EX]   = gMonIcon_MarisaEx,
    [SPECIES_BLACK_ALICE]      = gMonIcon_BlackAlice,
    [SPECIES_TENMA]     = gMonIcon_Tenma,
    [SPECIES_SENDAI]    = gMonIcon_Sendai,
    [SPECIES_CHIBI_SENDAI]      = gMonIcon_ChibiSendai,
    [SPECIES_CHIBI_TENMA]      = gMonIcon_ChibiTenma,
    [SPECIES_TENSOKU_G]     = gMonIcon_TensokuG,
    [SPECIES_AYAKASHI]      = gMonIcon_Ayakashi,
    [SPECIES_CHIBI_VIVIT]    = gMonIcon_ChibiVIVIT,
    [SPECIES_EGG]         = gMonIcon_Egg,
    [SPECIES_UNOWN_B]     = gMonIcon_UnownB,
    [SPECIES_UNOWN_C]     = gMonIcon_UnownC,
    [SPECIES_UNOWN_D]     = gMonIcon_UnownD,
    [SPECIES_UNOWN_E]     = gMonIcon_UnownE,
    [SPECIES_UNOWN_F]     = gMonIcon_UnownF,
    [SPECIES_UNOWN_G]     = gMonIcon_UnownG,
    [SPECIES_UNOWN_H]     = gMonIcon_UnownH,
    [SPECIES_UNOWN_I]     = gMonIcon_UnownI,
    [SPECIES_UNOWN_J]     = gMonIcon_UnownJ,
    [SPECIES_UNOWN_K]     = gMonIcon_UnownK,
    [SPECIES_UNOWN_L]     = gMonIcon_UnownL,
    [SPECIES_UNOWN_M]     = gMonIcon_UnownM,
    [SPECIES_UNOWN_N]     = gMonIcon_UnownN,
    [SPECIES_UNOWN_O]     = gMonIcon_UnownO,
    [SPECIES_UNOWN_P]     = gMonIcon_UnownP,
    [SPECIES_UNOWN_Q]     = gMonIcon_UnownQ,
    [SPECIES_UNOWN_R]     = gMonIcon_UnownR,
    [SPECIES_UNOWN_S]     = gMonIcon_UnownS,
    [SPECIES_UNOWN_T]     = gMonIcon_UnownT,
    [SPECIES_UNOWN_U]     = gMonIcon_UnownU,
    [SPECIES_UNOWN_V]     = gMonIcon_UnownV,
    [SPECIES_UNOWN_W]     = gMonIcon_UnownW,
    [SPECIES_UNOWN_X]     = gMonIcon_UnownX,
    [SPECIES_UNOWN_Y]     = gMonIcon_UnownY,
    [SPECIES_UNOWN_Z]     = gMonIcon_UnownZ,
    [SPECIES_UNOWN_EMARK] = gMonIcon_UnownExclamationMark,
    [SPECIES_UNOWN_QMARK] = gMonIcon_UnownQuestionMark
};

const u8 gMonIconPaletteIndices[] = {
    //TODO: update palettes
    [SPECIES_NONE]        = 0,
    [SPECIES_CHIBI_SANAE]   = 1,
    [SPECIES_SANAE]     = 1,
    [SPECIES_CHIBI_ALICE]    = 1,
    [SPECIES_ALICE]  = 0,
    [SPECIES_CHIBI_REISEN]  = 0,
    [SPECIES_REISEN]   = 0,
    [SPECIES_CHIBI_LILY_WHITE]    = 0,
    [SPECIES_LILY_WHITE]   = 2,
    [SPECIES_CHIBI_LILY_BLACK]   = 2,
    [SPECIES_LILY_BLACK]    = 1,
    [SPECIES_CHIBI_MOMIJI]     = 1,
    [SPECIES_MOMIJI]  = 0,
    [SPECIES_CHIBI_SHIZUHA]      = 1,
    [SPECIES_SHIZUHA]      = 2,
    [SPECIES_CHIBI_MEDICINE]    = 2,
    [SPECIES_MEDICINE]      = 0,
    [SPECIES_CHIBI_KOAKUMA]   = 0,
    [SPECIES_KOAKUMA]     = 0,
    [SPECIES_CHIBI_NAZRIN]     = 2,
    [SPECIES_NAZRIN]    = 1,
    [SPECIES_CHIBI_TOKIKO]     = 0,
    [SPECIES_TOKIKO]      = 0,
    [SPECIES_CHIBI_REMILIA]       = 2,
    [SPECIES_REMILIA]       = 2,
    [SPECIES_CHIBI_CIRNO]     = 2,
    [SPECIES_CIRNO]      = 0,
    [SPECIES_CHIBI_RUMIA]   = 2,
    [SPECIES_RUMIA]   = 2,
    [SPECIES_SHANGHAI]   = 2,
    [SPECIES_CHIBI_FLANDRE]    = 2,
    [SPECIES_FLANDRE]   = 2,
    [SPECIES_HOURAI]   = 2,
    [SPECIES_CHIBI_YOUMU]    = 2,
    [SPECIES_YOUMU]    = 2,
    [SPECIES_CHIBI_YUKARI]    = 0,
    [SPECIES_YUKARI]    = 0,
    [SPECIES_CHIBI_SUIKA]      = 2,
    [SPECIES_SUIKA]   = 1,
    [SPECIES_CHIBI_MYSTIA]  = 0,
    [SPECIES_MYSTIA]  = 0,
    [SPECIES_CHIBI_MINORIKO]       = 2,
    [SPECIES_MINORIKO]      = 2,
    [SPECIES_CHIBI_KEINE]      = 1,
    [SPECIES_KEINE]       = 0,
    [SPECIES_ATTACK_KEINE]   = 0,
    [SPECIES_CHIBI_IKU]       = 0,
    [SPECIES_IKU]    = 0,
    [SPECIES_CHIBI_KOISHI]     = 0,
    [SPECIES_KOISHI]    = 2,
    [SPECIES_CHIBI_YAMAME]     = 2,
    [SPECIES_YAMAME]     = 2,
    [SPECIES_CHIBI_CHEN]      = 1,
    [SPECIES_CHEN]     = 1,
    [SPECIES_CHIBI_YORIHIME]     = 1,
    [SPECIES_YORIHIME]     = 2,
    [SPECIES_CHIBI_KAGUYA]      = 1,
    [SPECIES_KAGUYA]    = 2,
    [SPECIES_CHIBI_MOKOU]   = 0,
    [SPECIES_MOKOU]    = 0,
    [SPECIES_CHIBI_HATATE]     = 0,
    [SPECIES_HATATE]   = 0,
    [SPECIES_CHIBI_LETTY]   = 0,
    [SPECIES_LETTY]        = 2,
    [SPECIES_CHIBI_PATCHOULI]     = 2,
    [SPECIES_PATCHOULI]    = 2,
    [SPECIES_CHIBI_YUUGI]      = 0,
    [SPECIES_YUUGI]     = 2,
    [SPECIES_CHIBI_TENSHI]     = 0,
    [SPECIES_TENSHI]  = 1,
    [SPECIES_CHIBI_PARSEE]  = 1,
    [SPECIES_PARSEE]  = 1,
    [SPECIES_CHIBI_MURASA]   = 2,
    [SPECIES_MURASA]  = 2,
    [SPECIES_CHIBI_KISUME]     = 1,
    [SPECIES_KISUME]    = 1,
    [SPECIES_CHIBI_KOGASA]       = 1,
    [SPECIES_KOGASA]      = 0,
    [SPECIES_CHIBI_SATORI]    = 0,
    [SPECIES_SATORI]    = 0,
    [SPECIES_CHIBI_EIRIN]     = 0,
    [SPECIES_EIRIN]   = 0,
    [SPECIES_CHIBI_RAN]    = 0,
    [SPECIES_RAN]   = 1,
    [SPECIES_YOUSEI]       = 2,
    [SPECIES_DAIYOUSEI]      = 2,
    [SPECIES_CHIBI_NITORI]        = 2,
    [SPECIES_NITORI]     = 2,
    [SPECIES_CHIBI_HINA]      = 2,
    [SPECIES_HINA]         = 2,
    [SPECIES_CHIBI_KOMACHI]    = 2,
    [SPECIES_KOMACHI]    = 2,
    [SPECIES_CHIBI_YUYUKO]      = 2,
    [SPECIES_YUYUKO]     = 2,
    [SPECIES_CHIBI_MEILING]      = 2,
    [SPECIES_MEILING]        = 2,
    [SPECIES_CHIBI_SAKUYA]     = 2,
    [SPECIES_SAKUYA]       = 1,
    [SPECIES_CHIBI_LUNASA]      = 2,
    [SPECIES_LUNASA]     = 2,
    [SPECIES_CHIBI_MERLIN]     = 0,
    [SPECIES_MERLIN]   = 0,
    [SPECIES_CHIBI_LYRICA]   = 0,
    [SPECIES_LYRICA]   = 1,
    [SPECIES_CHIBI_LAYLA]      = 1,
    [SPECIES_LAYLA]     = 1,
    [SPECIES_CHIBI_KANAKO]   = 2,
    [SPECIES_KANAKO]  = 2,
    [SPECIES_CHIBI_RIN]   = 1,
    [SPECIES_RIN]     = 2,
    [SPECIES_CHIBI_UTSUHO]     = 2,
    [SPECIES_UTSUHO]     = 1,
    [SPECIES_CHIBI_ICHIRIN]      = 1,
    [SPECIES_ICHIRIN]     = 0,
    [SPECIES_CHIBI_SUNNY_MILK]     = 0,
    [SPECIES_SUNNY_MILK]  = 1,
    [SPECIES_CHIBI_LUNA_CHILD]      = 0,
    [SPECIES_LUNA_CHILD]      = 0,
    [SPECIES_CHIBI_STAR_SAPPHIRE]     = 0,
    [SPECIES_STAR_SAPPHIRE]     = 0,
    [SPECIES_CHIBI_YUUKA]      = 2,
    [SPECIES_YUUKA]     = 2,
    [SPECIES_CHIBI_AYA]     = 0,
    [SPECIES_AYA]     = 1,
    [SPECIES_CHIBI_EIKI]        = 2,
    [SPECIES_EIKI]  = 1,
    [SPECIES_CHIBI_SHOU]      = 0,
    [SPECIES_SHOU]      = 2,
    [SPECIES_CHIBI_WRIGGLE]      = 2,
    [SPECIES_WRIGGLE]    = 0,
    [SPECIES_CHIBI_TEWI]    = 0,
    [SPECIES_TEWI]      = 2,
    [SPECIES_KEDAMA]       = 2,
    [SPECIES_CHIBI_NUE]       = 2,
    [SPECIES_NUE]    = 0,
    [SPECIES_CHIBI_SUWAKO]     = 0,
    [SPECIES_SUWAKO]     = 0,
    [SPECIES_GYOKUTO]     = 0,
    [SPECIES_CHIBI_MARISA]     = 0,
    [SPECIES_MARISA]     = 0,
    [SPECIES_CHIBI_REIMU]      = 2,
    [SPECIES_REIMU]    = 2,
    [SPECIES_CHIBI_BYAKUREN]  = 0,
    [SPECIES_BYAKUREN]     = 1,
    [SPECIES_KONGALA]    = 0,
    [SPECIES_KIKURI]      = 0,
    [SPECIES_YUUGEN]     = 0,
    [SPECIES_CHIBI_TOYOHIME]     = 0,
    [SPECIES_TOYOHIME]   = 0,
    [SPECIES_GOLIATH]   = 2,
    [SPECIES_SARIEL]      = 2,
    [SPECIES_SATSUKI]         = 0,
    [SPECIES_AKYUU]   = 1,
    [SPECIES_GENJI]     = 1,
    [SPECIES_TORI]    = 1,
    [SPECIES_OONAMAZU]   = 1,
    [SPECIES_CHIBI_ELLY]     = 1,
    [SPECIES_ELLY]  = 1,
    [SPECIES_CHIBI_ORANGE]    = 2,
    [SPECIES_ORANGE]    = 2,
    [SPECIES_YOUKI]  = 2,
    [SPECIES_CHIBI_CHIYURI]     = 2,
    [SPECIES_CHIYURI]      = 2,
    [SPECIES_CHIBI_YUMEMI]    = 2,
    [SPECIES_YUMEMI]     = 2,
    [SPECIES_CHIBI_ELLEN]      = 0,
    [SPECIES_ELLEN]      = 0,
    [SPECIES_CHIBI_KAZAMI]    = 1,
    [SPECIES_KAZAMI]     = 0,
    [SPECIES_CHIBI_MEIRA]      = 2,
    [SPECIES_MEIRA]    = 2,
    [SPECIES_CHIBI_SARA]     = 0,
    [SPECIES_SARA]       = 0,
    [SPECIES_CHIBI_LOUISE]      = 0,
    [SPECIES_LOUISE]   = 1,
    [SPECIES_CHIBI_ALICE_98]      = 2,
    [SPECIES_ALICE_98]     = 2,
    [SPECIES_CHIBI_KANA]        = 0,
    [SPECIES_KANA]        = 0,
    [SPECIES_CHIBI_HAKUREI]      = 2,
    [SPECIES_HAKUREI]     = 0,
    [SPECIES_CHIBI_KIRISAME]    = 0,
    [SPECIES_KIRISAME]   = 1,
    [SPECIES_CHIBI_KOTOHIME]      = 2,
    [SPECIES_KOTOHIME]   = 2,
    [SPECIES_CHIBI_YUMEKO]   = 1,
    [SPECIES_YUMEKO]    = 1,
    [SPECIES_CHIBI_SHINKI]      = 1,
    [SPECIES_SHINKI]    = 1,
    [SPECIES_MIMI_CHAN]    = 2,
    [SPECIES_RUUKOTO]       = 2,
    [SPECIES_CHIBI_RIKA]     = 1,
    [SPECIES_RIKA]    = 1,
    [SPECIES_CHIBI_MUGETSU]       = 1,
    [SPECIES_MUGETSU]      = 0,
    [SPECIES_CHIBI_GENGETSU]    = 0,
    [SPECIES_GENGETSU]      = 2,
    [SPECIES_CHIBI_KURUMI]     = 2,
    [SPECIES_KURUMI]     = 2,
    [SPECIES_CHIBI_RIKAKO]    = 0,
    [SPECIES_RIKAKO]  = 0,
    [SPECIES_UNOWN]       = 0,
    [SPECIES_CHIBI_YUKI]   = 0,
    [SPECIES_YUKI]   = 1,
    [SPECIES_CHIBI_MAI]      = 0,
    [SPECIES_MAI]  = 2,
    [SPECIES_DEFENSE_SUNNY_MILK]   = 2,
    [SPECIES_TECHNICAL_LUNA_CHILD]      = 2,
    [SPECIES_SUPPORT_STAR_SAPPHIRE]     = 0,
    [SPECIES_ATTACK_NITORI]    = 0,
    [SPECIES_TECHNICAL_NITORI]    = 2,
    [SPECIES_ATTACK_MOKOU]    = 0,
    [SPECIES_DEFENSE_MOKOU]      = 0,
    [SPECIES_DEFENSE_REISEN]     = 1,
    [SPECIES_TECHNICAL_REISEN]   = 2,
    [SPECIES_TECHNICAL_SAKUYA]     = 0,
    [SPECIES_SUPPORT_SAKUYA]   = 0,
    [SPECIES_ATTACK_RUMIA]    = 2,
    [SPECIES_SPEED_RUMIA]      = 0,
    [SPECIES_SUPPORT_MYSTIA]    = 0,
    [SPECIES_ATTACK_MYSTIA]      = 2,
    [SPECIES_ATTACK_CHEN]   = 2,
    [SPECIES_TECHNICAL_CHEN]     = 0,
    [SPECIES_DEFENSE_YOUMU]    = 0,
    [SPECIES_SPEED_YOUMU]   = 0,
    [SPECIES_ATTACK_YUUKA]    = 0,
    [SPECIES_TECHNICAL_YUUKA]     = 2,
    [SPECIES_SPEED_AYA]    = 0,
    [SPECIES_TECHNICAL_AYA]    = 0,
    [SPECIES_ATTACK_TENSHI]    = 0,
    [SPECIES_DEFENSE_TENSHI]     = 0,
    [SPECIES_DEFENSE_PARSEE]      = 0,
    [SPECIES_TECHNICAL_PARSEE]     = 0,
    [SPECIES_ATTACK_KOISHI]    = 0,
    [SPECIES_SPEED_KOISHI]    = 2,
    [SPECIES_DEFENSE_BYAKUREN]    = 1,
    [SPECIES_TECHNICAL_BYAKUREN]     = 2,
    [SPECIES_DEFENSE_ICHIRIN]   = 2,
    [SPECIES_TECHNICAL_ICHIRIN]    = 1,
    [SPECIES_ATTACK_KANAKO]      = 1,
    [SPECIES_DEFENSE_KANAKO]       = 1,
    [SPECIES_ATTACK_KAGUYA]     = 1,
    [SPECIES_DEFENSE_KAGUYA]     = 1,
    [SPECIES_SHINGYOKU_ORB]      = 0,
    [SPECIES_SHINGYOKU_MALE]       = 2,
    [SPECIES_SHINGYOKU_FEMALE]     = 0,
    [SPECIES_ATTACK_UTSUHO]    = 1,
    [SPECIES_SPEED_UTSUHO]     = 0,
    [SPECIES_DEFENSE_MOMIJI]   = 1,
    [SPECIES_ELIS]       = 0,
    [SPECIES_MIMA]       = 1,
    [SPECIES_ISAMI]      = 1,
    [SPECIES_DUMMY1] = 0,
    [SPECIES_DUMMY2] = 0,
    [SPECIES_DUMMY3] = 0,
    [SPECIES_DUMMY4] = 0,
    [SPECIES_DUMMY5] = 0,
    [SPECIES_DUMMY6] = 0,
    [SPECIES_CHIBI_KYOUKO] = 0,
    [SPECIES_KYOUKO] = 0,
    [SPECIES_CHIBI_MIYAKO] = 0,
    [SPECIES_MIYAKO] = 0,
    [SPECIES_CHIBI_SEIGA] = 0,
    [SPECIES_SEIGA] = 0,
    [SPECIES_CHIBI_TOJIKO] = 0,
    [SPECIES_CHIBI_MIKO] = 0,
    [SPECIES_MIKO] = 0,
    [SPECIES_CHIBI_MAMIZOU] = 0,
    [SPECIES_DUMMY7] = 0,
    [SPECIES_DUMMY8] = 0,
    [SPECIES_TECHNICAL_SENDAI] = 0,
    [SPECIES_PRISMRIVER_SISTERS] = 0,
    [SPECIES_MAMIZOU] = 0,
    [SPECIES_ADVENT_YOUMU] = 0,
    [SPECIES_ADVENT_YUYUKO] = 0,
    [SPECIES_ADVENT_EIRIN] = 0,
    [SPECIES_SPEED_TENMA] = 0,
    [SPECIES_SPEED_KOMACHI]     = 1,
    [SPECIES_ATTACK_KOMACHI]     = 0,
    [SPECIES_ATTACK_EIRIN]    = 1,
    [SPECIES_SUPPORT_EIRIN]     = 0,
    [SPECIES_DEFENSE_YUKARI]   = 0,
    [SPECIES_TECHNICAL_YUKARI]    = 0,
    [SPECIES_ATTACK_ALICE]      = 0,
    [SPECIES_TECHNICAL_ALICE]   = 0,
    [SPECIES_ATTACK_REIMU]    = 0,
    [SPECIES_DEFENSE_REIMU]   = 2,
    [SPECIES_SPEED_CIRNO]   = 2,
    [SPECIES_TECHNICAL_CIRNO]   = 2,
    [SPECIES_DEFENSE_YUYUKO]     = 2,
    [SPECIES_ATTACK_YUYUKO]     = 0,
    [SPECIES_SUPPORT_LUNASA]     = 2,
    [SPECIES_SUPPORT_MERLIN]   = 0,
    [SPECIES_SUPPORT_LYRICA]     = 2,
    [SPECIES_ATTACK_RAN]      = 1,
    [SPECIES_SUPPORT_RAN]       = 1,
    [SPECIES_ATTACK_MEDICINE]      = 1,
    [SPECIES_TECHNICAL_MEDICINE]    = 1,
    [SPECIES_ATTACK_HINA]      = 1,
    [SPECIES_DEFENSE_HINA]     = 1,
    [SPECIES_SPEED_SHIZUHA]     = 0,
    [SPECIES_SPEED_RIN]     = 1,
    [SPECIES_ATTACK_RIN]     = 1,
    [SPECIES_ZOMBIE_FAIRY]    = 1,
    [SPECIES_SUPPORT_SHIZUHA]     = 2,
    [SPECIES_ATTACK_SUWAKO]     = 2,
    [SPECIES_ATTACK_SHOU]   = 1,
    [SPECIES_TECHNICAL_SHOU]     = 1,
    [SPECIES_ATTACK_TOKIKO]      = 1,
    [SPECIES_DEFENSE_SUWAKO]     = 0,
    [SPECIES_SPEED_YAMAME]    = 0,
    [SPECIES_SPEED_KOGASA]     = 2,
    [SPECIES_TECHNICAL_KOGASA]  = 0,
    [SPECIES_DEFENSE_REMILIA]     = 2,
    [SPECIES_SPEED_FLANDRE]     = 0,
    [SPECIES_DEFENSE_WRIGGLE]      = 0,
    [SPECIES_DEFENSE_MEILING]    = 2,
    [SPECIES_TENSOKU]     = 1,
    [SPECIES_ATTACK_SUNNY_MILK]      = 1,
    [SPECIES_ATTACK_LUNA_CHILD]     = 0,
    [SPECIES_SPEED_WRIGGLE]    = 0,
    [SPECIES_SUPPORT_KOAKUMA]     = 1,
    [SPECIES_SPEED_MEILING]     = 2,
    [SPECIES_CHIBI_ELIS]    = 0,
    [SPECIES_CHIBI_SARIEL]    = 0,
    [SPECIES_ADVENT_MYSTIA]     = 0,
    [SPECIES_TECHNICAL_HATATE]    = 0,
    [SPECIES_DEFENSE_HATATE]   = 0,
    [SPECIES_AKI_SISTERS]      = 2,
    [SPECIES_DEFENSE_LAYLA]     = 0,
    [SPECIES_DEFENSE_KEINE]    = 0,
    [SPECIES_ATTACK_REMILIA]    = 0,
    [SPECIES_DEFENSE_TEWI]    = 1,
    [SPECIES_ATTACK_TEWI]     = 1,
    [SPECIES_DEFENSE_EIKI]      = 1,
    [SPECIES_DEFENSE_MINORIKO]    = 2,
    [SPECIES_TECHNICAL_SUIKA]    = 1,
    [SPECIES_ATTACK_SANAE]   = 1,
    [SPECIES_TECHNICAL_SANAE]   = 0,
    [SPECIES_ATTACK_FLANDRE]       = 1,
    [SPECIES_ATTACK_DAIYOUSEI]    = 0,
    [SPECIES_TOJIKO]      = 2,
    [SPECIES_CHIBI_FUTO]      = 2,
    [SPECIES_FUTO]     = 0,
    [SPECIES_ATTACK_EIKI]      = 1,
    [SPECIES_TECHNICAL_TOYOHIME]    = 1,
    [SPECIES_MISSING_POWER_SUIKA]     = 2,
    [SPECIES_SCHOOLGIRL_SANAE]      = 0,
    [SPECIES_CHIBI_KONGALA]    = 1,
    [SPECIES_CHIBI_YUUGEN]     = 0,
    [SPECIES_ATTACK_SUIKA]     = 2,
    [SPECIES_SUPPORT_LILY_WHITE]      = 0,
    [SPECIES_SUPPORT_LILY_BLACK]     = 2,
    [SPECIES_ATTACK_NUE]      = 0,
    [SPECIES_TECHNICAL_NUE]       = 0,
    [SPECIES_DEFENSE_PATCHOULI]      = 2,
    [SPECIES_DEFENSE_LETTY]    = 0,
    [SPECIES_SUPPORT_LETTY]    = 0,
    [SPECIES_SPEED_YORIHIME]      = 0,
    [SPECIES_CHIBI_SHINGYOKU]     = 0,
    [SPECIES_ANGEL_VIVIT]      = 0,
    [SPECIES_ATTACK_LILY_BLACK]     = 0,
    [SPECIES_CHIBI_KASEN]    = 0,
    [SPECIES_DEFENSE_YUUGI]     = 0,
    [SPECIES_TECHNICAL_NAZRIN]     = 2,
    [SPECIES_SUPPORT_NAZRIN]    = 2,
    [SPECIES_TECHNICAL_KISUME]     = 1,
    [SPECIES_ATTACK_YUUGI]      = 1,
    [SPECIES_SUPPORT_KEINE]      = 2,
    [SPECIES_KASEN]     = 1,
    [SPECIES_DEFENSE_IKU]     = 0,
    [SPECIES_TECHNICAL_IKU]     = 2,
    [SPECIES_ATTACK_MINORIKO]     = 2,
    [SPECIES_RINNOSUKE]    = 0,
    [SPECIES_ADVENT_CIRNO]     = 0,
    [SPECIES_ADVENT_REISEN]    = 0,
    [SPECIES_VIVIT]       = 0,
    [SPECIES_SPEED_SARIEL]     = 0,
    [SPECIES_ATTACK_LILY_WHITE]     = 0,
    [SPECIES_CHIBI_MIMA]     = 2,
    [SPECIES_CHIBI_KIKURI]    = 0,
    [SPECIES_ADVENT_MEILING]   = 1,
    [SPECIES_ATTACK_PATCHOULI]        = 2,
    [SPECIES_SPEED_MARISA]      = 2,
    [SPECIES_ATTACK_MARISA]      = 2,
    [SPECIES_FAIRY]    = 0,
    [SPECIES_DEFENSE_MURASA]     = 0,
    [SPECIES_ATTACK_MURASA]    = 2,
    [SPECIES_ATTACK_STAR_SAPPHIRE]      = 2,
    [SPECIES_ATTACK_KAZAMI]     = 0,
    [SPECIES_SPEED_MEDICINE]     = 0,
    [SPECIES_DEFENSE_DAIYOUSEI]     = 0,
    [SPECIES_TECHNICAL_YAMAME]       = 1,
    [SPECIES_TECHNICAL_SATORI]      = 1,
    [SPECIES_DEFENSE_SATORI]   = 1,
    [SPECIES_ADVENT_LETTY]       = 2,
    [SPECIES_ADVENT_TEWI]     = 2,
    [SPECIES_ADVENT_MARISA]   = 0,
    [SPECIES_ADVENT_ALICE]      = 0,
    [SPECIES_ADVENT_MOKOU]      = 0,
    [SPECIES_ADVENT_YUKARI]   = 0,
    [SPECIES_ADVENT_RAN]    = 2,
    [SPECIES_ADVENT_CHEN]      = 2,
    [SPECIES_MARISA_EX]   = 2,
    [SPECIES_BLACK_ALICE]      = 2,
    [SPECIES_TENMA]     = 0,
    [SPECIES_SENDAI]    = 1,
    [SPECIES_CHIBI_SENDAI]      = 0,
    [SPECIES_CHIBI_TENMA]      = 2,
    [SPECIES_TENSOKU_G]     = 0,
    [SPECIES_AYAKASHI]      = 0,
    [SPECIES_CHIBI_VIVIT]    = 0,
    [SPECIES_EGG]         = 1,
    [SPECIES_UNOWN_B]     = 0,
    [SPECIES_UNOWN_C]     = 0,
    [SPECIES_UNOWN_D]     = 0,
    [SPECIES_UNOWN_E]     = 0,
    [SPECIES_UNOWN_F]     = 0,
    [SPECIES_UNOWN_G]     = 0,
    [SPECIES_UNOWN_H]     = 0,
    [SPECIES_UNOWN_I]     = 0,
    [SPECIES_UNOWN_J]     = 0,
    [SPECIES_UNOWN_K]     = 0,
    [SPECIES_UNOWN_L]     = 0,
    [SPECIES_UNOWN_M]     = 0,
    [SPECIES_UNOWN_N]     = 0,
    [SPECIES_UNOWN_O]     = 0,
    [SPECIES_UNOWN_P]     = 0,
    [SPECIES_UNOWN_Q]     = 0,
    [SPECIES_UNOWN_R]     = 0,
    [SPECIES_UNOWN_S]     = 0,
    [SPECIES_UNOWN_T]     = 0,
    [SPECIES_UNOWN_U]     = 0,
    [SPECIES_UNOWN_V]     = 0,
    [SPECIES_UNOWN_W]     = 0,
    [SPECIES_UNOWN_X]     = 0,
    [SPECIES_UNOWN_Y]     = 0,
    [SPECIES_UNOWN_Z]     = 0,
    [SPECIES_UNOWN_EMARK] = 0,
    [SPECIES_UNOWN_QMARK] = 0
};

const struct SpritePalette gMonIconPaletteTable[] = {
    { gMonIconPalettes[0], POKE_ICON_BASE_PAL_TAG + 0 },
    { gMonIconPalettes[1], POKE_ICON_BASE_PAL_TAG + 1 },
    { gMonIconPalettes[2], POKE_ICON_BASE_PAL_TAG + 2 },
    // The following three point outside the gMonIconPalettes array
    // and are therefore invalid. Fortunately, they are never used.
    { gMonIconPalettes[3], POKE_ICON_BASE_PAL_TAG + 3 },
    { gMonIconPalettes[4], POKE_ICON_BASE_PAL_TAG + 4 },
    { gMonIconPalettes[5], POKE_ICON_BASE_PAL_TAG + 5 }
};

static const struct OamData sMonIconOamData = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0x000,
    .priority = 1,
    .paletteNum = 0
};

static const union AnimCmd sMonIconAnim_Fast[] = {
    ANIMCMD_FRAME(0,  6),
    ANIMCMD_FRAME(1,  6),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sMonIconAnim_MediumFast[] = {
    ANIMCMD_FRAME(0,  8),
    ANIMCMD_FRAME(1,  8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sMonIconAnim_MediumSlow[] = {
    ANIMCMD_FRAME(0, 14),
    ANIMCMD_FRAME(1, 14),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sMonIconAnim_Slow[] = {
    ANIMCMD_FRAME(0, 22),
    ANIMCMD_FRAME(1, 22),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sMonIconAnim_Still[] = {
    ANIMCMD_FRAME(0, 29),
    ANIMCMD_FRAME(0, 29),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sMonIconAnims[] = {
    sMonIconAnim_Fast,
    sMonIconAnim_MediumFast,
    sMonIconAnim_MediumSlow,
    sMonIconAnim_Slow,
    sMonIconAnim_Still
};

static const union AffineAnimCmd sMonIconAffineAnim_0[] = {
    AFFINEANIMCMD_FRAME(0, 0, 0, 10),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sMonIconAffineAnim_1[] = {
    AFFINEANIMCMD_FRAME(-2, -2, 0, 122),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sMonIconAffineAnims[] = {
    sMonIconAffineAnim_0,
    sMonIconAffineAnim_1
};

static const u16 sSpriteImageSizes[][4] = {
    [ST_OAM_SQUARE] = {
        [ST_OAM_SIZE_0] = 0x020,
        [ST_OAM_SIZE_1] = 0x080,
        [ST_OAM_SIZE_2] = 0x200,
        [ST_OAM_SIZE_3] = 0x800,
    },
    [ST_OAM_H_RECTANGLE] = {
        [ST_OAM_SIZE_0] = 0x040,
        [ST_OAM_SIZE_1] = 0x080,
        [ST_OAM_SIZE_2] = 0x100,
        [ST_OAM_SIZE_3] = 0x400,
    },
    [ST_OAM_V_RECTANGLE] = {
        [ST_OAM_SIZE_0] = 0x040,
        [ST_OAM_SIZE_1] = 0x080,
        [ST_OAM_SIZE_2] = 0x100,
        [ST_OAM_SIZE_3] = 0x400,
    },
};

u8 CreateMonIcon(u16 species, SpriteCallback callback, s16 x, s16 y, u8 subpriority, u32 personality, bool32 extra)
{
    u8 spriteId;
    struct MonIconSpriteTemplate iconTemplate =
        {
            .oam = &sMonIconOamData,
            .image = GetMonIconPtr(species, personality, extra),
            .anims = sMonIconAnims,
            .affineAnims = sMonIconAffineAnims,
            .callback = callback,
            .paletteTag = POKE_ICON_BASE_PAL_TAG + gMonIconPaletteIndices[species],
        };

    if (species > NUM_SPECIES)
        iconTemplate.paletteTag = POKE_ICON_BASE_PAL_TAG;

    spriteId = CreateMonIconSprite(&iconTemplate, x, y, subpriority);

    UpdateMonIconFrame(&gSprites[spriteId]);

    return spriteId;
}

u8 CreateMonIcon_HandleDeoxys(u16 species, SpriteCallback callback, s16 x, s16 y, u8 subpriority, bool32 extra)
{
    u8 spriteId;
    struct MonIconSpriteTemplate iconTemplate =
        {
            .oam = &sMonIconOamData,
            .image = NULL,
            .anims = sMonIconAnims,
            .affineAnims = sMonIconAffineAnims,
            .callback = callback,
            .paletteTag = POKE_ICON_BASE_PAL_TAG + gMonIconPaletteIndices[species],
        };

    iconTemplate.image = GetMonIconTiles(species, extra);
    spriteId = CreateMonIconSprite(&iconTemplate, x, y, subpriority);

    UpdateMonIconFrame(&gSprites[spriteId]);

    return spriteId;
}

u16 GetIconSpecies(u16 species, u32 personality)
{
    u16 result;

    if (species == SPECIES_UNOWN)
    {
        u16 letter = GetUnownLetterByPersonality(personality);
        if (letter == 0)
            letter = SPECIES_UNOWN;
        else
            letter += (SPECIES_UNOWN_B - 1);
        result = letter;
    }
    else
    {
        if (species > NUM_SPECIES)
            result = SPECIES_NONE;
        else
            result = species;
    }

    return result;
}

u16 GetUnownLetterByPersonality(u32 personality)
{
    if (!personality)
        return 0;
    else
        return (((personality & 0x3000000) >> 18) | ((personality & 0x30000) >> 12) | ((personality & 0x300) >> 6) | (personality & 0x3)) % 0x1C;
}

u16 MailSpeciesToIconSpecies(u16 species)
{
    u16 value;

    if (MailSpeciesToSpecies(species, &value) == SPECIES_UNOWN)
    {
        if (value == 0)
            value += SPECIES_UNOWN;
        else
            value += (SPECIES_UNOWN_B - 1);
        return value;
    }
    else
    {
        if (species > (SPECIES_UNOWN_B - 1))
            species = SPECIES_NONE;
        return GetIconSpecies(species, 0);
    }
}

const u8 *GetMonIconTiles(u16 species, bool32 extra)
{
    const u8 *iconSprite = gMonIconTable[species];
    if (species == SPECIES_AYAKASHI && extra == TRUE)
        iconSprite += 0x400;
    return iconSprite;
}

const u8 *GetMonIconPtr(u16 species, u32 personality, bool32 extra)
{
    return GetMonIconTiles(GetIconSpecies(species, personality), extra);
}

void DestroyMonIcon(struct Sprite *sprite)
{
    DestroyMonIconInternal(sprite);
}

void LoadMonIconPalettes(void)
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(gMonIconPaletteTable); i++)
        LoadSpritePalette(&gMonIconPaletteTable[i]);
}

void SafeLoadMonIconPalette(u16 species)
{
    u8 palIndex;
    if (species > NUM_SPECIES)
        species = SPECIES_NONE;
    palIndex = gMonIconPaletteIndices[species];
    if (IndexOfSpritePaletteTag(gMonIconPaletteTable[palIndex].tag) == 0xFF)
        LoadSpritePalette(&gMonIconPaletteTable[palIndex]);
}

void LoadMonIconPalette(u16 species)
{
    u8 palIndex;
    palIndex = gMonIconPaletteIndices[species];
    if (IndexOfSpritePaletteTag(gMonIconPaletteTable[palIndex].tag) == 0xFF)
        LoadSpritePalette(&gMonIconPaletteTable[palIndex]);
}

void FreeMonIconPalettes(void)
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(gMonIconPaletteTable); i++)
        FreeSpritePaletteByTag(gMonIconPaletteTable[i].tag);
}

void SafeFreeMonIconPalette(u16 species)
{
    u8 palIndex;
    if (species > NUM_SPECIES)
        species = SPECIES_NONE;
    palIndex = gMonIconPaletteIndices[species];
    FreeSpritePaletteByTag(gMonIconPaletteTable[palIndex].tag);
}

void FreeMonIconPalette(u16 species)
{
    u8 palIndex;
    palIndex = gMonIconPaletteIndices[species];
    FreeSpritePaletteByTag(gMonIconPaletteTable[palIndex].tag);
}

void SpriteCB_MonIcon(struct Sprite *sprite)
{
    UpdateMonIconFrame(sprite);
}

void LoadMonIconPalettesAt(u16 offset)
{
    int i;
    if (offset <= BG_PLTT_ID(16 - ARRAY_COUNT(gMonIconPaletteTable)))
    {
        for (i = 0; i < (int)ARRAY_COUNT(gMonIconPaletteTable); i++)
        {
            LoadPalette(gMonIconPaletteTable[i].data, offset, PLTT_SIZE_4BPP);
            offset += 16;
        }
    }
}

const u16 *GetValidMonIconPalettePtr(u16 species)
{
    if (species > NUM_SPECIES)
        species = SPECIES_NONE;
    return gMonIconPaletteTable[gMonIconPaletteIndices[species]].data;
}

u8 GetValidMonIconPalIndex(u16 species)
{
    if (species > NUM_SPECIES)
        species = SPECIES_NONE;
    return gMonIconPaletteIndices[species];
}

u8 GetMonIconPaletteIndexFromSpecies(u16 species)
{
    return gMonIconPaletteIndices[species];
}

u8 UpdateMonIconFrame(struct Sprite *sprite)
{
    u8 result = 0;

    if (sprite->animDelayCounter == 0)
    {
        s16 frame = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.imageValue;

        switch (frame)
        {
        case -1:
            break;
        case -2:
            sprite->animCmdIndex = 0;
            break;
        default:
            RequestSpriteCopy(
                // pointer arithmetic is needed to get the correct pointer to perform the sprite copy on.
                // because sprite->images is a struct def, it has to be casted to (u8 *) before any
                // arithmetic can be performed.
                (u8 *)sprite->images + (sSpriteImageSizes[sprite->oam.shape][sprite->oam.size] * frame),
                (u8 *)(OBJ_VRAM0 + sprite->oam.tileNum * TILE_SIZE_4BPP),
                sSpriteImageSizes[sprite->oam.shape][sprite->oam.size]);
            sprite->animDelayCounter = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.duration & 0xFF;
            sprite->animCmdIndex++;
            result = sprite->animCmdIndex;
            break;
        }
    }
    else
    {
        sprite->animDelayCounter--;
    }
    return result;
}

static u8 CreateMonIconSprite(const struct MonIconSpriteTemplate * iconTemplate, s16 x, s16 y, u8 subpriority)
{
    u8 spriteId;

    struct SpriteFrameImage image = { NULL, sSpriteImageSizes[iconTemplate->oam->shape][iconTemplate->oam->size] };

    struct SpriteTemplate spriteTemplate =
    {
        .tileTag = TAG_NONE,
        .paletteTag = iconTemplate->paletteTag,
        .oam = iconTemplate->oam,
        .anims = iconTemplate->anims,
        .images = &image,
        .affineAnims = iconTemplate->affineAnims,
        .callback = iconTemplate->callback,
    };

    spriteId = CreateSprite(&spriteTemplate, x, y, subpriority);
    gSprites[spriteId].animPaused = TRUE;
    gSprites[spriteId].animBeginning = FALSE;
    gSprites[spriteId].images = (const struct SpriteFrameImage *)iconTemplate->image;
    return spriteId;
}

static void DestroyMonIconInternal(struct Sprite *sprite)
{
    struct SpriteFrameImage image = { NULL, sSpriteImageSizes[sprite->oam.shape][sprite->oam.size] };
    sprite->images = &image;
    DestroySprite(sprite);
}

void SetPartyHPBarSprite(struct Sprite *sprite, u8 animNum)
{
    sprite->animNum = animNum;
    sprite->animDelayCounter = 0;
    sprite->animCmdIndex = 0;
}
